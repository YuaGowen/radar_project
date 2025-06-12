/**
 * Copyright (c) 2024, Airtouching Intelligence Technology.
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

#include "adc_key.h"
#include "radar_mgr.h"
#include "hal_key.h"
#include "hal_ls.h"
#include "atlog.h"

#ifdef CFG_OS
#include "osal_task.h"
#include "sys_task.h"
#endif

#ifdef CFG_APP_LIGHT
#include "app_light.h"
#endif

#ifdef CFG_ADC_KEY

#define KEY_FUNC_NUM					(sizeof(m_key_func_map)/sizeof(key_func_map_t))
#define KEY_DIST_LVL_MAX				((u8)RAD_DIST_LV_MAX - 1u)
#define KEY_LOT_LVL_MAX					((sizeof(m_lot_map_tbl)/sizeof(m_lot_map_tbl[0])) - 1u)
#define KEY_LUX_LVL_MAX					((sizeof(m_lux_map_tbl)/sizeof(m_lux_map_tbl[0])) - 1u)

#define DGB_ADC_KEY_LOG_EN

typedef void (*key_func_hdl_t)(void);

typedef struct {
	u8 key_id;
	enum_key_func_t key_func[KEY_EVENT_NUM];
} key_func_map_t;


/**
 * ADC Key Functions List
 */
static void adc_key_dist_increase(void);
static void adc_key_dist_decrease(void);
static void adc_key_lot_increase(void);
static void adc_key_lot_decrease(void);
#ifdef CFG_LS
static void adc_key_lux_increase(void);
static void adc_key_lux_decrease(void);
#endif

/* Mapping functions enum and implemented funcions */
static const key_func_hdl_t m_key_func_list[KEY_FUNC_MAX] = {
	NULL,
	adc_key_dist_increase,
	adc_key_dist_decrease,
	adc_key_lot_increase,
	adc_key_lot_decrease,
#ifdef CFG_LS
	adc_key_lux_increase,
	adc_key_lux_decrease,
#else
	NULL,
	NULL,
#endif
};

static const key_func_map_t m_key_func_map[] = PARA_ADC_KEY_FUNC_MAP;
static const u16 m_lot_map_tbl[] = PARA_ADC_KEY_LOT_LV;
#ifdef CFG_LS
static const u16 m_lux_map_tbl[] = PARA_ADC_KEY_LUX_LV;
#endif

static u8 m_dist_lvl = 0u;
static u8 m_lot_lvl = 0u;
#ifdef CFG_LS
static u8 m_lux_lvl = 0u;
#endif


/**
 * ADC Key Functions Implementation
 */
static void adc_key_dist_increase(void)
{
	m_dist_lvl = (m_dist_lvl < KEY_DIST_LVL_MAX) ? (m_dist_lvl + 1u) : m_dist_lvl;
	radar_mgr_mdet_user_cfg_set(m_dist_lvl);
#ifdef DGB_ADC_KEY_LOG_EN
	logi("set dist %d\n\r", m_dist_lvl);
#endif
}

static void adc_key_dist_decrease(void)
{
	m_dist_lvl = (m_dist_lvl > 0u) ? (m_dist_lvl - 1u) : m_dist_lvl;
	radar_mgr_mdet_user_cfg_set(m_dist_lvl);
#ifdef DGB_ADC_KEY_LOG_EN
	logi("set dist %d\n\r", m_dist_lvl);
#endif
}

static void adc_key_lot_increase(void)
{
	u32 lot;
	m_lot_lvl = (m_lot_lvl < KEY_LOT_LVL_MAX) ? (m_lot_lvl + 1u) : m_lot_lvl;
	lot = (u32)m_lot_map_tbl[m_lot_lvl] * 1000u;

#ifdef CFG_APP_LIGHT
	app_light_set_lot(lot);
#endif
#ifdef DGB_ADC_KEY_LOG_EN
	logi("set lot %d\n\r", lot);
#endif
}

static void adc_key_lot_decrease(void)
{
	u32 lot;
	m_lot_lvl = (m_lot_lvl > 0u) ? (m_lot_lvl - 1u) : m_lot_lvl;
	lot = (u32)m_lot_map_tbl[m_lot_lvl] * 1000u;

#ifdef CFG_APP_LIGHT
	app_light_set_lot(lot);
#endif
#ifdef DGB_ADC_KEY_LOG_EN
	logi("set lot %d\n\r", lot);
#endif
}

#ifdef CFG_LS
static void adc_key_lux_increase(void)
{
	u16 lux;
	m_lux_lvl = (m_lux_lvl < KEY_LUX_LVL_MAX) ? (m_lux_lvl + 1u) : m_lux_lvl;
	lux = m_lux_map_tbl[m_lux_lvl];
	hal_ls_set_threshold(lux);
#ifdef DGB_ADC_KEY_LOG_EN
	logi("set lux %d\n\r", lux);
#endif
}

static void adc_key_lux_decrease(void)
{
	u16 lux;
	m_lux_lvl = (m_lux_lvl > 0u) ? (m_lux_lvl - 1u) : m_lux_lvl;
	lux = m_lux_map_tbl[m_lux_lvl];
	hal_ls_set_threshold(lux);
#ifdef DGB_ADC_KEY_LOG_EN
	logi("set lux %d\n\r", lux);
#endif
}
#endif /* CFG_LS */

static void key_func_proc(u8 key_id, u8 event)
{
	key_func_hdl_t hdl = NULL;

	for (u8 i = 0u; i < KEY_FUNC_NUM; i++) {
		if (key_id == m_key_func_map[i].key_id) {
			enum_key_func_t func_id = m_key_func_map[i].key_func[event];
			hdl = m_key_func_list[func_id];
			break;
		}
	}

	if (hdl != NULL) {
		hdl();
	}
}

#ifdef CFG_OS
static void key_msg_sender(u8 key_id, u8 event)
{
	osal_msg_t msg;
	msg.id = (u32)MSG_SYS_ADC_KEY_IDLE + key_id;
	msg.para = event;
	if (OSAL_OK != osal_task_send_msg(MOD_SYS, &msg, OSAL_WAIT_FOREVER)) {
		loge("adc key send msg failed!\r\n");
	}
}

static bool adc_key_msg_handler(const osal_msg_t *msg)
{
	bool ret = FALSE;

	if ((u32)MSG_SYS_GRP_ADC_KEY == (msg->id & (u32)MSG_SYS_GRP_ADC_KEY)) {
		key_func_proc((u8)msg->id, (u8)msg->para);
		ret = TRUE;
	}
	return ret;
}
#else
static void key_hdl(u8 key_id, u8 event)
{
	key_func_proc(key_id, event);
}
#endif /* #ifdef CFG_OS */

void adc_key_func_reg(void)
{
#if defined(CFG_OS) && defined(KEY_EVENT_CALLBACK_EN)
	adc_key_reg_cb_hdl(key_msg_sender);
	sys_task_reg_msg(MSG_SYS_GRP_ADC_KEY, adc_key_msg_handler);
#elif !defined(CFG_OS) && defined(KEY_EVENT_MSG_POST_EN)
	adc_key_reg_msg_hdl(key_hdl);
#elif !defined(CFG_OS) && defined(KEY_EVENT_CALLBACK_EN)
	adc_key_reg_cb_hdl(key_hdl);
#else
#error "Invalid adc key definition!"
#endif
}

#endif /* CFG_ADC_KEY */
