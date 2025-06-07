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

#include "hal_relay.h"
#include "frequency_measure.h"
#include "gpio.h"
#include "hal_timer.h"
#include "at6010.h"

#ifdef CFG_RELAY_ZERO_PROTECTION
static u8 m_light_relay_onoff;
static u32 m_half_period = 10000U;

static void relay_timer_cb(void *arg)
{
	(void)arg;
	gpio_set_value(RADAR_OUT_PIN, (s32)m_light_relay_onoff);
	hal_timer_stop(PARA_RELAY_TIMER);
}

static void power_falling_hdl(void)
{
	gpio_irq_clr_flag((u32)POWER_SAMPLE_PIN);
	gpio_irq_disable((u32)POWER_SAMPLE_PIN);
	hal_timer_start(PARA_RELAY_TIMER);
}

void hal_relay_init(void)
{
	gpio_set_pinmux((u32)POWER_SAMPLE_PIN, 1u);
	gpio_set_direction((u32)POWER_SAMPLE_PIN, 0u);
	gpio_set_rd_en((u32)POWER_SAMPLE_PIN, 1u);
	gpio_set_analog_en((u32)POWER_SAMPLE_PIN, 0u);
	gpio_set_pull_down((u32)POWER_SAMPLE_PIN, 0u);
	gpio_set_pull_up((u32)POWER_SAMPLE_PIN, 0u);

	(void)frequency_measure_wait((u32)POWER_SAMPLE_PIN, 4u, PARA_RELAY_TIMER);
	m_half_period = freq_measure_get_half_period();

	gpio_set_irq_type((u32)POWER_SAMPLE_PIN, IRQ_TYPE_EDGE_FALLING);
	gpio_irq_set_hdl((u32)POWER_SAMPLE_PIN, power_falling_hdl);
	if (((u32)GPIO_MCU_0 <= POWER_SAMPLE_PIN) && (POWER_SAMPLE_PIN <= (u32)GPIO_MCU_6)) {
		NVIC_EnableIRQ((IRQn_Type)(POWER_SAMPLE_PIN + GPIO0_IRQn));
	}
	else {
		NVIC_EnableIRQ(GPIO_IRQn);
	}
}

void hal_relay_set_delay(u8 onoff)
{
	u32 delay;
	delay = (onoff == 1u) ? RELAY_ON_DELAY_US : RELAY_OFF_DELAY_US;
	delay = (m_half_period > delay) ? (m_half_period - delay)
	                                : (m_half_period + m_half_period - delay);
	hal_timer_set(PARA_RELAY_TIMER, delay, relay_timer_cb);
	m_light_relay_onoff = onoff;
	gpio_irq_enable((u32)POWER_SAMPLE_PIN);
}

#endif
