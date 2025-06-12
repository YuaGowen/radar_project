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

#include "osal_common.h"
#include "osal_sem.h"
#include "FreeRTOS.h"
#include "semphr.h"

osal_status_t osal_sem_create(osal_sem_t *sem, u32 init_cnt, u32 max_cnt)
{
	sem->handle = xSemaphoreCreateCounting(max_cnt, init_cnt);
	if (sem->handle == NULL) {
		OSAL_ERR("Semaphore create failed\r\n");

		return OSAL_ERROR;
	}

	return OSAL_OK;
}

osal_status_t osal_sem_create_binary(osal_sem_t *sem)
{
	sem->handle = xSemaphoreCreateBinary();
	if (sem->handle == NULL) {
		OSAL_ERR("Binary Semaphore create failed\r\n");

		return OSAL_ERROR;
	}

	return OSAL_OK;
}

osal_status_t osal_sem_delete(osal_sem_t *sem)
{
	if (sem->handle == NULL) {
		OSAL_ERR("semaphore handle is NULL\r\n");

		return OSAL_ERROR;
	}

	vSemaphoreDelete(sem->handle);
	sem->handle = NULL;

	return OSAL_OK;
}

osal_status_t osal_sem_wait(osal_sem_t *sem, u32 ms)
{
	BaseType_t ret;

	if (sem->handle == NULL) {
		OSAL_ERR("semaphore handle is NULL\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		BaseType_t taskWoken;
		if (ms != 0) {
			OSAL_ERR("semaphore not allowed!\r\n");

			return OSAL_NOTALLOWED;
		}

		taskWoken = pdFALSE;
		ret = xSemaphoreTakeFromISR(sem->handle, &taskWoken);
		if (ret != pdPASS) {
			OSAL_ERR("wait semaphore from ISR failed\r\n");

			return OSAL_ERROR;
		}

		portEND_SWITCHING_ISR(taskWoken);
	} else {
		ret = xSemaphoreTake(sem->handle, (ms == OSAL_WAIT_FOREVER) ? OSAL_WAIT_FOREVER : OSAL_MS2TICK(ms));
		if (ret == errQUEUE_EMPTY) {
			OSAL_ERR("semaphore queue empty!\r\n");

			return OSAL_ERROR;
		}
	}

	return OSAL_OK;
}

osal_status_t osal_sem_release(osal_sem_t *sem)
{
	BaseType_t ret;

	if (sem->handle == NULL) {
		//OSAL_ERR("semaphore handle is NULL\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		BaseType_t taskWoken = pdFALSE;

		ret = xSemaphoreGiveFromISR(sem->handle, &taskWoken);
		if (ret == errQUEUE_FULL) {
			OSAL_ERR("ISR Semaphore queue full!\r\n");

			return OSAL_ERROR;
		}
		portEND_SWITCHING_ISR(taskWoken);
	} else {
		ret = xSemaphoreGive(sem->handle);
		if (ret == errQUEUE_FULL) {
			OSAL_ERR("Semaphore queue full!\r\n");

			return OSAL_ERROR;
		}
	}

	return OSAL_OK;
}

