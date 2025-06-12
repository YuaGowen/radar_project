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

#ifndef __OSAL_TIMER_H__
#define __OSAL_TIMER_H__

#include "osal_common.h"

typedef enum {
	OSAL_TIMER_MODE_SINGLE = 0,
	OSAL_TIMER_MODE_PERIODIC = 1
} osal_timer_mode_t;

typedef osal_hdl_t osal_timer_hdl_t;

typedef void (*osal_timer_cb_t)(void *arg);

typedef struct osal_timer_cb_param {
	void *arg;
	osal_timer_cb_t cb;
} osal_timer_cb_param_t;

typedef struct osal_timer {
	osal_timer_hdl_t handle;
	osal_timer_cb_param_t *param;
} osal_timer_t;

osal_status_t osal_timer_create(osal_timer_t *timer, osal_timer_mode_t mode, osal_timer_cb_t cb,
                              void *arg, u32 ms);
osal_status_t osal_timer_delete(osal_timer_t *timer);
osal_status_t osal_timer_start(osal_timer_t *timer);
osal_status_t osal_timer_stop(osal_timer_t *timer);
osal_status_t osal_timer_restart(const osal_timer_t *timer);

void osal_timer_change_mode(const osal_timer_t *timer, osal_timer_mode_t mode);
void osal_timer_change_callback(const osal_timer_t *timer, osal_timer_cb_t cb);
osal_status_t osal_timer_change_period(osal_timer_t *timer, u32 period_ms);

u8 osal_timer_is_active(osal_timer_t *timer);

#endif /* __OSAL_TIMER_H__ */
