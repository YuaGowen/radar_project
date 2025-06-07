
#include "at6010.h"
#include "clock.h"
#include "hal_timer.h"
#include "hal_uart.h"
#include "hal_dualtimer.h"

#define TIMER_SAMPLE  TIMER_ID_3
#define TIMER_SOURCE  TIMER_SOURCE_EXTERNAL_32K
#define TIM_PERIOD_MS 1000U

static u8 timer_cnt;

static void timer_callback(void *arg)
{
	printf("timer callback.\r\n");
	timer_cnt++;
	if (timer_cnt > 10U) {
		hal_timer_stop(TIMER_SAMPLE);
		printf("timer stop.\r\n");
	}
}

s32 main(void)
{
	uart_cfg_t cfg;
	timer_cfg_t timer_cfg;

	clock_init();
	hal_dualtimer_init();

	cfg.baud_rate = UART_BAUD_RATE_921600;
	cfg.parity = UART_PARITY_DISABLE;
	cfg.stop_bits = UART_STOP_BITS_1;
	cfg.data_width = UART_DATA_WIDTH_8BIT;
	cfg.fifo_cfg.fifo_en = 1U;

	(void)hal_uart_init(UART_ID_0, &cfg);

	printf("*** example code: timer counter. ***\r\n");

	timer_cfg.source = TIMER_SOURCE;
	timer_cfg.value_mode = TIMER_VALUE_INC;
	timer_cfg.irq_en = 0U;
	timer_cfg.start = 1U;
	hal_timer_init(TIMER_SAMPLE, &timer_cfg);

	for (;;) {
		printf("current value: %u\r\n", hal_timer_value_get(TIMER_SAMPLE));
		hal_dualtimer_delay_ms(1000U);
	}
}
