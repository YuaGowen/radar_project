
#include "at6010.h"
#include "clock.h"
#include "timer.h"
#include "hal_uart.h"

#define TIMER_SAMPLE  TIMER_ID_2
#define TIM_PERIOD_MS 1000U

static u8 timer_cnt;

static void timer_callback(void *arg)
{
	(void)arg;
	printf("timer callback.\r\n");
	timer_cnt++;
	if (timer_cnt == 1) {
		timer_reload_set(TIMER_SAMPLE, TIM_PERIOD_MS * 32);
		timer_value_set(TIMER_SAMPLE, 0);
	}
	if (timer_cnt > 10U) {
		timer_stop(TIMER_SAMPLE);
		printf("timer stop.\r\n");
	}
}

s32 main(void)
{
	uart_cfg_t uart_cfg;

	clock_init();

	sys_clk_enable_timer0();
	sys_clk_enable_timer1();
	sys_clk_enable_timer2();
	sys_clk_enable_timer3();

	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;

	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	printf("*** example code: timer source external. ***\r\n");

	timer_init(TIMER_SAMPLE, TIMER_SOURCE_EXTERNAL_32K);
	timer_reload_set(TIMER_SAMPLE, TIM_PERIOD_MS * 16);
	timer_cb_set(TIMER_SAMPLE, timer_callback);
	timer_irq_enable(TIMER_SAMPLE);
	timer_start(TIMER_SAMPLE);

	for (;;) {}
}
