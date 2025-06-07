/**
* Copyright (c) 2023, Airtouching Intelligence Technology.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form, except as embedded into a Airtouching
* Intelligence Technology integrated circuit in a product or a software update for
* such product, must reproduce the above copyright notice, this list of
* conditions and the following disclaimer in the documentation and/or other
* materials provided with the distribution.
*
* 3. Any software provided in binary form under this license must not be reverse
* engineered, decompiled, modified and/or disassembled.
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

#ifndef __OSAL_TASK_H__
#define __OSAL_TASK_H__

#include "osal_common.h"
#include "k_msg.h"

typedef struct osal_task {
	osal_mod_id_t id;
	osal_task_hdl_t task_hdl;
	osal_queue_hdl_t queue_hdl;
} osal_task_t;

typedef struct _wrap_msg_t {
	u8         bUsed;
	osal_msg_t os_msg;   /**< OS version   */
	msg_t      noos_msg; /**< NOOS version */
} wrap_msg_t;

typedef bool (*msg_handler_t)(const osal_msg_t *msg);

extern osal_task_t g_task_lists[MOD_ID_MAX];

/**
 * @brief Get task context by mod id.
 */
__STATIC_INLINE osal_task_t * osal_mod_to_task(osal_mod_id_t id)
{
	osal_task_t *task = NULL;

	if (id < MOD_ID_MAX) {
		task = &(g_task_lists[id]);
	}

	return task;
}

/**
 * @brief Create and start a task
 *
 * @param id Task module id
 * @param name A name for the task.
 * @param task_func Function pointer of the task
 * @param arg The argument passed to taskFunc
 * @param priority The priority of the task
 * @param stk_size The number of bytes the task stack can hold
 * @param nflag
 * @return osal_task_t* Task handler pointer
 */
osal_task_t * osal_task_create(osal_mod_id_t id, const char *name,
						osal_task_func_t task_func, void *arg,
						osal_priority_t priority, u16 stk_size, u16 nflag);

osal_status_t osal_task_delete(osal_mod_id_t id);

osal_status_t osal_task_suspend(osal_mod_id_t id);

osal_status_t osal_task_resume(osal_mod_id_t id);

osal_status_t osal_task_send_msg(osal_mod_id_t id, const osal_msg_t *msg, u32 ms);

/**
 * @brief Invalid a msg object for reuse (Just in NOOS version)
 *
 * @param msg Actually it is a type of <b>msg_t *</b>
 * @return osal_status_t
 */
osal_status_t osal_task_invalid_msg(void *msg);

osal_status_t osal_task_wait_msg(osal_mod_id_t id, osal_msg_t *msg, u32 ms);

/**
 * @brief Start the task scheduler running.
 */
void osal_task_start_scheduler(void);

/**
 * @brief Suspend the task scheduler
 */
void osal_task_suspend_all(void);

/**
 * @brief Resume the thread scheduler
 */
void osal_task_resume_all(void);

/**
 * @brief Get the handle of the current running task
 */
osal_task_hdl_t osal_task_get_cur_hdl(void);

/**
 * @brief Check whether the task scheduler is running or not
 * @return 1 on runing, 0 on not running
 */
int osal_task_is_running(void);

osal_status_t osal_task_give_notify(osal_mod_id_t id);

osal_status_t osal_task_wait_notify(u32 ms);

void osal_task_set_idle_hook(osal_task_func_t func);

osal_task_hdl_t osal_task_get_handle_by_id(osal_mod_id_t id);


#endif /* __OSAL_TASK_H__ */
