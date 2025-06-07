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

#ifndef __OSAL_EVENT_H__
#define __OSAL_EVENT_H__

#include "osal_common.h"

typedef osal_hdl_t osal_event_hdl_t;

typedef struct {
	osal_event_hdl_t handle;
} osal_event_t;

typedef enum {
	EVENT_WAIT_ANY_BITS = 0,
	EVENT_WAIT_ALL_BITS = 1
} osal_event_wait_mode_t;

typedef enum {
	EVENT_WAIT_SET_ON_EXIT = 0,
	EVENT_WAIT_CLEAR_ON_EXIT = 1
} osal_event_wait_clear_t;

typedef struct {
	u32 wait_bits;                 /* bits wait for */
	u32 wait_ms;                   /* max time to wait */
	osal_event_wait_clear_t clear; /* bits cleared before exit or keep set */
	osal_event_wait_mode_t mode;   /* wait for all bits or either */
} osal_event_wait_t;

typedef struct {
	u32 set_bits;  /* bits to set */
	u32 wait_bits; /* bits wait for */
	u32 wait_ms;   /* max time to wait */
} osal_event_sync_t;

osal_status_t osal_event_create(osal_event_t *event);
osal_status_t osal_event_delete(osal_event_t *event);

osal_status_t osal_event_set(osal_event_t *event, u32 bits);
osal_status_t osal_event_get(osal_event_t *event, u32 *bits);
osal_status_t osal_event_clear(osal_event_t *event, u32 bits);
osal_status_t osal_event_wait(osal_event_t *event, const osal_event_wait_t *wait_cfg, u32 *bits_set);
osal_status_t osal_event_sync(osal_event_t *event, const osal_event_sync_t *sync_cfg, u32 *bits_set);

#endif /* __OSAL_EVENT_H__ */
