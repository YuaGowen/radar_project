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

#include "cmsis_compiler.h"
#include "atlog.h"
#include "cpu.h"
#include "k_msg.h"
#include "mem_heap.h"
#include "osal_task.h"

/** message queue define */
typedef struct
{
	struct list_head list_head;
	void (*hdl)(msg_t *);
} msg_queue_t;

/** message queue init by MSG_ID */
static msg_queue_t m_msg_queue[MSG_ID_MAX] = {0};

/** Initialize a message queue management structure */
#define INIT_MSG(q)                    \
    do                                      \
    {                                       \
        INIT_LIST_HEAD(&((q)->list_head));  \
        (q)->hdl = NULL;                 \
    } while(0)


static bool check_msg_existed(const struct list_head *entry, const struct list_head *head)
{
	bool ret = 0U;
	const struct list_head *pos;
	list_for_each_prev(pos, head) {
		if (pos == entry) {
			ret = 1U;
			break;
		}
	}
	return ret;
}

/** message queue list init by MSG_ID */
void k_msg_init(void)
{
	for (u32 i = 0; i < MSG_ID_MAX; i++) {
		INIT_MSG(&m_msg_queue[i]);
	}

/*----------------------------------------------------------------------------*/
#if 0

	INIT_MSG(&m_msg_queue[MSG_ID_TIMER_EXPIRED]);

#ifdef CFG_CI
	INIT_MSG(&m_msg_queue[MSG_ID_CI]);
#endif

#ifdef CFG_DIP_SWITCH
	INIT_MSG(&m_msg_queue[MSG_ID_DIP]);
#endif

#ifdef CFG_ADC_KEY
	INIT_MSG(&m_msg_queue[MSG_ID_ADC_KEY]);
#endif

#endif /* 0 */
/*----------------------------------------------------------------------------*/
}

/**
 *
 * k_msg_reg_hdl
 *
 * This function bind msg_id to a message handler.
 *
 * @msg_id         Message ID number
 * @handler        Message handler
 *
 * return   0 on sucess, -1 on wrong message id or bad message
 *
 */
int k_msg_reg_hdl(msg_id_t msg_id, void (*hdl)(msg_t *))
{
	if (hdl == NULL || msg_id >= MSG_ID_MAX) {
		loge("Register bad msg(%u)\r\n", msg_id);
		return -1;
	}

	logv("Register msg(%u) hdl(%p)\r\n", msg_id, hdl);

	critical_section_start();
	// INIT_MSG(&m_msg_queue[msg_id]); /* NOTE: no need to re-init list again. */
	m_msg_queue[msg_id].hdl  = hdl;
	critical_section_end();

	return 0;
}

/**
 *
 * k_msg_post
 *
 *  Send a message
 *
 * @msg_id: Message ID number
 * @msg: Message
 *
 * return 0: sucess, -1: wrong message id or bad message
 *
 */
int k_msg_post(msg_id_t msg_id, msg_t *msg)
{
	if (msg == NULL) {
		loge("Post msg(%u) NULL\n", msg_id);
		return (-1);
	}

	if (msg_id >= MSG_ID_MAX) {
		loge("Post bad msg(%u)\r\n", msg_id);
		return (-1);
	}

	critical_section_start();
	if (0U == check_msg_existed(&msg->entry, &m_msg_queue[msg_id].list_head)) {
		list_add_tail(&msg->entry, &m_msg_queue[msg_id].list_head);
	}
	critical_section_end();

	/*logv("Post msg(%u)\r\n", msg_id);*/

	return 0;
}

/**
 *
 * k_msg_loop
 *
 * This function is message main loop, run in main.c loop, it is always
 * check message queue and call message handler
 *
 */
void k_msg_loop(void)
{
	int msg_id;

	for (msg_id = MSG_ID_TIMER_EXPIRED; msg_id < MSG_ID_MAX; msg_id++) {
		msg_t *msg = NULL;

		do {
			critical_section_start();
			if (!list_empty(&m_msg_queue[msg_id].list_head)) {
				msg = list_first_entry(&m_msg_queue[msg_id].list_head, msg_t, entry);
				list_del(&msg->entry);
			}
			critical_section_end();

			if (msg != NULL) {
				/*logv("Handle msg(%u)\r\n", msg_id);*/
				if (m_msg_queue[msg_id].hdl != NULL) {
					(*(m_msg_queue[msg_id].hdl))(msg);
					osal_task_invalid_msg(msg);
				} else {
					loge("NULL handler for msg(%u)!\r\n", msg_id);
				}
			}
		} while (!list_empty(&m_msg_queue[msg_id].list_head));
	}
}
