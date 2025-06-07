/**
 *******************************************************************************
 * @file    light_ctrl.h
 * @author  Airtouch Software Team
 * @brief   Light control
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
 *    Intelligence Technology integrated circuit in a product or a software update
 *    for such product, must reproduce the above copyright notice, this list of
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


#ifndef __LIGHT_CTRL_H__
#define __LIGHT_CTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_types.h"


void light_ctrl_init(void);
void light_ctrl_switch(u8 onoff, u32 ms);

/**
 * @brief light operation: set light blinking
 *
 * @param ms light on-off period
 * @return @ref AT_TYPE_ERROR
 */
u8 light_ctrl_blink(u32 ms);

/**
 * @brief  light operation: set light on
 *
 * @param ms light on keep time
 * @return @ref AT_TYPE_ERROR
 */
u8 light_ctrl_on(u32 ms);

/**
 * @brief  light operation: set light off
 *
 * @return @ref AT_TYPE_ERROR
 */
u8 light_ctrl_off(void);

/**
 * @brief Get current status of light
 *
 * @return u8 0: light is off, 1: light is on.
 */
u8 get_light_state(void);

#ifdef CFG_AUX_LIGHT_EN
void light_ctrl_switch_aux(u8 onoff, u32 ms);
#endif

void light_ctrl_switch_onoff(u8 mod);

void light_ctrl_event(u32 para);

#ifdef __cplusplus
}
#endif

#endif /* !__LIGHT_CTRL_H__ */
