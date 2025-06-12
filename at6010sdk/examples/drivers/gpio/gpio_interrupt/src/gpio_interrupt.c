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

#define GPIO_INTR_PIN                   GPIO_MCU_9

/* use another pin to pull interrupt pin. */
#define GPIO_PULL_EN
#define GPIO_PULL_IO                    GPIO_MCU_0

static IRQn_Type irq_type[] = {
	GPIO0_IRQn, /* GPIO_MCU_0 */
	GPIO1_IRQn, /* GPIO_MCU_1 */
	GPIO2_IRQn, /* GPIO_MCU_2 */
	GPIO3_IRQn, /* GPIO_MCU_3 */
	GPIO4_IRQn, /* GPIO_MCU_4 */
	GPIO5_IRQn, /* GPIO_MCU_5 */
	GPIO6_IRQn, /* GPIO_MCU_6 */

	GPIO_IRQn,  /* GPIO_MCU_7 */
	GPIO_IRQn,  /* GPIO_MCU_8 */
	GPIO_IRQn,  /* GPIO_MCU_9 */
	GPIO_IRQn,  /* GPIO_ANO_6 */
	GPIO_IRQn,  /* GPIO_ANO_0 */
	GPIO_IRQn,  /* GPIO_ANO_1 */
	GPIO_IRQn,  /* GPIO_ANO_3 */
	GPIO_IRQn,  /* GPIO_ANO_4 */
	GPIO_IRQn,  /* GPIO_ANO_5 */
	GPIO_IRQn   /* GPIO_ANO_6 */
};

static void test_intr_hdl(void)
{
	u32 gpio_irq;
	u32 gpio_irq_rise;

	NVIC_DisableIRQ(irq_type[GPIO_INTR_PIN]);

	gpio_irq = gpio_irq_get_status(GPIO_INTR_PIN);
	gpio_irq_rise = gpio_irq_rise_get_status(GPIO_INTR_PIN);

	if (gpio_irq == 1U) {
		(void)printf("gpio intr.\r\n");
	}

	if (gpio_irq_rise == 1U) {
		(void)printf("rise edge irq.\r\n");
	} else {
		(void)printf("fall edge irq.\r\n");
	}

	gpio_irq_clr_flag(GPIO_INTR_PIN);

	NVIC_EnableIRQ(irq_type[GPIO_INTR_PIN]);
}

/*---------------------------------  MAIN  -----------------------------------*/

int main(void)
{
	u8 tmp = 0U;
	uart_cfg_t uart_cfg;

	clock_init();
	hal_dualtimer_init();

	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	printf("\r\n example code: gpio_interrupt \r\n");
	printf("Build: %s %s\r\n", __DATE__, __TIME__);

	sys_clk_enable_gpio();

	/* io configuration. */
	gpio_set_pinmux(GPIO_INTR_PIN, 1U);
	gpio_set_direction(GPIO_INTR_PIN, 0U);

	/* irq configuration. */
	if (GPIO_INTR_PIN == GPIO_AON_2) {
		printf("ERROR: GPIO_AON_2 has no interrupt!\r\n");
		while (1);
	}

	gpio_set_irq_type(GPIO_INTR_PIN, IRQ_TYPE_EDGE_BOTH);
	gpio_irq_set_hdl(GPIO_INTR_PIN, test_intr_hdl);
	gpio_irq_enable(GPIO_INTR_PIN);
	NVIC_EnableIRQ(irq_type[GPIO_INTR_PIN]);

#ifdef GPIO_PULL_EN
	gpio_set_pinmux(GPIO_PULL_IO, 1U);
	gpio_direction_output(GPIO_PULL_IO, 0U);
#endif

	for (;;) {
#ifdef GPIO_PULL_EN
		printf("------ pull io = %d ------\r\n", tmp);
		gpio_set_value(GPIO_PULL_IO, tmp);
#endif
		tmp = 1 - tmp;
		hal_dualtimer_delay_ms(500U);
	}
}
