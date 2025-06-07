/**
 * Copyright (c) 2024, Airtouching Intelligence Technology.
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

#include "ci_port.h"
#include "ci_main.h"
#include "hal_common.h"
#ifdef CFG_OS
#include "osal_task.h"
#include "sys_msg.h"
#else
#include "k_msg.h"
#endif

#include "common.h"
#include "i2c_host.h"
#include "hal_uart.h"
#include "gpio.h"

#ifdef CFG_CI

/* Constants -------------------------------------------------------------------------------------*/

/**
 * @defgroup CI_PORT_DEF CI Port Define
 * @details CI port is selected by macro @ref PARA_CI_PORT in target_config.h file on each project.
 * @{
 */
#define CI_PORT_UART                   0 /**< CI via UART */
#define CI_PORT_I2C                    1 /**< CI via I2C  */
/**
 * @}
 */

/**
 * @defgroup CI_PORT_CFG CI Port Configuration
 * @{
 */
#if PARA_CI_PORT == CI_PORT_UART
#  define UART_IDLE_TIME               0x20U
#elif PARA_CI_PORT == CI_PORT_I2C
#define CI_PORT_I2C_ID                 I2C_ID_0
#endif
/**
 * @}
 */

/* Data Types ------------------------------------------------------------------------------------*/



/* Private Variables -----------------------------------------------------------------------------*/

ci_port_ctx_t m_ci_ctxt = {
	.rx_buf = {0},             /* rx_buf */
	.rx_idx = 0,               /* rx_idx */
	.para_len = 0,               /* para_len */
	.state = CI_STATE_HEAD,   /* state */
	.is_ready = 0,               /* is_ready */
	.rx_mod = CI_RX_MOD_CMD,   /* rx_mod */
	.rx_d_buf = 0,               /* rx_d_buf */
	.rx_d_size = 0,               /* rx_d_size */
#ifdef CFG_OTA
	.m_dld_ctxt = { 0 }
#endif
};

/* support only one buf now. */
static ci_port_buf_t m_ci_buf[1];

/* Private Function Declarations -----------------------------------------------------------------*/

static void ci_port_rx_hdl(u8 dat);


/* Function Implementations ----------------------------------------------------------------------*/

#ifdef CI_RECV_TIMEOUT_EN
static void ci_recv_timeout_cb(void *arg)
{
	(void)arg;
	ci_port_reset();
}
#endif

#if PARA_CI_PORT == CI_PORT_UART
ATTR_RAM_SECTION static void ci_uart_rx_idle_hdl(void)
{
	u8 data;
	ci_port_reset();
	while (0U == uart_get_status(UART_CI_PORT, UART_RX_BUF_EMPTY)) {
		data = uart_recv_byte(UART_CI_PORT);
		ci_port_rx_hdl(data);
	}
}

ATTR_RAM_SECTION static void ci_uart_rx_hdl(void)
{
	u8 data;
	while (0U == uart_get_status(UART_CI_PORT, UART_RX_BUF_EMPTY)) {
		data = uart_recv_byte(UART_CI_PORT);
		ci_port_rx_hdl(data);
	}
}

static void ci_uart_init(void)
{
	uart_cfg_t cfg;

	cfg.baud_rate = UART_CI_BAUDRATE;
	cfg.parity = UART_PARITY_DISABLE;
	cfg.stop_bits = UART_STOP_BITS_1;
	cfg.data_width = UART_DATA_WIDTH_8BIT;
	cfg.irq_enable = 0;
	cfg.fifo_cfg.fifo_en = 1;
	cfg.fifo_cfg.rx_fifo_level = UART_RX_FIFO_CHAR_1;
	cfg.fifo_cfg.tx_fifo_level = UART_TX_FIFO_CHAR_2;
	(void)hal_uart_init(UART_CI_PORT, &cfg);

	uart_set_hdl(UART_CI_PORT, UART_RX_IDLE_INTR, ci_uart_rx_idle_hdl);
	uart_irq_enable(UART_CI_PORT, UART_RX_IDLE_INTR);

	uart_set_hdl(UART_CI_PORT, UART_RX_INTR, ci_uart_rx_hdl);
	uart_irq_enable(UART_CI_PORT, UART_RX_INTR);

	NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + UART_CI_PORT));
}

#elif PARA_CI_PORT == CI_PORT_I2C

static volatile u32 ci_i2c_tx_cplt_flag = 0;

ATTR_RAM_SECTION static void ci_i2c_rx_hdl(u8 data)
{
	ci_port_rx_hdl(data);
}

ATTR_RAM_SECTION static void ci_i2c_tx_cplt_hdl(void)
{
	ci_i2c_tx_cplt_flag = 1;
}

ATTR_RAM_SECTION void ci_i2c_send_bs(const u8 *buf, u16 size)
{
	i2c_slave_write_int(CI_PORT_I2C_ID, buf, size);
	while (ci_i2c_tx_cplt_flag == 0);
	ci_i2c_tx_cplt_flag = 0;

	i2c_slave_read_int_start(CI_PORT_I2C_ID);
}

static void ci_i2c_init(void)
{
	i2c_cfg_t cfg;
	i2c_hdl_t i2c_hdl;
	gpio_cfg_t gpio_cfg;

	/* I2C clock enable */
	sys_clk_enable_i2c();

	/* GPIO init for I2C */
	gpio_cfg.mode = GPIO_MODE_AF;
	gpio_cfg.pull = GPIO_PULL_UP;

	if (PARA_CI_I2C_SCL_PIN == GPIO_MCU_6) {
		gpio_cfg.af = GPIO_MCU6_AF3_I2C_SCL;
	} else if (PARA_CI_I2C_SCL_PIN == GPIO_MCU_8) {
		gpio_cfg.af = GPIO_MCU8_AF2_I2C_SCL;
	} else if (PARA_CI_I2C_SCL_PIN == GPIO_AON_3) {
		gpio_cfg.af = GPIO_AON3_AF1_I2C_SCL;
	} else {
		loge("PARA_CI_I2C_SCL_PIN invalid\r\n");
		while (1);
	}
	(void)gpio_init(PARA_CI_I2C_SCL_PIN, &gpio_cfg);

	if (PARA_CI_I2C_SDA_PIN == GPIO_MCU_7) {
		gpio_cfg.af = GPIO_MCU7_AF3_I2C_SDA;
	} else if (PARA_CI_I2C_SDA_PIN == GPIO_MCU_9) {
		gpio_cfg.af = GPIO_MCU9_AF2_I2C_SDA;
	} else if (PARA_CI_I2C_SDA_PIN == GPIO_AON_4) {
		gpio_cfg.af = GPIO_AON4_AF1_I2C_SDA;
	} else {
		loge("PARA_CI_I2C_SDA_PIN invalid\r\n");
		while (1);
	}
	(void)gpio_init(PARA_CI_I2C_SDA_PIN, &gpio_cfg);

	/* I2C init */
	cfg.mode = I2C_MODE_SLAVE;
	cfg.addr = PARA_CI_I2C_ADDR;
	i2c_init(CI_PORT_I2C_ID, &cfg);

	i2c_hdl.i2c_rx_hdl = ci_i2c_rx_hdl;
	i2c_hdl.i2c_tx_cplt_hdl = ci_i2c_tx_cplt_hdl;
	i2c_set_hdl(CI_PORT_I2C_ID, &i2c_hdl);
	i2c_irq_enable();
	i2c_slave_read_int_start(CI_PORT_I2C_ID);
}
#endif /* !PARA_CI_PORT */

void ci_port_init(void)
{
#if PARA_CI_PORT == CI_PORT_UART
	ci_uart_init();
#elif PARA_CI_PORT == CI_PORT_I2C
	ci_i2c_init();
#endif

	m_ci_buf[0].buf = m_ci_ctxt.rx_buf;
#ifndef CFG_OS
	m_ci_buf[0].msg.para = m_ci_buf;
#endif

#ifdef CI_RECV_TIMEOUT_EN
	OSAL_ASSERT_ERR(osal_timer_create(&m_ci_ctxt.tm_timeout, OSAL_TIMER_MODE_SINGLE, ci_recv_timeout_cb, NULL, CI_RECV_TIMEOUT_MS), "create CI timer failed");
#endif
}

void ci_port_reset(void)
{
	m_ci_ctxt.state = CI_STATE_HEAD;
	m_ci_ctxt.is_ready = 0;
	m_ci_ctxt.rx_idx = 0;
}

u8 ci_port_data_is_ready(void)
{
	return m_ci_ctxt.is_ready;
}

u8 * ci_port_get_buf(void)
{
	return m_ci_ctxt.rx_buf;
}

void ci_port_set_rx_mod(u8 mod)
{
	m_ci_ctxt.rx_mod = (ci_rx_mod_t)mod;
}

void ci_port_prepare_rx_data(u16 size)
{
	m_ci_ctxt.rx_d_size = size;
	m_ci_ctxt.is_ready = 0;
	m_ci_ctxt.rx_idx = 0;
}

ATTR_RAM_SECTION static void ci_port_rx_hdl(u8 dat)
{
#ifdef CFG_OTA
	// Save UART data to download context fifo
	if (m_ci_ctxt.m_dld_ctxt.m_ymodem_run) {
		fifo_enqueue(&m_ci_ctxt.m_dld_ctxt.m_dld_fifo, dat);
		return;
	}
#endif

	if (m_ci_ctxt.is_ready) {
		return;
	}

	/*
	 * rx data process.
	 */
	if (m_ci_ctxt.rx_mod == CI_RX_MOD_DATA) {
		m_ci_ctxt.rx_buf[m_ci_ctxt.rx_idx++] = dat;
		m_ci_ctxt.rx_d_size--;
		if ((m_ci_ctxt.rx_d_size == 0) || (m_ci_ctxt.rx_idx >= CI_RX_BUF_MAX)) {
			m_ci_ctxt.is_ready = 1;
		}
		return;
	}

	if (m_ci_ctxt.rx_idx >= CI_RX_BUF_MAX) {
		ci_port_reset();
	}

	/*
	 * rx cmd process.
	 */
	switch (m_ci_ctxt.state) {
	case CI_STATE_HEAD:
		if ((dat == CI_CTRL_FRM_HEAD) || (dat == CI_RESP_FRM_HEAD)) {
			m_ci_ctxt.rx_buf[m_ci_ctxt.rx_idx++] = dat;
			m_ci_ctxt.state = CI_STATE_CMD;
#ifdef CI_RECV_TIMEOUT_EN
			if (OSAL_OK != osal_timer_start(&m_ci_ctxt.tm_timeout)) {
				ci_port_reset();
			}
#endif
		}
		break;

	case CI_STATE_CMD:
		m_ci_ctxt.rx_buf[m_ci_ctxt.rx_idx++] = dat;
		m_ci_ctxt.state = CI_STATE_PARA_LEN;
		break;

	case CI_STATE_PARA_LEN:
		m_ci_ctxt.rx_buf[m_ci_ctxt.rx_idx++] = dat;
		m_ci_ctxt.para_len = dat;
		if (dat != 0) {
			m_ci_ctxt.state = CI_STATE_PARA;
		} else {
			m_ci_ctxt.state = CI_STATE_CHECK_CODE;
		}
		break;

	case CI_STATE_PARA:
		m_ci_ctxt.rx_buf[m_ci_ctxt.rx_idx++] = dat;
		if (m_ci_ctxt.rx_idx - 3 >= m_ci_ctxt.para_len) {
			m_ci_ctxt.state = CI_STATE_CHECK_CODE;
			m_ci_ctxt.para_len = 0;
		}
		break;

	case CI_STATE_CHECK_CODE:
		m_ci_ctxt.para_len++; /* record check code byte */
		m_ci_ctxt.rx_buf[m_ci_ctxt.rx_idx++] = dat;
		if (m_ci_ctxt.para_len >= 2U) {
#ifdef CFG_OS
			osal_msg_t msg;
			msg.id = (u32)MSG_SYS_CI_FRAME_RECEIVED;
			msg.para = (u32)&m_ci_buf[0];
			(void)osal_task_send_msg(MOD_SYS, &msg, OSAL_WAIT_FOREVER);
			/* TODO return not handled cause in isr */
#else
			msg_t *msg = NULL;
			msg = &m_ci_buf[0].msg;
			k_msg_post(MSG_ID_CI, msg);
#endif /* #ifdef CFG_OS */
			m_ci_ctxt.state = CI_STATE_HEAD;
			m_ci_ctxt.is_ready = 1;
			m_ci_ctxt.para_len = 0;

#ifdef CI_RECV_TIMEOUT_EN
			osal_timer_stop(&m_ci_ctxt.tm_timeout);
#endif

#if PARA_CI_PORT == CI_PORT_I2C
			i2c_slave_read_int_stop(CI_PORT_I2C_ID);
#endif
		}
		break;

	default:
		break;
	}
}

void ci_send_bytes(u8 *buf, u16 size)
{
#if PARA_CI_PORT == CI_PORT_UART

#if defined(CFG_OTA)
	if (!m_ci_ctxt.m_dld_ctxt.m_ymodem_run) {
#endif
		uart_send_bs(UART_CI_PORT, buf, size);
		while (uart_get_status(UART_CI_PORT, UART_TX_BUF_EMPTY) == 0U);
#if defined(CFG_OTA)
	}
#endif

#elif PARA_CI_PORT == CI_PORT_I2C
	ci_i2c_send_bs(buf, size);
#endif
}

#endif /* !CFG_CI */
