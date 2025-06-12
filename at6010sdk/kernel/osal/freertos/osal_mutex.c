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

#include "osal_mutex.h"
#include "osal_task.h"
#include "FreeRTOS.h"
#include "semphr.h"

osal_status_t osal_mutex_create(osal_mutex_t *mutex)
{
	if (mutex->handle != NULL) {
		OSAL_ERR("handle=0x%x\r\n", (u32)mutex->handle);

		return OSAL_ERROR;
	}

	mutex->handle = xSemaphoreCreateMutex();
	if (mutex->handle == NULL) {
		OSAL_ERR("Mutex Create Failed!\r\n");

		return OSAL_ERROR;
	}

	return OSAL_OK;
}

osal_status_t osal_mutex_delete(osal_mutex_t *mutex)
{
	if (mutex->handle == NULL) {
		OSAL_ERR("Mutex handle NULL!\r\n");

		return OSAL_ERROR;
	}

	vSemaphoreDelete(mutex->handle);
	mutex->handle = NULL;

	return OSAL_OK;
}

osal_status_t osal_mutex_lock(osal_mutex_t *mutex, u32 block_ms)
{
	BaseType_t ret;

	if (mutex->handle == NULL) {
		OSAL_ERR("Mutex handle NULL!\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		BaseType_t taskWoken = pdFALSE;
		ret = xSemaphoreTakeFromISR(mutex->handle, &taskWoken);
		portEND_SWITCHING_ISR(taskWoken);
	} else {
		ret = xSemaphoreTake(mutex->handle, OSAL_MS2TICK(block_ms));
	}

	return (ret != pdPASS) ? OSAL_ERROR : OSAL_OK;
}

osal_status_t osal_mutex_unlock(osal_mutex_t *mutex)
{
	BaseType_t ret;

	if (mutex->handle == NULL) {
		OSAL_ERR("Mutex handle NULL!\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		BaseType_t taskWoken = pdFALSE;
		ret = xSemaphoreGiveFromISR(mutex->handle, &taskWoken);
		portEND_SWITCHING_ISR(taskWoken);
	} else {
		ret = xSemaphoreGive(mutex->handle);
	}

	return (ret != pdPASS) ? OSAL_ERROR : OSAL_OK;
}

osal_status_t osal_recursive_mutex_create(osal_mutex_t *mutex)
{
	if (mutex->handle != NULL) {
		OSAL_ERR("Mutex handle is %x!\r\n", (u32)mutex->handle);

		return OSAL_ERROR;
	}

	mutex->handle = xSemaphoreCreateRecursiveMutex();
	if (mutex->handle == NULL) {
		OSAL_ERR("Create Recursive Mutex failed!\r\n");

		return OSAL_ERROR;
	}

	return OSAL_OK;
}

osal_status_t osal_recursive_mutex_lock(osal_mutex_t *mutex, u32 block_ms)
{
	BaseType_t ret;

	if (mutex->handle == NULL) {
		OSAL_ERR("Mutex handle is NULL\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		OSAL_ERR("recursive Mutex lock in isr!\r\n");

		return OSAL_NOTALLOWED;
	}

	ret = xSemaphoreTakeRecursive(mutex->handle, OSAL_MS2TICK(block_ms));

	return (ret != pdPASS) ? OSAL_ERROR : OSAL_OK;
}

osal_status_t osal_recursive_mutex_unlock(osal_mutex_t *mutex)
{
	BaseType_t ret;

	if (mutex->handle == NULL) {
		OSAL_ERR("Mutex handle is NULL\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		OSAL_ERR("recursive Mutex unlock in isr!\r\n");

		return OSAL_NOTALLOWED;
	}

	ret = xSemaphoreGiveRecursive(mutex->handle);
	if (ret != pdPASS) {
		OSAL_ERR("Unlock Recursive Mutex failed\r\n");

		return OSAL_ERROR;
	}

	return OSAL_OK;
}
