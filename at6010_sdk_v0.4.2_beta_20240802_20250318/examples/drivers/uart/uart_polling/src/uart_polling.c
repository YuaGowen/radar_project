#include "clock.h"
#include "hal_uart.h"
#include "hal_clock.h"

#define UART_ID       UART_ID_0
#define UART_BAUDRATE UART_BAUD_RATE_921600
#define UART_PARITY   UART_PARITY_DISABLE

s32 main(void)
{
	uart_cfg_t cfg;
	u8 info[] = "\r\n*** example code: uart polling. ***\r\n";
	u8 recv_byte;

	clock_init();

	cfg.baud_rate = UART_BAUDRATE;
	cfg.parity = UART_PARITY;
	cfg.stop_bits = UART_STOP_BITS_1;
	cfg.data_width = UART_DATA_WIDTH_8BIT;
	cfg.fifo_cfg.fifo_en = 1U;
	cfg.irq_enable = 0U;
	(void)hal_uart_init(UART_ID, &cfg);

	uart_send_str(UART_ID, info);

	for (;;) {
		recv_byte = uart_recv_byte(UART_ID);
		(void)uart_send_byte(UART_ID, recv_byte);
	}
}
