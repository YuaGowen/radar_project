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

#include "dma.h"
#include "i2c_host.h"
#include "reg_i2c.h"
#include "at_common.h"

typedef enum {
	I2C_STATE_IDLE,
	I2C_STATE_RX,
	I2C_STATE_TX
} i2c_state_t;

static i2c_state_t m_i2c_state;
static u32 *rx_data;
static u32 rx_size;

i2c_err_t i2c_slave_write_dma(i2c_id_t id, u32 *data, u8 size)
{
	u32 dma_ch;
	if (E_OK != dma_get_free_channel(DMA1, &dma_ch)) {
		return I2C_ERR_BUSY;
	}
	m_i2c_state = I2C_STATE_TX;
	for (u8 i = 0U; i < size; i++) {
		data[i] = CMD_TRANS(1U) | CMD_DATA(data[i]);
	}
	dma_set_src_addr(DMA1, dma_ch, (u32)data);
	dma_set_src_addr_inc(DMA1, dma_ch, LL_DMA_ADDR_INCREMENT);
	dma_set_src_tr_width(DMA1, dma_ch, LL_DMA_TR_WIDTH_32_BITS);
	dma_set_src_burst_length(DMA1, dma_ch, LL_DMA_BURST_1_ITEMS);
	dma_set_src_handshake_type(DMA1, dma_ch, LL_DMA_HANDSHAKE_TYPE_HARDWARE);

	dma_set_dst_addr(DMA1, dma_ch, REG_I2C_BASE + CMD_OFFSET);
	dma_set_dst_addr_inc(DMA1, dma_ch, LL_DMA_ADDR_NO_CHANGE);
	dma_set_dst_tr_width(DMA1, dma_ch, LL_DMA_TR_WIDTH_32_BITS);
	dma_set_dst_burst_length(DMA1, dma_ch, LL_DMA_BURST_1_ITEMS);
	dma_set_dst_handshake_type(DMA1, dma_ch, LL_DMA_HANDSHAKE_TYPE_HARDWARE);
	dma_set_dst_handshake_if(DMA1, dma_ch, LL_DMA_HANDSHAKE_IF_UART2_TX);

	dma_set_block_transfer_size(DMA1, dma_ch, size);
	dma_set_flowctrl(DMA1, dma_ch, LL_DMA_TT_M2P_FC_DMA);
	dma_set_int_en(DMA1, dma_ch, 0U);
	dma_enable_channel(DMA1, dma_ch);
	return I2C_ERR_NONE;
}

i2c_err_t i2c_slave_read_dma(i2c_id_t id, u32 *data, u8 size)
{
	u32 dma_ch;
	if (E_OK != dma_get_free_channel(DMA1, &dma_ch)) {
		return I2C_ERR_BUSY;
	}
	m_i2c_state = I2C_STATE_RX;
	rx_data = data;
	rx_size = size;
	hwp_i2c->cmd = CMD_RECEIVE(1U) | CMD_ACK(0U);

	dma_set_src_addr(DMA1, dma_ch, REG_I2C_BASE + STATUS_OFFSET);
	dma_set_src_addr_inc(DMA1, dma_ch, LL_DMA_ADDR_NO_CHANGE);
	dma_set_src_tr_width(DMA1, dma_ch, LL_DMA_TR_WIDTH_32_BITS);
	dma_set_src_burst_length(DMA1, dma_ch, LL_DMA_BURST_1_ITEMS);
	dma_set_src_handshake_type(DMA1, dma_ch, LL_DMA_HANDSHAKE_TYPE_HARDWARE);
	dma_set_src_handshake_if(DMA1, dma_ch, LL_DMA_HANDSHAKE_IF_UART2_RX);

	dma_set_dst_addr(DMA1, dma_ch, (u32)data);
	dma_set_dst_addr_inc(DMA1, dma_ch, LL_DMA_ADDR_INCREMENT);
	dma_set_dst_tr_width(DMA1, dma_ch, LL_DMA_TR_WIDTH_32_BITS);
	dma_set_dst_burst_length(DMA1, dma_ch, LL_DMA_BURST_1_ITEMS);
	dma_set_dst_handshake_type(DMA1, dma_ch, LL_DMA_HANDSHAKE_TYPE_HARDWARE);

	dma_set_block_transfer_size(DMA1, dma_ch, size);
	dma_set_flowctrl(DMA1, dma_ch, LL_DMA_TT_P2M_FC_DMA);
	dma_set_int_en(DMA1, dma_ch, 0U);
	dma_enable_channel(DMA1, dma_ch);
	return I2C_ERR_NONE;
}

u8 i2c_state_get(void)
{
	return (m_i2c_state == I2C_STATE_IDLE) ? 0U : 1U;
}

ATTR_RAM_SECTION void i2c_dma_isr(void)
{
	hwp_i2c->ctrl_1 |= CTRL_1_IRQ_CLR_MSK;
	if (m_i2c_state == I2C_STATE_RX) {
		hwp_i2c->ctrl_1 |= CTRL_1_RST_MSK;
		hwp_i2c->ctrl_1 &= ~CTRL_1_RST_MSK;
		for (u8 i = 0; i < rx_size; i++) {
			rx_data[i] = (rx_data[i] & STATUS_DATA_MSK) >> STATUS_DATA_POS;
		}
	}
	m_i2c_state = I2C_STATE_IDLE;
}
