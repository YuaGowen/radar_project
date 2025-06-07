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

#include "hal_light.h"
#include "at6010.h"
#include "gpio.h"
#include "hal_pwm.h"
#include "atlog.h"
#include "hal_relay.h"

#define LIGHT_PWM_DIVIDER           7U

#if defined(CFG_PWRON_SELF_CHECK) && (PRIMARY_LIGHT_TYPE == LIGHT_TYPE_PWM)
#define LIGHT_PWM_INIT_DUTY PARA_PWM_LON_DUTY
#else
#define LIGHT_PWM_INIT_DUTY PARA_PWM_LOFF_DUTY
#endif

typedef struct {
	light_type_t light_type;
	u32 pin;
#ifdef CFG_LED_PWM
	u16 pwm_duty_current;
	u16 pwm_on_duty;
	u16 pwm_standby_duty;
#if (PARA_PWM_LUMACYCLE != LED_LUMACYCLE_LIGHT_NONE)
	u8 luma_cycle_en: 1;
	u16 luma_cycle_target;
#endif
#endif
	u8 light_status;
} hal_light_dev_t;

#ifdef CFG_LED_PWM
typedef struct {
	pwm_id_t id;
	u8 mux;
} gpio_pwm_t;

static const gpio_pwm_t gpio_pwm[] = {
        {PWM_ID_0,   3U}, /* GPIO_MCU_0 */
        {PWM_ID_1,   3U}, /* GPIO_MCU_1 */
        {PWM_ID_QTY, 0U}, /* GPIO_MCU_2 */
        {PWM_ID_QTY, 0U}, /* GPIO_MCU_3 */
        {PWM_ID_3,   1U}, /* GPIO_MCU_4 */
        {PWM_ID_4,   1U}, /* GPIO_MCU_5 */
        {PWM_ID_0,   2U}, /* GPIO_MCU_6 */
        {PWM_ID_1,   2U}, /* GPIO_MCU_7 */
        {PWM_ID_QTY, 0U}, /* GPIO_MCU_8 */
        {PWM_ID_QTY, 0U}, /* GPIO_MCU_9 */
        {PWM_ID_0,   0U}, /* GPIO_AON_0 */
        {PWM_ID_1,   0U}, /* GPIO_AON_1 */
        {PWM_ID_2,   0U}, /* GPIO_AON_2 */
        {PWM_ID_3,   2U}, /* GPIO_AON_3 */
        {PWM_ID_4,   2U}, /* GPIO_AON_4 */
        {PWM_ID_3,   3U}, /* GPIO_AON_5 */
        {PWM_ID_4,   3U}, /* GPIO_AON_6 */
};
#endif /* CFG_LED_PWM */

static hal_light_dev_t m_light_dev[LIGHT_NUM];

#ifdef CFG_LED_PWM

void hal_pwm_set(u8 id, u16 duty)
{
	logv("%s, duty=%d\r\n", __func__, duty);
	if (duty != m_light_dev[id].pwm_duty_current) {
		m_light_dev[id].pwm_duty_current = duty;
		pwm_duty_set((pwm_id_t)m_light_dev[id].pin, duty);
	}
}

void hal_pwm_duty_set(u8 id, u16 duty)
{
	m_light_dev[id].pwm_on_duty = duty;
}

u16 hal_pwm_duty_get(u8 id)
{
	return m_light_dev[id].pwm_duty_current;
}

void hal_pwm_standby_duty_set(u8 id, u16 duty)
{
	m_light_dev[id].pwm_standby_duty = duty;
}

#if (PARA_PWM_LUMACYCLE != LED_LUMACYCLE_LIGHT_NONE)
__STATIC_INLINE u16 get_lumacycle_next_duty(u16 current, u16 target)
{
	u16 duty;
	if (current > target) {
		duty = current - target;
		duty = (duty > LED_LUMACYCLE_STEP) ? (current - LED_LUMACYCLE_STEP) : target;
	} else if (current < target) {
		duty = target - current;
		duty = (duty > LED_LUMACYCLE_STEP) ? (current + LED_LUMACYCLE_STEP) : target;
	} else {
		duty = current;
	}
	return duty;
}

u8 hal_led_lumacycle_get_en(u8 id)
{
	return (u8)m_light_dev[id].luma_cycle_en;
}

void hal_led_lumacycle_proc(void)
{
	u16 current, target, duty = 0;
	for (u8 id = 0; id < LIGHT_NUM; id++) {
		if (1u == m_light_dev[id].luma_cycle_en) {
			current = m_light_dev[id].pwm_duty_current;
			target = m_light_dev[id].luma_cycle_target;
			duty = get_lumacycle_next_duty(current, target);
			if (duty != current) {
				hal_pwm_set(id, duty);
			} else {
				m_light_dev[id].luma_cycle_en = 0;
			}
		}
	}
}

__STATIC_INLINE void hal_led_lumacycle_set(u8 id, u8 on)
{
	m_light_dev[id].luma_cycle_en = on;
}
#endif /* PARA_PWM_LUMACYCLE */

__STATIC_INLINE void hal_light_pwm_set(u8 id, u8 onoff)
{
	if (HAL_LIGHT_SET_ON == onoff) {
#if (PARA_PWM_LUMACYCLE & LED_LUMACYCLE_LIGHT_ON)
		m_light_dev[id].luma_cycle_target = m_light_dev[id].pwm_on_duty;
		hal_led_lumacycle_set(id, 1);
#else
		hal_pwm_set(id, m_light_dev[id].pwm_on_duty);
#endif
	} else {
		u16 target_duty = (HAL_LIGHT_SET_OFF == onoff)
		                ? m_light_dev[id].pwm_standby_duty
		                : MAX((u16)PARA_PWM_LDIM_DUTY, m_light_dev[id].pwm_standby_duty);
#if (PARA_PWM_LUMACYCLE & LED_LUMACYCLE_LIGHT_OFF)
		m_light_dev[id].luma_cycle_target = target_duty;
		hal_led_lumacycle_set(id, 1);
#else
		hal_pwm_set(id, target_duty);
#endif
	}
}
#endif /* CFG_LED_PWM */

__STATIC_INLINE void hal_light_io_set(u8 id, u8 onoff)
{
#ifdef CFG_RELAY_ZERO_PROTECTION
	if (LIGHT_PRIMARY == id) {
		hal_relay_set_delay(onoff);
		return;
	}
#endif

#if (PARA_LIGHT_IO_INVERT == 1)
	onoff = !onoff;
#endif

	gpio_set_value(m_light_dev[id].pin, (s32)onoff);
}

#if (PARA_PWM_PHASE_SYNC == 1)
static void power_edge_sync_hdl(void)
{
	gpio_irq_clr_flag((u32)POWER_SAMPLE_PIN);
	pwm_reinit();
}

__STATIC_INLINE void power_sample_pin_init(void)
{
	gpio_cfg_t cfg;
	cfg.mode = GPIO_MODE_INPUT;
	cfg.pull = GPIO_PULL_NONE;
	(void)gpio_init(POWER_SAMPLE_PIN, &cfg);
	gpio_set_irq_type((u32)POWER_SAMPLE_PIN, IRQ_TYPE_EDGE_FALLING);
	gpio_irq_set_hdl((u32)POWER_SAMPLE_PIN, power_edge_sync_hdl);
	gpio_irq_enable((u32)POWER_SAMPLE_PIN);
	if (((u32)GPIO_MCU_0 <= POWER_SAMPLE_PIN) && (POWER_SAMPLE_PIN <= (u32)GPIO_MCU_6)) {
		NVIC_EnableIRQ((IRQn_Type)(POWER_SAMPLE_PIN + GPIO0_IRQn));
	}
	else {
		NVIC_EnableIRQ(GPIO_IRQn);
	}
}
#endif

void hal_light_init(u8 id, u32 gpio, light_type_t type)
{
	if (gpio >= NUM_OF_GPIO) {
		loge("[%s]Invalid gpio number.\r\n", __func__);
	} else {
		m_light_dev[id].light_type = type;
		m_light_dev[id].pin = gpio;

		if (type == LIGHT_TYPE_IO) {
			gpio_set_pinmux(gpio, 1);
			gpio_set_direction(gpio, 1);
#if (PARA_LIGHT_IO_INVERT == 1)
#ifndef CFG_PWRON_SELF_CHECK
			gpio_set_value(gpio, 1); /* light off(light io invert). */
#endif
#else /* (PARA_LIGHT_IO_INVERT == 0) */
#ifdef CFG_PWRON_SELF_CHECK
			gpio_set_value(gpio, 1); /* light on. */
#endif
#endif
		} else {
#ifdef CFG_LED_PWM
			gpio_set_pinmux(gpio, 0);
			gpio_set_alternate_function(gpio, gpio_pwm[gpio].mux);
			m_light_dev[id].pin = (u32)gpio_pwm[gpio].id;
			pwm_clk_divider_set(LIGHT_PWM_DIVIDER);
			pwm_init((pwm_id_t)m_light_dev[id].pin, LIGHT_PWM_INIT_DUTY, PARA_PWM_FREQ, PARA_PWM_INVERT);
			m_light_dev[id].pwm_on_duty = PARA_PWM_LON_DUTY;
			m_light_dev[id].pwm_standby_duty = PARA_PWM_LOFF_DUTY;
			m_light_dev[id].pwm_duty_current = PARA_PWM_LOFF_DUTY;
#endif
		}
	}
#if (PARA_PWM_PHASE_SYNC == 1)
	power_sample_pin_init();
#endif
}

u8 hal_light_get_status(u8 id)
{
	return m_light_dev[id].light_status;
}

void hal_light_set(u8 id, u8 onoff)
{
	logv("hal_light_set: %d\r\n", onoff);

	if (m_light_dev[id].light_type == LIGHT_TYPE_IO) {
		hal_light_io_set(id, onoff);
	} else if (m_light_dev[id].light_type == LIGHT_TYPE_PWM) {
#ifdef CFG_LED_PWM
		hal_light_pwm_set(id, onoff);
#endif
	} else {
		/* Do nothing */
	}
	m_light_dev[id].light_status = onoff;
}
