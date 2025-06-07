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

#include "osal.h"
#include "if_sample.h"
#include "radar_mgr.h"
#include "atlog.h"
#include "k_msg.h"
#include "mem_heap.h"

extern bool mdet_msg_handler(osal_msg_t *msg);
extern bool radar_mgr_msg_handler(osal_msg_t *msg);

void radar_task_entry(void *arg)
{
	osal_msg_t msg;
	osal_status_t status;

	radar_mgr_init();

	logi("radar_task is running!\r\n");

	while (1) {
		status = osal_task_wait_msg(MOD_RAD, &msg, OSAL_WAIT_FOREVER);
		if (status != OSAL_OK) {
			continue;
		}

		/* process msg event. */
		if (mdet_msg_handler(&msg)) {

		} else if(radar_mgr_msg_handler(&msg)) {

		} else{ /* default msg. */
			logw("ignore msg, id=%d\n", msg.id);
		}
	}
}

/**
 * @brief Radar task message handler of NOOS version.
 *
 * @param noos_msg This type <b>mst_t *</b> is a wrapper of <b>osal_msg_t *</b>.
 */
void radar_task_process_msg(msg_t *noos_msg)
{
	osal_msg_t *msg = (osal_msg_t *)noos_msg->para;

	/* process msg event. */
	if (mdet_msg_handler(msg)) {

	} else if(radar_mgr_msg_handler(msg)) {

	} else{ /* default msg. */
		logw("ignore msg, id=%d\n", msg->id);
	}
}
