
#include "clock.h"
#include "hal_uart.h"
#include <stdio.h>

s32 main(void)
{
	u8 i;
	uart_cfg_t uart_cfg;

	clock_init();

	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;
	hal_uart_init(UART_ID_0, &uart_cfg);

	printf("*** sample code: hello airtouch. ***\r\n");

	for (i = 0U; i < 9U; i++) {
		printf("flash test loop: %d\r\n", i);
	}

	printf("*** end ***\r\n");

	while(1);
}
