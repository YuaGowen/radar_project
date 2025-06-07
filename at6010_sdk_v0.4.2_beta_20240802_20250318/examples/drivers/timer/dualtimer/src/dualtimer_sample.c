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
#include "hal_uart.h"
#include "clock.h"
#include "hal_dualtimer.h"

#define TIMEOUT_MS 1000U

static volatile u32 notify_occur = 0U;

static void dualtimer_notify()
{
	printf("timeout!\n\r");
	notify_occur++;
	return;
}

int main()
{
	uart_cfg_t cfg;
	u32 i = 10U, reconfig = 0U, cnt = 0U, elapsed = 0U;

	clock_init();

	cfg.baud_rate = UART_BAUD_RATE_921600;
	cfg.parity = UART_PARITY_DISABLE;
	cfg.stop_bits = UART_STOP_BITS_1;
	cfg.data_width = UART_DATA_WIDTH_8BIT;
	cfg.fifo_cfg.fifo_en = 1U;

	(void)hal_uart_init(UART_ID_0, &cfg);

	hal_dualtimer_init();

	cnt = hal_get_start_cnt_value();
	printf("start dualtimer count test  1 \n\r");
	printf("start dualtimer count test  2 \n\r");

	elapsed = hal_get_elapsed_time_us(cnt);
	printf("dualtimer elapsed %uus \n\r", elapsed);


	while (i--) {
		hal_dualtimer_delay_ms(1000U);
		printf("dualtimer delay test %d\n\r", i);
	}

	printf(" create a dualtimer with one shot mode. \n\r");
	hal_dualtimer_config(DUALTIMER_TIM_0, DUALTIMER_ONE_SHOT_MODE, TIMEOUT_MS * 1000U, dualtimer_notify);
	hal_dualtimer_start(DUALTIMER_TIM_0);

	while (notify_occur == 0U) {}
	notify_occur = 0U;

	printf("reconfig timer counter time,and auto restart. \n\r");
	hal_dualtimer_set_time(DUALTIMER_TIM_0, DUALTIMER_SET_CUR_LOAD, 520000U);

	while (notify_occur == 0U) {}
	notify_occur = 0U;

	printf("dualtimer set period mode test\n\r");
	hal_dualtimer_config(DUALTIMER_TIM_0, DUALTIMER_PERIODIC_MODE, TIMEOUT_MS * 100U, dualtimer_notify);
	hal_dualtimer_start(DUALTIMER_TIM_0);

	for (;;) {
		if ((notify_occur > 10U) && (reconfig == 0U)) {
			reconfig = 1U;
			printf("reconfig timer counter time. \n\r");
			hal_dualtimer_set_time(DUALTIMER_TIM_0, DUALTIMER_SET_BG_LOAD, TIMEOUT_MS * 100U);
		} else if ((notify_occur > 20U) && (reconfig == 1U)) {
			break;
		}
	}

	hal_dualtimer_stop(DUALTIMER_TIM_0);
	printf("dualtimer test done. \n\r");

	for (;;) {}
}
