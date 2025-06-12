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

#include "osal_event.h"
#include "FreeRTOS.h"
#include "event_groups.h"

osal_status_t osal_event_create(osal_event_t *event)
{
	osal_status_t ret = OSAL_OK;

	event->handle = xEventGroupCreate();
	if (event->handle == NULL) {
		OSAL_ERR("EventGroup Create failed!\r\n");
		ret = OSAL_ERROR;
	}

	return ret;
}

osal_status_t osal_event_delete(osal_event_t *event)
{
	osal_status_t ret = OSAL_OK;

	if (event->handle == NULL) {
		OSAL_ERR("EventGroup handle is NULL\r\n");
		ret = OSAL_ERROR;
	}

	vEventGroupDelete(event->handle);
	event->handle = NULL;

	return ret;
}

osal_status_t osal_event_set(osal_event_t *event, u32 bits)
{
	BaseType_t ret;

	if (event->handle == NULL) {
		OSAL_ERR("EventGroup handle is NULL\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		BaseType_t taskWoken = pdFALSE;

		ret = xEventGroupSetBitsFromISR(event->handle, bits, &taskWoken);
		if (ret != pdPASS) {
			OSAL_ERR("EventGroup send from ISR failed! ret=0x%x\r\n", (u32)ret);

			return OSAL_ERROR;
		}
		portEND_SWITCHING_ISR(taskWoken);
	} else {
		(void)xEventGroupSetBits(event->handle, bits);
	}

	return OSAL_OK;
}

osal_status_t osal_event_get(osal_event_t *event, u32 *bits)
{
	osal_status_t ret = OSAL_OK;

	if (event->handle == NULL) {
		OSAL_ERR("EventGroup handle is NULL\r\n");
		ret = OSAL_ERROR;
	} else {
		if (pdTRUE == xPortIsInsideInterrupt()) {
			*bits = xEventGroupGetBitsFromISR(event->handle);
		} else {
			*bits = xEventGroupGetBits(event->handle);
		}
	}

	return ret;
}

osal_status_t osal_event_clear(osal_event_t *event, u32 bits)
{
	if (event->handle == NULL) {
		OSAL_ERR("EventGroup handle is NULL\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		BaseType_t ret;

		ret = xEventGroupClearBitsFromISR(event->handle, bits);
		if (ret != pdPASS) {
			OSAL_ERR("EventGroup send from ISR failed! ret=0x%x\r\n", (u32)ret);

			return OSAL_ERROR;
		}
	} else {
		(void)xEventGroupClearBits(event->handle, bits);
	}

	return OSAL_OK;
}

osal_status_t osal_event_wait(osal_event_t *event, const osal_event_wait_t *wait_cfg, u32 *bits_set)
{
	if (event->handle == NULL) {
		OSAL_ERR("EventGroup handle is NULL\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		OSAL_ERR("Cannot wait event from isr!\r\n");

		return OSAL_ERROR;
	}

	*bits_set = xEventGroupWaitBits(event->handle, wait_cfg->wait_bits, (BaseType_t)wait_cfg->clear,
	                            (BaseType_t)wait_cfg->mode, wait_cfg->wait_ms);

	return OSAL_OK;
}

osal_status_t osal_event_sync(osal_event_t *event, const osal_event_sync_t *sync_cfg, u32 *bits_set)
{
	if (event->handle == NULL) {
		OSAL_ERR("EventGroup handle is NULL\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		OSAL_ERR("Cannot wait event from isr!\r\n");

		return OSAL_ERROR;
	}

	*bits_set = xEventGroupSync(event->handle, sync_cfg->set_bits, sync_cfg->wait_bits,
	                            sync_cfg->wait_ms);

	return OSAL_OK;
}
