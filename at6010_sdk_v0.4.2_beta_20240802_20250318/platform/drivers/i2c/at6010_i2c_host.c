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

#include "CMSDK_CM0plus.h"
#include "reg_i2c.h"
#include "i2c_host.h"
#include "clock.h"
#include "common.h"

#define I2C_STA_EN

#ifndef I2C_STA_EN
#define I2C_DELAY 300
#endif
#define I2C_IRQ_MSK (u32)(STATUS_FULL_MSK | STATUS_ADDR_HIT_MSK | STATUS_EMPTY_MSK | STATUS_ERR_MSK)

typedef enum {
	I2C_STATE_IDLE,
	I2C_STATE_ADDR_RX,
	I2C_STATE_ADDR_TX
} i2c_state_t;

typedef enum {
	I2C_HOST_ADDR_W,
	I2C_HOST_MEM,
	I2C_HOST_ADDR_R,
	I2C_HOST_DATA
} i2c_host_step_t;

typedef struct {
	i2c_mode_t mode;
	u8 tx_head;
	u8 tx_tail;
	u8 rx_enable;
	u8 tx_buf[I2C_WRITE_BUF_SIZE];
} i2c_int_cfg_t;

typedef void (*m_i2c_isr_t)(i2c_id_t id, u32 status);

void i2c_slave_int_isr(i2c_id_t id, u32 status);
void i2c_master_int_isr(i2c_id_t id, u32 status);
void i2c_host_int_isr(i2c_id_t id, u32 status);

static HWP_I2C_T *const m_i2c_hwd[] = {hwp_i2c};
static u32 m_i2c_ctrl2;

static i2c_state_t m_i2c_state[I2C_ID_QTY];
static i2c_host_step_t m_i2c_step;

static i2c_hdl_t m_i2c_hdl[I2C_ID_QTY];
static i2c_int_cfg_t m_i2c_int_cfg[I2C_ID_QTY];
static const m_i2c_isr_t m_i2c_isr[] = {i2c_slave_int_isr, i2c_master_int_isr, i2c_host_int_isr};

__STATIC_INLINE void i2c_ctrl2_set(u32 msk, u32 val)
{
	m_i2c_ctrl2 &= ~msk;
	m_i2c_ctrl2 |= val;
	hwp_i2c->ctrl_2 = m_i2c_ctrl2;
}

void i2c_host_init(void)
{
	hwp_i2c->ctrl_1 = CTRL_1_RST(1U) | CTRL_1_IRQ_CLR(1U);
	hwp_i2c->ctrl_1 = CTRL_1_RST(0U);
	hwp_i2c->ctrl_1 = CTRL_1_EN(1U) | CTRL_1_MASTER(1U) | CTRL_1_IRQ_EN(0U) | CTRL_1_SCL_I_SEL(3U) |
	                  CTRL_1_SDA_I_SEL(3U) | CTRL_1_ADDR(0U) | CTRL_1_DIV(100U);
	hwp_i2c->ctrl_2 = CTRL_2_SDA_O_SEL(7U);

	m_i2c_int_cfg[I2C_ID_0].mode = I2C_MODE_HOST;
}

static void clean_i2c_status(i2c_id_t id)
{
	u32 tmp;

	tmp = m_i2c_hwd[id]->ctrl_1;
	tmp = tmp | CTRL_1_IRQ_CLR(1U);
	m_i2c_hwd[id]->ctrl_1 = tmp;
}

/**
 * i2c status:
 *   empty: 1, not empty: 0;
 *   full: 1, not full: 0;
 *   ack: 0, nack: 1;
 *
 * return: 0: ack, 1: no ack.
 */
static int check_status_empty_ack(i2c_id_t id)
{
	u32 tmp;
	u32 ack;

	/* wait empty. */
	do {
		tmp = m_i2c_hwd[id]->status;
	} while (!(tmp & STATUS_EMPTY_MSK));

	/* read ack. */
	ack = (tmp & STATUS_ACK_MSK) >> STATUS_ACK_POS;

	/* clear status. */
	clean_i2c_status(id);

	return (int)ack;
}

static int check_status_full_nack(i2c_id_t id)
{
	u32 tmp;
	u32 ack;

	/* wait full. */
	do {
		tmp = m_i2c_hwd[id]->status;
	} while (!(tmp & STATUS_FULL_MSK));

	/* read ack. */
	ack = (tmp & STATUS_ACK_MSK) >> STATUS_ACK_POS;

	/* clear status. */
	clean_i2c_status(id);

	return (int)ack;
}

/**
 * @brief Wait i2c addr hit and return rw flag
 * @return u8 0: write addr 1: read addr
 */
static u8 check_status_hit_rw(i2c_id_t id)
{
	u32 tmp;
	do {
		tmp = m_i2c_hwd[id]->status;
	} while ((0U == (tmp & STATUS_ADDR_HIT_MSK)) || (0U == (tmp & STATUS_ADDR_MATCH_MSK)) ||
	         (0U == (tmp & STATUS_STATE_MSK)));

	return (tmp & STATUS_RW_MSK) >> STATUS_RW_POS;
}

__STATIC_INLINE void i2c_send_addr(i2c_id_t id, u8 addr)
{
	m_i2c_hwd[id]->cmd = CMD_START(1U) | CMD_TRANS(1U) | CMD_DATA(addr);
}

__STATIC_INLINE void i2c_send_data(i2c_id_t id, u8 data)
{
	m_i2c_hwd[id]->cmd = CMD_TRANS(1U) | CMD_DATA(data);
}

__STATIC_INLINE void i2c_recv_data(i2c_id_t id)
{
	m_i2c_hwd[id]->cmd = CMD_RECEIVE(1U) | CMD_ACK(1U);
}

__STATIC_INLINE void i2c_set_irq_en(i2c_id_t id)
{
	m_i2c_hwd[id]->ctrl_1 |= CTRL_1_IRQ_EN_MSK;
}

__STATIC_INLINE void i2c_clear_irq_en(i2c_id_t id)
{
	m_i2c_hwd[id]->ctrl_1 &= ~CTRL_1_IRQ_EN_MSK;
}

i2c_err_t i2c_host_write_byte(u8 slv_addr, u8 mem_addr, u8 data)
{
	i2c_err_t ret = I2C_ERR_NONE;

	clean_i2c_status(I2C_ID_0);

	hwp_i2c->cmd = CMD_START(1U) | CMD_TRANS(1U) | CMD_DATA((((u32)slv_addr << 1U) | 0U));
#ifdef I2C_STA_EN
	if (check_status_empty_ack(I2C_ID_0)) {
		ret = I2C_ERR_ADDR_NACK;
		goto write_error;
	}
#else
	delay_nop(I2C_DELAY);
#endif

	hwp_i2c->cmd = CMD_TRANS(1U) | CMD_DATA(mem_addr);
#ifdef I2C_STA_EN
	if (check_status_empty_ack(I2C_ID_0)) {
		ret = I2C_ERR_MEM_NACK;
		goto write_error;
	}
#else
	delay_nop(I2C_DELAY);
#endif

	hwp_i2c->cmd = CMD_TRANS(1U) | CMD_DATA(data);
#ifdef I2C_STA_EN
	if (check_status_empty_ack(I2C_ID_0)) {
		ret = I2C_ERR_DATA_NACK;
		goto write_error;
	}
#else
	delay_nop(I2C_DELAY);
#endif

write_error:
	hwp_i2c->cmd = CMD_STOP(1U);

	return ret;
}

i2c_err_t i2c_host_read_byte(u8 slv_addr, u8 mem_addr, u8 *pdat)
{
	i2c_err_t ret = I2C_ERR_NONE;

	clean_i2c_status(I2C_ID_0);

	hwp_i2c->cmd = CMD_START(1U) | CMD_TRANS(1U) | CMD_DATA((((u32)slv_addr << 1U) | 0U));
#ifdef I2C_STA_EN
	if (check_status_empty_ack(I2C_ID_0)) {
		ret = I2C_ERR_ADDR_NACK;
		goto read_error;
	}
#else
	delay_nop(I2C_DELAY);
#endif

	hwp_i2c->cmd = CMD_TRANS(1U) | CMD_DATA(mem_addr);
#ifdef I2C_STA_EN
	if (check_status_empty_ack(I2C_ID_0)) {
		ret = I2C_ERR_MEM_NACK;
		goto read_error;
	}
#else
	delay_nop(I2C_DELAY);
#endif

	hwp_i2c->cmd = CMD_START(1U) | CMD_TRANS(1U) | CMD_DATA((((u32)slv_addr << 1U) | 1U));
#ifdef I2C_STA_EN
	if (check_status_empty_ack(I2C_ID_0)) {
		ret = I2C_ERR_DATA_NACK;
		goto read_error;
	}
#else
	delay_nop(I2C_DELAY);
#endif

	hwp_i2c->cmd = CMD_RECEIVE(1U) | CMD_ACK(1U);
#ifdef I2C_STA_EN
	(void)check_status_full_nack(I2C_ID_0);
#else
	delay_nop(I2C_DELAY);
#endif

read_error:
	hwp_i2c->cmd = CMD_STOP(1U);
	if (ret == I2C_ERR_NONE) {
		*pdat = ((hwp_i2c->status & STATUS_DATA_MSK) >> STATUS_DATA_POS);
	}

	return ret;
}

i2c_err_t i2c_host_write_byte_int(u8 slv_addr, u8 mem_addr, u8 data)
{
	i2c_err_t err;

	if (m_i2c_int_cfg[I2C_ID_0].mode != I2C_MODE_HOST) {
		err = I2C_ERR_FALSE_MODE;
	} else if (m_i2c_state[I2C_ID_0] != I2C_STATE_IDLE) {
		err = I2C_ERR_BUSY;
	} else {
		m_i2c_state[I2C_ID_0] = I2C_STATE_ADDR_TX;
		m_i2c_int_cfg[I2C_ID_0].tx_buf[0U] = mem_addr;
		m_i2c_int_cfg[I2C_ID_0].tx_buf[1U] = data;
		m_i2c_step = I2C_HOST_ADDR_W;

		i2c_set_irq_en(I2C_ID_0);
		i2c_send_addr(I2C_ID_0, (u8)(slv_addr << 1U));

		err = I2C_ERR_NONE;
	}

	return err;
}

i2c_err_t i2c_host_read_byte_int(u8 slv_addr, u8 mem_addr)
{
	i2c_err_t err;

	if (m_i2c_int_cfg[I2C_ID_0].mode != I2C_MODE_HOST) {
		err = I2C_ERR_FALSE_MODE;
	} else if (m_i2c_state[I2C_ID_0] != I2C_STATE_IDLE) {
		err = I2C_ERR_BUSY;
	} else {
		m_i2c_state[I2C_ID_0] = I2C_STATE_ADDR_RX;
		m_i2c_int_cfg[I2C_ID_0].tx_buf[0U] = mem_addr;
		m_i2c_int_cfg[I2C_ID_0].tx_buf[1U] = slv_addr;
		m_i2c_step = I2C_HOST_ADDR_W;

		i2c_set_irq_en(I2C_ID_0);
		i2c_send_addr(I2C_ID_0, (u8)(slv_addr << 1U));

		err = I2C_ERR_NONE;
	}

	return err;
}

ATTR_RAM_SECTION static void i2c_event_nack(i2c_id_t id)
{
	i2c_err_t err;

	hwp_i2c->cmd = CMD_STOP(1U);
	i2c_clear_irq_en(id);

	switch (m_i2c_step) {
	case I2C_HOST_ADDR_W:
	case I2C_HOST_ADDR_R: err = I2C_ERR_ADDR_NACK; break;
	case I2C_HOST_MEM: err = I2C_ERR_MEM_NACK; break;
	case I2C_HOST_DATA: err = I2C_ERR_DATA_NACK; break;
	default: err = I2C_ERR_FALSE_MODE; break;
	}
	if (NULL != m_i2c_hdl[id].i2c_err_hdl) {
		m_i2c_hdl[id].i2c_err_hdl(err);
	}
	m_i2c_state[id] = I2C_STATE_IDLE;
}

ATTR_RAM_SECTION static void i2c_event_ack(i2c_id_t id, u32 status)
{
	switch (m_i2c_step) {
	case I2C_HOST_ADDR_W:
		i2c_send_data(I2C_ID_0, m_i2c_int_cfg[id].tx_buf[0U]);
		m_i2c_step = I2C_HOST_MEM;
		break;
	case I2C_HOST_MEM:
		if (I2C_STATE_ADDR_TX == m_i2c_state[id]) {
			i2c_send_data(I2C_ID_0, m_i2c_int_cfg[id].tx_buf[1U]);
			m_i2c_step = I2C_HOST_DATA;
		} else {
			i2c_send_addr(I2C_ID_0, (u8)(m_i2c_int_cfg[id].tx_buf[1U] << 1U) | 1U);
			m_i2c_step = I2C_HOST_ADDR_R;
		}
		break;
	case I2C_HOST_ADDR_R:
		i2c_recv_data(I2C_ID_0);
		m_i2c_step = I2C_HOST_DATA;
		break;
	case I2C_HOST_DATA:
		hwp_i2c->cmd = CMD_STOP(1U);
		i2c_clear_irq_en(id);

		if ((I2C_STATE_ADDR_TX == m_i2c_state[id]) && (NULL != m_i2c_hdl[id].i2c_tx_cplt_hdl)) {
			m_i2c_hdl[id].i2c_tx_cplt_hdl();
		} else if ((I2C_STATE_ADDR_RX == m_i2c_state[id]) && (NULL != m_i2c_hdl[id].i2c_rx_hdl)) {
			m_i2c_hdl[id].i2c_rx_hdl((status & STATUS_DATA_MSK) >> STATUS_DATA_POS);
		} else {
			/* Nothing to be done */
		}
		m_i2c_state[id] = I2C_STATE_IDLE;
		break;
	default:
		/* Should not reach here */
		break;
	}
}

ATTR_RAM_SECTION void i2c_host_int_isr(i2c_id_t id, u32 status)
{
	clean_i2c_status(id);

	if (status & STATUS_ACK_MSK) {
		i2c_event_nack(id);
	} else {
		i2c_event_ack(id, status);
	}
}

/*******************************************************************************
**                      I2C Configuration Functions                           **
*******************************************************************************/

void i2c_init(i2c_id_t id, const i2c_cfg_t *cfg)
{
	u32 div;
	u8 master;

	m_i2c_int_cfg[id].mode = cfg->mode;
	div = (cfg->mode == I2C_MODE_MASTER) ? (clk_mcu_get() / cfg->sck) : 0U;
	master = (cfg->mode == I2C_MODE_SLAVE) ? 0U : 1U;

	m_i2c_hwd[id]->ctrl_1 = CTRL_1_RST(1U) | CTRL_1_IRQ_CLR(1U);
	m_i2c_hwd[id]->ctrl_1 = CTRL_1_RST(0U);
	m_i2c_hwd[id]->ctrl_1 = CTRL_1_EN(1U) | CTRL_1_MASTER(master) | CTRL_1_SCL_I_SEL(2U) | CTRL_1_SDA_I_SEL(3U) |
	                        CTRL_1_ADDR(cfg->addr) | CTRL_1_DIV(div);
	i2c_ctrl2_set(CTRL_2_SDA_O_SEL_MSK, CTRL_2_SDA_O_SEL(1U));
}

void i2c_deinit(i2c_id_t id)
{
	m_i2c_hwd[id]->ctrl_1 &= ~CTRL_1_EN_MSK;
}

void i2c_irq_enable(i2c_irq_t irq)
{
#ifdef CHIP_AT6010_U01
	NVIC_ClearPendingIRQ(I2C_IRQn);
	NVIC_EnableIRQ(I2C_IRQn);
#else
	i2c_ctrl2_set(1U << irq << CTRL_2_IRQ_ADDR_HIT_EN_POS, 1U << irq << CTRL_2_IRQ_ADDR_HIT_EN_POS);
	hwp_i2c->ctrl_1 |= CTRL_1_IRQ_EN_MSK;
#endif
}

void i2c_irq_disable(i2c_irq_t irq)
{
#ifdef CHIP_AT6010_U01
	NVIC_DisableIRQ(I2C_IRQn);
#else
	i2c_ctrl2_set(1U << irq << CTRL_2_IRQ_ADDR_HIT_EN_POS, 0U);
#endif
}

void i2c_set_hdl(i2c_id_t id, const i2c_hdl_t *hdl)
{
	m_i2c_hdl[id].i2c_err_hdl = hdl->i2c_err_hdl;
	m_i2c_hdl[id].i2c_rx_hdl = hdl->i2c_rx_hdl;
	m_i2c_hdl[id].i2c_tx_cplt_hdl = hdl->i2c_tx_cplt_hdl;
}

void i2c_dma_enable(void)
{
	i2c_ctrl2_set(CTRL_2_DMA_RX_EN_MSK | CTRL_2_DMA_TX_EN_MSK, CTRL_2_DMA_RX_EN(1U) | CTRL_2_DMA_TX_EN(1U));
}

/*******************************************************************************
**                      I2C Polling Functions                                 **
*******************************************************************************/

i2c_err_t i2c_master_write_wait(i2c_id_t id, u8 slv_addr, const u8 *data, u8 size)
{
	i2c_err_t ret = I2C_ERR_NONE;

	clean_i2c_status(id);

	m_i2c_hwd[id]->cmd = CMD_START(1U) | CMD_TRANS(1U) | CMD_DATA(((u32)slv_addr << 1U));
#ifdef I2C_STA_EN
	if (check_status_empty_ack(id)) {
		ret = I2C_ERR_ADDR_NACK;
		goto write_error;
	}
#else
	delay_nop(I2C_DELAY);
#endif

	for (u8 cnt = 0U; cnt < size; cnt++) {
		m_i2c_hwd[id]->cmd = CMD_TRANS(1U) | CMD_DATA(data[cnt]);
#ifdef I2C_STA_EN
		if (check_status_empty_ack(id)) {
			ret = I2C_ERR_DATA_NACK;
			goto write_error;
		}
#else
		delay_nop(I2C_DELAY);
#endif
	}

write_error:
	m_i2c_hwd[id]->cmd = CMD_STOP(1U);
	return ret;
}

i2c_err_t i2c_master_read_wait(i2c_id_t id, u8 slv_addr, u8 *data, u8 size)
{
	u8 ack;
	i2c_err_t ret = I2C_ERR_NONE;

	clean_i2c_status(id);

	m_i2c_hwd[id]->cmd = CMD_START(1U) | CMD_TRANS(1U) | CMD_DATA((((u32)slv_addr << 1U) | 1U));
#ifdef I2C_STA_EN
	if (check_status_empty_ack(id)) {
		ret = I2C_ERR_ADDR_NACK;
		goto read_error;
	}
#else
	delay_nop(I2C_DELAY);
#endif

	for (u8 cnt = 0U; cnt < size; cnt++) {
		ack = (cnt == (size - 1U)) ? 1U : 0U;
		m_i2c_hwd[id]->cmd = CMD_RECEIVE(1U) | CMD_ACK(ack);
#ifdef I2C_STA_EN
		(void)check_status_full_nack(id);
#else
		delay_nop(I2C_DELAY);
#endif
		data[cnt] = ((m_i2c_hwd[id]->status & STATUS_DATA_MSK) >> STATUS_DATA_POS);
	}

read_error:
	m_i2c_hwd[id]->cmd = CMD_STOP(1U);
	return ret;
}

i2c_err_t i2c_slave_write_wait(i2c_id_t id, const u8 *data, u8 size)
{
	u8 cnt = 0U;

	while (1U != check_status_hit_rw(id)) {}

	while (cnt < size) {
		m_i2c_hwd[id]->cmd = CMD_TRANS(1U) | CMD_DATA(data[cnt]);
		cnt++;
		if (check_status_empty_ack(id)) {
			break;
		}
	}
	return (cnt == size) ? I2C_ERR_NONE : I2C_ERR_DATA_NACK;
}

i2c_err_t i2c_slave_read_wait(i2c_id_t id, u8 *data, u8 size, u8 *recvd_size)
{
	u8 cnt;
	u32 status;

	while (0U != check_status_hit_rw(id)) {}

	for (cnt = 0U; cnt < size; cnt++) {
		clean_i2c_status(id);
		m_i2c_hwd[id]->cmd = CMD_RECEIVE(1U) | CMD_ACK(0U);
		do {
			status = m_i2c_hwd[id]->status;
		} while ((0U == (status & STATUS_FULL_MSK)) && (0U != (status & STATUS_ADDR_MATCH_MSK)));
		if (0U == (status & STATUS_ADDR_MATCH_MSK)) {
			break;
		}
		data[cnt] = (m_i2c_hwd[id]->status & STATUS_DATA_MSK) >> STATUS_DATA_POS;
	}
	*recvd_size = cnt;
	return I2C_ERR_NONE;
}

/*******************************************************************************
**                      I2C Interrupt Functions                               **
*******************************************************************************/

ATTR_RAM_SECTION static void i2c_buf_send_byte(i2c_id_t id)
{
	u8 data;
	data = (m_i2c_int_cfg[id].tx_head < m_i2c_int_cfg[id].tx_tail) ?
	               m_i2c_int_cfg[id].tx_buf[m_i2c_int_cfg[id].tx_head] :
	               0xFFU;
	i2c_send_data(id, data);

	m_i2c_int_cfg[id].tx_head++;
	if (m_i2c_int_cfg[id].tx_head == m_i2c_int_cfg[id].tx_tail) {
		i2c_clear_irq_en(id);
		if (NULL != m_i2c_hdl[id].i2c_tx_cplt_hdl) {
			m_i2c_hdl[id].i2c_tx_cplt_hdl();
		}
	}
}

ATTR_RAM_SECTION void i2c_master_int_isr(i2c_id_t id, u32 status)
{
	(void)id;
	(void)status;

	//TODO implement i2c master interrupt r/w
}

/**
 * @brief Put data into buffer and wait i2c master to read. Drop remainder.
 */
void i2c_slave_write_int(i2c_id_t id, const u8 *pdat, u8 size)
{
	i2c_int_cfg_t *cfg = &m_i2c_int_cfg[id];

	cfg->tx_tail = 0U;
	cfg->tx_head = 0U;
	for (u8 i = 0U; i < size; i++) {
		cfg->tx_buf[cfg->tx_tail] = pdat[i];
		cfg->tx_tail++;
		if (cfg->tx_tail >= I2C_WRITE_BUF_SIZE) {
			cfg->tx_tail = 0U;
		}
	}
	i2c_set_irq_en(id);
}

void i2c_slave_read_int_start(i2c_id_t id)
{
	m_i2c_int_cfg[id].rx_enable = 1U;
	i2c_set_irq_en(id);
}

void i2c_slave_read_int_stop(i2c_id_t id)
{
	m_i2c_int_cfg[id].rx_enable = 0U;
	m_i2c_state[id] = I2C_STATE_IDLE;
	i2c_clear_irq_en(id);
}

ATTR_RAM_SECTION void i2c_slave_int_isr(i2c_id_t id, u32 status)
{
	clean_i2c_status(id);

	switch (m_i2c_state[id]) {
	case I2C_STATE_IDLE:
		if ((status & STATUS_ADDR_HIT_MSK) && (status & STATUS_ADDR_MATCH_MSK)) {
			if (status & STATUS_RW_MSK) {
				m_i2c_state[id] = I2C_STATE_ADDR_RX;
				i2c_buf_send_byte(id);
			} else {
				m_i2c_state[id] = I2C_STATE_ADDR_TX;
				m_i2c_hwd[id]->cmd = CMD_RECEIVE(1U) | CMD_ACK(0U);
			}
		}
		break;
	case I2C_STATE_ADDR_RX:
		if (status & STATUS_ACK_MSK) {
			m_i2c_state[id] = I2C_STATE_IDLE;
		} else {
			i2c_buf_send_byte(id);
		}
		break;
	case I2C_STATE_ADDR_TX:
		if ((status & STATUS_ADDR_HIT_MSK) && (status & STATUS_ADDR_MATCH_MSK)) {
			if (status & STATUS_RW_MSK) {
				m_i2c_state[id] = I2C_STATE_ADDR_RX;
				i2c_buf_send_byte(id);
			} else {
				m_i2c_hwd[id]->cmd = CMD_RECEIVE(1U) | CMD_ACK(0U);
			}
		} else {
			if (NULL != m_i2c_hdl[id].i2c_rx_hdl) {
				m_i2c_hdl[id].i2c_rx_hdl((status & STATUS_DATA_MSK) >> STATUS_DATA_POS);
			}
			if (1U == m_i2c_int_cfg[id].rx_enable) {
				m_i2c_hwd[id]->cmd = CMD_RECEIVE(1U) | CMD_ACK(0U);
			}
		}
		break;
	default: m_i2c_state[id] = I2C_STATE_IDLE; break;
	}
}

ATTR_RAM_SECTION void i2c_isr(void)
{
	u32 status;

	for (u8 i = 0U; i < (sizeof(m_i2c_hwd) / sizeof(m_i2c_hwd[0U])); i++) {
		status = m_i2c_hwd[i]->status;
		if (0U != (status & I2C_IRQ_MSK)) {
			m_i2c_isr[m_i2c_int_cfg[i].mode]((i2c_id_t)i, status);
		}
	}
}
