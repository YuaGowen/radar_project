/**
 *******************************************************************************
 * @file    hal_uart.h
 * @author  Airtouch Software Team
 * @brief   UART HAL Driver
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


#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_UART UART
 * @brief UART HAL Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"
#include "uart.h"
#ifdef HAL_UART_DMA_EN
 #include "hal_dma.h"
#endif

/* Data Types ----------------------------------------------------------------*/


/**
 * @defgroup HAL_UART_PINMUX_SEL_GRP UART Pin Mux Selection
 * @brief UART Pin Mux Selection
 * @{
 */

/**
 * @defgroup HAL_UART_PINMUX_SEL_GRP_UART0 UART0 Pin Mux Selection
 * @details User-defined macro <b>UART0_PINMUX_SEL_X</b> can be one of the following values.
 * If not defined, then <b>UART0_PINMUX_SEL_0</b> is used by default.
 * @{
 */
#define UART0_PINMUX_SEL_0      0x10 /**< UART0_TX -- GPIO_MCU_6, UART0_RX -- GPIO_MCU_7 (default)*/
#define UART0_PINMUX_SEL_2      0x12 /**< UART0_TX -- GPIO_MCU_2, UART0_RX -- GPIO_MCU_3          */
#define UART0_PINMUX_SEL_3      0x13 /**< UART0_TX -- GPIO_MCU_4, UART0_RX -- GPIO_MCU_5          */
/**
 * @}
 */

/**
 * @defgroup HAL_UART_PINMUX_SEL_GRP_UART1 UART1 Pin Mux Selection
 * @details User-defined macro <b>UART1_PINMUX_SEL_X</b> can be one of the following values.
 * If not defined, then <b>UART1_PINMUX_SEL_0</b> is used by default.
 * @{
 */
#define UART1_PINMUX_SEL_0      0x20 /**< UART1_TX -- GPIO_MCU_8, UART1_RX -- GPIO_MCU_9 (default)*/
#define UART1_PINMUX_SEL_3      0x23 /**< UART1_TX -- GPIO_MCU_2, UART1_RX -- GPIO_MCU_3          */
/**
 * @}
 */

/**
 * @defgroup HAL_UART_PINMUX_SEL_GRP_UART2 UART2 Pin Mux Selection
 * @details User-defined macro <b>UART2_PINMUX_SEL_X</b> must be one of the following values.
 * If macro <b>UART2_PINMUX_SEL_X</b> is not defined, no pins are configured in hal_uart_init() and compiler does not complain.
 * @{
 */
#define UART2_PINMUX_SEL_1      0x31 /**< UART2_TX -- GPIO_MCU_0, UART2_RX -- GPIO_MCU_1          */
#define UART2_PINMUX_SEL_2      0x32 /**< UART2_TX -- GPIO_AON_5, UART2_RX -- GPIO_AON_6          */
#define UART2_PINMUX_SEL_3      0x33 /**< UART2_TX -- GPIO_AON_0, UART2_RX -- GPIO_ANO_1          */
/**
 * @}
 */

/**
 * @}
 */

typedef enum {
	HAL_UART_STATE_RESET = 0x00U,      /*!< Peripheral is not yet Initialized */
	HAL_UART_STATE_READY = 0x20U,      /*!< Peripheral Initialized and ready for use */
	HAL_UART_STATE_BUSY = 0x24U,       /*!< an internal process is ongoing */
	HAL_UART_STATE_BUSY_TX = 0x21U,    /*!< Data Transmission process is ongoing */
	HAL_UART_STATE_BUSY_RX = 0x22U,    /*!< Data Reception process is ongoing */
	HAL_UART_STATE_BUSY_TX_RX = 0x23U, /*!< Data Transmission and Reception process is ongoing */
	HAL_UART_STATE_TIMEOUT = 0xA0U,    /*!< Timeout state */
	HAL_UART_STATE_ERROR = 0xE0U       /*!< Error */
} hal_uart_state_t;

typedef struct {
	uart_baud_rate_t  baud_rate;
	uart_data_width_t data_width;
	uart_parity_t     parity;
	uart_stop_bits_t  stop_bits;
	u8                dma_mode;
	u8                irq_enable;
	u8                padding[2];
	uart_fifo_cfg_t   fifo_cfg;
} uart_cfg_t;

#ifdef HAL_UART_DMA_EN
typedef struct {
	uart_id_t      id;
	u8             padding[3];
	uart_cfg_t     cfg;
	dma_handle_t   *hdmatx;
	dma_handle_t   *hdmarx;
} uart_handle_t;
#endif

/* Function Declarations -----------------------------------------------------*/

u8 hal_uart_init(uart_id_t idn, const uart_cfg_t *cfg);
void hal_uart_deinit(uart_id_t idn);
hal_uart_state_t hal_uart_state_get(uart_id_t idn);

/**
 * @brief uart send data and wait until complete
 *
 * @param idn uart id
 * @param data data pointer
 * @param size data length
 * @param timeout Not applicable for now
 * @return u8 @ref AT_TYPE_ERROR
 */
u8 hal_uart_send_wait(uart_id_t idn, u8 *data, u16 size, u32 timeout);
u8 hal_uart_recv_wait(uart_id_t idn, u8 *data, u16 size, u32 timeout);

#ifdef HAL_UART_DMA_EN
/**
 * @brief UART DMA transmission
 *
 * @param huart UART handle
 * @param tx_data Pointer to transmission data buffer
 * @param size The amount of data to be sent
 * @return u8 HAL status
 */
u8 hal_uart_transmit_dma(uart_handle_t *huart, u8 *tx_data, u16 size);

/**
 * @brief UART DMA reception
 *
 * @param huart UART handle
 * @param tx_data Pointer to reception data buffer
 * @param size The amount of data to be received
 * @return u8 HAL status
 */
u8 hal_uart_receive_dma(uart_handle_t *huart, u8 *rx_data, u16 size);

/**
 * @brief UART DMA interrupt transmission
 *
 * @param huart UART handle
 * @param tx_data Pointer to transmission data buffer
 * @param size The amount of data to be sent
 * @return u8 HAL status
 */
u8 hal_uart_transmit_dma_it(uart_handle_t *huart, u8 *tx_data, u16 size);

/**
 * @brief UART DMA interrupt  reception
 *
 * @param huart UART handle
 * @param tx_data Pointer to reception data buffer
 * @param size The amount of data to be received
 * @return u8 HAL status
 */
u8 hal_uart_receive_dma_it(uart_handle_t *huart, u8 *rx_data, u16 size);
#endif

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_UART_H__ */
