/**
 *******************************************************************************
 * @file    ir_para.h
 * @author  Airtouch Software Team
 * @brief   target configuration of the current project
 *******************************************************************************
 * @copyright Copyright (c) 2024, Airtouching Intelligence Technology.
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

#ifndef __APP_IR_PARA_H__
#define __APP_IR_PARA_H__

#include "at_types.h"


#define IR_PARA_MAX_INTENSITY           (100u)
#define IR_PARA_MIN_INTENSITY           (10u)

#define IR_MODE_ON                      (0u)
#define IR_MODE_OFF                     (1u)
#define IR_MODE_AUTO                    (2u)
#define IR_MODE_TEST                    (3u)
#define IR_MODE_SC1                     (4u)
#define IR_MODE_SC2                     (5u)
#define IR_MODE_SC3                     (6u)
#define IR_MODE_SC4                     (7u)

typedef enum {
    IR_PARA_MODE,
    IR_PARA_INTENSITY,
    IR_PARA_DETECTION_RANGE,
    IR_PARA_STANDBY_LEVEL,
    IR_PARA_ACTIVE_TIMER,
    IR_PARA_LUX_LEVEL,
} ir_para_type_t;

void set_ir_para(ir_para_type_t para, u8 value);
u8 get_ir_para(ir_para_type_t para);

#endif

