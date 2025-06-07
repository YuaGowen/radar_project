/**
 *******************************************************************************
 * @file    hal_pwm.c
 * @author  Airtouch Software Team
 * @brief   PWM HAL Driver
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

#include "hal_pwm.h"
#include "hal_clock.h"
#include "at_error.h"

#define PWM_PERIOD_MAX (0x3FFFU)
#define PWM_DUTY_MAX   (0x1FFFU)

static u16 calculate_config_duty(u16 duty, u16 period)
{
	u8 carry;
	u16 con_duty;
	u32 product;

	if (duty == 0u) {
		con_duty = 0u;
	} else {
		product = (u32)duty * period;
		carry = ((product % PWM_MAX_DUTY) < ((u32)PWM_MAX_DUTY >> 1)) ? 0u : 1u;
		con_duty = (u16)(product / PWM_MAX_DUTY) + carry;
		con_duty = (con_duty == 0u) ? 1u : con_duty;
	}

	return con_duty;
}

u8 hal_pwm_init(pwm_id_t id, u16 duty, u32 freq, u8 invert)
{
	u16 con_period;
	u16 con_duty;
	u32 con_divider;
	pwm_cfg_t cfg;

	con_divider = pwm_get_clk_divider();

#if (PARA_PWM_PHASE_SYNC == 1) && (PARA_PWM_SYNC_EDGE == 1)
	/* NOTE: Enable invert when using rising edge to sync */
	invert = 1U - invert;
	duty = (duty < 1000U) ? (1000U - duty) : 0U;
#endif

	/**
	 * freq: sys_freq/div/con_period
	 * duty rate: con_duty/con_period
	 */
	con_period = (u16)(((hal_get_system_clk() / (con_divider + 1U)) / freq) - 1U);
	if (con_period > PWM_PERIOD_MAX) {
		return E_PAR;
	}

	con_duty = calculate_config_duty(duty, con_period);
	if (con_duty > PWM_DUTY_MAX) {
		return E_PAR;
	}

	cfg.duty = con_duty;
	cfg.period = con_period;
	cfg.invert = invert;
	pwm_init(id, &cfg);

	pwm_enable(id);

	return E_OK;
}

void hal_pwm_set_duty(pwm_id_t id, u16 duty)
{
	u16 con_period, con_duty;

#if (PARA_PWM_PHASE_SYNC == 1) && (PARA_PWM_SYNC_EDGE == 1)
	/* NOTE Enable invert when using rising edge to sync, so get the inverted duty */
	duty = (duty < 1000U) ? (1000U - duty) : 0U;
#endif

	con_period = pwm_get_period(id);
	con_duty = calculate_config_duty(duty, con_period);

	pwm_set_duty(id, con_duty);
}
