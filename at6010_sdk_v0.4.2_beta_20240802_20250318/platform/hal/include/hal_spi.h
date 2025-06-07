/**
 *******************************************************************************
 * @file    hal_spi.h
 * @author  Airtouch Software Team
 * @brief   SPI HAL Driver
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

#ifndef __HAL_SPI_H__
#define __HAL_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at6010.h"
#include "hal_def.h"
#include "spi.h"
#include "hal_dma.h"

#define USE_HAL_SPI_REGISTER_CALLBACKS  1U

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */


/**
 * @addtogroup HAL_SPI SPI
 * @brief SPI HAL Driver
 * @{
 */


/* Exported Types ------------------------------------------------------------*/
/**
 * @defgroup SPI_Exported_Types SPI Exported Types
 * @{
 */

/**
 * @brief SPI Configuration structure definition
 */
typedef struct {

	uint32_t mode;           /**< Master or Slave, @ref SPI_Mode */

	uint32_t clk_pol;        /**< SPI Clock Polarity, @ref SPI_Clock_Polarity */

	uint32_t clk_pha;        /**< SPI Clock Phase, @ref SPI_Clock_Phase */

	uint32_t dfs;            /**< SPI Data Frame Size, @ref SPI_Data_Frame_Size  */

	uint32_t frame_protocol; /**< SPI Frame Protocol: Motorola SPI, TI SSP or NS Microwire, @ref SPI_Frame_Protocol  */

	uint32_t spi_frf;        /**< SPI IO lines, @ref SPI_Frame_Format */  // 1line, 2line, 4line

	uint32_t tmod;           /**< SPI Transfer Mode, @ref SPI_Transfer_Mode */  // transfer mode: TX_AND_RX, TX_ONLY, RX_ONLY, EEPROM_READ

	uint32_t baudrate;         /**< Must be greater than 0U */

	uint32_t sste;           /**< slave select toggle enable ,only valid when SCPH == 0, @ref SPI_SSTE_Type */

	// DMA configuration (TODO: remove)
	uint32_t tx_dma_en;
	uint32_t rx_dma_en;
	uint32_t tx_dma_lvl;
	uint32_t rx_dma_lvl;

	// Dual/Quad mode instruction length in bits (TODO: 移动到子结构体中)
	uint32_t inst_length;
	uint32_t addr_length;
	uint32_t trans_type;

	// Microwire Frame Format
	uint32_t microwire_cfs;

} spi_init_t;

/**
  * @brief  HAL SPI State structure definition
  */
typedef enum
{
  HAL_SPI_STATE_RESET      = 0x00U,    /**< Peripheral not Initialized                         */
  HAL_SPI_STATE_READY      = 0x01U,    /**< Peripheral Initialized and ready for use           */
  HAL_SPI_STATE_BUSY       = 0x02U,    /**< an internal process is ongoing                     */
  HAL_SPI_STATE_BUSY_TX    = 0x03U,    /**< Data Transmission process is ongoing               */
  HAL_SPI_STATE_BUSY_RX    = 0x04U,    /**< Data Reception process is ongoing                  */
  HAL_SPI_STATE_BUSY_TX_RX = 0x05U,    /**< Data Transmission and Reception process is ongoing */
  HAL_SPI_STATE_ERROR      = 0x06U,    /**< SPI error state                                    */
  HAL_SPI_STATE_ABORT      = 0x07U     /**< SPI abort is ongoing                               */
} hal_spi_state_t;


/**
 * @brief SPI handle structure definition
 */
typedef struct __spi_handle_t {
	spi_base_t           instance;
	spi_init_t           init;

	uint8_t             *tx_buf_ptr;
	uint16_t             tx_xfer_size;
	__IO uint16_t        tx_xfer_count;

	uint8_t             *rx_buf_ptr;
	uint16_t             rx_xfer_size;
	__IO uint16_t        rx_xfer_count;

	void                 (* rx_isr)(struct __spi_handle_t *hspi);
	void                 (* tx_isr)(struct __spi_handle_t *hspi);

	dma_handle_t        *hdmatx;
	dma_handle_t        *hdmarx;

	hal_lock_t           lock;

	__IO hal_spi_state_t state;

	__IO uint32_t        error_code;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
	void (* tx_cplt_callback   )(struct __spi_handle_t *hspi);
	void (* rx_cplt_callback   )(struct __spi_handle_t *hspi);
	void (* txrx_cplt_callback )(struct __spi_handle_t *hspi);
	void (* error_callback     )(struct __spi_handle_t *hspi);
	void (* abort_cplt_callback)(struct __spi_handle_t *hspi);
	void (* msp_init_callback  )(struct __spi_handle_t *hspi);
	void (* msp_deinit_callback)(struct __spi_handle_t *hspi);
#endif
} spi_handle_t;


#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
/**
  * @brief  HAL SPI Callback ID enumeration definition
  */
typedef enum
{
	HAL_SPI_TX_COMPLETE_CB_ID             = 0x00U,    /**< SPI Tx Completed callback ID         */
	HAL_SPI_RX_COMPLETE_CB_ID             = 0x01U,    /**< SPI Rx Completed callback ID         */
	HAL_SPI_TX_RX_COMPLETE_CB_ID          = 0x02U,    /**< SPI TxRx Completed callback ID       */
	HAL_SPI_ERROR_CB_ID                   = 0x03U,    /**< SPI Error callback ID                */
	HAL_SPI_ABORT_CB_ID                   = 0x04U,    /**< SPI Abort callback ID                */
	HAL_SPI_MSPINIT_CB_ID                 = 0x05U,    /**< SPI Msp Init callback ID             */
	HAL_SPI_MSPDEINIT_CB_ID               = 0x06U     /**< SPI Msp DeInit callback ID           */
} hal_spi_callback_id_t;

/**
  * @brief  HAL SPI Callback pointer definition
  */
typedef  void (*spi_callback_func_t)(spi_handle_t *hspi); /**< pointer to an SPI callback function */
#endif

// SPI_Exported_Types
/**
 * @}
 */




/* Exported constants --------------------------------------------------------*/
/**
 * @defgroup SPI_Exported_Constants SPI Exported Constants
 * @{
 */


/** @defgroup SPI_Error_Code SPI Error Code
  * @{
  */
#define HAL_SPI_ERROR_NONE                (0x00000000U)   /**< No error                           */
#define HAL_SPI_ERROR_DCOL                (0x00000001U)   /**< DCOL error                         */
#define HAL_SPI_ERROR_TXERR               (0x00000002U)   /**< Transmission error                 */
#define HAL_SPI_ERROR_MULTIMASTER         (0x00000004U)   /**< Multi-Master Contention error      */
#define HAL_SPI_ERROR_RXOVERFLOW          (0x00000008U)   /**< RX FIFO Overflow error             */
#define HAL_SPI_ERROR_RXUNDERFLOW         (0x00000010U)   /**< RX FIFO Underflow error rocedure   */
#define HAL_SPI_ERROR_TXOVERFLOW          (0x00000020U)   /**< RX FIFO Underflow error rocedure   */
#define HAL_SPI_ERROR_DMA                 (0x00000040U)   /**< DMA transfer error                 */
#define HAL_SPI_ERROR_FLAG                (0x00000080U)   /**< Error on RXNE/TXE/BSY Flag         */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
#define HAL_SPI_ERROR_INVALID_CALLBACK    (0x00000100U)   /**< Invalid Callback error             */
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
/**
  * @}
  */


/** @defgroup SPI_Mode SPI Mode
  * @{
  */
#define SPI_MODE_SLAVE                    0U
#define SPI_MODE_MASTER                   1U
/**
  * @}
  */

/**
 * @defgroup SPI_Frame_Format SPI Frame Format
 * @{
 */
#define SPI_FRF_STANDARD                  0U
#define SPI_FRF_DUAL                      1U
#define SPI_FRF_QUAD                      2U
/**
 * @}
 */

/**
 * @defgroup SPI_SSTE_Type SPI Slave Select Toggle Enable
 * @{
 */
#define SPI_SSTE_DISABLE                  0U
#define SPI_SSTE_ENABLE                   1U
/**
 * @}
 */

/**
 * @defgroup SPI_Transfer_Mode SPI Transfer Mode
 * @{
 */
#define SPI_TMOD_TX_RX                    0U
#define SPI_TMOD_TX_ONLY                  1U
#define SPI_TMOD_RX_ONLY                  2U
#define SPI_TMOD_EEPROM_READ              3U
/**
 * @}
 */

/** @defgroup SPI_Clock_Polarity SPI Clock Polarity In Idle State
  * @{
  */
#define SPI_CLK_POL_LOW                   0U
#define SPI_CLK_POL_HIGH                  1U
/**
  * @}
  */

/** @defgroup SPI_Clock_Phase SPI Clock Phase In Idle State
  * @{
  */
#define SPI_CLK_PHA_1EDGE                 0U
#define SPI_CLK_PHA_2EDGE                 1U
/**
  * @}
  */

/**
 * @defgroup SPI_Frame_Protocol SPI Frame Protocol
 * @{
 */
#define SPI_FRAME_PROTOCOL_MOTOROLA_SPI   0U
#define SPI_FRAME_PROTOCOL_TI_SSP         1U
#define SPI_FRAMEPROTOCOL_NS_MICROWIRE    2U
/**
 * @}
 */

/**
 * @defgroup SPI_Data_Frame_Size SPI Data Frame Size
 * @{
 */
#define SPI_DFS_4_BIT                     3U
#define SPI_DFS_5_BIT                     4U
#define SPI_DFS_6_BIT                     5U
#define SPI_DFS_7_BIT                     6U
#define SPI_DFS_8_BIT                     7U
#define SPI_DFS_9_BIT                     8U
#define SPI_DFS_10_BIT                    9U
#define SPI_DFS_11_BIT                    10U
#define SPI_DFS_12_BIT                    11U
#define SPI_DFS_13_BIT                    12U
#define SPI_DFS_14_BIT                    13U
#define SPI_DFS_15_BIT                    14U
#define SPI_DFS_16_BIT                    15U
#define SPI_DFS_17_BIT                    16U
#define SPI_DFS_18_BIT                    17U
#define SPI_DFS_19_BIT                    18U
#define SPI_DFS_20_BIT                    19U
#define SPI_DFS_21_BIT                    20U
#define SPI_DFS_22_BIT                    21U
#define SPI_DFS_23_BIT                    22U
#define SPI_DFS_24_BIT                    23U
#define SPI_DFS_25_BIT                    24U
#define SPI_DFS_26_BIT                    25U
#define SPI_DFS_27_BIT                    26U
#define SPI_DFS_28_BIT                    27U
#define SPI_DFS_29_BIT                    28U
#define SPI_DFS_30_BIT                    29U
#define SPI_DFS_31_BIT                    30U
#define SPI_DFS_32_BIT                    31U
/**
 * @}
 */

/**
 * @defgroup SPI_STATUS SPI Status
 * @brief SPI Status
 * @{
 */
#define SPI_STATUS_BUSY                   0x01U /**< Busy */
#define SPI_STATUS_TX_FIFO_NOT_FULL       0x02U /**< Transmit FIFO Not Full */
#define SPI_STATUS_TX_FIFO_EMPTY          0x04U /**< Transmit FIFO Empty */
#define SPI_STATUS_RX_FIFO_NOT_EMPTY      0x08U /**< Receive FIFO Not Empty */
#define SPI_STATUS_RX_FIFO_FULL           0x10U /**< Receive FIFO Full */
#define SPI_STATUS_TX_ERR                 0x20U /**< Transmission Error */
#define SPI_STATUS_DCOL_ERR               0x40U /**< Data Collision Error */
/**
 * @}
 */

/**
 * @defgroup SPI_IT_Mask_Status_Type SPI Interrupt Mask and Status Type
 * @brief SPI Interrupt Mask and Status Type
 * @{
 */
#define SPI_IT_TX_FIFO_EMPTY              0x01U /**< TX FIFO Empty */
#define SPI_IT_TX_FIFO_OVERFLOW           0x02U /**< TX FIFO Overflow */
#define SPI_IT_RX_FIFO_UNDERFLOW          0x04U /**< RX FIFO Underflow */
#define SPI_IT_RX_FIFO_OVERFLOW           0x08U /**< RX FIFO Overflow */
#define SPI_IT_RX_FIFO_FULL               0x10U /**< RX FIFO Full */
#define SPI_IT_MULTI_MASTER_CONT          0x20U /**< Multi-Master Contention */
#define SPI_IT_SPI_TX_ERR                 0x400U/**< SPI Transmit Error */
/**
 * @}
 */



/**
 * @defgroup SPI_Microwire_Control_Frame_Size SPI Microwire Frame Format Control Frame Size
 * @{
 */
#define SPI_MICROWIRE_CFS_1_BIT           0U
#define SPI_MICROWIRE_CFS_2_BIT           1U
#define SPI_MICROWIRE_CFS_3_BIT           2U
#define SPI_MICROWIRE_CFS_4_BIT           3U
#define SPI_MICROWIRE_CFS_5_BIT           4U
#define SPI_MICROWIRE_CFS_6_BIT           5U
#define SPI_MICROWIRE_CFS_7_BIT           6U
#define SPI_MICROWIRE_CFS_8_BIT           7U
#define SPI_MICROWIRE_CFS_9_BIT           8U
#define SPI_MICROWIRE_CFS_10_BIT          9U
#define SPI_MICROWIRE_CFS_11_BIT          10U
#define SPI_MICROWIRE_CFS_12_BIT          11U
#define SPI_MICROWIRE_CFS_13_BIT          12U
#define SPI_MICROWIRE_CFS_14_BIT          13U
#define SPI_MICROWIRE_CFS_15_BIT          14U
#define SPI_MICROWIRE_CFS_16_BIT          15U
/**
 * @}
*/

/**
 * @defgroup SPI_Dual_Quad_Instruction_Length SPI Dual/Quad/Octal Mode Instruction Length in bits
 * @{
 */
#define SPI_DUAL_QUAD_INST_LEN_0_BIT      0U
#define SPI_DUAL_QUAD_INST_LEN_4_BIT      1U
#define SPI_DUAL_QUAD_INST_LEN_8_BIT      2U
#define SPI_DUAL_QUAD_INST_LEN_16_BIT     3U
/**
 * @}
 */

/**
 * @defgroup SPI_Dual_Quad_Address_Length SPI Dual/Quad Mode Address Length in bits
 * @{
 */
#define SPI_DUAL_QUAD_ADDR_LEN_0_BIT      0U
#define SPI_DUAL_QUAD_ADDR_LEN_4_BIT      1U
#define SPI_DUAL_QUAD_ADDR_LEN_8_BIT      2U
#define SPI_DUAL_QUAD_ADDR_LEN_12_BIT     3U
#define SPI_DUAL_QUAD_ADDR_LEN_16_BIT     4U
#define SPI_DUAL_QUAD_ADDR_LEN_20_BIT     5U
#define SPI_DUAL_QUAD_ADDR_LEN_24_BIT     6U
#define SPI_DUAL_QUAD_ADDR_LEN_28_BIT     7U
#define SPI_DUAL_QUAD_ADDR_LEN_32_BIT     8U
#define SPI_DUAL_QUAD_ADDR_LEN_36_BIT     9U
#define SPI_DUAL_QUAD_ADDR_LEN_40_BIT     10U
#define SPI_DUAL_QUAD_ADDR_LEN_44_BIT     11U
#define SPI_DUAL_QUAD_ADDR_LEN_48_BIT     12U
#define SPI_DUAL_QUAD_ADDR_LEN_52_BIT     13U
#define SPI_DUAL_QUAD_ADDR_LEN_56_BIT     14U
#define SPI_DUAL_QUAD_ADDR_LEN_60_BIT     15U
/**
 * @}
 */


/**
 * @defgroup SPI_Dual_Quad_Address_Instruction_Transfer_Type SPI Dual/Quad Address and Instruction Transfer Type
 * @{
 */
#define SPI_DUAL_QUAD_TRANS_TYPE_TT0      0U
#define SPI_DUAL_QUAD_TRANS_TYPE_TT1      1U
#define SPI_DUAL_QUAD_TRANS_TYPE_TT2      2U
/**
 * @}
 */


// SPI Exported Constants
/**
 * @}
 */


/* Exported Macros -----------------------------------------------------------*/
/**
 * @defgroup SPI_Exported_Macros SPI Exported Macros
 * @{
 */

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
#define __HAL_SPI_RESET_HANDLE_STATE(hdl)                           \
                            do {                                    \
                                (hdl)->state = HAL_SPI_STATE_RESET; \
                                (hdl)->msp_init_callback = NULL;    \
                                (hdl)->msp_deinit_callback = NULL;  \
                            } while (0)
#else
#define __HAL_SPI_RESET_HANDLE_STATE(hdl)       ((hdl)->state = HAL_SPI_STATE_RESET)
#endif



// SPI_Exported_Macros
/**
 * @}
 */

/* Private Macros ------------------------------------------------------------*/
/**
 * @defgroup SPI_Private_Macros SPI Private Macors
 * @{
 */


#define IS_SPI_ALL_INSTANCE(v)          (((v) == SPI0) || ((v) == SPI1))


/** @brief  Checks if SPI Mode parameter is in allowed range.
  * @param  mode specifies the SPI Mode.
  *         This parameter can be a value of @ref SPI_Mode
  * @retval None
  */
#define IS_SPI_MODE(mode)               (((mode) == SPI_MODE_SLAVE) || ((mode) == SPI_MODE_MASTER))

/**
 * @brief Checks if SPI Frame Format is in allowed range.
 * @param frf
 * @retval None
 */
#define IS_SPI_FRF(frf)                 (((frf) == SPI_FRF_STANDARD) || \
                                        ((frf) == SPI_FRF_DUAL)      || \
                                        ((frf) == SPI_FRF_QUAD))


#define IS_SPI_TMOD(tmod)               (((tmod) == SPI_TMOD_TX_RX) || \
                                        ((tmod) == SPI_TMOD_TX_ONLY) || \
                                        ((tmod) == SPI_TMOD_RX_ONLY) || \
                                        ((tmod) == SPI_TMOD_EEPROM_READ))

/** @brief  Checks if SPI Serial clock steady state parameter is in allowed range.
  * @param  cpol specifies the SPI serial clock steady state.
  *         This parameter can be a value of @ref SPI_Clock_Polarity
  * @retval None
  */
#define IS_SPI_CPOL(cpol)               (((cpol) == SPI_CLK_POL_LOW) || \
                                        ((cpol) == SPI_CLK_POL_HIGH))


/** @brief  Checks if SPI Clock Phase parameter is in allowed range.
  * @param  cpha specifies the SPI Clock Phase.
  *         This parameter can be a value of @ref SPI_Clock_Phase
  * @retval None
  */
#define IS_SPI_CPHA(cpha)               (((cpha) == SPI_CLK_PHA_1EDGE) || \
                                        ((cpha) == SPI_CLK_PHA_2EDGE))

#define IS_SPI_FRAME_PROTOCOL(fp)       (((fp) == SPI_FRAME_PROTOCOL_MOTOROLA_SPI) || \
                                        ((fp) == SPI_FRAME_PROTOCOL_TI_SSP) || \
                                        ((fp) == SPI_FRAMEPROTOCOL_NS_MICROWIRE))


#define IS_SPI_DFS(dfs)                 (((dfs) >= SPI_DFS_4_BIT) && ((dfs) <= SPI_DFS_32_BIT))

#define IS_SPI_DFS_IN_4_8_BIT(dfs)      (((dfs) >= SPI_DFS_4_BIT) && ((dfs) <= SPI_DFS_8_BIT))
#define IS_SPI_DFS_IN_9_16_BIT(dfs)     (((dfs) >= SPI_DFS_9_BIT) && ((dfs) <= SPI_DFS_16_BIT))
#define IS_SPI_DFS_IN_17_32_BIT(dfs)    (((dfs) >= SPI_DFS_17_BIT) && ((dfs) <= SPI_DFS_32_BIT))

#define IS_SPI_NDF(ndf)                 ((ndf) < 0xFFFFU)

#define IS_SPI_BAUDRATE(bd)             ((bd) > 0U)

/**
 * @brief Checks if Transfer start FIFO level is in allowed range.
 * @param v Specifies the transfer start FIFO level
 * @retval None
 */
#define IS_SPI_TX_FIFO_START_LEVEL(v)   ((v) < 0x1FU)

/**
 * @brief Checks if Transmit FIFO threshold is in allowed range.
 * @param v Specifies the transmit FIFO threshold value, which controls the level of entries (or below) at which the
 * transmit FIFO controller triggers an interrupt.
 */
#define IS_SPI_TX_FIFO_THR(v)           ((v) < 0x1FU)

/**
 * @brief Checks if Receive FIFO threshold is in allowed range.
 * @param v Specifies the receive FIFO threshold value, which controls the level of entries (or above) at which the
 * receive FIFO controller triggers an interrupt.
 */
#define IS_SPI_RX_FIFO_THR(v)           ((v) < 0x1FU)

/** @brief  Checks if DMA handle is valid.
  * @param  hdl specifies a DMA Handle.
  * @retval None
  */
#define IS_SPI_DMA_HANDLE(hdl)          ((hdl) != NULL)

// SPI_Private_Macros
/**
 * @}
 */


/* Exported Functions --------------------------------------------------------*/

/**
 * @addtogroup SPI_Exported_Functions
 * @{
 */


/**
 * @addtogroup SPI_Exported_Functions_Group1
 * @{
 */

hal_status_t hal_spi_init(spi_handle_t *hspi);
hal_status_t hal_spi_deinit(spi_handle_t *hspi);

void hal_spi_msp_init(spi_handle_t *hspi);
void hal_spi_msp_deinit(spi_handle_t *hspi);

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
hal_status_t hal_spi_register_callback(spi_handle_t *hspi, hal_spi_callback_id_t id, spi_callback_func_t cb);
hal_status_t hal_spi_unregister_callback(spi_handle_t *hspi, hal_spi_callback_id_t id);
#endif

// SPI_Exported_Functions_Group1
/**
 * @}
 */


/**
 * @addtogroup SPI_Exported_Functions_Group2
 * @{
 */

hal_status_t hal_spi_transmit(spi_handle_t *hspi, uint8_t *pdata, uint16_t size, uint32_t timeout);

hal_status_t hal_spi_receive(spi_handle_t *hspi, uint8_t *pdata, uint16_t size, uint32_t timeout);

hal_status_t hal_spi_transmit_receive(spi_handle_t *hspi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint32_t timeout);

hal_status_t hal_spi_transmit_it(spi_handle_t *hspi, uint8_t *pdata, uint16_t size);

hal_status_t hal_spi_receive_it(spi_handle_t *hspi, uint8_t *pdata, uint16_t size);

hal_status_t hal_spi_transmit_receive_it(spi_handle_t *hspi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size);

hal_status_t hal_spi_transmit_dma(spi_handle_t *hspi, uint8_t *pdata, uint16_t size);

hal_status_t hal_spi_receive_dma(spi_handle_t *hspi, uint8_t *pdata, uint16_t size);

hal_status_t hal_spi_transmit_receive_dma(spi_handle_t *hspi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size);

void hal_spi_irq_handler(spi_handle_t *hspi);

void hal_spi_tx_cplt_callback(spi_handle_t *hspi);

void hal_spi_rx_cplt_callback(spi_handle_t *hspi);

void hal_spi_txrx_cplt_callback(spi_handle_t *hspi);

void hal_spi_error_callback(spi_handle_t *hspi);

void hal_spi_abort_cplt_callback(spi_handle_t *hspi);

// SPI_Exported_Functions_Group2
/**
 * @}
 */


/**
 * @addtogroup SPI_Exported_Functions_Group3
 * @{
 */

hal_spi_state_t hal_spi_get_state(spi_handle_t *hspi);
uint32_t        hal_spi_get_error(spi_handle_t *hspi);

// SPI_Exported_Functions_Group3
/**
 * @}
 */

// SPI_Exported_Functions
/**
 * @}
 */

// SPI
/**
 * @}
 */

// AT6010_HAL_DRIVER
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_SPI_H__ */
