/**
 * Copyright (c) 2022, Airtouching Intelligence Technology.
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
#include "hal_dma.h"
#include "hal_uart.h"
#include <stdio.h>

/* Private Constants ---------------------------------------------------------*/
#define DMA_CH DMA_CHANNEL_0

/* Global Variables ----------------------------------------------------------*/
static u8 tx_buf[3][16];
static u8 rx_buf[3][16] = { {0U} };
static dma_link_list_t lli[3] = { 0U };
dma_handle_t hdmax;

static volatile int dma_cplt_flag = 0;
static volatile int dma_error_flag = 0;

/* Function Prototypes -------------------------------------------------------*/
static bool check_mem(const u8* src, const u8* dest, u32 len);
void dma_cplt_callback(struct __dma_handle_t *hdma);
void dma_error_callback(struct __dma_handle_t *hdma);

/* Main Entry ----------------------------------------------------------------*/
int main(void)
{
	uart_cfg_t uart_cfg;
	hal_status_t retval;

	clock_init();

	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);
	printf("example: %s\r\n", __FILE__);

	if (28U != sizeof(dma_link_list_t)) {
		printf("ERROR: lli is %u bytes, not aligned to 4 bytes!!!\r\n",
			(u32)sizeof(dma_link_list_t));
	}

	sys_clk_enable_dma();

	hal_dma_lli_struct_init(lli, 3);

	lli[0].src_addr             = (u32)tx_buf[0];
	lli[0].dst_addr             = (u32)rx_buf[0];
	lli[0].control.dst_tr_width = DMA_TR_WIDTH_8_BITS;
	lli[0].control.src_tr_width = DMA_TR_WIDTH_8_BITS;
	lli[0].control.src_llp_en   = 1;
	lli[0].control.dst_llp_en   = 1;
	lli[0].control.block_size   = 16U;
	lli[0].control.int_en       = 1; // NOTE: This should set to 1 if interrupt mode is used.

	lli[1].src_addr             = (u32)tx_buf[1];
	lli[1].dst_addr             = (u32)rx_buf[1];
	lli[1].control.dst_tr_width = DMA_TR_WIDTH_8_BITS;
	lli[1].control.src_tr_width = DMA_TR_WIDTH_8_BITS;
	lli[1].control.src_llp_en   = 1;
	lli[1].control.dst_llp_en   = 1;
	lli[1].control.block_size   = 16U;
	lli[2].control.int_en       = 1;

	lli[2].src_addr             = (u32)tx_buf[2];
	lli[2].dst_addr             = (u32)rx_buf[2];
	lli[2].control.dst_tr_width = (u32)DMA_TR_WIDTH_8_BITS;
	lli[2].control.src_tr_width = (u32)DMA_TR_WIDTH_8_BITS;
	lli[2].control.src_llp_en   = 0;
	lli[2].control.dst_llp_en   = 0;
	lli[2].control.block_size   = 16U;
	lli[2].control.int_en       = 1;

	for (u8 i = 0U; i < 16U; i++) {
		tx_buf[0][i] = i;
		tx_buf[1][i] = i + 0x10U;
		tx_buf[2][i] = i ^ 0xFFU;
	}


	hal_dma_init_struct_init(&hdmax.init);
	hdmax.instance = DMA1;
	hdmax.init.channel = DMA_CH;
	hdmax.init.transfer_type = DMA_TRANSFER_TYPE_ROW10;
	hdmax.init.llp_loc = (uint32)&lli[0];

	retval = hal_dma_init(&hdmax);

	hal_dma_register_callbacks(&hdmax, HAL_DMA_XFER_CPLT_CB_ID, dma_cplt_callback);
	hal_dma_register_callbacks(&hdmax, HAL_DMA_XFER_ERROR_CB_ID, dma_error_callback);
	NVIC_EnableIRQ(DMA_IRQn);
	retval = hal_dma_start_it(&hdmax, 0, 0, 16, 3);

	while (1) {
		if (dma_cplt_flag != 0U) {
			break;
		}

		if (dma_error_flag != 0U) {
			break;
		}
	}

	if (dma_error_flag != 0U) {
		printf("Error: DMA goes wrong state!!! \r\n");
		while (1);
	}

	printf("==========  DMA transfer result check  ==========\r\n");
	for (u8 i = 0U; i < 3U; i++) {
		if (check_mem(tx_buf[i], rx_buf[i], 16U)) {
			printf("Link list item %u PASS!!!!\r\n", i);
		} else {
			printf("Link list item %u FAIL!!!\r\n", i);
		}
	}

	while (1);
}

/* Function Implementations --------------------------------------------------*/
static bool check_mem(const u8* src, const u8* dest, u32 len)
{
	bool ret = true;
	for (u32 i = 0U; i < len; i++) {
		if (src[i] != dest[i]) {
			printf("Error at %u, expect %x, actual %x\r\n", i, src[i], dest[i]);
			ret = false;
			break;
		}
	}
	return ret;
}

void dma_cplt_callback(struct __dma_handle_t *hdma)
{
	if (hdma == &hdmax) {
		dma_cplt_flag = 1;
	}
}
void dma_error_callback(struct __dma_handle_t *hdma)
{
	if (hdma == &hdmax) {
		dma_error_flag = 1;
	}
}
