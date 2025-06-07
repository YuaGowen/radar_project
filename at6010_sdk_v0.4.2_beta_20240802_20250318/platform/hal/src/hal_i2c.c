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

#include "hal_i2c.h"
#include "common.h"
#include "clock.h"
#include "hal_clock.h"

#define I2C_HOST_SCK_FREQ (200000U)

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

typedef void (*i2c_isr_t)(i2c_id_t id);

void i2c_slave_int_isr(i2c_id_t id);
void i2c_master_int_isr(i2c_id_t id);
void i2c_host_int_isr(i2c_id_t id);

static void hal_i2c0_isr(void);
static void hal_i2c1_isr(void);

static i2c_state_t m_i2c_state[I2C_ID_QTY];
static i2c_host_step_t m_i2c_step;
static i2c_int_cfg_t m_i2c_int_cfg[I2C_ID_QTY];
static i2c_cb_t m_i2c_cb[I2C_ID_QTY];
static const i2c_isr_t m_i2c_isr[] = {i2c_slave_int_isr, i2c_master_int_isr, i2c_host_int_isr};
static const i2c_hdl_t m_i2c_hdl[] = {hal_i2c0_isr, hal_i2c1_isr};

void hal_i2c_host_init(void)
{
	u32 div = hal_get_system_clk() / I2C_HOST_SCK_FREQ;
	sys_clk_enable_i2c();
	i2c_host_init((u16)div);
	m_i2c_int_cfg[I2C_ID_0].mode = I2C_MODE_HOST;
	i2c_set_hdl(I2C_ID_0, hal_i2c0_isr);
}

void hal_i2c_init(i2c_id_t id, const i2c_cfg_t *cfg)
{
	i2c_ctrl_t ctrl;

	sys_clk_enable_i2c();

	m_i2c_int_cfg[id].mode = cfg->mode;
	ctrl.div =
	        (cfg->mode == I2C_MODE_MASTER) ? (u16)(hal_get_system_clk() / cfg->sck) - 1U : 0U;
	ctrl.mode = cfg->mode;
	ctrl.addr = cfg->addr;

	i2c_init(id, &ctrl);
}

i2c_err_t hal_i2c_host_write_byte_int(u8 slv_addr, u8 mem_addr, u8 data)
{
	i2c_err_t err;

	if (m_i2c_int_cfg[I2C_ID_0].mode != I2C_MODE_HOST) {
		err = I2C_ERR_FALSE_MODE;
	} else if (m_i2c_state[I2C_ID_0] != I2C_STATE_IDLE) {
		err = I2C_ERR_BUSY;
	} else {
		m_i2c_state[I2C_ID_0] = I2C_STATE_ADDR_TX;
		m_i2c_int_cfg[I2C_ID_0].tx_buf[0] = mem_addr;
		m_i2c_int_cfg[I2C_ID_0].tx_buf[1] = data;
		m_i2c_step = I2C_HOST_ADDR_W;

		i2c_set_irq_en(I2C_ID_0);
		i2c_send_addr(I2C_ID_0, (u8)(slv_addr << 1));

		err = I2C_ERR_NONE;
	}

	return err;
}

i2c_err_t hal_i2c_host_read_byte_int(u8 slv_addr, u8 mem_addr)
{
	i2c_err_t err;

	if (m_i2c_int_cfg[I2C_ID_0].mode != I2C_MODE_HOST) {
		err = I2C_ERR_FALSE_MODE;
	} else if (m_i2c_state[I2C_ID_0] != I2C_STATE_IDLE) {
		err = I2C_ERR_BUSY;
	} else {
		m_i2c_state[I2C_ID_0] = I2C_STATE_ADDR_RX;
		m_i2c_int_cfg[I2C_ID_0].tx_buf[0] = mem_addr;
		m_i2c_int_cfg[I2C_ID_0].tx_buf[1] = slv_addr;
		m_i2c_step = I2C_HOST_ADDR_W;

		i2c_set_irq_en(I2C_ID_0);
		i2c_send_addr(I2C_ID_0, (u8)(slv_addr << 1));

		err = I2C_ERR_NONE;
	}

	return err;
}

ATTR_RAM_SECTION static void i2c_event_nack(i2c_id_t id)
{
	i2c_err_t err;

	i2c_send_stop(id);
	i2c_clear_irq_en(id);

	switch (m_i2c_step) {
	case I2C_HOST_ADDR_W:
	case I2C_HOST_ADDR_R: err = I2C_ERR_ADDR_NACK; break;
	case I2C_HOST_MEM: err = I2C_ERR_MEM_NACK; break;
	case I2C_HOST_DATA: err = I2C_ERR_DATA_NACK; break;
	default: err = I2C_ERR_FALSE_MODE; break;
	}
	if (NULL != m_i2c_cb[id].i2c_err_hdl) {
		m_i2c_cb[id].i2c_err_hdl(err);
	}
	m_i2c_state[id] = I2C_STATE_IDLE;
}

ATTR_RAM_SECTION static void i2c_event_ack(i2c_id_t id)
{
	switch (m_i2c_step) {
	case I2C_HOST_ADDR_W:
		i2c_send_data(I2C_ID_0, m_i2c_int_cfg[id].tx_buf[0]);
		m_i2c_step = I2C_HOST_MEM;
		break;
	case I2C_HOST_MEM:
		if (I2C_STATE_ADDR_TX == m_i2c_state[id]) {
			i2c_send_data(I2C_ID_0, m_i2c_int_cfg[id].tx_buf[1]);
			m_i2c_step = I2C_HOST_DATA;
		} else {
			i2c_send_addr(I2C_ID_0, (u8)(m_i2c_int_cfg[id].tx_buf[1] << 1) | 1U);
			m_i2c_step = I2C_HOST_ADDR_R;
		}
		break;
	case I2C_HOST_ADDR_R:
		i2c_recv_data(I2C_ID_0, 1U);
		m_i2c_step = I2C_HOST_DATA;
		break;
	case I2C_HOST_DATA:
		i2c_send_stop(id);
		i2c_clear_irq_en(id);

		if ((I2C_STATE_ADDR_TX == m_i2c_state[id]) &&
		    (NULL != m_i2c_cb[id].i2c_tx_cplt_hdl)) {
			m_i2c_cb[id].i2c_tx_cplt_hdl();
		} else if ((I2C_STATE_ADDR_RX == m_i2c_state[id]) &&
		           (NULL != m_i2c_cb[id].i2c_rx_hdl)) {
			m_i2c_cb[id].i2c_rx_hdl(i2c_get_data(id));
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

ATTR_RAM_SECTION void i2c_host_int_isr(i2c_id_t id)
{
	i2c_clear_status(id);

	if (1U == i2c_get_ack(id)) {
		i2c_event_nack(id);
	} else {
		i2c_event_ack(id);
	}
}

/*******************************************************************************
**                      I2C Interrupt Functions                               **
*******************************************************************************/

void hal_i2c_set_callback(i2c_id_t id, const i2c_cb_t *cb)
{
	m_i2c_cb[id].i2c_err_hdl = cb->i2c_err_hdl;
	m_i2c_cb[id].i2c_rx_hdl = cb->i2c_rx_hdl;
	m_i2c_cb[id].i2c_tx_cplt_hdl = cb->i2c_tx_cplt_hdl;
	i2c_set_hdl(id, m_i2c_hdl[id]);
}

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
		if (NULL != m_i2c_cb[id].i2c_tx_cplt_hdl) {
			m_i2c_cb[id].i2c_tx_cplt_hdl();
		}
	}
}

ATTR_RAM_SECTION void i2c_master_int_isr(i2c_id_t id)
{
	(void)id;
	//TODO implement i2c master interrupt r/w
}

/**
 * @brief Put data into buffer and wait i2c master to read. Drop remainder.
 */
void i2c_slave_write_int(i2c_id_t id, const u8 *pdat, u8 size)
{
	u8 i = 0;
	i2c_int_cfg_t *cfg = &m_i2c_int_cfg[id];

	cfg->tx_tail = 0U;
	cfg->tx_head = 0U;
	for (i = 0U; i < size; i++) {
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

ATTR_RAM_SECTION void i2c_slave_int_isr(i2c_id_t id)
{
	switch (m_i2c_state[id]) {
	case I2C_STATE_IDLE:
		if (1U == i2c_get_status_match(id)) {
			if (1U == i2c_get_rw(id)) {
				m_i2c_state[id] = I2C_STATE_ADDR_RX;
				i2c_buf_send_byte(id);
			} else {
				m_i2c_state[id] = I2C_STATE_ADDR_TX;
				i2c_recv_data(id, 0U);
			}
		}
		break;
	case I2C_STATE_ADDR_RX:
		if (1U == i2c_get_ack(id)) {
			m_i2c_state[id] = I2C_STATE_IDLE;
		} else {
			i2c_buf_send_byte(id);
		}
		break;
	case I2C_STATE_ADDR_TX:
		if (1U == i2c_get_status_match(id)) {
			if (1U == i2c_get_rw(id)) {
				m_i2c_state[id] = I2C_STATE_ADDR_RX;
				i2c_buf_send_byte(id);
			} else {
				i2c_recv_data(id, 0U);
			}
		} else {
			if (NULL != m_i2c_cb[id].i2c_rx_hdl) {
				m_i2c_cb[id].i2c_rx_hdl(i2c_get_data(id));
			}
			if (1U == m_i2c_int_cfg[id].rx_enable) {
				i2c_recv_data(id, 0U);
			}
		}
		break;
	default: m_i2c_state[id] = I2C_STATE_IDLE; break;
	}
	i2c_clear_status(id);
}

ATTR_RAM_SECTION static void hal_i2c0_isr(void)
{
	m_i2c_isr[m_i2c_int_cfg[0].mode](I2C_ID_0);
}

ATTR_RAM_SECTION static void hal_i2c1_isr(void)
{
	m_i2c_isr[m_i2c_int_cfg[1].mode](I2C_ID_1);
}
