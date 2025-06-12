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
#include "dma.h"
#include "hal_uart.h"
#include "reg_uart.h"
#include "gpio.h"

/*
 * Output port selection:
 *  0: uart0
 *  1: uart1
 *  2: uart2
 */
#define ADC_OUTPUT_PORT_SEL   0

#define CH_MASK               (1 << 0)
#define ADC_OUTPUT_DMA_CH     LL_DMA_CHANNEL_0

void output_dma_config(void)
{
	uart_id_t uart_port = UART_ID_0;
	uart_cfg_t uart_cfg = {
		.baud_rate = UART_DUMP_BAUDRATE,
		.data_width = UART_DATA_WIDTH_8BIT,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.dma_mode = 1,
		.fifo_cfg.fifo_en = 1,
		.fifo_cfg.tx_fifo_level = UART_TX_FIFO_HALF_FULL,
	};

	sys_clk_enable_dma();

#if (ADC_OUTPUT_PORT_SEL == 0)
	dma_enable_global(DMA1);
	dma_set_dst_addr(DMA1, ADC_OUTPUT_DMA_CH, REG_UART_BASE_0);
	dma_set_dst_handshake_if(DMA1, ADC_OUTPUT_DMA_CH, LL_DMA_HANDSHAKE_IF_UART0_TX);
#elif (ADC_OUTPUT_PORT_SEL == 1)
	uart_port = UART_ID_1;

	dma_enable_global(DMA1);
	dma_set_dst_addr(DMA1, ADC_OUTPUT_DMA_CH, REG_UART_BASE_1);
	dma_set_dst_handshake_if(DMA1, ADC_OUTPUT_DMA_CH, LL_DMA_HANDSHAKE_IF_UART1_TX);
#elif (ADC_OUTPUT_PORT_SEL == 2)
	uart_port = UART_ID_2;

	dma_enable_global(DMA1);
	dma_set_dst_addr(DMA1, ADC_OUTPUT_DMA_CH, REG_UART_BASE_2);
	dma_set_dst_handshake_if(DMA1, ADC_OUTPUT_DMA_CH, LL_DMA_HANDSHAKE_IF_UART2_TX);
#else
#error "Select adc output port!"
#endif

	if (HAL_UART_STATE_RESET != hal_uart_state_get(uart_port)) {
		hal_uart_deinit(uart_port);
	}
	hal_uart_init(uart_port, &uart_cfg);

	dma_set_int_en(DMA1, ADC_OUTPUT_DMA_CH, 0U);
	dma_set_flowctrl(DMA1, ADC_OUTPUT_DMA_CH, LL_DMA_TT_M2P_FC_DMA);
	dma_set_block_transfer_size(DMA1, ADC_OUTPUT_DMA_CH, 16U);

	dma_set_dst_handshake_type(DMA1, ADC_OUTPUT_DMA_CH, LL_DMA_HANDSHAKE_TYPE_HARDWARE);
	dma_set_dst_tr_width(DMA1, ADC_OUTPUT_DMA_CH, LL_DMA_TR_WIDTH_8_BITS);
	dma_set_dst_burst_length(DMA1, ADC_OUTPUT_DMA_CH, LL_DMA_BURST_1_ITEMS);
	dma_set_dst_addr_inc(DMA1, ADC_OUTPUT_DMA_CH, LL_DMA_ADDR_NO_CHANGE);
	dma_set_dst_scatter_en(DMA1, ADC_OUTPUT_DMA_CH, 0U);

	dma_set_src_tr_width(DMA1, ADC_OUTPUT_DMA_CH, LL_DMA_TR_WIDTH_8_BITS);
	dma_set_src_burst_length(DMA1, ADC_OUTPUT_DMA_CH, LL_DMA_BURST_1_ITEMS);
	dma_set_src_addr_inc(DMA1, ADC_OUTPUT_DMA_CH, LL_DMA_ADDR_INCREMENT);
	dma_set_src_gather_en(DMA1, ADC_OUTPUT_DMA_CH, 0U);
}

void wait_output_dma_done(void)
{
	while (1 == dma_is_active_channel(DMA1, ADC_OUTPUT_DMA_CH)) {}
	dma_clear_it_tfr(DMA1, ADC_OUTPUT_DMA_CH);
}

void start_output_dma(u32 addr, u32 size)
{
	dma_set_src_addr(DMA1, ADC_OUTPUT_DMA_CH, addr);
	dma_set_block_transfer_size(DMA1, ADC_OUTPUT_DMA_CH, size);
	dma_enable_channel(DMA1, ADC_OUTPUT_DMA_CH);
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
