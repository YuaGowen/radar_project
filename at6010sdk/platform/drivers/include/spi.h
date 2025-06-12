/**
 *******************************************************************************
 * @file    spi.h
 * @author  Airtouch Software Team
 * @brief   SPI Low-level driver header
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
 ******************************************************************************/

#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "memory_map.h"

/**
 * @addtogroup AT6010_LL_DRIVER
 * @{
 */

/**
 * @addtogroup AT6010_LL_PER_BASE
 * @{
 */
typedef enum {
	SPI0 = (uint32_t)REG_SSI_BASE_0, /**< SPI0 instance */
	SPI1 = (uint32_t)REG_SSI_BASE_1, /**< SPI1 instance */
} spi_base_t;
/**
 * @}
 */


/**
 * @defgroup SPI_LL  SPI
 * @brief SPI Low-Level Driver
 * @{
 */

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/
/**
 * @defgroup SPI_LL_Exported_Constants SPI Exported Constants
 * @{
 */

/**
 * @defgroup SPI_LL_EC_SPI_WORK_MODE SPI Work Mode
 * @brief SPI work mode
 * @{
 */
#define LL_SPI_WORK_MODE_SLAVE            (0U) /**< Slave mode */
#define LL_SPI_WORK_MODE_MASTER           (1U) /**< Master mode, default */
/**
 * @}
 */

/**
 * @defgroup SPI_LL_EC_SPI_FRAME_FORMAT SPI Frame Format
 * @brief SPI Frame Format
 * @{
 */
#define LL_SPI_SPI_FRAME_FORMAT_STANDARD  (0U) /**< Standard SPI format */
#define LL_SPI_SPI_FRAME_FORMAT_DUAL      (1U) /**< Dual SPI format */
#define LL_SPI_SPI_FRAME_FORMAT_QUAD      (2U) /**< Quad SPI format */
#define LL_SPI_SPI_FRAME_FORMAT_OCTAL     (3U) /**< Octal SPI format */
/**
 * @}
 */

/**
 * @defgroup SPI_LL_EC_CTRL_FRAME_SIZE SPI (Microwire frame format) Control Frame Size
 * @brief SPI (Microwire frame) Control Frame Size
 * @{
 */
#define LL_SPI_CTRL_FRAME_SIZE_1_BITS     (0U) /**< 1 bit */
#define LL_SPI_CTRL_FRAME_SIZE_2_BITS     (1U) /**< 2 bits */
#define LL_SPI_CTRL_FRAME_SIZE_3_BITS     (2U) /**< 3 bits */
#define LL_SPI_CTRL_FRAME_SIZE_4_BITS     (3U) /**< 4 bits */
#define LL_SPI_CTRL_FRAME_SIZE_5_BITS     (4U) /**< 5 bits */
#define LL_SPI_CTRL_FRAME_SIZE_6_BITS     (5U) /**< 6 bits */
#define LL_SPI_CTRL_FRAME_SIZE_7_BITS     (6U) /**< 7 bits */
#define LL_SPI_CTRL_FRAME_SIZE_8_BITS     (7U) /**< 8 bits */
#define LL_SPI_CTRL_FRAME_SIZE_9_BITS     (8U) /**< 9 bits */
#define LL_SPI_CTRL_FRAME_SIZE_10_BITS    (9U) /**< 10 bits */
#define LL_SPI_CTRL_FRAME_SIZE_11_BITS    (10U)/**< 11 bits */
#define LL_SPI_CTRL_FRAME_SIZE_12_BITS    (11U)/**< 12 bits */
#define LL_SPI_CTRL_FRAME_SIZE_13_BITS    (12U)/**< 13 bits */
#define LL_SPI_CTRL_FRAME_SIZE_14_BITS    (13U)/**< 14 bits */
#define LL_SPI_CTRL_FRAME_SIZE_15_BITS    (14U)/**< 15 bits */
#define LL_SPI_CTRL_FRAME_SIZE_16_BITS    (15U)/**< 16 bits */
/**
 * @}
 */

/**
 * @defgroup SPI_LL_EC_SLAVE_SELECT_TOGGLE SPI Slave Select Toggle Enable
 * @brief SPI Slave Select Toggle Enable. While operating in SPI mode with clock phase (SCPH) set to 0,
 * this parameter controls the behavior of the CS between data frames.
 * @{
 */
#define LL_SPI_SLAVE_SELECT_TGL_DISABLE   (0U) /**< CS will stay low and SCLK will run continuously for the duration of the transfer */
#define LL_SPI_SLAVE_SELECT_TGL_ENABLE    (1U) /**< CS will toggle between consecutive data frames, with the serial clock (SCLK) being held to its default value while CS is high */
/**
 * @}
 */

/**
 * @defgroup SPI_LL_EC_SLAVE_OUTPUT_ENABLE SPI Slave Output Enable
 * @brief Relevant only when SPI is configured as a serial-slave device.
 * This bit is enabled after reset and must be disabled by software (when
 * boradcast mode is used), if you do not want this device to respond with data.
 * @{
 */
#define LL_SPI_SLAVE_OUTPUT_ENABLE        (0U) /**< Slave output is enabled */
#define LL_SPI_SLAVE_OUTPUT_DISALBE       (1U) /**< Slave output is disabled */
/**
 * @}
 */

/**
 * @defgroup SPI_LL_EC_TMOD SPI Transfer Mode
 * @brief SPI Transfer Mode
 * @{
 */
#define LL_SPI_TMODE_TX_AND_RX            (0U) /**< Transmit & Receive */
#define LL_SPI_TMODE_TX_ONLY              (1U) /**< Transmit only */
#define LL_SPI_TMODE_RX_ONLY              (2U) /**< Receive only */
#define LL_SPI_TMODE_EEPROM_READ          (3U) /**< EEPROM Read mode */
/**
 * @}
 */


/**
 * @defgroup SPI_LL_EC_SCPOL SPI Serial Clock Polarity
 * @brief Serial Clock Polarity. Valid when the frame format (FRF) is set to the Motorola SPI.
 * @{
 */
#define LL_SPI_CLK_POLARITY_LOW           (0U)
#define LL_SPI_CLK_POLARITY_HIGH          (1U)
/**
 * @}
 */

/**
 * @defgroup SPI_LL_EC_SCPH SPI Serial Clock Phase
 * @brief Serial Clock Phase. Valid when the frame format (FRF) is set to Motorola SPI.
 * @{
 */
#define LL_SPI_CLK_PHASE_FIRST_EDGE       (0U) /**< Data are captured on the first edge of the serial clock */
#define LL_SPI_CLK_PHASE_SECOND_EDGE      (1U) /**< Data are captured on the second edge of the serial clock */
/**
 * @}
 */


/**
 * @defgroup SPI_LL_EC_FRF SPI Frame Protocol
 * @brief SPI Frame Protocol
 * @{
 */
#define LL_SPI_FRAME_PROTOCOL_MOTOROLA_SPI (0U) /**< Motorola SPI */
#define LL_SPI_FRAME_PROTOCOL_TI_SSP       (1U) /**< Texas Instruments SSP */
#define LL_SPI_FRAME_PROTOCOL_NS_MICROWIRE (2U) /**< National Semiconductors Microwire */
/**
 * @}
 */


/**
 * @defgroup SPI_LL_EC_DFS SPI Data Frame Size
 * @brief SPI Data Frame Size
 * @{
 */
#define LL_SPI_DFS_4_BITS                 (3U) /**< 4-bit serial data transfer */
#define LL_SPI_DFS_5_BITS                 (4U) /**< 5-bit serial data transfer */
#define LL_SPI_DFS_6_BITS                 (5U) /**< 6-bit serial data transfer */
#define LL_SPI_DFS_7_BITS                 (6U) /**< 7-bit serial data transfer */
#define LL_SPI_DFS_8_BITS                 (7U) /**< 8-bit serial data transfer */
#define LL_SPI_DFS_9_BITS                 (8U) /**< 9-bit serial data transfer */
#define LL_SPI_DFS_10_BITS                (9U) /**< 10-bit serial data transfer */
#define LL_SPI_DFS_11_BITS                (10U) /**< 11-bit serial data transfer */
#define LL_SPI_DFS_12_BITS                (11U) /**< 12-bit serial data transfer */
#define LL_SPI_DFS_13_BITS                (12U) /**< 13-bit serial data transfer */
#define LL_SPI_DFS_14_BITS                (13U) /**< 14-bit serial data transfer */
#define LL_SPI_DFS_15_BITS                (14U) /**< 15-bit serial data transfer */
#define LL_SPI_DFS_16_BITS                (15U) /**< 16-bit serial data transfer */
#define LL_SPI_DFS_17_BITS                (16U) /**< 17-bit serial data transfer */
#define LL_SPI_DFS_18_BITS                (17U) /**< 18-bit serial data transfer */
#define LL_SPI_DFS_19_BITS                (18U) /**< 19-bit serial data transfer */
#define LL_SPI_DFS_20_BITS                (19U) /**< 20-bit serial data transfer */
#define LL_SPI_DFS_21_BITS                (20U) /**< 21-bit serial data transfer */
#define LL_SPI_DFS_22_BITS                (21U) /**< 22-bit serial data transfer */
#define LL_SPI_DFS_23_BITS                (22U) /**< 23-bit serial data transfer */
#define LL_SPI_DFS_24_BITS                (23U) /**< 24-bit serial data transfer */
#define LL_SPI_DFS_25_BITS                (24U) /**< 25-bit serial data transfer */
#define LL_SPI_DFS_26_BITS                (25U) /**< 26-bit serial data transfer */
#define LL_SPI_DFS_27_BITS                (26U) /**< 27-bit serial data transfer */
#define LL_SPI_DFS_28_BITS                (27U) /**< 28-bit serial data transfer */
#define LL_SPI_DFS_29_BITS                (28U) /**< 29-bit serial data transfer */
#define LL_SPI_DFS_30_BITS                (29U) /**< 30-bit serial data transfer */
#define LL_SPI_DFS_31_BITS                (30U) /**< 31-bit serial data transfer */
#define LL_SPI_DFS_32_BITS                (31U) /**< 32-bit serial data transfer */
/**
 * @}
 */

/**
 * @defgroup SPI_LL_EC_MDD Microwire Data Direction
 * @brief Microwire Data Direction
 * @{
 */
#define LL_SPI_MICRO_DIR_RECEIVE          (0U) /**< Microwire Receive Data (default) */
#define LL_SPI_MICRO_DIR_TRANSMIT         (1U) /**< Microwire Transmit Data */
/**
 * @}
 */

/**
 * @defgroup SPI_LL_EC_MWMOD Microwire Transfer Mode
 * @brief Microwire Transfer Mode
 * @{
 */
#define LL_SPI_MICRO_TMOD_NON_SEQ         (0U) /**< Non-Sequential Transfer */
#define LL_SPI_MICRO_TMODE_SEQ            (1U) /**< Sequential Transfer */
/**
 * @}
 */


/**
 * @defgroup SPI_LL_EC_STATUS SPI Status
 * @brief SPI Status
 * @{
 */
#define LL_SPI_STATUS_BUSY                (0x01U) /**< Busy */
#define LL_SPI_STATUS_TX_FIFO_NOT_FULL    (0x02U) /**< Transmit FIFO Not Full */
#define LL_SPI_STATUS_TX_FIFO_EMPTY       (0x04U) /**< Transmit FIFO Empty */
#define LL_SPI_STATUS_RX_FIFO_NOT_EMPTY   (0x08U) /**< Receive FIFO Not Empty */
#define LL_SPI_STATUS_RX_FIFO_FULL        (0x10U) /**< Receive FIFO Full */
#define LL_SPI_STATUS_TX_ERR              (0x20U) /**< Transmission Error */
#define LL_SPI_STATUS_DCOL_ERR            (0x40U) /**< Data Collision Error */
/**
 * @}
 */

/**
 * @defgroup SPI_LL_EC_IT SPI Interrupt Mask and Status Type
 * @brief SPI Interrupt Mask and Status Type
 * @{
 */
#define LL_SPI_IT_TX_FIFO_EMPTY           (0x01U) /**< TX FIFO Empty */
#define LL_SPI_IT_TX_FIFO_OVERFLOW        (0x02U) /**< TX FIFO Overflow */
#define LL_SPI_IT_RX_FIFO_UNDERFLOW       (0x04U) /**< RX FIFO Underflow */
#define LL_SPI_IT_RX_FIFO_OVERFLOW        (0x08U) /**< RX FIFO Overflow */
#define LL_SPI_IT_RX_FIFO_FULL            (0x10U) /**< RX FIFO Full */
#define LL_SPI_IT_MULTI_MASTER_CONT       (0x20U) /**< Multi-Master Contention */
#define LL_SPI_IT_SPI_TX_ERR              (0x400U)/**< SPI Transmit Error */
/**
 * @}
 */

/**
 * @defgroup SPI_LL_EC_INST_LENGTH SPI Dual/Quad/Octal mode Instruction Length
 * @brief SPI Dual/Quad/Octal mode Instruction Length
 * @{
 */
#define LL_SPI_INSTRUCTION_LEN_0_BITS     (0x00U) /**< No Instruction */
#define LL_SPI_INSTRUCTION_LEN_4_BITS     (0x01U) /**< 4 bits */
#define LL_SPI_INSTRUCTION_LEN_8_BITS     (0x02U) /**< 8 bits */
#define LL_SPI_INSTRUCTION_LEN_16_BITS    (0x03U) /**< 16 bits */
/**
 * @}
 */

/**
 * @defgroup SPI_LL_EC_ADDR_LENGTH SPI Dual/Quad/Octal Mode Address Length
 * @brief SPI Dual/Quad/Octal Mode Address Length
 * @{
 */
#define LL_SPI_ADDR_LEN_0_BITS            (0x00U) /**< No address */
#define LL_SPI_ADDR_LEN_4_BITS            (0x01U) /**< 4 bits address */
#define LL_SPI_ADDR_LEN_8_BITS            (0x02U) /**< 8 bits address */
#define LL_SPI_ADDR_LEN_12_BITS           (0x03U) /**< 12 bits address */
#define LL_SPI_ADDR_LEN_16_BITS           (0x04U) /**< 16 bits address */
#define LL_SPI_ADDR_LEN_20_BITS           (0x05U) /**< 20 bits address */
#define LL_SPI_ADDR_LEN_24_BITS           (0x06U) /**< 24 bits address */
#define LL_SPI_ADDR_LEN_28_BITS           (0x07U) /**< 28 bits address */
#define LL_SPI_ADDR_LEN_32_BITS           (0x08U) /**< 32 bits address */
#define LL_SPI_ADDR_LEN_36_BITS           (0x09U) /**< 36 bits address */
#define LL_SPI_ADDR_LEN_40_BITS           (0x0AU) /**< 40 bits address */
#define LL_SPI_ADDR_LEN_44_BITS           (0x0BU) /**< 44 bits address */
#define LL_SPI_ADDR_LEN_48_BITS           (0x0CU) /**< 48 bits address */
#define LL_SPI_ADDR_LEN_52_BITS           (0x0DU) /**< 52 bits address */
#define LL_SPI_ADDR_LEN_56_BITS           (0x0EU) /**< 56 bits address */
#define LL_SPI_ADDR_LEN_60_BITS           (0x0FU) /**< 60 bits address */
/**
 * @}
 */

/**
 * @defgroup SPI_LL_EC_TRANS_TYPE SPI Dual/Quad/Octal mode Transfer Type
 * @brief SPI Dual/Quad/Octal mode Transfer Type
 * @{
 */
#define LL_SPI_INST_SPI_ADDR_SPI          (0x00U) /**< Instruction and Address will be sent in Standard SPI */
#define LL_SPI_INST_SPI_ADDR_FRF          (0x01U) /**< Instruction in Standard SPI, Address in FRF */
#define LL_SPI_INST_FRF_ADDR_FRF          (0x02U) /**< Instruction and Address will be sent in FRF */
/**
 * @}
 */

/**
 * @}
 */

/* Exported Functions --------------------------------------------------------*/
/**
 * @defgroup SPI_LL_Exported_Functions SPI Exported Functions
 * @{
 */

/**
 * @defgroup SPI_LL_EF_Configuration Configuration
 * @{
 */

/**
 * @brief Select if SPI work in Master or Slave mode.
 *
 * @param spix SPIx instance
 * @param mode This parameter can be one of the following values:
 *        @arg @ref LL_SPI_WORK_MODE_SLAVE
 *        @arg @ref LL_SPI_WORK_MODE_MASTER
 * @return None
 */
void spi_set_work_mode(spi_base_t spix, uint32_t mode);

uint32_t spi_get_work_mode(spi_base_t spix);

/**
 * @brief Enable/Disable Dynamic wait states in SPI mode of operation.
 * Only valid when frame format is Motorola SPI frame format.
 *
 * @param spix SPIx instance
 * @param en 1 - enable, 0 -disable.
 * @return void
 */
void spi_set_dynamic_wait_state_enable(spi_base_t spix, uint32_t en);

uint32_t spi_get_dynamic_wait_state_enable(spi_base_t spix);

/**
 * @brief Select data frame format for Transmitting/Receiving the data.
 *
 * @param spix SPIx instance
 * @param spi_frf This parameter can be one of the following values:
 *        @arg @ref LL_SPI_SPI_FRAME_FORMAT_STANDARD
 *        @arg @ref LL_SPI_SPI_FRAME_FORMAT_DUAL
 *        @arg @ref LL_SPI_SPI_FRAME_FORMAT_QUAD
 *        @arg @ref LL_SPI_SPI_FRAME_FORMAT_OCTAL
 * @return None
 */
void spi_set_spi_frame_format(spi_base_t spix, uint32_t spi_frf);

uint32_t spi_get_spi_frame_format(spi_base_t spix);

/**
 * @brief Set the length of the control word for the Microwire frame format.
 *
 * @param spix SPIx instance
 * @param cfs This parameter can be one of the following values:
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_1_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_2_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_3_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_4_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_5_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_6_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_7_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_8_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_9_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_10_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_11_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_12_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_13_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_14_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_15_BITS
 *        @arg @ref LL_SPI_CTRL_FRAME_SIZE_16_BITS
 * @return None
 */
void spi_set_control_frame_size(spi_base_t spix, uint32_t cfs);

uint32_t spi_get_control_frame_size(spi_base_t spix);

/**
 * @brief While operating in SPI mode with clock phase (SCPH) set to 0,
 * this parameter controls the behavior of the CS between data frames.
 *
 * @param spix SPIx instance
 * @param en This parameter can be one of the following values:
 *        @arg @ref LL_SPI_SLAVE_SELECT_TGL_DISABLE
 *        @arg @ref LL_SPI_SLAVE_SELECT_TGL_ENABLE
 * @return None
 */
void spi_set_slave_select_toggle_enable(spi_base_t spix, uint32_t en);

uint32_t spi_get_slave_select_toggle_enable(spi_base_t spix);

/**
 * @brief Slave output enable
 *
 * @param spix SPIx instance
 * @param en This parameter can be one of the following values:
 *        @arg @ref LL_SPI_SLAVE_OUTPUT_ENABLE
 *        @arg @ref LL_SPI_SLAVE_OUTPUT_DISALBE
 * @return None
 */
void spi_set_slave_output_enable(spi_base_t spix, uint32_t en);

uint32_t spi_get_slave_output_enable(spi_base_t spix);

/**
 * @brief Set Transfer Mode.
 *
 * @param spix SPIx instance
 * @param tmod This parameter can one of the following values:
 *        @arg @ref LL_SPI_TMODE_TX_AND_RX
 *        @arg @ref LL_SPI_TMODE_TX_ONLY
 *        @arg @ref LL_SPI_TMODE_RX_ONLY
 *        @arg @ref LL_SPI_TMODE_EEPROM_READ
 * @return None
 */
void spi_set_transfer_mode(spi_base_t spix, uint32_t tmod);

uint32_t spi_get_transfer_mode(spi_base_t spix);

/**
 * @brief Set Serial Clock Polarity.
 * Valid when the frame format (FRF) is set to Motorola SPI.
 * @param spix SPIx instance
 * @param scpol This parameter can be one of the following values:
 *        @arg @ref LL_SPI_CLK_POLARITY_LOW
 *        @arg @ref LL_SPI_CLK_POLARITY_HIGH
 * @return None
 */
void spi_set_clock_polarity(spi_base_t spix, uint32_t scpol);

uint32_t spi_get_clock_polarity(spi_base_t spix);

/**
 * @brief Set Serial Clock Phase.
 *
 * @param spix SPIx instance
 * @param scph This parameter can be one of the following values:
 *        @arg @ref LL_SPI_CLK_PHASE_FIRST_EDGE
 *        @arg @ref LL_SPI_CLK_PHASE_SECOND_EDGE
 * @return None
 */
void spi_set_clock_phase(spi_base_t spix, uint32_t scph);

uint32_t spi_get_clock_phase(spi_base_t spix);

/**
 * @brief Select which serial protocol transfers the data.
 *
 * @param spix SPIx instance
 * @param frf This parameter can be one of the following values:
 *        @arg @ref LL_SPI_FRAME_PROTOCOL_MOTOROLA_SPI
 *        @arg @ref LL_SPI_FRAME_PROTOCOL_TI_SSP
 *        @arg @ref LL_SPI_FRAME_PROTOCOL_NS_MICROWIRE
 * @return None
 */
void spi_set_frame_protocol(spi_base_t spix, uint32_t frf);

uint32_t spi_get_frame_protocol(spi_base_t spix);

/**
 * @brief Set Data Frame Size. When the data frame size is programmed to be less
 * than 32 bits, the received data is automatically right-justified by the receive
 * logic, with the upper bits of the receive FIFO zero-padded.
 * You must right-justify transmit data before writing into the transmit FIFO.
 * The transmit logic ignores the upper unused bits when transmitting the data.
 *
 * @param spix SPIx instance
 * @param dfs This parameter can be one of the following values:
 *        @arg @ref LL_SPI_DFS_4_BITS
 *        @arg @ref LL_SPI_DFS_5_BITS
 *        @arg @ref LL_SPI_DFS_6_BITS
 *        @arg @ref LL_SPI_DFS_7_BITS
 *        @arg @ref LL_SPI_DFS_8_BITS
 *        @arg @ref LL_SPI_DFS_9_BITS
 *        @arg @ref LL_SPI_DFS_10_BITS
 *        @arg @ref LL_SPI_DFS_11_BITS
 *        @arg @ref LL_SPI_DFS_12_BITS
 *        @arg @ref LL_SPI_DFS_13_BITS
 *        @arg @ref LL_SPI_DFS_14_BITS
 *        @arg @ref LL_SPI_DFS_15_BITS
 *        @arg @ref LL_SPI_DFS_16_BITS
 *        @arg @ref LL_SPI_DFS_17_BITS
 *        @arg @ref LL_SPI_DFS_18_BITS
 *        @arg @ref LL_SPI_DFS_19_BITS
 *        @arg @ref LL_SPI_DFS_20_BITS
 *        @arg @ref LL_SPI_DFS_21_BITS
 *        @arg @ref LL_SPI_DFS_22_BITS
 *        @arg @ref LL_SPI_DFS_23_BITS
 *        @arg @ref LL_SPI_DFS_24_BITS
 *        @arg @ref LL_SPI_DFS_25_BITS
 *        @arg @ref LL_SPI_DFS_26_BITS
 *        @arg @ref LL_SPI_DFS_27_BITS
 *        @arg @ref LL_SPI_DFS_28_BITS
 *        @arg @ref LL_SPI_DFS_29_BITS
 *        @arg @ref LL_SPI_DFS_30_BITS
 *        @arg @ref LL_SPI_DFS_31_BITS
 *        @arg @ref LL_SPI_DFS_32_BITS
 * @return None
 */
void spi_set_data_frame_size(spi_base_t spix, uint32_t dfs);

uint32_t spi_get_data_frame_size(spi_base_t spix);

/**
 * @brief Exist only when SPI is configured as a Master device.
 * This controls the end of serial transfers when in receive-only mode.
 * It is impossible to write to this register when SPI is enabled.
 *
 * @param spix SPIx instance
 * @param ndf Number of data frames to be continusly received by the SPI
 * @return None
 */
void spi_set_number_of_data_frames(spi_base_t spix, uint32_t ndf);

uint32_t spi_get_number_of_data_frames(spi_base_t spix);

/**
 * @brief Enable SPI
 * @details It is impossible to program some of the SPI control registers when enabled.
 * @param spix SPIx instance
 * @return None
 */
void spi_enable(spi_base_t spix);

/**
 * @brief Disable SPI
 * @details When disabled, all serial transfers are halted immediately.
 * Transmit and receive FIFO buffers are cleared when the device is disabled.

 * @param spix
 * @return None
 */
void spi_disable(spi_base_t spix);

/**
 * @brief Check whether SPI is enabled.
 *
 * @param spix SPIx instance
 * @return state of bit (1 or 0)
 */
uint32_t spi_is_enable(spi_base_t spix);

/**
 * @brief Enables the individual slave select output lines from the SPI Master.
 * @details Valid only when SPI is configured as a Master device. You cannot write to this register
 * when SPI is busy and SPI is enabled.
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_enable_slave(spi_base_t spix);

/**
 * @brief Disables the individual slave select output lines from the SPI Master.
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_disable_slave(spi_base_t spix);

uint32_t spi_is_slave_enable(spi_base_t spix);

/**
 * @brief This function configures the frequency of the serial clock that regulates tha data transfer.
 * @details Valid only when SPI is configuraed as a Master device.
 * It is impossible to write this register when the SPI is enabled.
 * Fsclk_out = Fspi_clk / (sckdiv * 2)
 *
 * @param spix SPIx instance
 * @param div If the value is 0, the serial output clock is disabled.
 * @return
 */
void spi_set_sckdiv(spi_base_t spix, uint32_t div);

uint32_t spi_get_sckdiv(spi_base_t spix);

/**
 * @brief Transfer start FIFO level.
 * @details Used to control the level of entries in transmit FIFO above which transfer will start.
 * This can be used to ensure that sufficient data is present in transmit FIFO before starting a write
 * operaion on serial line.
 * Valid only when SPI is configured as a Master device.
 *
 * @param spix SPIx instance
 * @param tx_start can be 0~31.
 * @return
 */
void spi_set_tx_fifo_start_level(spi_base_t spix, uint32_t tx_start);

uint32_t spi_get_tx_fifo_start_level(spi_base_t spix);

/**
 * @brief Transmit FIFO Threshold.
 * @details Controls the level of entries (or below) at which the transmit FIFO controller triggers
 * an interrupt. When the number of transmit FIFO entries is less than or equal to this value, the
 * transmit FIFO empty interrupt is triggered.
 *
 * @param spix SPIx instance
 * @param tft can be 0~31.
 * @return None
 */
void spi_set_tx_fifo_threshold(spi_base_t spix, uint32_t tft);

uint32_t spi_get_tx_fifo_threshold(spi_base_t spix);

/**
 * @brief Receive FIFO Threshold.
 * @details Controls the level of entries (or above) at which the receive FIFO controller triggers an interrupt.
 * When the number of receive FIFO entries is greater than or equal to this value, the receive FIFO full interrupt is triggered.
 *
 * @param spix
 * @param rft
 * @return
 */
void spi_set_rx_fifo_threshold(spi_base_t spix, uint32_t rft);

uint32_t spi_get_rx_fifo_threshold(spi_base_t spix);

/**
 * @}
 */

/**
 * @defgroup SPI_LL_EF_DUAL_QUAD_OCTAL_MODE SPI Dual/Quad/Octal Mode APIs
 * @brief SPI Dual/Quad/Octal Mode APIs
 * @{
 */

/**
 * @brief Set receive data sampling edge.
 *
 * @param spix SPIx instance
 * @param edge 0 - positive dge; 1 - negative edge.
 * @return None
 */
void spi_set_rxd_sampling_edge(spi_base_t spix, uint32_t edge);

/**
 * @brief Get receive data sampling edge.
 *
 * @param spix SPIx instance
 * @retval 0 - positive edge
 * @retval 1 - negative edge
 */
uint32_t spi_get_rxd_sampling_edge(spi_base_t spix);

/**
 * @brief Set recevie data sample delay.
 *
 * @param spix SPIx instance
 * @param delay number between [0, 255]
 * @return None
 */
void spi_set_rxd_sampling_delay(spi_base_t spix, uint32_t delay);

/**
 * @brief Get receive data sample delay.
 *
 * @param spix SPIx instance
 * @return delay
 */
uint32_t spi_get_rxd_sampling_delay(spi_base_t spix);

/**
 * @brief Set wait cycles in Dual/Quad/Octal mode between control frames transmit and data recption.
 * @details Specified as number of SPI clock cycles.
 *
 * @param spix SPIx instance
 * @param cycles number between [0, 31]
 * @return None
 */
void spi_set_wait_cycles(spi_base_t spix, uint32_t cycles);

uint32_t spi_get_wait_cycles(spi_base_t spix);

/**
 * @brief Set Dual/Quad/Octal mode instruction length in bits.
 *
 * @param spix SPIx instance
 * @param len This parameter can be one of the following values:
 *        @arg @ref LL_SPI_INSTRUCTION_LEN_0_BITS
 *        @arg @ref LL_SPI_INSTRUCTION_LEN_4_BITS
 *        @arg @ref LL_SPI_INSTRUCTION_LEN_8_BITS
 *        @arg @ref LL_SPI_INSTRUCTION_LEN_16_BITS
 * @return None
 */
void spi_set_instruction_length_bits(spi_base_t spix, uint32_t len);

/**
 * @brief Get Dual/quda/Octal mode instruction length in bits.
 *
 * @param spix SPIx instance
 * @return
 */
uint32_t spi_get_instruction_length_bits(spi_base_t spix);

/**
 * @brief Set address length.
 *
 * @param spix SPIx instance
 * @param len This parameter can be one of the following values:
 *        @arg @ref LL_SPI_ADDR_LEN_0_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_4_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_8_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_12_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_16_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_20_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_24_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_28_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_32_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_36_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_40_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_44_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_48_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_52_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_56_BITS
 *        @arg @ref LL_SPI_ADDR_LEN_60_BITS
 * @return None
 */
void spi_set_addr_length(spi_base_t spix, uint32_t len);

uint32_t spi_get_addr_length(spi_base_t spix);

/**
 * @brief Set Address and Instruction transfer type.
 *
 * @param spix SPIx instance
 * @param type This parameter can be one of the following values:
 *        @arg @ref LL_SPI_INST_SPI_ADDR_SPI
 *        @arg @ref LL_SPI_INST_SPI_ADDR_FRF
 *        @arg @ref LL_SPI_INST_FRF_ADDR_FRF
 * @return None
 */
void spi_set_trans_type(spi_base_t spix, uint32_t type);

uint32_t spi_get_trans_type(spi_base_t spix);

/**
 * @}
 */

/**
 * @defgroup SPI_LL_EF_STATE SPI State Functions
 * @brief SPI State Functions
 * @{
 */

/**
 * @brief Get the number of valid data entries in the transmit FIFO.
 *
 * @param spix SPIx instance
 * @return
 */
uint32_t spi_get_tx_fifo_level(spi_base_t spix);

/**
 * @brief Get the number of valid data entries in the receive FIFO.
 *
 * @param spix SPIx instance
 * @return
 */
uint32_t spi_get_rx_fifo_level(spi_base_t spix);

/**
 * @brief Get SPI current transfer status, FIFO status, and any transmission/reception errors that may have occured.
 *
 * @param spix SPIx instance
 * @return Can be a combination of the following values:
 *        @arg @ref LL_SPI_STATUS_BUSY
 *        @arg @ref LL_SPI_STATUS_TX_FIFO_NOT_FULL
 *        @arg @ref LL_SPI_STATUS_TX_FIFO_EMPTY
 *        @arg @ref LL_SPI_STATUS_RX_FIFO_NOT_EMPTY
 *        @arg @ref LL_SPI_STATUS_RX_FIFO_FULL
 *        @arg @ref LL_SPI_STATUS_TX_ERR
 *        @arg @ref LL_SPI_STATUS_DCOL_ERR
 */
uint32_t spi_get_status(spi_base_t spix);

/**
 * @brief Get SPI current status -- data collision. This bit is cleared when read.
 *
 * @param spix SPIx instance
 * @retval 0 inactive
 * @retval 1 active
 */
uint32_t spi_get_status_data_collision(spi_base_t spix);

/**
 * @brief Get SPI current status -- Transmission Error. This bit is cleared when read.
 *
 * @param spix SPIx instance
 * @retval 0 inactive
 * @retval 1 active
 */
uint32_t spi_get_status_tx_error(spi_base_t spix);

/**
 * @brief Get SPI current status -- RX FIFO Full.
 *
 * @param spix SPIx instance
 * @retval 0 inactive
 * @retval 1 active
 */
uint32_t spi_get_status_rx_fifo_full(spi_base_t spix);

/**
 * @brief Get SPI current status -- RX FIFO Not Empty.
 *
 * @param spix SPIx instance
 * @retval 0 inactive
 * @retval 1 active
 */
uint32_t spi_get_status_rx_fifo_not_empty(spi_base_t spix);

/**
 * @brief Get SPI current status -- TX FIFO Empty.
 *
 * @param spix SPIx instance
 * @retval 0 inactive
 * @retval 1 active
 */
uint32_t spi_get_status_tx_fifo_empty(spi_base_t spix);

/**
 * @brief Get SPI current status -- TX FIFO Not Full
 *
 * @param spix SPIx instance
 * @retval 0 inactive
 * @retval 1 active
 */
uint32_t spi_get_status_tx_fifo_not_full(spi_base_t spix);

/**
 * @brief Get SPI current status -- Busy.
 *
 * @param spix SPIx instance
 * @retval 0 inactive
 * @retval 1 active
 */
uint32_t spi_get_status_busy(spi_base_t spix);

/**
 * @}
 */


/**
 * @defgroup SPI_LL_EF_IT SPI Interrupt Functions
 * @brief SPI Interrupt Functions
 * @{
 */

/**
 * @brief Enable SPI Interrupt -- Transmit FIFO Empty
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_enable_it_tx_fifo_empty(spi_base_t spix);

/**
 * @brief Disable SPI Interrupt -- Transmit FIFO Empty
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_disable_it_tx_fifo_empty(spi_base_t spix);

/**
 * @brief Enable SPI Interrupt -- Transmit FIFO Overflow
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_enable_it_tx_fifo_overflow(spi_base_t spix);

/**
 * @brief Disable SPI Interrupt -- Transmit FIFO Overflow
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_disable_it_tx_fifo_overflow(spi_base_t spix);

/**
 * @brief Enable SPI Interrupt -- Receive FIFO Underflow
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_enable_it_rx_fifo_underflow(spi_base_t spix);

/**
 * @brief Disable SPI Interrupt -- Receive FIFO Underflow
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_disable_it_rx_fifo_underflow(spi_base_t spix);

/**
 * @brief Enable SPI Interrupt -- Receive FIFO Overflow
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_enable_it_rx_fifo_overflow(spi_base_t spix);

/**
 * @brief Disable SPI Interrupt -- Receive FIFO Overflow
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_disable_it_rx_fifo_overflow(spi_base_t spix);

/**
 * @brief Enable SPI Interrupt -- Receive FIFO Full
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_enable_it_rx_fifo_full(spi_base_t spix);

/**
 * @brief Disable SPI Interrupt -- Receive FIFO Full
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_disable_it_rx_fifo_full(spi_base_t spix);

/**
 * @brief Enable SPI Interrupt -- Multi-Master Contention
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_enable_it_multi_master_cont(spi_base_t spix);

/**
 * @brief Disable SPI Interrupt -- Multi-Master Contention
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_disable_it_multi_master_cont(spi_base_t spix);

/**
 * @brief Enable SPI Interrupt -- SPI Transmission Error
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_enable_it_spi_tx_err(spi_base_t spix);

/**
 * @brief Disable SPI Interrupt -- SPI Transmission Error
 *
 * @param spix SPIx instance
 * @return None
 */
void spi_disable_it_spi_tx_err(spi_base_t spix);

/**
 * @brief Enable SPI Interrupt with specified type.
 *
 * @param spix SPIx instance
 * @param type can be combination of the following values:
 *        @arg @ref LL_SPI_IT_TX_FIFO_EMPTY
 *        @arg @ref LL_SPI_IT_TX_FIFO_OVERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_UNDERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_OVERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_FULL
 *        @arg @ref LL_SPI_IT_MULTI_MASTER_CONT
 *        @arg @ref LL_SPI_IT_SPI_TX_ERR
 * @return None
 */
void spi_enable_it(spi_base_t spix, uint32_t type);

/**
 * @brief Disable SPI Interrupt with specified type.
 *
 * @param spix SPIx instance
 * @param type can be combination of the following values:
 *        @arg @ref LL_SPI_IT_TX_FIFO_EMPTY
 *        @arg @ref LL_SPI_IT_TX_FIFO_OVERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_UNDERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_OVERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_FULL
 *        @arg @ref LL_SPI_IT_MULTI_MASTER_CONT
 *        @arg @ref LL_SPI_IT_SPI_TX_ERR
 * @return None
 */
void spi_disable_it(spi_base_t spix, uint32_t type);

/**
 * @brief After reset, the SPI interrupts are enabled. User should call this function to disable SPI interrupts.
 *
 * @param spix SPIx instance
 */
void spi_disable_it_all(spi_base_t spix);

/**
 * @brief Check if some kinds of SPI Interrupt is enabled or disabled.
 *
 * @param spix SPIx instance
 * @param type This parameter can be one of the following values:
 *        @arg @ref LL_SPI_IT_TX_FIFO_EMPTY
 *        @arg @ref LL_SPI_IT_TX_FIFO_OVERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_UNDERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_OVERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_FULL
 *        @arg @ref LL_SPI_IT_MULTI_MASTER_CONT
 *        @arg @ref LL_SPI_IT_SPI_TX_ERR
 * @retval State of bit (1 or 0)
 */
uint32_t spi_is_enable_it(spi_base_t spix, uint32_t type);

/**
 * @brief Check SPI interrupt Status after masked.
 *
 * @param spix SPIx instance
 * @param type This parameter can be one of the following values:
 *        @arg @ref LL_SPI_IT_TX_FIFO_EMPTY
 *        @arg @ref LL_SPI_IT_TX_FIFO_OVERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_UNDERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_OVERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_FULL
 *        @arg @ref LL_SPI_IT_MULTI_MASTER_CONT
 *        @arg @ref LL_SPI_IT_SPI_TX_ERR
 * @return State of bit (1 or 0)
 */
uint32_t spi_is_active_it(spi_base_t spix, uint32_t type);

/**
 * @brief Get SPI status.
 *
 * @param spix SPIx instance
 * @return uint32_t return value can be combination of the following values:
 *        @arg @ref LL_SPI_IT_TX_FIFO_EMPTY
 *        @arg @ref LL_SPI_IT_TX_FIFO_OVERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_UNDERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_OVERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_FULL
 *        @arg @ref LL_SPI_IT_MULTI_MASTER_CONT
 *        @arg @ref LL_SPI_IT_SPI_TX_ERR
 */
uint32_t spi_get_it_status(spi_base_t spix);

/**
 * @brief Check SPI raw interrupt Status.
 *
 * @param spix SPIx instance
 * @param type This parameter can be one of the following values:
 *        @arg @ref LL_SPI_IT_TX_FIFO_EMPTY
 *        @arg @ref LL_SPI_IT_TX_FIFO_OVERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_UNDERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_OVERFLOW
 *        @arg @ref LL_SPI_IT_RX_FIFO_FULL
 *        @arg @ref LL_SPI_IT_MULTI_MASTER_CONT
 *        @arg @ref LL_SPI_IT_SPI_TX_ERR
 * @return State of bit (1 or 0)
 */
uint32_t spi_is_raw_active_it(spi_base_t spix, uint32_t type);

/**
 * @brief Clear Transmit FIFO Overflow/Underflow Interrupt.
 *
 * @param spix SPIx instance
 * @return indicates this interrupt status
 */
uint32_t spi_clear_it_tx_fifo_err(spi_base_t spix);

/**
 * @brief Clear Receive FIFO Overflow Interrupt.
 *
 * @param spix SPIx instance
 * @return indicates this interrupt status
 */
uint32_t spi_clear_rx_overflow(spi_base_t spix);

/**
 * @brief Clear Receive FIFO Underflow Interrupt.
 *
 * @param spix SPIx instance
 * @return indicates this interrupt status.
 */
uint32_t spi_clear_rx_underflow(spi_base_t spix);

/**
 * @brief Clear Multi-Master Contention Interrrupt.
 *
 * @param spix SPIx instance
 * @return indicates this interrupt status
 */
uint32_t spi_clear_multi_master(spi_base_t spix);

/**
 * @brief Clear SPI interrupts: TXO/RXU/RXO/MST
 *
 * @param spix SPIx instance
 * @retval 1 indicates the SPI TXO/RXU/RXO/MST interrupt is active.
 * @retval 0 indicates the SPI TXO/RXU/RXO/MST interrupt is inactive.
 */
uint32_t spi_clear_all(spi_base_t spix);

/**
 * @}
 */

/**
 * @defgroup SPI_LL_EF_DMA_Configuration SPI DMA Configuration
 * @brief SPI DMA Configuration
 * @{
 */

/**
 * @brief Enable/Disable Transmit FIFO DMA channel.
 *
 * @param spix SPIx instance
 * @param en 1 - enable; 0 - disable.
 * @return None
 */
void spi_tx_dma_enable(spi_base_t spix, uint32_t en);

/**
 * @brief Check if transmit FIFO DMA channel is enabled or disabled.
 *
 * @param spix SPIx instance
 * @retval 1 enabled
 * @retval 0 disabled
 */
uint32_t spi_is_tx_dma_enable(spi_base_t spix);

/**
 * @brief Enable/Disabe the Receive FIFO DMA Channel.
 *
 * @param spix SPIx instance
 * @param en 1 - enable; 0 - disable.
 * @return None
 */
void spi_rx_dma_enable(spi_base_t spix, uint32_t en);

/**
 * @brief Check if receive FIFO DMA channel is enabled or disabled.
 *
 * @param spix SPIx instance
 * @retval 1 enabled
 * @retval 0 disabled
 */
uint32_t spi_is_rx_dma_enable(spi_base_t spix);

/**
 * @brief Set DMA Transmit Data Level.
 * @details The dma_tx_req signal is generated when the number of valid data entries in the transmit FIFO is equal to
 * or below this field value.
 *
 * @param spix SPIx instance
 * @param dmatdl numbers between [0, 31].
 * @return None
 */
void spi_set_tx_dma_data_lvl(spi_base_t spix, uint32_t dmatdl);

/**
 * @brief Get DMA Transmit Data Level.
 *
 * @param spix SPIx instance
 * @return DMA transmit data level
 */
uint32_t spi_get_tx_dma_data_lvl(spi_base_t spix);

/**
 * @brief Set DMA Receive Data Level.
 * @details The dma_rx_req signal is generated when the number of valid data entries in the receive FIFO is equal to
 * or above this value.
 *
 * @param spix
 * @param dmardl number between [0, 31]
 * @return None
 */
void spi_set_rx_dma_data_lvl(spi_base_t spix, uint32_t dmardl);

uint32_t spi_get_rx_dma_data_lvl(spi_base_t spix);

/**
 * @}
 */

/**
 * @defgroup SPI_LL_EF_MICROWIRE_Configuration Microwire Configuration
 * @brief Microwire Configuration
 * @{
 */

/**
 * @brief Enable Microwire Handshaking.
 * @note Valid only when SPI is configured as a serial-master device.
 * @param spix
 * @return None
 */
void spi_enable_microwire_handshaking(spi_base_t spix);

void spi_disable_microwire_handshaking(spi_base_t spix);

uint32_t spi_is_active_microwire_handshaking(spi_base_t spix);

/**
 * @brief Defines the direction of the data word when the Microwire serial protocol is used.
 *
 * @param spix SPIx instance
 * @param mdd This parameter can be one of the following values:
 *        @arg @ref LL_SPI_MICRO_DIR_RECEIVE
 *        @arg @ref LL_SPI_MICRO_DIR_TRANSMIT
 * @return None
 */
void spi_set_microwire_data_direction(spi_base_t spix, uint32_t mdd);

uint32_t spi_get_microwire_data_direction(spi_base_t spix);

/**
 * @brief Defines whether the Microwire transfer is sequential or non-sequential.
 * @details When sequential mode is used, only one control word is needed to transmit or receive a
 * block of data words. When non-sequential mode is used, there must be a control word for each data
 * word that is transmit or received.
 *
 * @param spix SPIx instance
 * @param mwmod This parameter can be one of the following values:
 *        @arg @ref LL_SPI_MICRO_TMOD_NON_SEQ
 *        @arg @ref LL_SPI_MICRO_TMODE_SEQ
 * @return None
 */
void spi_set_microwire_transfer_mode(spi_base_t spix, uint32_t mwmod);

uint32_t spi_get_microwire_transfer_mode(spi_base_t spix);

/**
 * @}
 */

/**
 * @defgroup SPI_LL_EF_FIFO_OPERATION SPI FIFO Operations
 * @brief SPI FIFO Operations
 * @{
 */

/**
 * @brief Write data to FIFO.
 * @details A write can occur only when SPI is enabled.
 *
 * @param spix SPIx instance
 * @param data
 * @return None
 */
void spi_write_data(spi_base_t spix, uint32_t data);

/**
 * @brief Read data from FIFO
 *
 * @param spix SPIx instance
 * @return 32-bit value, right-justified.
 */
uint32_t spi_read_data(spi_base_t spix);

/**
 * @brief Get SPI Data Register Address.
 *
 * @param spix SPIx instance
 * @return uint32_t DR address
 */
uint32_t spi_get_dr_addr(spi_base_t spix);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__SPI_H__ */
