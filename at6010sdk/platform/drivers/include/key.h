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

#ifndef __KEY_H__
#define __KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_types.h"

typedef struct {
	u8 en        : 1;
	u8 en_sel    : 1; /* enable/disable key via 0: ana, 1: mcu */
	u8 invert    : 1;
	u8 smooth_en : 1;
} key_cfg_t;

typedef void (*key_hdl_t)(void);

void key_init(const key_cfg_t *cfg);

/**
 * @brief enable/disable key via mcu
 */
void key_enable(void);
void key_disable(void);

/**
 * @brief enable/disable key via ana
 *
 * @param en 0: enable 1: disable
 */
void key_set_en(u8 en);

void key_set_inv_inverse(void);
u8   key_get_inv(void);
void key_set_smooth(u8 en, u8 smooth_high, u8 smooth_low);

void key_irq_enable(void);
void key_irq_disable(void);

void key_set_irq_clr(u8 en);
void key_irq_set_hdl(key_hdl_t hdl);
void key_isr(void);

#ifdef __cplusplus
}
#endif

#endif /* !__KEY_H__ */
