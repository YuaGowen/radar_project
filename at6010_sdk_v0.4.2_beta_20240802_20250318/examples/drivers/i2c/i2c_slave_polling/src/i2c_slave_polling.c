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

#include "i2c_host.h"
#include "gpio.h"
#include "clock.h"
#include "hal_uart.h"

#define I2C_ID                  I2C_ID_0
#define I2C_SLAVE_ADDR          (0x55u)

static void printf_buf(const u8 *buf, u8 size)
{
	for (u8 i = 0U; i < size; i++) {
		(void)printf("%02X ", buf[i]);
	}
}

int main(void)
{
	u8 wr_data[8] = {0x00U, 0x11U, 0x22U, 0x33U, 0x44U, 0x55U, 0x66U, 0x77U};
	u8 rd_data[8] = {0U};
	i2c_err_t err;
	i2c_cfg_t i2c_cfg;
	gpio_cfg_t gpio_cfg;
	u8 recvd_size;
	uart_cfg_t uart_cfg;

	clock_init();
	sys_clk_enable_i2c();

	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;

	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	(void)printf("*** example code: i2c slave polling. ***\r\n");
	printf("Build: %s %s\r\n", __DATE__, __TIME__);

	i2c_cfg.mode = I2C_MODE_SLAVE;
	i2c_cfg.addr = I2C_SLAVE_ADDR;
	i2c_init(I2C_ID, &i2c_cfg);

	gpio_cfg.mode = GPIO_MODE_AF;
	gpio_cfg.pull = GPIO_PULL_UP;

	gpio_cfg.af = GPIO_AON3_AF1_I2C_SCL;
	(void)gpio_init(GPIO_AON_3, &gpio_cfg);

	gpio_cfg.af = GPIO_AON4_AF1_I2C_SDA;
	(void)gpio_init(GPIO_AON_4, &gpio_cfg);

	for (;;) {

		(void)printf("\r\ni2c write value, wait master...\r\n");
		printf_buf(wr_data, 8U);
		(void)printf("\r\n");
		err = i2c_slave_write_wait(I2C_ID, wr_data, 8U);
		if (I2C_ERR_NONE == err) {
			(void)printf("i2c write success!\r\n");
		}else{
			(void)printf("i2c write error %d\r\n", err);
		}

		(void)printf("\r\ni2c read value, wait master...\r\n");
		err = i2c_slave_read_wait(I2C_ID, rd_data, 8U, &recvd_size);
		if (I2C_ERR_NONE == err) {
			(void)printf("i2c read value: ");
			printf_buf(rd_data, recvd_size);
			(void)printf("\r\n");
		} else {
			(void)printf("i2c read error %d\r\n", err);
		}
	}
}
