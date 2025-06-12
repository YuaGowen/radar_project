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

#include "self_check.h"
#include "osal.h"
#include "hal_light.h"
#include "atlog.h"
#ifdef CFG_APP_LIGHT
#include "app_light.h"
#endif


#ifdef CFG_PWRON_SELF_CHECK

//#define DBG_LOG_SC

#ifdef DBG_LOG_SC
#define dbg_sc_log        logi
#else
#define dbg_sc_log(...)
#endif

typedef struct {
	u8 loop_cnt;
	sc_status_t status;
	u32 lot_time;
	osal_timer_t tm;
} self_check_env_t;

static self_check_env_t m_sc_env = {0};

static void sc_lon_timeout_hdl(void *arg);

static void sc_loff_timeout_hdl(void *arg)
{
	(void)arg;

	m_sc_env.loop_cnt--;
	dbg_sc_log("%s, residue loop_cnt: %d\r\n", __func__, m_sc_env.loop_cnt);

	if (m_sc_env.loop_cnt > 0U) {
		hal_light_set(LIGHT_PRIMARY, 1);
		osal_timer_change_callback(&m_sc_env.tm, sc_lon_timeout_hdl);
		(void)osal_timer_change_period(&m_sc_env.tm, m_sc_env.lot_time);
		(void)osal_timer_start(&m_sc_env.tm);
	} else {
		/* free soft timer. */
		self_check_exit();
	}
}

static void sc_lon_timeout_hdl(void *arg)
{
	(void)arg;

	dbg_sc_log("%s\r\n", __func__);

	hal_light_set(LIGHT_PRIMARY, 0);

	osal_timer_change_callback(&m_sc_env.tm, sc_loff_timeout_hdl);
	(void)osal_timer_change_period(&m_sc_env.tm, (m_sc_env.lot_time >> 1));
	(void)osal_timer_start(&m_sc_env.tm);
}

sc_status_t self_check_get_status(void)
{
	return m_sc_env.status;
}

/**
 * @brief Power on self check:
 *        light on x seconds -> light off x/2 seconds -> loop c times -> normal status.
 * @param loop_count loop times
 * @param lot_time light on seconds
 */
void self_check_entry(u8 loop_count, u32 lot_time)
{
	dbg_sc_log("%s\r\n", __func__);

	if (loop_count == 0)
		return;

	hal_light_set(LIGHT_PRIMARY, 1);

	m_sc_env.loop_cnt = loop_count;
	m_sc_env.lot_time = lot_time;

	OSAL_ASSERT_ERR(osal_timer_create(&m_sc_env.tm, OSAL_TIMER_MODE_SINGLE, sc_lon_timeout_hdl,
	                                  NULL, m_sc_env.lot_time),
	                "Create timer tm-sc failed!\r\n");
	OSAL_ASSERT_ERR(osal_timer_start(&m_sc_env.tm), "Start timer tm-sc failed!\r\n");
	m_sc_env.status = SC_STATUS_RUNNING;
}

void self_check_exit(void)
{
	(void)osal_timer_delete(&m_sc_env.tm);
	m_sc_env.status = SC_STATUS_STOP;
}

#endif

