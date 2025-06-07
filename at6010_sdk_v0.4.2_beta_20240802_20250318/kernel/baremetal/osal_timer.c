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
#include "sys_timer.h"


typedef void (*sys_timer_cb_t)(u32 arg);

osal_status_t osal_timer_create(osal_timer_t *timer, osal_timer_mode_t mode,
                                osal_timer_cb_t cb, void *arg, u32 ms)
{
	sys_timer_t *tm = create_timer((const char *)"xx-tm", (u8)mode, (sys_timer_cb_t)cb, (u32)arg);
	if (NULL == tm) {
		return OSAL_ERROR;
	}

	tm->delay = ms;
	tm->period = ms;

	timer->handle = tm;

	return OSAL_OK;
}

osal_status_t osal_timer_delete(osal_timer_t *timer)
{
	if (NULL == timer) {
		return OSAL_INVALID;
	}

	sys_timer_t *tm = (sys_timer_t *)timer->handle;
	stop_timer(tm);
	del_timer(tm);

	return OSAL_OK;
}

osal_status_t osal_timer_start(osal_timer_t *timer)
{
	if (NULL == timer) {
		return OSAL_INVALID;
	}

	sys_timer_t *tm = timer->handle;

	start_timer(tm, tm->period);

	return OSAL_OK;
}

osal_status_t osal_timer_change_period(osal_timer_t *timer, u32 period_ms)
{
	if (NULL == timer) {
		return OSAL_INVALID;
	}

	sys_timer_t *tm = (sys_timer_t *)timer->handle;

	reconfig_timer(tm, tm->opt, tm->callback, tm->arg);
	tm->period = period_ms;

	return OSAL_OK;
}

osal_status_t osal_timer_stop(osal_timer_t *timer)
{
	if (NULL == timer) {
		return OSAL_INVALID;
	}

	sys_timer_t *tm = (sys_timer_t *)timer->handle;
	return stop_timer(tm);
}

osal_status_t osal_timer_restart(const osal_timer_t *timer)
{
	if (NULL == timer) {
		return OSAL_INVALID;
	}

	sys_timer_t *tm = (sys_timer_t *)timer->handle;
	return start_timer(tm, tm->period);
}

void osal_timer_change_mode(const osal_timer_t *timer, osal_timer_mode_t mode)
{
	if (NULL == timer) {
		return;
	}

	sys_timer_t *tm = (sys_timer_t *)timer->handle;
	tm->opt = (timer_opt_t)mode;
}

void osal_timer_change_callback(const osal_timer_t *timer, osal_timer_cb_t cb)
{
	if (NULL == timer) {
		return;
	}

	sys_timer_t *tm = (sys_timer_t *)timer->handle;
	tm->callback = (sys_timer_cb_t)cb;
}

u8 osal_timer_is_active(osal_timer_t *timer)
{
	if (NULL == timer) {
		return 0U;
	}

	sys_timer_t *tm = (sys_timer_t *)timer->handle;

	// 0 -- stopped; 1 -- expired; 2 -- running
	return (0U == tm->state) ? (0) : (1);
}
