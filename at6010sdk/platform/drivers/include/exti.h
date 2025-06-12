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

#ifndef __EXTI_H__
#define __EXTI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_types.h"
#include "irq.h"

typedef enum {
	EXTI_IRQ_BT,
	EXTI_IRQ_QTY
} exti_irq_t;

typedef enum {
	BT_IRQ_LEVEL_HIGH,
	BT_IRQ_EDGE_RISE,
	BT_IRQ_EDGE_FALL,
	BT_IRQ_EDGE_BOTH
} bt_irq_t;


// FIXME: all the following functions are not implemented!!!

void exti_irq_hdl(exti_irq_t irq);
void exti_irq_set_hdl(exti_irq_t irq, isr_hdl_t hdl);
void exti_irq_enable(exti_irq_t irq);
void exti_irq_disable(exti_irq_t irq);
void exti_irq_clear(exti_irq_t irq);
u8   exti_irq_status(exti_irq_t irq);

/**
 * BT IRQ Functions
 */
void exti_set_bt_level_invert(u8 inv);
void exti_set_bt_irq_type(bt_irq_t type);
u8   exti_get_bt_irq_status(void);

#ifdef __cplusplus
}
#endif

#endif /* !__EXTI_H__ */
