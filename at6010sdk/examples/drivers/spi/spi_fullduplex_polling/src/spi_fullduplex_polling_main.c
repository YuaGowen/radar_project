/**
 *******************************************************************************
 * @file    spi_fullduplex_polling_main.c
 * @author  Airtouch Software Team
 * @brief   This sample code shows how to use AT6010 SPI HAL API to transmit and
 *          receive a data buffer with a communication process based on Polling
 *          transfer.
 *          The communication is done using 2 boards.
 *******************************************************************************
 * @copyright Copyright (c) 2023, Airtouching Intelligence Technology.
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
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "clock.h"
#include "gpio.h"
#include "hal_spi.h"
#include "hal_uart.h"
#include "hal_dualtimer.h"
#include <stdio.h>

/* Private Constants ---------------------------------------------------------*/

/**
 * @def BOARD_SPI_MASTER
 * @brief Define the board role is master. If commented, the board role is slave.
 */
#define BOARD_SPI_MASTER

/**
 * @def BUFFER_SIZE
 * @brief Define SPI TX && RX buffer size
 */
#define BUFFER_SIZE                 (1024U)

/* Function Declarations -----------------------------------------------------*/
void  app_spi_mspinit_callback(spi_handle_t *hspi);

void  app_init_buffer_u8(u8 *txbuf, u8 *rxbuf, u32 len);
int32 app_cmp_buffer_u8(u8 *txbuf, u8 *rxbuf, u32 len);


/* Global Variables ----------------------------------------------------------*/

static spi_handle_t spi_handle_polling;

static u8 tx_buffer[BUFFER_SIZE];
static u8 rx_buffer[BUFFER_SIZE];

static u32 pass_cnt = 0U;

/*---------------------------------  MAIN  -----------------------------------*/

int main(void)
{
	hal_status_t hal_status;

	clock_init();
	clk_mcu_set(CLK_SRC_PLL, 80000000U);
	hal_dualtimer_init();

	uart_cfg_t uart_cfg;
	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	printf("\r\n");
#if defined(BOARD_SPI_MASTER)
	printf("#     #                                  \r\n");
	printf("##   ##   ##    ####  ##### ###### ##### \r\n");
	printf("# # # #  #  #  #        #   #      #    #\r\n");
	printf("#  #  # #    #  ####    #   #####  #    #\r\n");
	printf("#     # ######      #   #   #      ##### \r\n");
	printf("#     # #    # #    #   #   #      #   # \r\n");
	printf("#     # #    #  ####    #   ###### #    #\r\n");
	printf("                                         \r\n");
#else
	printf(" #####                              \r\n");
	printf("#     # #        ##   #    # ###### \r\n");
	printf("#       #       #  #  #    # #      \r\n");
	printf(" #####  #      #    # #    # #####  \r\n");
	printf("      # #      ###### #    # #      \r\n");
	printf("#     # #      #    #  #  #  #      \r\n");
	printf(" #####  ###### #    #   ##   ###### \r\n");
	printf("                                    \r\n");
#endif
	printf("Build: %s %s\r\n", __DATE__, __TIME__);


	/* SPI handle init */
#if defined(BOARD_SPI_MASTER)
	spi_handle_polling.init.mode = SPI_MODE_MASTER;
#else
	spi_handle_polling.init.mode = SPI_MODE_SLAVE;
#endif

	spi_handle_polling.instance = SPI0;
	spi_handle_polling.init.tmod = SPI_TMOD_TX_RX;
	spi_handle_polling.init.dfs = SPI_DFS_8_BIT;
	spi_handle_polling.init.baudrate = 20 * 1000000;
	spi_handle_polling.init.clk_pol = SPI_CLK_POL_HIGH;
	spi_handle_polling.init.clk_pha = SPI_CLK_PHA_1EDGE;
	spi_handle_polling.init.frame_protocol = SPI_FRAME_PROTOCOL_MOTOROLA_SPI;
	spi_handle_polling.init.spi_frf = SPI_FRF_STANDARD;

	hal_status = hal_spi_register_callback(&spi_handle_polling, HAL_SPI_MSPINIT_CB_ID, app_spi_mspinit_callback);
	if (HAL_OK != hal_status) {
		printf("Error: hal_spi_register_callback, status = %u\r\n", hal_status);
		while (1);
	}

	hal_status = hal_spi_init(&spi_handle_polling);
	if (HAL_OK != hal_status) {
		printf("Error: hal_spi_init, status = %u\r\n", hal_status);
		while (1);
	}

	while (1) {
		/* re-init SPI RX buffer */
		app_init_buffer_u8((u8 *)tx_buffer, (u8 *)rx_buffer, BUFFER_SIZE);

		/* start communication  */
		hal_status = hal_spi_transmit_receive(&spi_handle_polling, (uint8 *)tx_buffer, (uint8 *)rx_buffer, BUFFER_SIZE, 0xFFFF);
		if (HAL_OK != hal_status) {
			printf("Error: hal_spi_init, status = %u\r\n", hal_status);
			while (1);
		}

		/* compare TX && RX buffer */
		if (0 != app_cmp_buffer_u8((u8 *)tx_buffer, (u8 *)rx_buffer, BUFFER_SIZE)) {
			printf("Error: app_cmp_buffer_u8\r\n");
			while (1);
		}

		/* Dump buffer */
		printf("\r\ntx_buffer: %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X\r\n",
		tx_buffer[0], tx_buffer[1], tx_buffer[2], tx_buffer[3], tx_buffer[4], tx_buffer[5], tx_buffer[6],
		tx_buffer[7], tx_buffer[8], tx_buffer[9], tx_buffer[10], tx_buffer[11], tx_buffer[12], tx_buffer[13],
		tx_buffer[14], tx_buffer[15]);

		printf("\r\nrx_buffer: %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X\r\n",
		rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3], rx_buffer[4], rx_buffer[5], rx_buffer[6],
		rx_buffer[7], rx_buffer[8], rx_buffer[9], rx_buffer[10], rx_buffer[11], rx_buffer[12], rx_buffer[13],
		rx_buffer[14], rx_buffer[15]);

		pass_cnt++;
		printf("SPI pass_cnt = %u\r\n", pass_cnt);

		delay_ms(1000);
	}
}

/* Function Implementations --------------------------------------------------*/

void app_spi_mspinit_callback(spi_handle_t *hspi)
{
	gpio_cfg_t gpio_cfg = { 0 };

	/* SPI clock init */
	sys_clk_enable_spi0();

	/* GPIO init */
	sys_clk_enable_gpio();

	gpio_cfg.mode = GPIO_MODE_AF;
	gpio_cfg.pull = GPIO_PULL_UP;

	/* SPI clock pin */
	gpio_cfg.af = GPIO_MCU0_AF0_SPI_CLK;
	gpio_init(GPIO_MCU_0, &gpio_cfg);

	/* SPI cs pin */
	gpio_cfg.af = GPIO_MCU1_AF0_SPI_CSN;
	gpio_init(GPIO_MCU_1, &gpio_cfg);

#ifdef BOARD_SPI_MASTER
	/* SPI MOSI pin */
	gpio_cfg.af = GPIO_AON0_AF1_SPI_IO0; /* MOSI -- GPIO_AON_0 */
	gpio_init(GPIO_AON_0, &gpio_cfg);

	/* SPI MISO pin */
	gpio_cfg.af = GPIO_AON1_AF1_SPI_IO1; /* MISO -- GPIO_AON_1 */
	gpio_init(GPIO_AON_1, &gpio_cfg);

	/* NOTE: force GPIO_MCU_2/3 --> GPIO or other alter functions. */
	gpio_cfg.mode = GPIO_MODE_OUTPUT;
	gpio_init(GPIO_MCU_2, &gpio_cfg);
	gpio_init(GPIO_MCU_3, &gpio_cfg);
#else
	/* SPI MOSI pin */
	gpio_cfg.af = GPIO_MCU2_AF0_SPI_IO0; /* MOSI -- GPIO_MCU_2 */
	gpio_init(GPIO_MCU_2, &gpio_cfg);

	/* SPI MISO pin */
	gpio_cfg.af = GPIO_MCU3_AF0_SPI_IO1; /* MISO -- GPIO_MCU_3 */
	gpio_init(GPIO_MCU_3, &gpio_cfg);
#endif

}

void  app_init_buffer_u8(u8 *txbuf, u8 *rxbuf, u32 len)
{
	for (u32 i = 0; i < len; i++) {
		*(txbuf + i) = i & 0xFF;
		*(rxbuf + i) = 0;
	}
}

int32 app_cmp_buffer_u8(u8 *txbuf, u8 *rxbuf, u32 len)
{
	for (u32 i = 0; i < len; i++) {
		if (*(txbuf + i) != *(rxbuf + i)) {
			return 1;
		}
	}

	return 0;
}
