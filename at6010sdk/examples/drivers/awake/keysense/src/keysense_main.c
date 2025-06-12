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
#include "hal_uart.h"
#include "key.h"
#include "gpio.h"
#include "aux_adc.h"
#include "hal_dualtimer.h"

/* Private Constants ---------------------------------------------------------*/

#define ADC_KEY_PIN                     (GPIO_AON_0)
#define ADC_KEY_IDLE_ADC                (2000U)
#define ADC_KEY_MARGIN                  (50U)

/* Private Types -------------------------------------------------------------*/

typedef struct {
	u8 key_id;
	u16 adc;
} key_adc_cal_t;

/* Private Variables ---------------------------------------------------------*/

static u8 m_key_pressed;

static const key_adc_cal_t adc_cal[] = {
        {1U, 0U  },
        {2U, 160U},
        {3U, 350U},
        {4U, 500U},
        {5U, 700U},
        {6U, 920U},
};

/* Private Function Declarations ---------------------------------------------*/

static u8   get_adc_key_num(void);
static void key_intr_hdl(void);

/*---------------------------------  MAIN  -----------------------------------*/

int main(void)
{
	key_cfg_t key_cfg;
	uart_cfg_t uart_cfg;

	/* Clock init */
	clock_init();
	sys_clk_enable_gpio();
	hal_dualtimer_init();

	/* Uart init */
	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	(void)printf("*** example code: keysense. ***\r\n");
	printf("Build: %s %s\r\n", __DATE__, __TIME__);

	/* GPIO init */
	gpio_set_pinmux(ADC_KEY_PIN, 1u);
	gpio_set_rd_en(ADC_KEY_PIN, 0u);
	gpio_set_analog_en(ADC_KEY_PIN, 0u);
	gpio_set_direction(ADC_KEY_PIN, 0u);
	gpio_set_pull_down(ADC_KEY_PIN, 0u);
	gpio_set_pull_up(ADC_KEY_PIN, 0u);

	/* Aux ADC init */
	adc_aux_init(ADC_AUX_DIR_EXTERNAL);

	/* Key init */
	key_cfg.en = 0;
	key_cfg.en_sel = 1;
	key_cfg.invert = 1;
	key_cfg.smooth_en = 0;
	key_init(&key_cfg);

	key_irq_set_hdl(key_intr_hdl);
	key_irq_enable();
	NVIC_EnableIRQ(KEY_IRQn);

	key_enable();

	for (;;) {
		delay_ms(5000);
	}
}

/* Private Function Implementations ------------------------------------------*/

static u8 get_adc_key_num(void)
{
	u8 num = 0xFFu;
	u16 adc, lower, upper;

	adc = (u16)adc_get_aux_voltage(ADC_KEY_PIN);
	if (adc > ADC_KEY_IDLE_ADC) {
		num = 0u;
	} else {
		for (u8 i = 0u; i < ARRAY_SIZE(adc_cal); i++) {
			lower = (adc_cal[i].adc > ADC_KEY_MARGIN) ? (adc_cal[i].adc - ADC_KEY_MARGIN) : 0u;
			upper = (adc_cal[i].adc + ADC_KEY_MARGIN);
			if ((adc <= upper) && (adc >= lower)) {
				num = adc_cal[i].key_id;
				break;
			}
		}
	}
	(void)printf("Key adc = %d, id = %d pressed!\n\r", adc, num);
	return num;
}

static void key_intr_hdl(void)
{
	NVIC_DisableIRQ(KEY_IRQn);
	key_disable();

	u8 level = 1U ^ key_get_inv();
	if (0U == level) {
		m_key_pressed = get_adc_key_num();
	} else {
		(void)printf("key %d released!\r\n", m_key_pressed);
	}

	key_set_inv_inverse();

	key_enable();
	NVIC_EnableIRQ(KEY_IRQn);
}
