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

#include "switch_tm.h"
#include "sys_timer.h"
#include "user_setting.h"
#include "atlog.h"
#include "osal_timer.h"

#ifdef CFG_SWITCH_TM

#ifdef CFG_OS
static osal_timer_t m_tim_clr_nv;
#else
static sys_timer_t *m_tim_clr_nv;
#endif

#ifdef DBG_LOG_SWITCH_TM
#define dbg_tm_log        logi
#else
#define dbg_tm_log(...)
#endif

static void switch_tm_clr_nv_to_hdl(void *arg)
{
	UNUSED(arg);
	dbg_tm_log("switch_tm_clr_nv_to_hdl\r\n");

	(void)tm_clr_powerup_time();

#ifndef CFG_OS
	stop_timer(m_tim_clr_nv);
	del_timer(m_tim_clr_nv);
#else
	(void)osal_timer_stop(&m_tim_clr_nv);
	(void)osal_timer_delete(&m_tim_clr_nv);
#endif
}

u8 tm_get_switch_times(void)
{
	u8 num = 0;
	int len = 0;

	if (USER_SETTING_STATUS_SUCCESS != user_setting_get("pu_num", (char *)&num, &len)) {
		num = 1;
		(void)user_setting_set("pu_num", (char *)&num, 4);
		dbg_tm_log("%s, set pwr num: %d\r\n", __func__, num);
	} else {
		num++;
		(void)user_setting_set("pu_num", (char *)&num, 4);
		dbg_tm_log("%s, get pwr num: %d\r\n", __func__, num);
	}

	(void)user_setting_store();

	return num;
}

u8 tm_clr_powerup_time(void)
{
	int num = 0;

	(void)user_setting_set("pu_num",(char*)&num, 4);
	(void)user_setting_store();
	dbg_tm_log("%s, clr pwr num: %d\r\n", __func__, num);

	return 0;
}

u8 switch_tm_proc(u32 clr_to)
{
	u8 tm_cnt;

	logi("%s\r\n", __func__);

	/* start a timer to clear nv data when the time expires. */
#ifndef CFG_OS
	m_tim_clr_nv = create_timer("tm-clr", TIMER_OPT_ONE_SHOT, switch_tm_clr_nv_to_hdl, 0);
	start_timer(m_tim_clr_nv, clr_to);
#else
	OSAL_ASSERT_ERR(osal_timer_create(&m_tim_clr_nv, OSAL_TIMER_MODE_SINGLE,
	                                  switch_tm_clr_nv_to_hdl, NULL, clr_to),
	                "Create tm_clr_nv timer failed!\r\n");
	OSAL_ASSERT_ERR(osal_timer_start(&m_tim_clr_nv), "Start tm_clr_nv timer failed!\r\n");
#endif
	(void)user_setting_init();

	/* get stored data from nv. */
	tm_cnt = tm_get_switch_times();

	return tm_cnt;
}

#endif /* end of CFG_SWITCH_TM */

