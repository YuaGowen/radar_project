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

#include "clock.h"
#include "i2c_host.h"
#include "hal_uart.h"
#include "gpio.h"

#define I2C_ID                  I2C_ID_0
#define I2C_SLAVE_ADDR          (0x37u)
#define I2C_SCK                 400000U

static volatile u8 i2c_rx_cnt;
static u8 i2c_rx_buf[20];
static volatile u8 tx_cplt;

static void i2c_rx_cb(u8 data)
{
	i2c_rx_buf[i2c_rx_cnt++] = data;
	if (i2c_rx_cnt >= 8U) {
		i2c_slave_read_int_stop(I2C_ID);
	}
}

static void i2c_tx_cplt_cb(void)
{
	tx_cplt = 1U;
}

static void printf_buf(const u8 *buf, u8 size)
{
	for (u8 i = 0U; i < size; i++) {
		(void)printf("%02X ", buf[i]);
	}
	(void)printf("\r\n");
}

int main(void)
{
	u8 wr_data[8] = {0x00U, 0x11U, 0x22U, 0x33U, 0x44U, 0x55U, 0x66U, 0x77U};
	i2c_cfg_t i2c_cfg;
	i2c_hdl_t i2c_hdl;
	gpio_cfg_t gpio_cfg;
	uart_cfg_t uart_cfg;

	clock_init();
	sys_clk_enable_i2c();

	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;

	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	(void)printf("*** example code: i2c slave interrupt. ***\r\n");
	printf("Build: %s %s\r\n", __DATE__, __TIME__);

	gpio_cfg.mode = GPIO_MODE_AF;
	gpio_cfg.pull = GPIO_PULL_UP;

	gpio_cfg.af = GPIO_AON3_AF1_I2C_SCL;
	(void)gpio_init(GPIO_AON_3, &gpio_cfg);

	gpio_cfg.af = GPIO_AON4_AF1_I2C_SDA;
	(void)gpio_init(GPIO_AON_4, &gpio_cfg);

	i2c_cfg.mode = I2C_MODE_SLAVE;
	i2c_cfg.addr = I2C_SLAVE_ADDR;
	i2c_cfg.sck = I2C_SCK;
	i2c_init(I2C_ID, &i2c_cfg);

	i2c_hdl.i2c_rx_hdl = i2c_rx_cb;
	i2c_hdl.i2c_tx_cplt_hdl = i2c_tx_cplt_cb;
	i2c_set_hdl(I2C_ID, &i2c_hdl);
	i2c_irq_enable(I2C_IRQ_FULL);

	for (;;) {
		(void)printf("wait i2c master send 8 bytes...\n");
		i2c_slave_read_int_start(I2C_ID);

		while (i2c_rx_cnt < 8U) {}
		(void)printf("i2c recv: ");
		printf_buf(i2c_rx_buf, i2c_rx_cnt);
		i2c_rx_cnt = 0U;

		(void)printf("wait i2c master read 8 bytes...\n\n");
		i2c_slave_write_int(I2C_ID, wr_data, 8U);

		while (tx_cplt == 0U) {}
		tx_cplt = 0U;
	}
}
