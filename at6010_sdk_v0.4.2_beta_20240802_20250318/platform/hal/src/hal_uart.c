/**
 *******************************************************************************
 * @file    hal_uart.c
 * @author  Airtouch Software Team
 * @brief   UART HAL driver
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
#include "gpio.h"
#include "hal_uart.h"
#include "at_error.h"

#ifndef UART0_PINMUX_SEL_X
#  define UART0_PINMUX_SEL_X      UART0_PINMUX_SEL_0
#elif (UART0_PINMUX_SEL_X != UART0_PINMUX_SEL_0) && (UART0_PINMUX_SEL_X != UART0_PINMUX_SEL_2) && (UART0_PINMUX_SEL_X != UART0_PINMUX_SEL_3)
#  error UART0_PINMUX_SEL_X Must be one of UART0_PINMUX_SEL_0/UART0_PINMUX_SEL_2/UART0_PINMUX_SEL_3 !!!
#endif

#ifndef UART1_PINMUX_SEL_X
#  define UART1_PINMUX_SEL_X      UART1_PINMUX_SEL_0
#elif (UART1_PINMUX_SEL_X != UART1_PINMUX_SEL_0) && (UART1_PINMUX_SEL_X != UART1_PINMUX_SEL_3)
#  error UART1_PINMUX_SEL_X Must be one of UART1_PINMUX_SEL_0/UART1_PINMUX_SEL_3 !!!
#endif

typedef void (*uart_clk_func_t)(void);

/* Private Variables ---------------------------------------------------------*/

static const uart_clk_func_t m_uart_enable[] = {sys_clk_enable_uart0, sys_clk_enable_uart1, sys_clk_enable_uart2};
static const uart_clk_func_t m_uart_disable[] = {sys_clk_disable_uart0, sys_clk_disable_uart1, sys_clk_disable_uart2};
static const IRQn_Type m_uart_nvic[] = {UART0_IRQn, UART1_IRQn, UART2_IRQn};
static hal_uart_state_t m_uart_state[UART_ID_QTY];

/* Private Functions ---------------------------------------------------------*/

static void uart_clk_init(uart_id_t idn)
{
	m_uart_enable[idn]();
}

static void uart_clk_deinit(uart_id_t idn)
{
	m_uart_disable[idn]();
}

static void uart_pinmux_init(uart_id_t idn)
{
	gpio_cfg_t gpio_cfg = { 0U };

	gpio_cfg.mode = GPIO_MODE_AF;
	gpio_cfg.pull = GPIO_PULL_UP;

	switch (idn) {
	case UART_ID_0:
#if   UART0_PINMUX_SEL_X == UART0_PINMUX_SEL_0
		gpio_cfg.af = GPIO_MCU6_AF0_UART0_TXD;
		gpio_init(GPIO_MCU_6, &gpio_cfg);

		gpio_cfg.af = GPIO_MCU7_AF0_UART0_RXD;
		gpio_init(GPIO_MCU_7, &gpio_cfg);
#elif UART0_PINMUX_SEL_X == UART0_PINMUX_SEL_2
		gpio_cfg.af = GPIO_MCU2_AF2_UART0_TXD;
		gpio_init(GPIO_MCU_2, &gpio_cfg);

		gpio_cfg.af = GPIO_MCU3_AF2_UART0_RXD;
		gpio_init(GPIO_MCU_3, &gpio_cfg);
#elif UART0_PINMUX_SEL_X == UART0_PINMUX_SEL_3
		gpio_cfg.af = GPIO_MCU4_AF3_UART0_TXD;
		gpio_init(GPIO_MCU_4, &gpio_cfg);

		gpio_cfg.af = GPIO_MCU5_AF3_UART0_RXD;
		gpio_init(GPIO_MCU_5, &gpio_cfg);
#else
#  error UART0_PINMUX_SEL_X Must be one of UART0_PINMUX_SEL_0/UART0_PINMUX_SEL_2/UART0_PINMUX_SEL_3 !!!
#endif
		break;

	case UART_ID_1:
#if   UART1_PINMUX_SEL_X == UART1_PINMUX_SEL_0
		gpio_cfg.af = GPIO_MCU8_AF0_UART1_TXD;
		gpio_init(GPIO_MCU_8, &gpio_cfg);

		gpio_cfg.af = GPIO_MCU9_AF0_UART1_RXD;
		gpio_init(GPIO_MCU_9, &gpio_cfg);
#elif UART1_PINMUX_SEL_X == UART1_PINMUX_SEL_3
		gpio_cfg.af = GPIO_MCU9_AF1_SPI_CSN;   // TODO temp fix
		gpio_init(GPIO_MCU_9, &gpio_cfg);

		gpio_cfg.af = GPIO_MCU2_AF3_UART1_TXD;
		gpio_init(GPIO_MCU_2, &gpio_cfg);

		gpio_cfg.af = GPIO_MCU3_AF3_UART1_RXD;
		gpio_init(GPIO_MCU_3, &gpio_cfg);
#else
#  error UART1_PINMUX_SEL_X Must be one of UART1_PINMUX_SEL_0/UART1_PINMUX_SEL_3 !!!
#endif
		break;

	case UART_ID_2:
#ifndef UART2_PINMUX_SEL_X
		/* UART2 not used, no need to init GPIO */
#elif UART2_PINMUX_SEL_X == UART2_PINMUX_SEL_1
		gpio_cfg.af = GPIO_MCU0_AF1_UART2_TXD;
		gpio_init(GPIO_MCU_0, &gpio_cfg);

		gpio_cfg.af = GPIO_MCU1_AF1_UART2_RXD;
		gpio_init(GPIO_MCU_1, &gpio_cfg);
#elif UART2_PINMUX_SEL_X == UART2_PINMUX_SEL_2
		gpio_cfg.af = GPIO_AON5_AF2_UART2_TXD;
		gpio_init(GPIO_AON_5, &gpio_cfg);

		gpio_cfg.af = GPIO_AON6_AF2_UART2_RXD;
		gpio_init(GPIO_AON_6, &gpio_cfg);
#elif UART2_PINMUX_SEL_X == UART2_PINMUX_SEL_3
		gpio_cfg.af = GPIO_AON0_AF3_UART2_TXD;
		gpio_init(GPIO_AON_0, &gpio_cfg);

		gpio_cfg.af = GPIO_AON1_AF3_UART2_RXD;
		gpio_init(GPIO_AON_1, &gpio_cfg);
#else
#  error UART2_PINMUX_SEL_X Must be one of UART2_PINMUX_SEL_1/UART2_PINMUX_SEL_2/UART2_PINMUX_SEL_3 !!!
#endif
		break;

	default:
		break;

	}
}

static void uart_nvic_init(uart_id_t idn, u8 en)
{
	if (en == 0U) {
		NVIC_DisableIRQ(m_uart_nvic[idn]);
	} else {
		NVIC_EnableIRQ(m_uart_nvic[idn]);
	}
}

/* Public Functions ----------------------------------------------------------*/

u8 hal_uart_init(uart_id_t idn, const uart_cfg_t *cfg)
{
	if (idn >= UART_ID_QTY) {
		return E_ID;
	}

#if 0 /* NOTE: Suppose APP will handle uart state */
	if (HAL_UART_STATE_RESET != m_uart_state[idn]) {
		return E_BUSY;
	}
#endif

	uart_pinmux_init(idn);

	uart_clk_init(idn);

	uart_rx_fifo_clr(idn);
	uart_tx_fifo_clr(idn);

	uart_baudrate_set(idn, (u32)cfg->baud_rate);
	uart_data_width_set(idn, cfg->data_width);
	uart_parity_set(idn, cfg->parity);
	uart_stop_bits_set(idn, cfg->stop_bits);

	uart_fifo_init(idn, &cfg->fifo_cfg);
	uart_set_dma_mode(idn, cfg->dma_mode);

	uart_nvic_init(idn, cfg->irq_enable);

	m_uart_state[idn] = HAL_UART_STATE_READY;
	return E_OK;
}

void hal_uart_deinit(uart_id_t idn)
{
	uart_clk_deinit(idn);
	m_uart_state[idn] = HAL_UART_STATE_RESET;
}

hal_uart_state_t hal_uart_state_get(uart_id_t idn)
{
	return m_uart_state[idn];
}

u8 hal_uart_send_wait(uart_id_t idn, u8 *data, u16 size, u32 timeout)
{
	UNUSED(timeout);
	m_uart_state[idn] = HAL_UART_STATE_BUSY_TX;
	uart_send_bs(idn, data, size);
	while (uart_get_status(idn, UART_TX_BUF_EMPTY) == 0U) {}
	m_uart_state[idn] = HAL_UART_STATE_READY;
	return E_OK;
}

u8 hal_uart_recv_wait(uart_id_t idn, u8 *data, u16 size, u32 timeout)
{
	UNUSED(timeout);
	if (idn >= UART_ID_QTY) {
		return E_ID;
	}
	if (hal_uart_state_get(idn) != HAL_UART_STATE_READY) {
		return E_BUSY;
	}
	m_uart_state[idn] = HAL_UART_STATE_BUSY_RX;
	for (u16 i = 0U; i < size; ++i) {
		data[i] = uart_recv_byte(idn);
	}
	m_uart_state[idn] = HAL_UART_STATE_READY;
	return E_OK;
}

#ifdef HAL_UART_DMA_EN
u8 hal_uart_transmit_dma_it(uart_handle_t *huart, u8 *tx_data, u16 size)
{
	if (huart->id >= UART_ID_QTY) {
		return E_ID;
	}

	if(HAL_OK != hal_dma_start_it(huart->hdmatx, (uint32)&tx_data[0], uart_get_dr_addr(huart->id), size, 1)) {
		return E_BUSY;
	}

	return E_OK;
}

u8 hal_uart_receive_dma_it(uart_handle_t *huart, u8 *rx_data, u16 size)
{
	if (huart->id >= UART_ID_QTY) {
		return E_ID;
	}

	if(HAL_OK != hal_dma_start_it(huart->hdmarx, uart_get_dr_addr(huart->id), (uint32)&rx_data[0], size, 1)) {
		return E_BUSY;
	}

	return E_OK;
}

u8 hal_uart_transmit_dma(uart_handle_t *huart, u8 *tx_data, u16 size)
{
	if (huart->id >= UART_ID_QTY) {
		return E_ID;
	}

	if(HAL_OK != hal_dma_start(huart->hdmatx, (uint32)&tx_data[0], uart_get_dr_addr(huart->id), size, 1)) {
		return E_BUSY;
	}

	return E_OK;
}

u8 hal_uart_receive_dma(uart_handle_t *huart, u8 *rx_data, u16 size)
{
	if (huart->id >= UART_ID_QTY) {
		return E_ID;
	}

	if(HAL_OK != hal_dma_start(huart->hdmarx, uart_get_dr_addr(huart->id), (uint32)&rx_data[0], size, 1)) {
		return E_BUSY;
	}

	return E_OK;
}
#endif
