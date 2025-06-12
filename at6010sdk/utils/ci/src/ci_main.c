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

#include "ci_main.h"
#include "ci_port.h"
#include "ci_cmd.h"
#ifdef CFG_OS
#include "sys_task.h"
#else
#include "k_msg.h"
#endif

#if defined(ULP_PROG_1)
#include "ci_ulp.h"
#endif

#ifdef CFG_CI

static void ci_rx_buf_proc(u8 *buf);
#ifdef CFG_OS
static bool ci_main_msg_handler(const osal_msg_t *msg);
#else
static void ci_main_proc_msg(msg_t *msg);
#endif

void ci_main_init(void)
{
	ci_port_init();
#ifdef CFG_OS
	sys_task_reg_msg(MSG_SYS_GRP_CI, ci_main_msg_handler);
#else
	k_msg_reg_hdl(MSG_ID_CI, ci_main_proc_msg);
#endif
}

void ci_main_proc(void)
{
	if (ci_port_data_is_ready() == 1U) {
		ci_rx_buf_proc(ci_port_get_buf());
	}
}

#ifdef CFG_OS
static bool ci_main_msg_handler(const osal_msg_t *msg)
{
	bool ret = FALSE;

	if ((u32)MSG_SYS_CI_FRAME_RECEIVED == msg->id) {
		ci_rx_buf_proc(((ci_port_buf_t *)msg->para)->buf);
		ret = TRUE;
	}
	return ret;
}
#else
static void ci_main_proc_msg(msg_t *msg)
{
	ci_rx_buf_proc(((ci_port_buf_t *)msg->para)->buf);
}
#endif /* #ifdef CFG_OS */

static void ci_rx_buf_proc(u8 *buf)
{
	u8 err;
	ci_ctrl_frm_t ci_frm;

	err = ci_frm_parser(&ci_frm, buf);
	if (err == 0U) {

#if defined(ULP_PROG_1)
		/* update ulp flash timer */
		ulp_prog1_refresh_active_timer(PARA_ULP_PROG_1_ACTIVE_TIME, REFRESH_MODE_RELOAD);
#endif

		if (ci_frm.head == CI_CTRL_FRM_HEAD) {
			(void)ci_cmd_proc(&ci_frm);
		} else if (ci_frm.head == CI_RESP_FRM_HEAD) {
			(void)ci_resp_proc(&ci_frm);
		} else {
			/* Invalid head */
		}
	}
	ci_port_reset();
}

#endif /* !CFG_CI */
