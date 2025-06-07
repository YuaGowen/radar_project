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

#include "k_msg.h"
#include "mem_heap.h"
#include "osal_task.h"
#include "at_assert.h"

#define MSG_POOL_ELEM_TOTAL                  30U

static osal_task_func_t m_task_idle_hook = NULL;

osal_task_t g_task_lists[MOD_ID_MAX] = { 0 };

static wrap_msg_t g_msg_pool[MSG_POOL_ELEM_TOTAL] = { 0 };

static osal_mod_id_t get_auto_user_id(void)
{
	u8 i = 0;

	for (i = MOD_USER_START; i < MOD_USER_END; i++) {
		if (g_task_lists[i].task_hdl == NULL) {
			break;
		}
	}

	if (i == MOD_USER_END) {
		i = MOD_ID_MAX;
	}

	return (osal_mod_id_t)i;
}

osal_task_t * osal_task_create(osal_mod_id_t id, const char *name,
						osal_task_func_t task_func, void *arg,
						osal_priority_t priority, u16 stk_size, u16 nflag)
{
	osal_task_t *pTask = NULL;

	assert(id < MOD_ID_MAX);

	/* for user-defined task id. */
	if (id == MOD_USER_AUTO) {
		id = get_auto_user_id();
		if (id == MOD_ID_MAX) {
			OSAL_ERR("Task User AUTO Failed!\r\n");

			return NULL;
		}
	}

	pTask = &(g_task_lists[id]);
	if (pTask->task_hdl != NULL) {
		OSAL_ERR("Task Error handle=0x%x\r\n", (u32)pTask->task_hdl);

		return NULL;
	}

	return pTask;
}

osal_status_t osal_task_delete(osal_mod_id_t id)
{
	osal_task_t *pTask = NULL;

	assert(id < MOD_ID_MAX);

	pTask = &(g_task_lists[id]);

	if (pTask->task_hdl == NULL) {
		OSAL_ERR("Task handle is NULL\r\n");

		return OSAL_ERROR;
	}

	return OSAL_OK;
}

osal_status_t osal_task_suspend(osal_mod_id_t id)
{
	osal_task_t *pTask = NULL;

	assert(id < MOD_ID_MAX);

	pTask = &(g_task_lists[id]);

	if (pTask->task_hdl == NULL) {
		OSAL_ERR("Task handle is NULL\r\n");

		return OSAL_ERROR;
	}


	return OSAL_OK;
}

osal_status_t osal_task_resume(osal_mod_id_t id)
{
	osal_task_t *pTask = NULL;

	assert(id < MOD_ID_MAX);

	pTask = &(g_task_lists[id]);

	if (pTask->task_hdl == NULL) {
		OSAL_ERR("Task handle is NULL\r\n");

		return OSAL_ERROR;
	}

	return OSAL_OK;
}

osal_status_t osal_task_send_msg(osal_mod_id_t id, const osal_msg_t *msg, u32 ms)
{
	assert(id < MOD_ID_MAX);

	u32 i = 0;
	for (; i < MSG_POOL_ELEM_TOTAL; i++) {
		if (g_msg_pool[i].bUsed == 0) {
			g_msg_pool[i].bUsed = 1;
			g_msg_pool[i].os_msg = *msg;
			g_msg_pool[i].noos_msg.para = (void *)&g_msg_pool[i].os_msg;
			k_msg_post((msg_id_t)id, &g_msg_pool[i].noos_msg);
			return OSAL_OK;
		}
	}

	OSAL_ERR("No memory for msg\r\n");
	return OSAL_NOMEM;
}

/**
 * @brief Invalid a msg object for reuse (Just in NOOS version)
 *
 * @param msg Actually it is a type of <b>msg_t *</b>
 * @return osal_status_t
 */
osal_status_t osal_task_invalid_msg(void *msg)
{
	if (NULL == msg) {
		return OSAL_INVALID;
	}

	wrap_msg_t *wrap_msg = (wrap_msg_t *) container_of(msg, wrap_msg_t, noos_msg);
	wrap_msg->bUsed = 0;

	return OSAL_OK;
}

osal_status_t osal_task_wait_msg(osal_mod_id_t id, osal_msg_t *msg, u32 ms)
{
	osal_task_t *pTask = NULL;

	assert(id < MOD_ID_MAX);

	pTask = &(g_task_lists[id]);

	if (pTask->queue_hdl == NULL) {
		OSAL_ERR("Queue handle is NULL\r\n");

		return OSAL_ERROR;
	}

	return OSAL_OK;
}

/**
 * @brief Start the task scheduler running.
 */
void osal_task_start_scheduler(void)
{

}

/**
 * @brief Suspend the task scheduler
 */
void osal_task_suspend_all(void)
{

}

/**
 * @brief Resume the thread scheduler
 */
void osal_task_resume_all(void)
{

}

/**
 * @brief Get the handle of the current running task
 */
osal_task_hdl_t osal_task_get_cur_hdl(void)
{
	return NULL;
}

/**
 * @brief Check whether the task scheduler is running or not
 * @return 1 on runing, 0 on not running
 */
int osal_task_is_running(void)
{
	return 1;
}

osal_status_t osal_task_give_notify(osal_mod_id_t id)
{
	osal_task_t *pTask = NULL;

	assert(id < MOD_ID_MAX);


	return OSAL_OK;
}

osal_status_t osal_task_wait_notify(u32 ms)
{
	return OSAL_OK;
}

void osal_task_set_idle_hook(osal_task_func_t func)
{
	m_task_idle_hook = func;
}

void vApplicationIdleHook(void)
{
	if (m_task_idle_hook != NULL) {
		m_task_idle_hook(NULL);
	}
}

osal_task_hdl_t osal_task_get_handle_by_id(osal_mod_id_t id)
{
	osal_task_hdl_t task_hdl = NULL;

	for (u8 i = 0; i < MOD_ID_MAX; i++) {
		if (id == g_task_lists[i].id) {
			task_hdl =  g_task_lists[i].task_hdl;
			break;
		}
	}

	return task_hdl;
}
