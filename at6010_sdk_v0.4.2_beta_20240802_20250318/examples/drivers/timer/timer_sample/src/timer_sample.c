
#include "hal_timer.h"
#include "hal_uart.h"
#include "hal_clock.h"
#include <stdio.h>

#define TIMER_SAMPLE                    TIMER_ID_3
#define TIM_PERIOD_MS                   1000u

static u8 timer_cnt;

void timer_callback(void *arg)
{
	printf("timer callback.\r\n");
	timer_cnt++;
	if (timer_cnt > 10u) {
		hal_timer_stop(TIMER_SAMPLE);
		printf("timer stop.\r\n");
	}
}

int main(void)
{
	uart_cfg_t cfg;
	timer_cfg_t timer_cfg;

	hal_clk_init(CLK_SYS_80M_FLASH_80M_HZ);

	cfg.baud_rate = UART_BAUD_RATE_921600;
	cfg.data_width = UART_DATA_WIDTH_8BIT;
	cfg.stop_bits = UART_STOP_BITS_1;
	cfg.parity = UART_PARITY_DISABLE;
	cfg.fifo_cfg.fifo_en = 1U;

	(void)hal_uart_init(UART_ID_0, &cfg);

	printf("*** example code: timer. ***\r\n");

	hal_timer_deinit(TIMER_SAMPLE);

	timer_cfg.source = TIMER_SOURCE_SYSTEM;
	timer_cfg.irq_en = 1U;
	hal_timer_init(TIMER_SAMPLE, &timer_cfg);

	hal_timer_set(TIMER_SAMPLE, TIM_PERIOD_MS * 1000U, timer_callback);
	hal_timer_start(TIMER_SAMPLE);

	for (;;) {}
}
