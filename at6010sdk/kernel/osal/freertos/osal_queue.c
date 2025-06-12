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

#include "osal_queue.h"
#include "FreeRTOS.h"
#include "queue.h"

osal_status_t osal_queue_create(osal_queue_t *queue, u32 queueLen, u32 item_size)
{
	queue->handle = xQueueCreate(queueLen, item_size);
	if (queue->handle == NULL) {
		OSAL_ERR("Queue Create failed!\r\n");

		return OSAL_ERROR;
	}

	return OSAL_OK;
}

osal_status_t osal_queue_delete(osal_queue_t *queue)
{
	UBaseType_t ret;

	if (queue->handle == NULL) {
		OSAL_ERR("Queue handle is NULL\r\n");

		return OSAL_ERROR;
	}

	ret = uxQueueMessagesWaiting(queue->handle);
	if (ret > 0) {
		OSAL_ERR("queue delete failed\r\n");

		return OSAL_ERROR;
	}

	vQueueDelete(queue->handle);
	queue->handle = NULL;

	return OSAL_OK;
}

osal_status_t osal_queue_send(osal_queue_t *queue, const void *item, u32 ms)
{
	BaseType_t ret;

	if (queue->handle == NULL) {
		OSAL_ERR("Queue handle is NULL\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		BaseType_t taskWoken = pdFALSE;

		ret = xQueueSendFromISR(queue->handle, item, &taskWoken);
		if (ret == errQUEUE_FULL) {
			OSAL_ERR("ISR Queue is full!\r\n");

			return OSAL_ERROR;
		}
		portEND_SWITCHING_ISR(taskWoken);
	} else {
		ret = xQueueSend(queue->handle, item, OSAL_MS2TICK(ms));
		if (ret == errQUEUE_FULL) {
			OSAL_ERR("Queue is full!\r\n");

			return OSAL_ERROR;
		}
	}

	return OSAL_OK;
}

osal_status_t osal_queue_receive(osal_queue_t *queue, void *item, u32 ms)
{
	BaseType_t ret;

	if (queue->handle == NULL) {
		OSAL_ERR("Queue handle is NULL\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		BaseType_t taskWoken = pdFALSE;

		ret = xQueueReceiveFromISR(queue->handle, item, &taskWoken);
		if (ret != pdPASS) {
			OSAL_ERR("Queue receive from ISR failed!\r\n");

			return OSAL_ERROR;
		}
		portEND_SWITCHING_ISR(taskWoken);
	} else {
		ret = xQueueReceive(queue->handle, item, (ms == OSAL_WAIT_FOREVER) ? OSAL_WAIT_FOREVER : OSAL_MS2TICK(ms));
		if (ret != pdPASS) {
			OSAL_ERR("Queue receive failed!\r\n");

			return OSAL_ERROR;
		}
	}

	return OSAL_OK;
}

