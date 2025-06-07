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

#include "at6010.h"
#include "clock.h"
#include "common.h"
#include "i2c_host.h"
#include "gpio.h"
#include "hal_clock.h"
#include "hal_uart.h"
#include "hal_dualtimer.h"

#define I2C_SLAVE_ADDR (0x28U)
#define I2C_MEM_ADDR   (0xE0U)

static void i2c_rx_cb(u8 data)
{
	(void)printf("i2c read value 0x%x!\r\n", data);
}

static void i2c_err_cb(i2c_err_t err)
{
	(void)printf("i2c tranmission error %d!\r\n", err);
}

static void i2c_tx_cplt_cb(void)
{
	(void)printf("i2c write completed!\r\n");
}

int main(void)
{
	u8 wr_data = 0U;
	uart_cfg_t uart_cfg;
	gpio_cfg_t gpio_cfg;
	i2c_hdl_t i2c_hdl;
	i2c_err_t err;

	hal_clk_init(CLK_SYS_40M_FLASH_20M_HZ);
	hal_dualtimer_init();
	sys_clk_enable_i2c();

	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	(void)printf("*** example code: i2c host interrupt. ***\r\n");
	printf("Build: %s %s\r\n", __DATE__, __TIME__);

	gpio_cfg.af = GPIO_AON3_AF1_I2C_SCL;
	gpio_cfg.mode = GPIO_MODE_AF;
	gpio_cfg.pull = GPIO_PULL_UP;

	(void)gpio_init(GPIO_AON_3, &gpio_cfg);

	gpio_cfg.af = GPIO_AON4_AF1_I2C_SDA;
	(void)gpio_init(GPIO_AON_4, &gpio_cfg);

	i2c_host_init();

	i2c_hdl.i2c_rx_hdl = i2c_rx_cb;
	i2c_hdl.i2c_err_hdl = i2c_err_cb;
	i2c_hdl.i2c_tx_cplt_hdl = i2c_tx_cplt_cb;
	i2c_set_hdl(I2C_ID_0, &i2c_hdl);

	i2c_irq_enable(I2C_IRQ_FULL);

	for (;;) {
		(void)printf("i2c write slave 0x%x mem 0x%x value 0x%x\r\n", I2C_SLAVE_ADDR, I2C_MEM_ADDR, wr_data);
		err = i2c_host_write_byte_int(I2C_SLAVE_ADDR, I2C_MEM_ADDR, wr_data);
		if (I2C_ERR_NONE != err) {
			(void)printf("i2c write error %d!\r\n", err);
		}

		hal_dualtimer_delay_ms(2000U);

		(void)printf("i2c read slave 0x%x mem 0x%x\r\n", I2C_SLAVE_ADDR, I2C_MEM_ADDR);
		err = i2c_host_read_byte_int(I2C_SLAVE_ADDR, I2C_MEM_ADDR);
		if (I2C_ERR_NONE != err) {
			(void)printf("i2c read error %d!\r\n", err);
		}

		wr_data++;
		hal_dualtimer_delay_ms(2000U);
	}
}
