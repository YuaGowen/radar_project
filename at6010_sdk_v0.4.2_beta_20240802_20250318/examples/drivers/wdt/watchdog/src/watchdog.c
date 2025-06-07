
#include "at6010.h"
#include "clock.h"
#include "common.h"
#include "gpio.h"
#include "hal_dualtimer.h"
#include "hal_uart.h"
#include "wtd.h"

#define FEED_DOG_PIN GPIO_MCU_2
#define FEED_DOG_MS  3000U

int main(void)
{
	gpio_cfg_t gpio_cfg;
	uart_cfg_t cfg;

	clock_init();
	sys_clk_enable_gpio();
	hal_dualtimer_init();

	cfg.baud_rate = UART_BAUD_RATE_921600;
	cfg.parity = UART_PARITY_DISABLE;
	cfg.stop_bits = UART_STOP_BITS_1;
	cfg.data_width = UART_DATA_WIDTH_8BIT;
	cfg.fifo_cfg.fifo_en = 1U;
	(void)hal_uart_init(UART_ID_0, &cfg);

	(void)printf("*** example code: watchdog. ***\r\n");

	gpio_cfg.mode = GPIO_MODE_INPUT;
	gpio_cfg.pull = GPIO_PULL_UP;
	(void)gpio_init(FEED_DOG_PIN, &gpio_cfg);

	wtd_init(FEED_DOG_MS);
	wtd_enable();

	for (;;) {
		if (0U == gpio_get_value((u32)FEED_DOG_PIN)) {
			wtd_feed(FEED_DOG_MS);
			printf("feed dog!\r\n");
		}
		delay_ms(100U);
	}
}
