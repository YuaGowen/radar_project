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

#include "timer.h"
#include "at6010.h"

#define HW_TIMER0 ((timer_reg_t *)CMSDK_TIMER0_BASE)
#define HW_TIMER1 ((timer_reg_t *)CMSDK_TIMER1_BASE)
#define HW_TIMER2 ((timer_reg_t *)CMSDK_TIMER2_BASE)
#define HW_TIMER3 ((timer_reg_t *)CMSDK_TIMER3_BASE)

#if defined(__CC_ARM)
#pragma anon_unions
#endif

typedef struct {
	volatile u32 ctrl;   /*!< Offset: 0x000 Control Register (R/W) */
	volatile u32 value;  /*!< Offset: 0x004 Current Value Register (R/W) */
	volatile u32 reload; /*!< Offset: 0x008 Reload Value Register  (R/W) */

	union {
		volatile u32 intstatus; /*!< Offset: 0x00C Interrupt Status Register (R/ ) */
		volatile u32 intclear;  /*!< Offset: 0x00C Interrupt Clear Register ( /W) */
	};
} timer_reg_t;

typedef struct {
	timer_cb_t cb;
	timer_reg_t *tim_hwd;
} timer_dev_t;

static timer_dev_t m_timer_dev[TIMER_ID_QTY] = {
	{NULL, HW_TIMER0},
	{NULL, HW_TIMER1},
	{NULL, HW_TIMER2},
	{NULL, HW_TIMER3}
};

void timer_init(timer_id_t id, timer_source_t source)
{
	timer_reg_t *hwd_timer = m_timer_dev[id].tim_hwd;

	if (TIMER_SOURCE_SYSTEM == source) {
		hwd_timer->ctrl &=
		        ~(CMSDK_TIMER_CTRL_SELEXTEN_Msk | CMSDK_TIMER_CTRL_SELEXTCLK_Msk);
	} else {
		hwd_timer->ctrl &= ~CMSDK_TIMER_CTRL_EXTCLKDIV_Msk;
		hwd_timer->ctrl |= CMSDK_TIMER_CTRL_SELEXTEN_Msk | CMSDK_TIMER_CTRL_SELEXTCLK_Msk
		                | (u32)((u8)source << CMSDK_TIMER_CTRL_EXTCLKDIV_Pos);
	}
}

void timer_irq_enable(timer_id_t id)
{
	timer_reg_t *hwd_timer = m_timer_dev[id].tim_hwd;
	IRQn_Type tim_irq = (IRQn_Type)(TIMER0_IRQn + id);

	hwd_timer->ctrl |= CMSDK_TIMER_CTRL_IRQEN_Msk;

	NVIC_ClearPendingIRQ(tim_irq);
	NVIC_EnableIRQ(tim_irq);
}

void timer_irq_disable(timer_id_t id)
{
	timer_reg_t *hwd_timer = m_timer_dev[id].tim_hwd;
	IRQn_Type tim_irq = (IRQn_Type)(TIMER0_IRQn + id);

	hwd_timer->ctrl &= ~CMSDK_TIMER_CTRL_IRQEN_Msk;
	NVIC_DisableIRQ(tim_irq);
}

void timer_start(timer_id_t id)
{
	timer_reg_t *hwd_timer = m_timer_dev[id].tim_hwd;

	hwd_timer->ctrl |= CMSDK_TIMER_CTRL_EN_Msk;
}

void timer_stop(timer_id_t id)
{
	timer_reg_t *hwd_timer = m_timer_dev[id].tim_hwd;

	hwd_timer->ctrl &= ~CMSDK_TIMER_CTRL_EN_Msk;
}

u32 timer_value_get(timer_id_t id)
{
	timer_reg_t *hwd_timer = m_timer_dev[id].tim_hwd;

	return hwd_timer->value;
}

void timer_value_set(timer_id_t id, u32 value)
{
	timer_reg_t *hwd_timer = m_timer_dev[id].tim_hwd;

	hwd_timer->value = value;
}

u32 timer_reload_get(timer_id_t id)
{
	timer_reg_t *hwd_timer = m_timer_dev[id].tim_hwd;

	return hwd_timer->reload;
}

void timer_reload_set(timer_id_t id, u32 value)
{
	timer_reg_t *hwd_timer = m_timer_dev[id].tim_hwd;

	hwd_timer->reload = value;
}

void timer_irq_clear(timer_id_t id)
{
	timer_reg_t *hwd_timer = m_timer_dev[id].tim_hwd;

	hwd_timer->intclear = CMSDK_TIMER_INTCLEAR_Msk;
}

u32 timer_irq_status(timer_id_t id)
{
	timer_reg_t *hwd_timer = m_timer_dev[id].tim_hwd;

	return hwd_timer->intstatus;
}

void timer_cb_set(timer_id_t id, timer_cb_t func)
{
	m_timer_dev[id].cb = func;
}

void timer_isr(timer_id_t id)
{
	if (m_timer_dev[id].cb != NULL) {
		m_timer_dev[id].cb(NULL);
	}
	timer_irq_clear(id);
}
