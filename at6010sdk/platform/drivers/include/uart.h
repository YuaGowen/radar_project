/**
 *******************************************************************************
 * @file    uart.h
 * @author  Airtouch Software Team
 * @brief   UART Low-Level Driver
 *******************************************************************************
 * @copyright Copyright (c) 2024, Airtouching Intelligence Technology.
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

#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_LL_DRIVER AT6010 LL Driver
 * @{
 */

/**
 * @defgroup UART_LL UART
 * @brief UART Low-Level Driver
 * @{
 */

#include "at_types.h"

/**
 * @defgroup UART_LL_EC UART Export Constants
 * @brief UART export constants
 * @{
 */

#define UART_FIFO_DEPTH_BYTES           32U

/**
 * @}
 */

/**
 * @defgroup UART_LL_ET UART Export Types
 * @brief UART export types
 * @{
 */


/**
 * @brief UART buadrate
 */
typedef enum {
	UART_BAUD_RATE_2400    = 0x00000960, /**< 2400    bps                     */
	UART_BAUD_RATE_4800    = 0x000012C0, /**< 4800    bps                     */
	UART_BAUD_RATE_9600    = 0x00002580, /**< 9600    bps                     */
	UART_BAUD_RATE_19200   = 0x00004B00, /**< 19200   bps                     */
	UART_BAUD_RATE_115200  = 0x0001C200, /**< 115200  bps                     */
	UART_BAUD_RATE_921600  = 0x000E1000, /**< 921600  bps                     */
	UART_BAUD_RATE_1625000 = 0x0018CBA8, /**< 1625000 bps                     */
	UART_BAUD_RATE_3250000 = 0x00319750, /**< 3250000 bps                     */
	UART_BAUD_RATE_5000000 = 0x004c4b40  /**< 5000000 bps                     */
} uart_baud_rate_t;

/**
 * @brief UART Data bit
 */
typedef enum {
	UART_DATA_WIDTH_5BIT   = 0, /**< 5 bits                                   */
	UART_DATA_WIDTH_6BIT   = 1, /**< 6 bits                                   */
	UART_DATA_WIDTH_7BIT   = 2, /**< 7 bits                                   */
	UART_DATA_WIDTH_8BIT   = 3  /**< 8 bits                                   */
} uart_data_width_t;

/**
 * @brief UART Stop bit
 */
typedef enum {
	UART_STOP_BITS_1       = 0, /**< 1 bit                                    */
	UART_STOP_BITS_1_5     = 1, /**< 1.5 bit                                  */
	UART_STOP_BITS_2       = 2  /**< 2 bit                                    */
} uart_stop_bits_t;

/**
 * @brief UART Flow control
 */
typedef enum {
	UART_FLOW_CONTROL_DISABLED = 0, /**< Disabled                             */
	UART_FLOW_CONTROL_CTS      = 1, /**< CTS enabled                          */
	UART_FLOW_CONTROL_RTS      = 2, /**< RTS enabled                          */
	UART_FLOW_CONTROL_CTS_RTS  = 3, /**< CTS & RTS enabled                    */
} uart_flow_control_t;

/**
 * @brief UART Parity
 */
typedef enum {
	UART_PARITY_ODD            = 0, /**< Odd Parity                           */
	UART_PARITY_EVEN           = 1, /**< Even Parity                          */
	UART_PARITY_DISABLE        = 2  /**< No Parity                            */
} uart_parity_t;

/**
 * @brief UART ID
 */
typedef enum {
	UART_ID_0,
	UART_ID_1,
	UART_ID_2,
	UART_ID_QTY
} uart_id_t;

/**
 * @brief UART Status
 */
typedef enum {
	UART_TX_BUF_EMPTY,
	UART_RX_BUF_EMPTY,
	UART_RX_OVERRUN,
	UART_TX_BUF_FULL,
	UART_RX_BUF_FULL,
	UART_BUSY
} uart_status_t;

/**
 * @brief UART RX FIFO level
 * @details Controls the level of entries (or above) at which the receive FIFO controller triggers
 * an interrupt. When the number of receive FIFO entries is greater than or equal to this value, the
 * receive FIFO full interrupt is triggered.
 */
typedef enum {
	UART_RX_FIFO_CHAR_1       = 0, /**< At least 1 character in FIFO                              */
	UART_RX_FIFO_QUARTER_FULL = 1, /**< At least a quater in RX FIFO, that is 32/4=8 items        */
	UART_RX_FIFO_HALF_FULL    = 2, /**< At least half in RX FIFO, that is 32/2=16 items           */
	UART_RX_FIFO_FULL_2       = 3  /**< Just 2 less than full, that is 32-2=30 items              */
} uart_rx_fifo_level_t;

/**
 * @brief UART TX FIFO Level
 * @details Controls the level of entries (or below) at which the transmit FIFO controller triggers
 * an interrupt. When the number of transmit FIFO entires is less than or equal to this value, the
 * transmit FIFO empty interrupt is triggered.
 */
typedef enum {
	UART_TX_FIFO_EMPTY        = 0,
	UART_TX_FIFO_CHAR_2       = 1, /* 2 character in FIFO */
	UART_TX_FIFO_QUARTER_FULL = 2,
	UART_TX_FIFO_HALF_FULL    = 3
} uart_tx_fifo_level_t;

typedef struct {
	u8 fifo_en                         : 1;
	uart_tx_fifo_level_t tx_fifo_level : 2;
	uart_rx_fifo_level_t rx_fifo_level : 2;
	u8 reserved                        : 3;
} uart_fifo_cfg_t;

/**
 * @brief UART IRQ type
 */
typedef enum {
	UART_TX_EMPTY_INTR,
	UART_RX_INTR,
	UART_RX_IDLE_INTR,
	UART_RX_STATUS_INTR,
	UART_MODEM_STATUS_INTR,
	UART_BUSY_INTR,
	UART_IRQ_QTY
} uart_irq_t;

/**
 * @brief UART user callback function prototype
 */
typedef void (*uart_hdl_t)(void);

// UART_LL_ET
/**
 * @}
 */

/**
 * @defgroup UART_LL_EF UART Export Functions
 * @brief UART export functions
 * @{
 */

/**
 * @defgroup UART_LL_EF_CFG UART Configuration
 * @brief UART configuration functions
 * @{
 */

void uart_baudrate_set(uart_id_t idn, u32 baudrate);
void uart_baudrate_reset(uart_id_t idn, u32 baudrate);
void uart_data_width_set(uart_id_t idn, uart_data_width_t width);
void uart_parity_set(uart_id_t idn, uart_parity_t parity);
void uart_stop_bits_set(uart_id_t idn, uart_stop_bits_t stop_bits);

/**
 * @brief Enable/Disable UART DMA interface request.
 *
 * @param idn UART id
 * @param mode This parameter can be one of the following values:
 *        @arg 1 -- enable
 *        @arg 0 -- disable
 */
void uart_set_dma_mode(uart_id_t idn, u8 mode);

/**
 * @}
 */

/**
 * @defgroup UART_LL_EF_IO UART I/O
 * @brief UART I/O functions
 * @{
 */

/**
 * @brief Send one byte data.
 *
 * @param idn UART id
 * @param dat one byte data
 * @return u8 the data has been sent
 */
u8   uart_send_byte(uart_id_t idn, u8 dat);

/**
 * @brief Send byte array.
 *
 * @param idn UART id
 * @param buf byte array address
 * @param size byte array length in bytes
 */
void uart_send_bs(uart_id_t idn, u8 *buf, u16 size);

/**
 * @brief Send string.
 *
 * @param idn UART id
 * @param str c-string address
 */
void uart_send_str(uart_id_t idn, u8 *str);

/**
 * @brief Receive one byte by polling mode.
 *
 * @param idn UART id
 * @return u8 the received byte
 */
u8   uart_recv_byte(uart_id_t idn);

/**
 * @brief Check if the specified status has been set.
 *
 * @param idn UART id
 * @param status the UART status to be checked
 * @return u8 state of bit: 0 or 1
 */
u8   uart_get_status(uart_id_t idn, uart_status_t status);

/**
 * @}
 */

/**
 * @defgroup UART_LL_EF_IRQ UART IRQ
 * @brief UART IRQ functions
 * @{
 */

/**
 * @brief Set a specific UART callback function.
 *
 * @param idn UART id
 * @param irq one UART IRQ type
 * @param hdl user callback function
 */
void uart_set_hdl(uart_id_t idn, uart_irq_t irq, uart_hdl_t hdl);

/**
 * @brief Enable one UART interrupt type.
 *
 * @param idn UART id
 * @param irq UART IRQ type
 */
void uart_irq_enable(uart_id_t idn, uart_irq_t irq);

/**
 * @brief Disable one UART interrupt type.
 *
 * @param idn UART id
 * @param irq UART IRQ type
 */
void uart_irq_disable(uart_id_t idn, uart_irq_t irq);

/**
 * @brief UART interrupt service routine.
 * @note This function should be called in UARTx_Handler() where x = 0/1/2.
 *
 * @param idn UART id
 */
void uart_isr(uart_id_t idn);

/**
 * @}
 */

/**
 * @defgroup UART_LL_EF_FIFO UART FIFO
 * @brief UART FIFO functions
 * @{
 */

/**
 * @brief Set UART Tx/Rx FIFO level.
 *
 * @param idn UART id
 * @param cfg FIFO configuration
 */
void uart_fifo_init(uart_id_t idn, const uart_fifo_cfg_t *cfg);
void uart_tx_fifo_clr(uart_id_t idn);
void uart_rx_fifo_clr(uart_id_t idn);
/**
 * @}
 */

/**
 * @defgroup UART_LL_EF_MISC UART MISC
 * @brief UART MISC functions
 * @{
 */

/**
 * @brief Get UART data register address.
 *
 * @param idn UART id
 */
u32 uart_get_dr_addr(uart_id_t idn);

/**
 * @}
 */

// UART_LL_EF
/**
 * @}
 */

// UART_LL
/**
 * @}
 */

// AT6010_LL_DRIVER
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__UART_H__ */
