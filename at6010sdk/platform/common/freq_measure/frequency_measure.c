/**
* Copyright (c) 2024, Airtouching Intelligence Technology.
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

#include "frequency_measure.h"
#include "gpio.h"
#include "hal_timer.h"
#include "clock.h"

#define FREQ_MEASURE_ERROR              (200U)          /* error unit us */
#define FREQ_50HZ_HALF_PERIOD           (10000U)
#define FREQ_60HZ_HALF_PERIOD           (8333U)

typedef struct {
	power_freq_t freq_type;
	u32 half_period;
} freq_para_t;

static freq_para_t m_freq_para = {
	.freq_type = POWER_FREQ_UNKNOW,
	.half_period = 10000U
};

__STATIC_INLINE bool in_range(u32 value, u32 target, u32 error)
{
	return ((value < (target + error)) && (value > (target - error))) ? TRUE : FALSE;
}

void frequency_measure_start(u32 gpio, u8 times, timer_id_t idn)
{
}

power_freq_t frequency_measure_wait(u32 gpio, u8 times, timer_id_t idn)
{
	power_freq_t power_freq;
	u32 half_period = 0U;
	u8 gpio_value, cnt;
	timer_cfg_t timer_cfg;
	/* Wait edge for start measure */
	gpio_value = (u8)gpio_get_value(gpio);
	while (gpio_value == (u8)gpio_get_value(gpio)) {}

	timer_cfg.source = TIMER_SOURCE_SYSTEM;
	timer_cfg.value_mode =TIMER_VALUE_INC;
	timer_cfg.start = 1;
	timer_cfg.reload = TIMER_VALUE_MAX;
	hal_timer_init(idn, &timer_cfg);

	for (cnt = 0U; cnt < times; cnt++) {
		gpio_value = 1U - gpio_value;

		timer_stop(idn);
		timer_value_set(idn, 0U);
		timer_start(idn);
		while (gpio_value == (u8)gpio_get_value(gpio)) {}

		half_period += 0xFFFFFFFFU - timer_value_get(idn);
	}
	timer_stop(idn);
	timer_irq_enable(idn);

	half_period = (half_period / (clk_mcu_get() / 1000000U)) / times;

	if (TRUE == in_range(half_period, FREQ_50HZ_HALF_PERIOD, FREQ_MEASURE_ERROR)) {
		power_freq = POWER_FREQ_50HZ;
	} else if (TRUE == in_range(half_period, FREQ_60HZ_HALF_PERIOD, FREQ_MEASURE_ERROR)) {
		power_freq = POWER_FREQ_60HZ;
	} else {
		power_freq = POWER_FREQ_UNKNOW;
	}

	m_freq_para.freq_type = power_freq;
	m_freq_para.half_period = half_period;

	return power_freq;
}

power_freq_t freq_measure_get_type(void)
{
	return m_freq_para.freq_type;
}

u32 freq_measure_get_half_period(void)
{
	return m_freq_para.half_period;
}
