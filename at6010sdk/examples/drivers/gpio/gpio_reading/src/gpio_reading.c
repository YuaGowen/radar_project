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

#include <stdio.h>
#include "at6010.h"
#include "clock.h"
#include "gpio.h"
#include "hal_uart.h"
#include "hal_dualtimer.h"

#define TEST_GPIO_PIN                   GPIO_AON_5

int main(void)
{
	uart_cfg_t uart_cfg;
	gpio_cfg_t gpio_cfg;
	u8 tmp = 0U;

	clock_init();
	sys_clk_enable_gpio();
	hal_dualtimer_init();

	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	/* gpio initial. */
	gpio_cfg.mode = GPIO_MODE_INPUT;
	gpio_cfg.pull = GPIO_PULL_NONE;
	(void)gpio_init(TEST_GPIO_PIN, &gpio_cfg);

	(void)printf("*** example code: gpio_reading. ***\r\n");
	printf("Build: %s %s\r\n", __DATE__, __TIME__);

	for (;;) {
		tmp = gpio_get_value((u32)TEST_GPIO_PIN);
		printf("gpio value is %u\r\n", tmp);
		delay_ms(1000U);
	}
}
