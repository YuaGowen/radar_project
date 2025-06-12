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

#include "osal_timer.h"
#include "osal_mem.h"
#include "FreeRTOS.h"
#include "timers.h"

static void osal_timer_timeout_callback(TimerHandle_t timer)
{
	osal_timer_cb_param_t *ptimer;

	ptimer = pvTimerGetTimerID(timer);
	if (ptimer && ptimer->cb) {
		ptimer->cb(ptimer->arg);
	} else {
		OSAL_ERR("Invalid timer callback\r\n");
	}
}

osal_status_t osal_timer_create(osal_timer_t *timer, osal_timer_mode_t mode,
			      osal_timer_cb_t cb, void *arg, u32 ms)
{
	osal_timer_cb_param_t *param = NULL;

	if (timer->handle != NULL) {
		OSAL_ERR("handle=0x%x\r\n", (u32)timer->handle);

		return OSAL_ERROR;
	}

	param = (osal_timer_cb_param_t *)osal_malloc(sizeof(osal_timer_cb_param_t));
	if (param == NULL) {
		return OSAL_NOMEM;
	}

	param->cb = cb;
	param->arg = arg;

	timer->handle = xTimerCreate("",
				     OSAL_MS2TICK(ms),
				     (mode == OSAL_TIMER_MODE_PERIODIC) ? pdTRUE : pdFALSE,
				     param,
				     osal_timer_timeout_callback);

	if (timer->handle == NULL) {
		OSAL_ERR("Timer create failed!\r\n");
		if (param)
			osal_free(param);

		return OSAL_ERROR;
	}
	timer->param = param;

	return OSAL_OK;
}

osal_status_t osal_timer_delete(osal_timer_t *timer)
{
	BaseType_t ret;

	if (timer->handle == NULL) {
		OSAL_ERR("Timer handle is NULL!\r\n");

		return OSAL_ERROR;
	}

	ret = xTimerDelete(timer->handle, OSAL_WAIT_FOREVER);
	if (ret != pdPASS) {
		OSAL_ERR("Timer delete failed!\r\n");

		return OSAL_ERROR;
	}

	timer->handle = NULL;
	if (timer->param) {
		osal_free(timer->param);
		timer->param = NULL;
	}

	return OSAL_OK;
}

osal_status_t osal_timer_start(osal_timer_t *timer)
{
	BaseType_t ret;


	if (timer->handle == NULL) {
		OSAL_ERR("Timer handle is NULL!\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		BaseType_t taskWoken = pdFALSE;

		ret = xTimerStartFromISR(timer->handle, &taskWoken);
		if (ret != pdPASS) {
			OSAL_ERR("Timer ISR start failed!\r\n");

			return OSAL_ERROR;
		}
		portEND_SWITCHING_ISR(taskWoken);
	} else {
		ret = xTimerStart(timer->handle, OSAL_WAIT_NONE);
		if (ret != pdPASS) {
			OSAL_ERR("Timer start failed!\r\n");

			return OSAL_ERROR;
		}
	}

	return OSAL_OK;
}

osal_status_t osal_timer_change_period(osal_timer_t *timer, u32 period_ms)
{
	BaseType_t ret;

	if (timer->handle == NULL) {
		OSAL_ERR("Timer handle is NULL!\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		BaseType_t taskWoken;
		taskWoken = pdFALSE;
		ret = xTimerChangePeriodFromISR(timer->handle, period_ms, &taskWoken);
		if (ret != pdPASS) {
			OSAL_ERR("Timer ISR change failed!\r\n");

			return OSAL_ERROR;
		}
		portEND_SWITCHING_ISR(taskWoken);
	} else {
		ret = xTimerChangePeriod(timer->handle, period_ms, OSAL_WAIT_NONE);
		if (ret != pdPASS) {
			OSAL_ERR("Timer change failed! ms=%d,ret=%d\r\n", period_ms, (s32)ret);

			return OSAL_ERROR;
		}
	}

	return OSAL_OK;
}

osal_status_t osal_timer_stop(osal_timer_t *timer)
{
	BaseType_t ret;

	if (timer->handle == NULL) {
		OSAL_ERR("Timer handle is NULL!\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		BaseType_t taskWoken = pdFALSE;

		ret = xTimerStopFromISR(timer->handle, &taskWoken);
		if (ret != pdPASS) {
			OSAL_ERR("Timer ISR stop failed!\r\n");

			return OSAL_ERROR;
		}
		portEND_SWITCHING_ISR(taskWoken);
	} else {
		ret = xTimerStop(timer->handle, OSAL_WAIT_FOREVER);
		if (ret != pdPASS) {
			OSAL_ERR("Timer stop failed!\r\n");

			return OSAL_ERROR;
		}
	}

	return OSAL_OK;
}

osal_status_t osal_timer_restart(const osal_timer_t *timer)
{
	BaseType_t ret;

	if (timer->handle == NULL) {
		OSAL_ERR("Timer handle is NULL!\r\n");

		return OSAL_ERROR;
	}

	if (pdTRUE == xPortIsInsideInterrupt()) {
		BaseType_t taskWoken = pdFALSE;

		ret = xTimerResetFromISR(timer->handle, &taskWoken);
		if (ret != pdPASS) {
			OSAL_ERR("Timer ISR start failed!\r\n");

			return OSAL_ERROR;
		}
		portEND_SWITCHING_ISR(taskWoken);
	} else {
		ret = xTimerReset(timer->handle, OSAL_WAIT_NONE);
		if (ret != pdPASS) {
			OSAL_ERR("Timer start failed!\r\n");

			return OSAL_ERROR;
		}
	}

	return OSAL_OK;
}

void osal_timer_change_mode(const osal_timer_t *timer, osal_timer_mode_t mode)
{
	vTimerSetReloadMode(timer->handle, (mode == OSAL_TIMER_MODE_PERIODIC) ? pdTRUE : pdFALSE);
}

void osal_timer_change_callback(const osal_timer_t *timer, osal_timer_cb_t cb)
{
	timer->param->cb = cb;
}

u8 osal_timer_is_active(osal_timer_t *timer)
{
	return xTimerIsTimerActive(timer->handle);
}

