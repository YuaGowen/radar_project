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

#include "hal_clock.h"
#include "hal_uart.h"
#include "hal_dualtimer.h"
#include "gpio.h"
#include "clock.h"
#include "pwm.h"

#define PWM_FREQ                        (1000U)

int main(void)
{
	u16 duty = 0U;
	gpio_cfg_t gpio_cfg;
	uart_cfg_t uart_cfg;

	hal_clk_init(CLK_SYS_80M_FLASH_80M_HZ);
	hal_dualtimer_init();

	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	(void)printf("*** example code: pwm. ***\r\n");
	printf("Build: %s %s\r\n", __DATE__, __TIME__);

	sys_clk_enable_pwm();
	pwm_clk_divider_set(4U);

	gpio_cfg.mode = GPIO_MODE_AF;
	gpio_cfg.pull = GPIO_PULL_NONE;
	gpio_cfg.af = GPIO_AON0_AF0_PWM0;
	(void)gpio_init(GPIO_AON_0, &gpio_cfg);

	gpio_cfg.af = GPIO_AON1_AF0_PWM1;
	(void)gpio_init(GPIO_AON_1, &gpio_cfg);

	gpio_cfg.af = GPIO_AON2_AF0_PWM2;
	(void)gpio_init(GPIO_AON_2, &gpio_cfg);

	gpio_cfg.af = GPIO_AON3_AF2_PWM3;
	(void)gpio_init(GPIO_AON_3, &gpio_cfg);

	gpio_cfg.af = GPIO_AON4_AF2_PWM4;
	(void)gpio_init(GPIO_AON_4, &gpio_cfg);

	pwm_init(PWM_ID_0, 0U, PWM_FREQ, 0U);
	pwm_init(PWM_ID_1, 100U, PWM_FREQ, 0U);
	pwm_init(PWM_ID_2, 200U, PWM_FREQ, 1U);
	pwm_init(PWM_ID_3, 300U, PWM_FREQ, 1U);
	pwm_init(PWM_ID_4, 400U, PWM_FREQ, 0U);

	for (;;) {
		duty = (duty >= 1000U) ? 0U : (duty + 100U);
		pwm_duty_set(PWM_ID_0, duty);
		hal_dualtimer_delay_ms(2000U);
	}
}
