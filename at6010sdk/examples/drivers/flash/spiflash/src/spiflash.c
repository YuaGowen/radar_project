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
#include "hal_uart.h"
#include "spi_flash_drv.h"

#define FLASH_OFFSET                    0x1F000U
#define FLASH_LINE_MODE                 SPIFLS_LINE_QUAD

int main (void)
{
	u8 reset_time;
	u32 rd_cnt, wr_cnt;
	spifls_err_id_t err;
	uart_cfg_t uart_cfg;

	clock_init();

	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	printf("*** sample code: spiflash. ***\r\n");
	printf("Build: %s %s\r\n", __DATE__, __TIME__);

	(void)spi_flash_init();
	(void)spifls_set_line_mode(FLASH_LINE_MODE);

	err = spi_flash_ll_read(FLASH_OFFSET, 1U, &rd_cnt, &reset_time);
	if (SPIFLS_ERR_NO != err) {
		printf("spiflash read error!\r\n");
	}

	reset_time++;
	printf("reset %d times\r\n", reset_time);

	err = spi_flash_erase(FLASH_OFFSET, (FLASH_OFFSET + 1U));
	if (SPIFLS_ERR_NO != err) {
		printf("spiflash erase error!\r\n");
	}

	err = spi_flash_write(FLASH_OFFSET, 1U, &wr_cnt, &reset_time);
	if (SPIFLS_ERR_NO != err) {
		printf("spiflash write error!\r\n");
	}
	printf("*** end ***\r\n");

	for (;;) {}
}
