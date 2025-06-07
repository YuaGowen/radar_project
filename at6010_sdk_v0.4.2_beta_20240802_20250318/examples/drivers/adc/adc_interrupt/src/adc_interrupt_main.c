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
#include "common.h"
#include "clock.h"
#include "aux_adc.h"
#include "gpio.h"
#include "hal_dualtimer.h"
#include "hal_uart.h"

/* Private Constants ---------------------------------------------------------*/

#define ADC_PIN                 GPIO_MCU_3 // select GPIO pin for ADC
#define ADC_BUF_SIZE            256U       // buffer size for ADC data

/* Private Data Types --------------------------------------------------------*/

typedef struct {
	u16 buf[ADC_BUF_SIZE];// ADC buffer
	u16 cnt;              // number of valid samples in buffer
	volatile u8 full;     // 1 if buffer is full, 0 otherwise
} app_adc_data_t;

/* Private Variables ---------------------------------------------------------*/

static app_adc_data_t s_app_adc_data = { 0 };

/* Private Function Declarations ---------------------------------------------*/

static void app_adc_sample_handler(u16 value);

static void app_print_adc_buf(void);

/*-------------------------------  MAIN  -------------------------------------*/

int main (void)
{
	gpio_cfg_t gpio_cfg;
	uart_cfg_t uart_cfg;

	/* clock init and module clock enable */
	clock_init();
	sys_clk_enable_dualtimer();
	sys_clk_enable_uart0();

	/* module init */
	hal_dualtimer_init();

	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	gpio_cfg.mode = GPIO_MODE_ANALOG;
	gpio_cfg.pull = GPIO_PULL_NONE;
	gpio_init(ADC_PIN, &gpio_cfg);

	adc_aux_init(ADC_AUX_DIR_EXTERNAL);

	printf("*** Aux ADC with Interrupt. ***\r\n");
	printf("Build: %s %s\r\n", __DATE__, __TIME__);

	adc_aux_irq_hdl_set(app_adc_sample_handler);
	adc_aux_start_cont(ADC_PIN);
	NVIC_EnableIRQ(AUX_ADC_IRQn);

	for (;;) {
		while (0U == s_app_adc_data.full) {}
		s_app_adc_data.full = 0U;

		app_print_adc_buf();
		NVIC_EnableIRQ(AUX_ADC_IRQn);
	}
}


/* Private Function Implementations ------------------------------------------*/

static void app_adc_sample_handler(u16 value)
{
	s_app_adc_data.buf[s_app_adc_data.cnt++] = value;
	if (s_app_adc_data.cnt >= ADC_BUF_SIZE) {
		s_app_adc_data.cnt = 0U;
		s_app_adc_data.full = 1U;

		NVIC_DisableIRQ(AUX_ADC_IRQn);
	}
}


static void app_print_adc_buf(void)
{
	for (u16 i = 0U; i < ADC_BUF_SIZE; i++) {
		printf("\r\n\t 0x%04x, %u", s_app_adc_data.buf[i], s_app_adc_data.buf[i]);
	}
}
