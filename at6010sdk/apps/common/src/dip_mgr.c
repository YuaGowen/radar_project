/**
 * Copyright (c) 2023, Airtouching Intelligence Technology.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Airtouching
 *    Intelligence Technology integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY AIRTOUCHING TECHNOLOGY "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AIRTOUCHING TECHNOLOGY OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "dip_mgr.h"
#include "hal_dip.h"
#include "radar_mgr.h"
#include "hal_ls.h"
#include "osal_timer.h"

#ifdef CFG_OS
#include "sys_task.h"
#endif

#ifdef CFG_APP_LIGHT
#include "app_light.h"
#endif

#ifdef CFG_DIP_SWITCH

#define DIST_LEVEL_MAX        16
#define LOT1_LEVEL_MAX        8
#define LUX_LEVEL_MAX         8
#define DIP_DEBOUNCE_MS       (500u)
#define DIP_MSG_POST_EN

#define PARA_DIP_IMME_EFFECT \
(PARA_DIP_DIST_IMME_EFFECT | PARA_DIP_LOT1_IMME_EFFECT | PARA_DIP_LUX_IMME_EFFECT)

/*******************************************************************************
**                      Private Variable Definitions                          **
*******************************************************************************/
static const u8 m_dist_map_tbl[DIST_LEVEL_MAX] = PARA_DIP_DIST_LV;
static const u32 m_lot1_map_tbl[LOT1_LEVEL_MAX] = PARA_DIP_LOT1_LV;
#ifdef CFG_LS
static const u32 m_lux_map_tbl[LUX_LEVEL_MAX] = PARA_DIP_LUX_LV;
#endif

static u8 m_dip_changed;
#if (DIP_DEBOUNCE_MS > 0u)
static osal_timer_t tm_debounce;
#endif

#if defined(DIP_MSG_POST_EN) && !defined(CFG_OS)
static msg_t m_dip_msg;
#endif

/*******************************************************************************
**                      Private Function Definitions                          **
*******************************************************************************/
static void dip_setting_imme_effect(void *arg)
{
	(void)arg;
	if (0u < (m_dip_changed & DIP_TYPE_DIST)) {
		dip_setting_dist();
	}

	if (0u < (m_dip_changed & DIP_TYPE_LOT1)) {
		dip_setting_lot1();
	}

#ifdef CFG_LS
	if (0u < (m_dip_changed & DIP_TYPE_LUX)) {
		dip_setting_lux();
	}
#endif
	m_dip_changed = 0u;
}

#ifdef DIP_MSG_POST_EN

#ifdef CFG_OS
static void dip_msg_send(void *arg)
{
	osal_msg_t msg;
	(void)arg;

	msg.id = (u32)MSG_SYS_DIP_SWITCH_CHANGED;
	msg.para = 0U;
	OSAL_ASSERT_ERR(osal_task_send_msg(MOD_SYS, &msg, OSAL_WAIT_FOREVER),
	                "dip send msg failed!\r\n");
}

static bool dip_msg_handler(const osal_msg_t *msg)
{
	bool ret = FALSE;
	if ((u32)MSG_SYS_GRP_DIP_SWITCH == msg->id) {
		dip_setting_imme_effect(NULL);
		ret = TRUE;
	}
	return ret;
}

#else
static void dip_msg_send(u32 arg)
{
	(void)arg;
	if (0 != k_msg_post(MSG_ID_DIP, &m_dip_msg)) {
		logw("%s, Send dip msg failed!\r\n", __func__);
	}
}

static void dip_proc_msg(msg_t *msg)
{
	(void)msg;
	dip_setting_imme_effect(0U);
}
#endif /* CFG_OS */

static void dip_msg_init(void)
{
#ifdef CFG_OS
	sys_task_reg_msg(MSG_SYS_GRP_DIP_SWITCH, dip_msg_handler);
#else
	if (0 != k_msg_reg_hdl(MSG_ID_DIP, dip_proc_msg)) {
		loge("%s, Create dip msg failed!\n\r", __func__);
	}
#endif
}

#endif /* DIP_MSG_POST_EN */

#if (DIP_DEBOUNCE_MS > 0u)
static void dip_timer_init(void)
{
	osal_timer_cb_t timer_cb;
#ifdef DIP_MSG_POST_EN
	timer_cb = dip_msg_send;
#else
	timer_cb = dip_setting_imme_effect;
#endif
	OSAL_ASSERT_ERR(osal_timer_create(&tm_debounce, OSAL_TIMER_MODE_SINGLE, timer_cb, NULL,
	                                  DIP_DEBOUNCE_MS),
	                "Create dip timer failed!\r\n");
}
#endif /* (DIP_DEBOUNCE_MS > 0u) */

static void dip_setting_hdl(u8 dip_type)
{
	m_dip_changed |= dip_type;

#if (DIP_DEBOUNCE_MS > 0u)
	OSAL_ASSERT_ERR(osal_timer_start(&tm_debounce), "Start dip timer failed!\r\n");
#else

#ifdef DIP_MSG_POST_EN
#ifdef CFG_OS
	dip_msg_send(NULL);
#else
	dip_msg_send(0U);
#endif /* CFG_OS */
#else
	dip_setting_imme_effect(NULL);
#endif /* DIP_MSG_POST_EN */

#endif /* (DIP_DEBOUNCE_MS > 0u) */
}

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
void dip_setting_dist(void)
{
	u8 dat = 0, dist_lv = 0;

	dat = hal_dip_get_data(DIP_SETTING_SEN_DIST);
	if (dat < DIST_LEVEL_MAX) {
		dist_lv = m_dist_map_tbl[dat];
		radar_mgr_mdet_user_cfg_set(dist_lv);
	}
}

void dip_setting_lot1(void)
{
	u8 dat = 0;
	u32 lot_lv = 0;

	dat = hal_dip_get_data(DIP_SETTING_LIGHT_TIME1);
	if (dat < LOT1_LEVEL_MAX) {
		lot_lv = m_lot1_map_tbl[dat];
#ifdef CFG_APP_LIGHT
		app_light_set_lot(lot_lv * 1000);
#endif
	}
}

#ifdef CFG_LS
void dip_setting_lux(void)
{
	u8 dat = 0;
	u32 lux = 0;

	dat = hal_dip_get_data(DIP_SETTING_LUX);
	if (dat < LUX_LEVEL_MAX) {
		lux = m_lux_map_tbl[dat];
		hal_ls_set_threshold(lux);
	}
}
#endif

void dip_mgr_init(void)
{
	hal_dip_init();

#if (PARA_DIP_IMME_EFFECT == 1U)
	hal_dip_set_hdl(dip_setting_hdl);

#if (DIP_DEBOUNCE_MS > 0u)
	dip_timer_init();
#endif

#ifdef DIP_MSG_POST_EN
	dip_msg_init();
#endif

#endif /* PARA_DIP_IMME_EFFECT */
}

void dip_mgr_update(void)
{
	dip_setting_lot1();
#ifdef CFG_LS
	dip_setting_lux();
#endif
	dip_setting_dist();
}

#endif /* end of CFG_DIP_SWITCH */
