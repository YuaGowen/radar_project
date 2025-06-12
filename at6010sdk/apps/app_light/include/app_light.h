/**
 *******************************************************************************
 * @file    app_light.h
 * @author  Airtouch Software Team
 * @brief   light application
 *******************************************************************************
 * @copyright Copyright (c) 2023, Airtouching Intelligence Technology.
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
 ******************************************************************************/

#ifndef __APP_LIGHT_H__
#define __APP_LIGHT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_types.h"
#include "osal_common.h"

void app_light_init(void);
void app_light_set_lot(u32 time);
u32  app_light_get_lot(void);
void app_light_proc(void *arg);
bool app_light_msg_handler(osal_msg_t *msg);

#ifdef CFG_IR
void app_light_always_on(void);
void app_light_always_off(void);
void app_light_set_intensity(u16 intensity);
void app_light_set_standby_level(u16 level);
#endif

#ifdef CFG_LS
void app_light_off_daytime_proc(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* !__APP_LIGHT_H__ */
