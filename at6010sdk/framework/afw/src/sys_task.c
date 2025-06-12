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

#include "sys_task.h"
#include "atlog.h"

#if (defined(CFG_CI) || defined(CFG_DIP_SWITCH) || defined(CFG_ADC_KEY) || \
     defined(CFG_AUX_ADC_KEY) || defined(CFG_IR))
#define HAS_SYS_TASK 1
#else
#define HAS_SYS_TASK 0
#endif

typedef enum {
#ifdef CFG_CI
	SYS_MOD_CI,
#endif
#ifdef CFG_DIP_SWITCH
	SYS_MOD_DIP_SWITCH,
#endif
#ifdef CFG_ADC_KEY
	SYS_MOD_ADC_KEY,
#endif
#ifdef CFG_AUX_ADC_KEY
	SYS_MOD_AUX_ADC_KEY,
#endif
#ifdef CFG_IR
	SYS_MOD_IR,
#endif
	SYS_MOD_QTY
} sys_mod_t;

#if HAS_SYS_TASK
static msg_handler_t sys_msg_handler[SYS_MOD_QTY];

__STATIC_INLINE bool process_msg_event(const osal_msg_t *msg)
{
	bool ret = FALSE;
	msg_handler_t hdl;

	/* NOTE: use sequential index here to keep priority */
	for (u8 i = 0U; i < (u8)SYS_MOD_QTY; i++) {
		hdl = sys_msg_handler[i];
		if (NULL != hdl) {
			ret = hdl(msg);
		}
		if (TRUE == ret) {
			break;
		}
	}
	return ret;
}

void sys_task_reg_msg(sys_msg_grp_t group, msg_handler_t hdl)
{
	sys_mod_t mod;

	switch (group) {
#ifdef CFG_CI
	case MSG_SYS_GRP_CI:
		mod = SYS_MOD_CI;
		break;
#endif
#ifdef CFG_DIP_SWITCH
	case MSG_SYS_GRP_DIP_SWITCH:
		mod = SYS_MOD_DIP_SWITCH;
		break;
#endif
#ifdef CFG_ADC_KEY
	case MSG_SYS_GRP_ADC_KEY:
		mod = SYS_MOD_ADC_KEY;
		break;
#endif
#ifdef CFG_AUX_ADC_KEY
	case MSG_SYS_GRP_AUX_ADC_KEY:
		mod = SYS_MOD_AUX_ADC_KEY;
		break;
#endif
#ifdef CFG_IR
	case MSG_SYS_GRP_IR:
		mod = SYS_MOD_IR;
		break;
#endif
	default:
		mod = SYS_MOD_QTY;
		break;
	}

	if (mod < SYS_MOD_QTY) {
		sys_msg_handler[mod] = hdl;
	}
}
#endif /* HAS_SYS_TASK */

void sys_task_entry(void *arg)
{
#if HAS_SYS_TASK
	osal_msg_t msg;
	osal_status_t status;

	UNUSED(arg);

	logi("sys_task is running!\r\n");
	for (;;) {
		status = osal_task_wait_msg(MOD_SYS, &msg, OSAL_WAIT_FOREVER);
		if (status != OSAL_OK) {
			continue;
		}

		if (FALSE == process_msg_event(&msg)) {
			logw("ignore sys msg, id=%d\r\n", msg.id);
		}
	}
#else
	logi("sys_task has no module, deleting task...\r\n");
	(void)osal_task_delete(MOD_SYS);
#endif /* HAS_SYS_TASK */
}
