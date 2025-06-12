
#include "at6010.h"
#include "clock.h"
#include "timer.h"
#include "hal_uart.h"

#define TIMER_SAMPLE  TIMER_ID_1
#define TIM_PERIOD_MS 1000U
#define TIMER_SOURCE  TIMER_SOURCE_SYSTEM_DIV_32

static u8 timer_cnt;

static void timer_callback(void *arg)
{
	(void)arg;
	printf("timer callback.\r\n");
	timer_cnt++;
	if (timer_cnt > 10U) {
		timer_stop(TIMER_SAMPLE);
		printf("timer stop.\r\n");
	}
}

s32 main(void)
{
	uart_cfg_t cfg;
	u8 div = 1U;

	sys_clk_enable_timer0();
	sys_clk_enable_timer1();
	sys_clk_enable_timer2();
	sys_clk_enable_timer3();

	clock_init();

	cfg.baud_rate = UART_BAUD_RATE_921600;
	cfg.parity = UART_PARITY_DISABLE;
	cfg.stop_bits = UART_STOP_BITS_1;
	cfg.data_width = UART_DATA_WIDTH_8BIT;
	cfg.fifo_cfg.fifo_en = 1U;

	(void)hal_uart_init(UART_ID_0, &cfg);

	printf("*** example code: timer source internal. ***\r\n");

	switch (TIMER_SOURCE) {
	case TIMER_SOURCE_SYSTEM: div = 1U; break;
	case TIMER_SOURCE_SYSTEM_DIV_16: div = 16U; break;
	case TIMER_SOURCE_SYSTEM_DIV_32: div = 32U; break;
	case TIMER_SOURCE_SYSTEM_DIV_64: div = 64U; break;
	default: printf("Invalid timer source!\r\n"); break;
	}

	timer_init(TIMER_SAMPLE, TIMER_SOURCE);
	timer_reload_set(TIMER_SAMPLE, TIM_PERIOD_MS * 40000U / div);
	timer_cb_set(TIMER_SAMPLE, timer_callback);
	timer_irq_enable(TIMER_SAMPLE);
	timer_start(TIMER_SAMPLE);

	for (;;) {}
}
