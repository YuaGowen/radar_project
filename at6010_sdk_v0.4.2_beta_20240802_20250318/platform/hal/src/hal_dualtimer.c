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
#include "at6010.h"
#include "clock.h"
#include "hal_dualtimer.h"

#define SYS_CLK_FREQ                    clk_mcu_get()

void hal_dualtimer_init(void)
{
	dualtimer_init();
	dualtimer_config cfg;

	dualtimer_get_default_config(&cfg);
	cfg.counter_mode = DUALTIMER_FREE_RUNNING_MODE;
	cfg.irq_enable   = 0;
	cfg.clock_prescaler = DUALTIMER_CLOCK_PRESCALER_DIV16;
	dualtimer_set_config(&cfg, DUALTIMER_TIM_0);

	dualtimer_get_default_config(&cfg);
	cfg.counter_mode = DUALTIMER_FREE_RUNNING_MODE;
	cfg.irq_enable   = 0;
	dualtimer_set_config(&cfg, DUALTIMER_TIM_1);

	dualtimer_start(DUALTIMER_TIM_0);
	dualtimer_start(DUALTIMER_TIM_1);

	NVIC_ClearPendingIRQ(DUALTIMER_IRQn);
	NVIC_EnableIRQ(DUALTIMER_IRQn);
}

void hal_dualtimer_config(dualtimer_timer idx, dualtimer_counter_mode mode, uint32 time_us, dualtimer_cb_t func)
{
	u32 reload;
	if (dualtimer_is_enabled(idx) == true) {
		dualtimer_stop(idx);
		NVIC_ClearPendingIRQ(DUALTIMER_IRQn);
		dualtimer_irq_clear(idx);
	}
	dualtimer_config cfg;
	dualtimer_get_default_config(&cfg);
	cfg.counter_mode = mode;
	dualtimer_set_config(&cfg, idx);

	/* convert to reload value, time in us. */
	reload = time_us * (SYS_CLK_FREQ / 1000000u);
	dualtimer_set_load(idx, DUALTIMER_SET_CUR_LOAD, reload);
	dualtimer_set_cb(idx, func);
	return;
}

void hal_dualtimer_set_time(dualtimer_timer idx, dualtimer_load_type mode, uint32 time_us)
{
	u32 reload;
	reload = time_us * (SYS_CLK_FREQ / 1000000u);
	dualtimer_set_load(idx, mode, reload);
	return;
}
void hal_dualtimer_start(dualtimer_timer idx)
{
	dualtimer_start(idx);
	return;
}

void hal_dualtimer_stop(dualtimer_timer idx)
{
	dualtimer_stop(idx);
	return;
}

static void hal_delay_tick(u32 ticks, dualtimer_timer idx)
{
	u32 start_time;
	volatile u32 now;

	if (ticks > 0U) {
		start_time = dualtimer_get_value(idx);
		do {
			now = dualtimer_get_value(idx);
		} while ((u32)(start_time - now) < ticks);
	}
}

void hal_dualtimer_delay_us(u32 us)
{
	u32 delay_tick;
	delay_tick = us * (SYS_CLK_FREQ / 1000000);

	hal_delay_tick(delay_tick, DUALTIMER_TIM_1);
	return;
}

void hal_dualtimer_delay_ms(u32 ms)
{
	hal_dualtimer_delay_us(ms * 1000);
}

u32 hal_get_start_cnt_value()
{
	u32 start_time = dualtimer_get_value(DUALTIMER_TIM_1);
	return start_time;
}

u32 hal_get_elapsed_time_us(u32 start_cnt_value)
{
	u32 now = 0, us_unit = SYS_CLK_FREQ / 1000000;

	now = dualtimer_get_value(DUALTIMER_TIM_1);

	return (start_cnt_value - now) / us_unit;
}

void hal_dualtimer_deinit(void)
{
	NVIC_DisableIRQ(DUALTIMER_IRQn);
	dualtimer_deinit();
	return;
}
