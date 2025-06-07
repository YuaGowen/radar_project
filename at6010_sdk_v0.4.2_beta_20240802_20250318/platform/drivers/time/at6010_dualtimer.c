/**
 * Copyright (c) 2022, Airtouching Intelligence Technology.
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
#include "at6010.h"
#include "dualtimer.h"
#include "clock.h"


#define HW_DUALTIM0                ((CMSDK_DUALTIMER_SINGLE_TypeDef *)CMSDK_DUALTIMER_1_BASE)
#define HW_DUALTIM1                ((CMSDK_DUALTIMER_SINGLE_TypeDef *)CMSDK_DUALTIMER_2_BASE)

typedef struct {
	dualtimer_cb_t cb;
	CMSDK_DUALTIMER_SINGLE_TypeDef *base;
} dual_timer_dev_t;

static dual_timer_dev_t m_dualtimer_dev[DUALTIMER_TIM_QTY] = {
	{ NULL, HW_DUALTIM0 },
	{ NULL, HW_DUALTIM1 },
};
void dualtimer_init()
{
	sys_clk_enable_dualtimer();
	return;
}

void dualtimer_deinit()
{
	m_dualtimer_dev[DUALTIMER_TIM_0].base->TimerControl &= ~CMSDK_DUALTIMER_CTRL_EN_Msk;
	m_dualtimer_dev[DUALTIMER_TIM_1].base->TimerControl &= ~CMSDK_DUALTIMER_CTRL_EN_Msk;
	sys_clk_disable_dualtimer();
	return;
}
void dualtimer_get_default_config(dualtimer_config *cfg)
{
	cfg->counter_mode    = DUALTIMER_PERIODIC_MODE;
	cfg->counter_size    = DUALTIMER_COUNTER_SIZE_32BIT;
	cfg->clock_prescaler = DUALTIMER_CLOCK_PRESCALER_DIV1;
	cfg->irq_enable      = true;
	cfg->load_value      =  0 ;
	return;
}

void dualtimer_set_config(dualtimer_config *cfg, dualtimer_timer id)
{
	u32 reg_val = 0;

	if (cfg->counter_mode == DUALTIMER_ONE_SHOT_MODE) {
		reg_val = CMSDK_DUALTIMER_CTRL_ONESHOOT_Msk | CMSDK_DUALTIMER_CTRL_MODE_Msk;
	} else if (cfg->counter_mode == DUALTIMER_PERIODIC_MODE) {
		reg_val = CMSDK_DUALTIMER_CTRL_MODE_Msk;
	} else {
		reg_val = 0;    /* free running mode. */
	}
	reg_val |= (u32)(cfg->clock_prescaler << CMSDK_DUALTIMER_CTRL_PRESCALE_Pos |
		    cfg->irq_enable  << CMSDK_DUALTIMER_CTRL_INTEN_Pos |
		    cfg->counter_size << CMSDK_DUALTIMER_CTRL_SIZE_Pos) ;

	m_dualtimer_dev[id].base->TimerLoad = cfg->load_value;
	m_dualtimer_dev[id].base->TimerControl = reg_val;
	return;
}
void dualtimer_set_load(dualtimer_timer id, dualtimer_load_type cur_bg, u32 value)
{
	if (cur_bg == DUALTIMER_SET_CUR_LOAD) {
		m_dualtimer_dev[id].base->TimerLoad = value;
	} else {
		m_dualtimer_dev[id].base->TimerBGLoad = value;

	}
}
void dualtimer_start(dualtimer_timer id)
{
	m_dualtimer_dev[id].base->TimerControl |= CMSDK_DUALTIMER_CTRL_EN_Msk;
	return;
}
void dualtimer_stop(dualtimer_timer id)
{
	m_dualtimer_dev[id].base->TimerControl &= ~CMSDK_DUALTIMER_CTRL_EN_Msk;
	return;
}
 void dualtimer_irq_clear(dualtimer_timer id)
{
	m_dualtimer_dev[id].base->TimerIntClr = CMSDK_DUALTIMER_INTCLR_Msk;
	return;
}
u32 dualtimer_get_value(dualtimer_timer id)
{
	return m_dualtimer_dev[id].base->TimerValue ;
}
void dualtimer_set_cb(dualtimer_timer id, dualtimer_cb_t func)
{
	m_dualtimer_dev[id].cb = func;
	return;
}
bool dualtimer_is_enabled(dualtimer_timer id)
{
	u32 enabled = m_dualtimer_dev[id].base->TimerControl & CMSDK_DUALTIMER_CTRL_EN_Msk;

	return (enabled != 0);
}

void dualtimer_hdl_proc(void)
{
	/* The Dual Timer interrupt handler are shared by the two timers */
	if (m_dualtimer_dev[DUALTIMER_TIM_0].base->TimerMIS != 0) {
		dualtimer_irq_clear(DUALTIMER_TIM_0);
		if (m_dualtimer_dev[DUALTIMER_TIM_0].cb != NULL) {
			m_dualtimer_dev[DUALTIMER_TIM_0].cb();
		}
	}
	if (m_dualtimer_dev[DUALTIMER_TIM_1].base->TimerMIS != 0) {
		dualtimer_irq_clear(DUALTIMER_TIM_1);
		if (m_dualtimer_dev[DUALTIMER_TIM_1].cb != NULL) {
			m_dualtimer_dev[DUALTIMER_TIM_1].cb();
		}
	}
	return;
}
