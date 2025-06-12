/**
 *******************************************************************************
 * @file    at6010_uart.c
 * @author  Airtouch Software Team
 * @brief   UART Low-Level Driver
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
 *    Intelligence Technology integrated circuit in a product or a software update
 *    for such product, must reproduce the above copyright notice, this list of
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

#include "at6010.h"
#include "clock.h"
#include "uart.h"
#include "reg_uart.h"
#include "dma.h"

//#define UART_DMA_EN
#define DLF_SIZE       6U

typedef enum {
	MODEM_STATUS = 0,
	NO_INTERRUPT_PENDING = 1,
	THR_EMPTY = 2,
	RECEIVED_DATA_AVAILABLE = 4,   /**< UART_RX_INTR      */
	RECEIVER_LINE_STATUS = 6,
	BUSY_DETECT = 7,
	CHARACTER_TIMEOUT = 0xC        /**< UART_RX_IDLE_INTR */
} irq_id_t;

#ifdef UART_DMA_EN
typedef struct {
	u8 *addr;
	s32 ch;
} uart_dma_t;

static uart_dma_t m_uart1_dma = {
	.addr = NULL,
	.ch = -1,
};
#endif

static HWP_UART_T *const m_uart_hwd[UART_ID_QTY] = {hwp_uart_0, hwp_uart_1, hwp_uart_2};
static uart_hdl_t m_uart_hdl[UART_ID_QTY][UART_IRQ_QTY];


__STATIC_INLINE void uart_bauddiv_set(uart_id_t idn, u32 div, u32 dlf)
{
	HWP_UART_T *uart_base = m_uart_hwd[idn];

	uart_base->uart_lcr |= UART_LCR_DLAB_MSK;
	uart_base->uart_rbr_dll_thr = div & 0xFFU;
	uart_base->uart_dlh_ier = div >> 8U;
	uart_base->uart_dlf = dlf;
	uart_base->uart_lcr &= (u32)~UART_LCR_DLAB_MSK;
}

void uart_baudrate_set(uart_id_t idn, u32 baudrate)
{
	u32 div;
	u32 res;
	u32 frac;
	u32 sys_clk = clk_mcu_get();

	div = (sys_clk >> 4) / baudrate;
	res = (sys_clk >> 4) % baudrate;
	frac = (res << DLF_SIZE) / baudrate;

	uart_bauddiv_set(idn, div, frac);
}

void uart_baudrate_reset(uart_id_t idn, u32 baudrate)
{
	HWP_UART_T *uart_base = m_uart_hwd[idn];
	u32 dlh_ier_val = 0;

	/* Wait tx fifo empty */
	while (0U == (uart_base->uart_usr & UART_USR_TFE_MSK)) {}
	/* Wait uart idle */
	while (0U != (uart_base->uart_usr & UART_USR_BUSY_MSK)) {}

	NVIC_DisableIRQ(UART0_IRQn);
	NVIC_ClearPendingIRQ(UART0_IRQn);
	dlh_ier_val = uart_base->uart_dlh_ier;
	uart_base->uart_dlh_ier = 0U; /* Disable all interrupts */
	NVIC_EnableIRQ(UART0_IRQn);

	uart_baudrate_set(idn, baudrate);

	uart_base->uart_dlh_ier = dlh_ier_val;
}

void uart_data_width_set(uart_id_t idn, uart_data_width_t width)
{
	HWP_UART_T *uart_base = m_uart_hwd[idn];
	u32 reg = uart_base->uart_lcr;
	reg &= (u32)~UART_LCR_DLS_MSK;
	reg |= UART_LCR_DLS((u32)width);
	uart_base->uart_lcr = reg;
}

void uart_parity_set(uart_id_t idn, uart_parity_t parity)
{
	HWP_UART_T *uart_base = m_uart_hwd[idn];
	u32 reg = uart_base->uart_lcr;

	if (UART_PARITY_DISABLE == parity) {
		reg &= ~UART_LCR_PEN_MSK;
	} else {
		reg |= UART_LCR_PEN_MSK;
		if (UART_PARITY_ODD == parity) {
			reg &= ~UART_LCR_EPS_MSK;
		} else {
			reg |= UART_LCR_EPS_MSK;
		}
	}
	uart_base->uart_lcr = reg;
}

void uart_stop_bits_set(uart_id_t idn, uart_stop_bits_t stop_bits)
{
	HWP_UART_T *uart_base = m_uart_hwd[idn];
	if (UART_STOP_BITS_1 == stop_bits) {
		uart_base->uart_lcr &= ~UART_LCR_STOP_MSK;
	} else {
		uart_base->uart_lcr |= UART_LCR_STOP_MSK;
	}
}

void uart_set_dma_mode(uart_id_t idn, u8 mode)
{
	m_uart_hwd[idn]->uart_sdmam = mode;
}

u8 uart_send_byte(uart_id_t idn, u8 dat)
{
	HWP_UART_T *uart_base = m_uart_hwd[idn];

	/* NOTE: default fifo enabled, otherwise use BUSY flag */
	while ((uart_base->uart_usr & UART_USR_TFNF_MSK) == 0U) {}
	uart_base->uart_rbr_dll_thr = dat;

	return dat;
}

void uart_send_bs(uart_id_t idn, u8 *buf, u16 size)
{
	u16 i;
	for (i = 0; i < size; i++) {
		(void)uart_send_byte(idn, buf[i]);
	}
}

void uart_send_str(uart_id_t idn, u8 *str)
{
	while (*str != 0U) {
		(void)uart_send_byte(idn, *str);
		str++;
	}
}

u8 uart_recv_byte(uart_id_t idn)
{
	HWP_UART_T *uart_base = m_uart_hwd[idn];

	while ((uart_base->uart_usr & UART_USR_RFNE_MSK) == 0U) {}
	return (u8)uart_base->uart_rbr_dll_thr;
}

u8 uart_get_status(uart_id_t idn, uart_status_t status)
{
	u32 state;
	switch (status) {
	case UART_TX_BUF_EMPTY:
		state = (m_uart_hwd[idn]->uart_lsr >> UART_LSR_TEMT_POS) & 1U;
		break;
	case UART_RX_BUF_EMPTY:
		state = (m_uart_hwd[idn]->uart_lsr >> UART_LSR_DR_POS) & 1U;
		state ^= 1U;
		break;
	case UART_RX_OVERRUN:
		state = (m_uart_hwd[idn]->uart_lsr >> UART_LSR_OE_POS) & 1U;
		break;
	case UART_TX_BUF_FULL:
		state = (m_uart_hwd[idn]->uart_usr >> UART_USR_TFNF_POS) & 1U;
		state ^= 1U;
		break;
	case UART_RX_BUF_FULL:
		state = (m_uart_hwd[idn]->uart_usr >> UART_USR_RFF_POS) & 1U;
		break;
	case UART_BUSY:
		state = (m_uart_hwd[idn]->uart_usr >> UART_USR_BUSY_POS) & 1U;
		break;
	default:
		state = 0U;
		break;
	}

	return (u8)state;
}

void uart_set_hdl(uart_id_t idn, uart_irq_t irq, uart_hdl_t hdl)
{
	m_uart_hdl[idn][irq] = hdl;
}

void uart_irq_enable(uart_id_t idn, uart_irq_t irq)
{
	HWP_UART_T *uart_base = m_uart_hwd[idn];
	switch (irq) {
	case UART_TX_EMPTY_INTR:
		uart_base->uart_dlh_ier |= UART_DLH_IER_ETBEI_MSK;
		break;
	case UART_RX_INTR:
		//TODO enable/disable all irq at same time
	case UART_RX_IDLE_INTR:
		uart_base->uart_dlh_ier |= UART_DLH_IER_ERBFI_MSK;
		break;
	case UART_RX_STATUS_INTR:
		uart_base->uart_dlh_ier |= UART_DLH_IER_ELSI_MSK;
		break;
	case UART_MODEM_STATUS_INTR:
		uart_base->uart_dlh_ier |= UART_DLH_IER_EDSSI_MSK;
		break;
	case UART_BUSY_INTR:
		break;
	default: /* Invalid irq */
		break;
	}
}

void uart_irq_disable(uart_id_t idn, uart_irq_t irq)
{
	HWP_UART_T *uart_base = m_uart_hwd[idn];
	switch (irq) {
	case UART_TX_EMPTY_INTR:
		uart_base->uart_dlh_ier &= ~UART_DLH_IER_ETBEI_MSK;
		break;
	case UART_RX_INTR:
		//TODO enable/disable all irq at same time
	case UART_RX_IDLE_INTR:
		uart_base->uart_dlh_ier &= ~UART_DLH_IER_ERBFI_MSK;
		break;
	case UART_RX_STATUS_INTR:
		uart_base->uart_dlh_ier &= ~UART_DLH_IER_ELSI_MSK;
		break;
	case UART_MODEM_STATUS_INTR:
		uart_base->uart_dlh_ier &= ~UART_DLH_IER_EDSSI_MSK;
		break;
	case UART_BUSY_INTR:
		break;
	default: /* Invalid irq */
		break;
	}
}

void uart_isr(uart_id_t idn)
{
	uart_hdl_t hdl;
	HWP_UART_T *uart_base = m_uart_hwd[idn];
	u32 irq_id = uart_base->uart_fcr_iir & 0xFU;
	switch (irq_id) {
	case MODEM_STATUS:
		hdl = m_uart_hdl[idn][UART_MODEM_STATUS_INTR];
		break;
	case THR_EMPTY:
		hdl = m_uart_hdl[idn][UART_TX_EMPTY_INTR];
		break;
	case RECEIVED_DATA_AVAILABLE:
		hdl = m_uart_hdl[idn][UART_RX_INTR];
		break;
	case RECEIVER_LINE_STATUS:
		hdl = m_uart_hdl[idn][UART_RX_STATUS_INTR];
		break;
	case BUSY_DETECT:
		hdl = m_uart_hdl[idn][UART_BUSY_INTR];
		break;
	case CHARACTER_TIMEOUT:
		hdl = m_uart_hdl[idn][UART_RX_IDLE_INTR];
		break;
	default:
		hdl = NULL;
		break;
	}
	if (hdl != NULL) {
		hdl();
	}
}

#ifdef UART_DMA_EN
void uart1_dma_send_bs(u8 *buf, u16 len)
{
	struct dma_config uart_cfg = {
		.auto_disable = 1,
		.data_width = DMA_BYTE,
		.slot_id = DMA_UART1,
		.offset = DMA_UART_OFFSET,
		.direction = DMA_AHB_APB,
		.wrap_enable = 0,
		.half_int_enable = 0,
		.irq_type = UART1_TX_DMA_IRQ,
		.started = 0,
		.half_callback = NULL,
		.end_callback = NULL,
	};
	m_uart1_dma.ch = (-1 == m_uart1_dma.ch) ? get_dma_free_channel() : m_uart1_dma.ch;

	if (-1 != m_uart1_dma.ch) {
		configure_dma_channel(m_uart1_dma.ch, &uart_cfg);
		start_dma_transfer(m_uart1_dma.ch, buf, len);
	}
}

static void uart1_dma_hdl(void)
{
	if (NULL != m_uart1_rx_hdl) {
		m_uart1_rx_hdl(m_uart1_dma.addr);
	}
}

void uart1_dma_recv_bs(u8 *buf, u16 len)
{
	struct dma_config uart_cfg = {
		.auto_disable = 1,
		.data_width = DMA_BYTE,
		.slot_id = DMA_UART1,
		.offset = DMA_UART_OFFSET,
		.direction = DMA_APB_AHB,
		.wrap_enable = 0,
		.half_int_enable = 0,
		.irq_type = UART1_RX_DMA_IRQ,
		.started = 0,
		.half_callback = uart1_dma_hdl,
		.end_callback = uart1_dma_hdl,
	};

	m_uart1_dma.ch = (-1 == m_uart1_dma.ch) ? get_dma_free_channel() : m_uart1_dma.ch;

	if (-1 != m_uart1_dma.ch) {
		configure_dma_channel(m_uart1_dma.ch, &uart_cfg);
		start_dma_transfer(m_uart1_dma.ch, buf, len);

		m_uart1_dma.addr = buf;
	}
}

u8 get_uart1_dma_channel(void)
{
	return m_uart1_dma.ch;
}
#endif

void uart_fifo_init(uart_id_t idn, const uart_fifo_cfg_t *cfg)
{
	u32 fifo_ctrl = 0U;
	fifo_ctrl |= ((u32)cfg->fifo_en << UART_FCR_IIR_FIFOE_POS);
	fifo_ctrl |= ((u32)cfg->tx_fifo_level << UART_FCR_IIR_TET_POS);
	fifo_ctrl |= ((u32)cfg->rx_fifo_level << UART_FCR_IIR_RT_POS);
	m_uart_hwd[idn]->uart_fcr_iir = fifo_ctrl;
}

void uart_tx_fifo_clr(uart_id_t idn)
{
	m_uart_hwd[idn]->uart_srr |= UART_SRR_XFR_MSK;
}

void uart_rx_fifo_clr(uart_id_t idn)
{
	m_uart_hwd[idn]->uart_srr |= UART_SRR_RFR_MSK;
}

u32 uart_get_dr_addr(uart_id_t idn)
{
	return (u32)&(m_uart_hwd[idn]->uart_rbr_dll_thr);
}
