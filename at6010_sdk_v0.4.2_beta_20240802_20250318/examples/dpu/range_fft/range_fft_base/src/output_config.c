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

#include "output_config.h"
#include "clock.h"
#include "hal_dma.h"
#include "hal_uart.h"
#include "gpio.h"
#include "clk_cfg.h"
#include "hal_spi.h"

#define ADC_OUTPUT_PORT_SEL   0 /* 0: uart0 1: uart1 2: uart2(m0) 3: spi0 */
#define ADC_OUTPUT_DMA_CH     DMA_CHANNEL_0

#if ADC_OUTPUT_PORT_SEL <= 2
#define ADC_OUTPUT_PORT_UART
#else
#define ADC_OUTPUT_PORT_SPI
#endif

static dma_handle_t hdmax;
#ifdef ADC_OUTPUT_PORT_UART
static uart_handle_t uart_handle;

static void uart_init(uart_id_t idn)
{
	uart_handle.id = idn;
	uart_handle.cfg.baud_rate = UART_BAUDRATE_SET;
	uart_handle.cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_handle.cfg.parity = UART_PARITY_DISABLE;
	uart_handle.cfg.stop_bits = UART_STOP_BITS_1;
	uart_handle.cfg.dma_mode = 1;
	uart_handle.cfg.fifo_cfg.fifo_en = 1;
	uart_handle.cfg.fifo_cfg.tx_fifo_level = UART_TX_FIFO_HALF_FULL;
	hal_uart_init(idn, &uart_handle.cfg);
}
#endif

#ifdef ADC_OUTPUT_PORT_SPI
static spi_handle_t spi_handle;
#endif

void output_dma_config(void)
{
	u32 dma_dst_handshake_interface;

#if ADC_OUTPUT_PORT_SEL == 0
	uart_init(UART_ID_0);

	__HAL_LINKDMA(&uart_handle, hdmatx, hdmax);

	dma_dst_handshake_interface = DMA_HANDSHAKE_IF_UART0_TX;

#elif ADC_OUTPUT_PORT_SEL == 1
	uart_init(UART_ID_1);

	__HAL_LINKDMA(&uart_handle, hdmatx, hdmax);

	dma_dst_handshake_interface = DMA_HANDSHAKE_IF_UART1_TX;

#elif ADC_OUTPUT_PORT_SEL == 2

	uart_init(UART_ID_2);

	__HAL_LINKDMA(&uart_handle, hdmatx, hdmax);

	dma_dst_handshake_interface = DMA_HANDSHAKE_IF_UART2_TX;

#elif ADC_OUTPUT_PORT_SEL == 3
	sys_clk_enable_spi0();

	spi_handle.init.mode = SPI_MODE_MASTER;
	spi_handle.instance = SPI0;
	spi_handle.init.tmod = SPI_TMOD_TX_ONLY;
	spi_handle.init.dfs = SPI_DFS_8_BIT;
	spi_handle.init.baudrate = SPI_SCK_FREQ;
	spi_handle.init.clk_pol = SPI_CLK_POL_HIGH;
	spi_handle.init.clk_pha = SPI_CLK_PHA_1EDGE;
	spi_handle.init.frame_protocol = SPI_FRAME_PROTOCOL_MOTOROLA_SPI;
	spi_handle.init.spi_frf = SPI_FRF_STANDARD;
	spi_handle.init.tx_dma_en = 1u;
	spi_handle.init.tx_dma_lvl = 8U;
	hal_spi_init(&spi_handle);

	__HAL_LINKDMA(&spi_handle, hdmatx, hdmax);

	dma_dst_handshake_interface = DMA_HANDSHAKE_IF_SPI0_TX;
#else
#error "Select adc output port!"
#endif

	hal_dma_init_struct_init(&hdmax.init);
	hdmax.instance = DMA1;
	hdmax.init.channel = ADC_OUTPUT_DMA_CH;
	hdmax.init.transfer_type = DMA_TRANSFER_TYPE_ROW1;
	hdmax.init.config.channel_priority = DMA_CHAN_PRIORITY_0;
	hdmax.init.control.int_en = 0;
	hdmax.init.control.tt_fc = DMA_TT_M2P_FC_DMA;

	hdmax.init.control.src_tr_width = DMA_TR_WIDTH_8_BITS;
	hdmax.init.control.dst_tr_width = DMA_TR_WIDTH_8_BITS;

	hdmax.init.control.src_addr_inc = DMA_ADDR_INCREMENT;
	hdmax.init.control.dst_addr_inc = DMA_ADDR_NO_CHANGE;

	hdmax.init.control.src_burst_size = DMA_BURST_SIZE_1_ITEMS;
	hdmax.init.control.dst_burst_size = DMA_BURST_SIZE_1_ITEMS;

	hdmax.init.config.dst_handshake_type = DMA_HANDSHAKE_TYPE_HARDWARE;
	hdmax.init.config.dst_handshake_interface = dma_dst_handshake_interface;

	sys_clk_disable_dma();
	sys_clk_enable_dma();

	hal_dma_init(&hdmax);
}

static void wait_output_dma_done(void)
{
#ifdef ADC_OUTPUT_PORT_UART
	if (hal_dma_get_state(uart_handle.hdmatx) == HAL_DMA_STATE_BUSY) {
		hal_dma_poll_transfer(uart_handle.hdmatx, 100U);
	}

	while (hal_dma_get_state(uart_handle.hdmatx) != HAL_DMA_STATE_READY) {}
#endif

#ifdef ADC_OUTPUT_PORT_SPI
	if (hal_dma_get_state(spi_handle.hdmatx) == HAL_DMA_STATE_BUSY) {
		hal_dma_poll_transfer(spi_handle.hdmatx, 100U);
		spi_handle.state = HAL_SPI_STATE_READY;
	}

	while (hal_dma_get_state(spi_handle.hdmatx) != HAL_DMA_STATE_READY) {}
#endif
}

static void start_output_dma(u32 addr, u32 size)
{
#ifdef ADC_OUTPUT_PORT_UART
	hal_uart_transmit_dma(&uart_handle, (u8 *)addr, (u16)size);
#endif

#ifdef ADC_OUTPUT_PORT_SPI
	hal_spi_transmit_dma(&spi_handle, (u8 *)addr, (u16)size);
#endif
}

void output_port_send_data(u32 addr, u32 size)
{
	if (size > 4095) {
		u32 data_size_2048 = size / 2048, j = 0;
		u32 data_size_mod = size % 2048;

		while (data_size_2048--) {
			wait_output_dma_done();
			start_output_dma(addr + j, 2048);
			j += 2048;
		}
		if (data_size_mod != 0) {
			wait_output_dma_done();
			start_output_dma(addr + j, data_size_mod);
		}
	} else {
		wait_output_dma_done();
		start_output_dma(addr, size);
	}
}
