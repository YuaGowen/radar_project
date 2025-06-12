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
#include "pwm.h"
#include "reg_pwm.h"
#include "sys_ctrl.h"
#include "clock.h"

#define PWM_PERIOD_MAX (0x3FFFU)
#define PWM_DUTY_MAX   (0x1FFFU)

typedef struct {
	REG32 en     : 1;
	REG32 start  : 1;
	REG32 invert : 1;
	REG32 duty   : 13;
	REG32 period : 14;
	REG32 hold   : 1;
} pwm_config_t;

typedef struct {
	pwm_config_t config[PWM_ID_QTY];
} reg_pwm_t;

static reg_pwm_t *const reg_pwm = (reg_pwm_t *)REG_PWM_BASE;

/**
 * duty rate = con_duty/con_period
 */
static u16 calculate_config_duty(u16 duty, u16 period)
{
	u16 con_duty;
	u32 product;

	if (duty == 0u) {
		con_duty = 0u;
	} else {
		product = (u32)duty * period;
		product += (u32)PWM_MAX_DUTY >> 1;
		con_duty = (u16)(product / PWM_MAX_DUTY);
		con_duty = (con_duty == 0u) ? 1u : con_duty;
	}

	return con_duty;
}

/**
 * freq: sys_freq/div/con_period
 */
__STATIC_INLINE u16 pwm_calc_con_period(u32 freq)
{

	u32 con_divider = pwm_clk_divider_get();
	u16 con_period = (u16)(((clk_mcu_get() / (con_divider + 1U)) / freq) - 1U);
	return con_period;
}

static u16 pwm_get_con_period(pwm_id_t id)
{
	u16 con_period = 0U;

	if (id < PWM_ID_QTY) {
		con_period = reg_pwm->config[id].period;
	}

	return con_period;
}

void pwm_duty_set(pwm_id_t id, u16 duty)
{
	u16 con_period, con_duty;

#if (PARA_PWM_PHASE_SYNC == 1) && (PARA_PWM_SYNC_EDGE == 1)
	/* NOTE Enable invert when using rising edge to sync, so get the inverted duty */
	duty = (duty < 1000U) ? (1000U - duty) : 0U;
#endif

	con_period = pwm_get_con_period(id);
	con_duty = calculate_config_duty(duty, con_period);

	if (id < PWM_ID_QTY) {
		reg_pwm->config[id].duty = con_duty;
	}
}

void pwm_clk_divider_set(u8 divider)
{
	hwp_pwm->pwt_config = PWT_CONFIG_DIV(divider) | PWT_CONFIG_DIV_EN(1U);
}

u8 pwm_clk_divider_get(void)
{
	return (u8)((hwp_pwm->pwt_config & PWT_CONFIG_DIV_MSK) >> PWT_CONFIG_DIV_POS);
}

void pwm_enable(pwm_id_t id)
{
	if (id < PWM_ID_QTY) {
		reg_pwm->config[id].en = 1U;
	}
}

void pwm_disable(pwm_id_t id)
{
	if (id < PWM_ID_QTY) {
		reg_pwm->config[id].en = 0U;
	}
}

void pwm_hold_set(pwm_id_t id, u8 en)
{
	if (id < PWM_ID_QTY) {
		reg_pwm->config[id].hold = en;
	}
}

void pwm_invert_set(pwm_id_t id, u8 invt)
{
	if (id < PWM_ID_QTY) {
		reg_pwm->config[id].invert = invt;
	}
}

void pwm_init(pwm_id_t id, u16 duty, u32 freq, u8 invert)
{
	u16 con_period;
	u16 con_duty;

#if (PARA_PWM_PHASE_SYNC == 1) && (PARA_PWM_SYNC_EDGE == 1)
	/* NOTE: Enable invert when using rising edge to sync */
	invert = 1U - invert;
	duty = (duty < 1000U) ? (1000U - duty) : 0U;
#endif

	con_period = pwm_calc_con_period(freq);
	if (con_period > PWM_PERIOD_MAX) {
		return;
	}

	con_duty = calculate_config_duty(duty, con_period);
	if (con_duty > PWM_DUTY_MAX) {
		return;
	}

	if (id < PWM_ID_QTY) {
		reg_pwm->config[id].duty = con_duty;
		reg_pwm->config[id].period = con_period;
		reg_pwm->config[id].invert = invert;

		pwm_enable(id);
	}
}

void pwm_reinit(void)
{
	HWP_PWM_T reg;

	reg = *hwp_pwm;

	hwp_sys_ctrl->soft_reset_apb &= ~SOFT_RESET_APB_RSTN_SW_PWM_MSK;
	hwp_sys_ctrl->soft_reset_apb |= SOFT_RESET_APB_RSTN_SW_PWM_MSK;

	*hwp_pwm = reg;
}
