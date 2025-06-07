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
#include "aux_adc.h"
#include "gpio.h"
#include "light.h"
#include "ana.h"
#include "ana_intf.h"
#include "exti.h"
#include "key.h"
#include "pmu.h"
#include "hal_dualtimer.h"
#include "hal_uart.h"
#include "hal_clock.h"

/* Private Constants ---------------------------------------------------------*/

#define WAKEUP_METHOD          2 /* 0: use source 1: use gpio 2: use both */
#define WAKEUP_SOURCE          PMU_WAKEUP_ALL
#define WAKEUP_GPIO_MASK       0x78U /* NOTE: disable aon1 if use LIGHT, disable aon0 if use KEY */

#define LS_PIN                 GPIO_AON_1
#define LS_ADC_TH              2000U
#define KEY_PIN                ((u32)GPIO_AON_0)
#define TIMER_SLEEP_MS         5000U

/* Private Types -------------------------------------------------------------*/

typedef void (*wakeup_setup_t)(void);

/* Private Function Declarations ---------------------------------------------*/

static void wakeup_setup_light(void);
static void wakeup_setup_key(void);
static void wakeup_setup_timer(void);
static void wakeup_setup_gpio(u16 mask);

static void wakeup_setup_source(pmu_wakeup_t source);
static void wakeup_log(void);

/* Private Variables ---------------------------------------------------------*/
static const wakeup_setup_t setup_func[] = {
	wakeup_setup_light,
	wakeup_setup_key,
	wakeup_setup_timer
};


/*---------------------------------  MAIN  -----------------------------------*/

int main(void)
{
	uart_cfg_t uart_cfg;
	gpio_cfg_t gpio_cfg;

	/* Clock init */
	hal_clk_init(CLK_SYS_40M_FLASH_40M_HZ);

	/* Dual Timer init */
	hal_dualtimer_init();
	NVIC_DisableIRQ(DUALTIMER_IRQn);

	/* GPIO init */
	sys_clk_enable_gpio();
	gpio_cfg.mode = GPIO_MODE_AF;
	gpio_cfg.af = GPIO_MCU6_AF0_UART0_TXD;
	(void)gpio_init(GPIO_MCU_6, &gpio_cfg);

	/* UART init */
	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.fifo_cfg.fifo_en = 1;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	printf("\r\n*** sample code: wakeup. ***\r\n");
	printf("Build: %s %s\r\n", __DATE__, __TIME__);

	wakeup_log();
	printf("enter sleep mode after 3 seconds...\r\n");

	hal_dualtimer_delay_ms(2000U);
	printf("enter sleep mode!\r\n");

	wakeup_setup_source(WAKEUP_SOURCE);
	wakeup_setup_gpio(WAKEUP_GPIO_MASK);

	hal_dualtimer_delay_ms(1000U);
	pmu_retention_select(PMU_RETENTION_ALL);
	pmu_enter_sleep();

	for (;;) {}
}

/* Private Function Implementations ------------------------------------------*/

static void wakeup_setup_light(void)
{
	gpio_cfg_t gpio_cfg;

	adc_aux_init(ADC_AUX_DIR_INTERNAL);
	light_init();

	gpio_cfg.mode = GPIO_MODE_ANALOG;
	gpio_cfg.pull = GPIO_PULL_NONE;
	(void)gpio_init(LS_PIN, &gpio_cfg);
	gpio_set_analog_en((u32)LS_PIN, 1U);

	light_set_th(LS_ADC_TH, LS_ADC_TH);
	/* NOTE: set sleep cnt small and disable smooth to speed up */
	light_set_sleep_cnt(0x10U);
	light_enable();

	printf("To awake, make adc > %u on LS pin(P%d)!\r\n", LS_ADC_TH, LS_PIN);
}

static void wakeup_setup_key(void)
{
	key_cfg_t key_cfg;

	gpio_set_pinmux(KEY_PIN, 1U);
	gpio_set_rd_en(KEY_PIN, 0U);
	gpio_set_analog_en(KEY_PIN, 0U);
	gpio_set_direction(KEY_PIN, 0U);
	gpio_set_pull_down(KEY_PIN, 0U);
	gpio_set_pull_up(KEY_PIN, 1U);

	key_cfg.en = 1;
	key_cfg.en_sel = 0;
	key_cfg.invert = 1;
	key_cfg.smooth_en = 1;
	key_init(&key_cfg);

	printf("To awake, make voltage < 2V on KEY pin(P%u)!\r\n", KEY_PIN);
}

static void wakeup_setup_timer(void)
{
	pmu_set_wakeup_time(TIMER_SLEEP_MS);

	printf("To awake, wait %u ms...\r\n", TIMER_SLEEP_MS);
}

static void wakeup_setup_gpio(u16 mask)
{
	gpio_cfg_t gpio_cfg;
	u8 i = 0U;

	pmu_disable_gpio_wakeup(0xFFU);

#if WAKEUP_METHOD != 0
	pmu_enable_gpio_wakeup((u8)mask);

	printf("To awake, make gpio aon 0x%04X high!\r\n", mask);

	gpio_cfg.mode = GPIO_MODE_INPUT;
	gpio_cfg.pull = GPIO_PULL_DOWN;
	while (mask > 0U) {
		if ((mask & 1U) > 0U) {
			(void)gpio_init((gpio_pin_t)(i + GPIO_AON_0), &gpio_cfg);
		}
		mask >>= 1;
		i++;
	}
#endif
}

static void wakeup_setup_source(pmu_wakeup_t source)
{
	pmu_disable_wakeup_source(PMU_WAKEUP_ALL);

#if WAKEUP_METHOD != 1
	switch (source) {
	case PMU_WAKEUP_LIGHT:
		setup_func[0]();
		break;

	case PMU_WAKEUP_KEY:
		setup_func[1]();
		break;

	case PMU_WAKEUP_TIMER:
		setup_func[2]();
		break;

	case PMU_WAKEUP_ALL:
		for (u8 i = 0U; i < ARRAY_SIZE(setup_func); i++) {
			setup_func[i]();
		}
		break;

	default: /* Invalid source */
		break;
	}

	pmu_enable_wakeup_source(source);
#endif
}

static void wakeup_log(void)
{
	u8 buf[2];
	u32 wakeup_cnt;

	/* Get 16bits data from PMU Software area */
	buf[0] = pmu_get_sw_data(30U); // lower  8bits
	buf[1] = pmu_get_sw_data(31U); // higher 8bits
	wakeup_cnt = ((u32)buf[1] << 8) + buf[0];

	printf("Wakeup %u times.\r\n", wakeup_cnt);

	wakeup_cnt++;

	/* Save 16bits data to PMU Software area */
	buf[0] = (u8)(wakeup_cnt & 0xFFU);
	buf[1] = (u8)((wakeup_cnt >> 8) & 0xFFU);
	pmu_set_sw_data(30U, buf[0]);
	pmu_set_sw_data(31U, buf[1]);
}
