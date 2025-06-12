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

#include "osal_task.h"
#include "at_error.h"
#ifdef CFG_MOD_TEST
#include "mod_test.h"
#endif

extern void app_task_entry(void * arg);
extern void sys_task_entry(void * arg);
extern void radar_task_entry(void * arg);
extern void wcn_task_entry(void * arg);

u8 afw_task_init(void)
{
	u8 ret = E_OK;
	osal_task_t *p_task = NULL;

	/**
	 * create internal tasks.
	 */
	do {
		/* create app task. */
		p_task = osal_task_create(MOD_APP, "app_task", app_task_entry, NULL, OSAL_PRIORITY_NORMAL,
							(1024 + 512), OASL_CTF_MSG_QUEUE|OASL_CTF_MSG_LEN_MUL(1));
		if (p_task == NULL) {
			ret = MOD_APP + 1;
			break;
		}

		/* create system task. */
		p_task = osal_task_create(MOD_SYS, "sys_task", sys_task_entry, NULL, OSAL_PRIORITY_NORMAL,
							2048, OASL_CTF_MSG_QUEUE|OASL_CTF_MSG_LEN_MUL(1));
		/* CLI show sys_task task-stats Stack 249*/
		if (p_task == NULL) {
			ret = MOD_SYS + 1;
			break;
		}

		/* create radar task. */
		p_task = osal_task_create(MOD_RAD, "radar_task", radar_task_entry, NULL, OSAL_PRIORITY_ABOVE_NORMAL,
							2048, OASL_CTF_MSG_QUEUE|OASL_CTF_MSG_LEN_MUL(1));
		if (p_task == NULL) {
			ret = MOD_RAD + 1;
			break;
		}

#if (defined(CFG_ATMESH) || defined(CFG_SIGMESH) || defined(CFG_BT))
		/* create wireless task. */
		p_task = osal_task_create(MOD_WCN, "wcn_task", wcn_task_entry, NULL, OSAL_PRIORITY_ABOVE_NORMAL,
							2048, OASL_CTF_MSG_QUEUE|OASL_CTF_MSG_LEN_MUL(2));
		if (p_task == NULL) {
			ret = MOD_WCN + 1;
			break;
		}
#endif

#ifdef CFG_MOD_TEST
		p_task = osal_task_create(MOD_USER_AUTO, "test_task", test_task_entry, NULL,
		                          OSAL_PRIORITY_NORMAL, 1024,
		                          OASL_CTF_MSG_QUEUE | OASL_CTF_MSG_LEN_MUL(1));
		if (p_task == NULL) {
			ret = MOD_USER_AUTO + 1;
			break;
		}
#endif /* CFG_MOD_TEST */
	} while (0);

	if (ret != E_OK) {
		printf("create task %d failed.\r\n", ret);
	}

	return ret;
}
