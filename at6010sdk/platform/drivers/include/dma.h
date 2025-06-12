/**
 *******************************************************************************
 * @file    dma.h
 * @author  Airtouch Software Team
 * @brief   DMA Low-level driver header
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

#ifndef __DMA_H__
#define __DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "at6010.h"

/**
 * @addtogroup AT6010_LL_DRIVER AT6010 LL Driver
 * @{
 */

/**
 * @addtogroup AT6010_LL_PER_BASE AT6010 Peripheral Base
 * @{
 */

typedef enum {
	DMA1 = (uint32_t)REG_DMA_BASE,  /**< DMA1 instance */
} dma_base_t;

/**
 * @}
 */


/**
 * @defgroup DMA_LL DMA
 * @brief DMA Low-Level Driver
 * @{
 */


/* Exported constants --------------------------------------------------------*/
/**
 * @defgroup DMA_LL_Exported_Constants DMA Exported Constants
 * @{
 */

/**
 * @defgroup DMA_LL_EC_Channel DMA Channel
 * @brief DMA Channel
 * @{
 */
#define LL_DMA_CHANNEL_0                  0x00U
#define LL_DMA_CHANNEL_1                  0x01U
#define LL_DMA_CHANNEL_2                  0x02U
#define LL_DMA_CHANNEL_3                  0x03U
#define LL_DMA_CHANNEL_TOTAL              0x04U
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_Transfer_Width DMA Transfer Width
 * @brief DMA transfer width
 * @{
 */
#define LL_DMA_TR_WIDTH_8_BITS            0x00U
#define LL_DMA_TR_WIDTH_16_BITS           0x01U
#define LL_DMA_TR_WIDTH_32_BITS           0x02U
#define LL_DMA_TR_WIDTH_64_BITS           0x03U
#define LL_DMA_TR_WIDTH_128_BITS          0x04U
#define LL_DMA_TR_WIDTH_256_BITS          0x05U
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_ADDRESS_INCREMENT_TYPE DMA Address Increment Type
 * @brief DMA Address Increment Type
 * @{
 */
#define LL_DMA_ADDR_INCREMENT             0x00U
#define LL_DMA_ADDR_DECREMENT             0x01U
#define LL_DMA_ADDR_NO_CHANGE             0x02U
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_BURST_TRANSACTION_LENGTH DMA Burt Transaction Length
 * @brief DMA Burst Transaction Length
 * @{
 */
#define LL_DMA_BURST_1_ITEMS              0x00U /**< Number of data items to be transfered is 1 */
#define LL_DMA_BURST_4_ITEMS              0x01U /**< Number of data items to be transfered is 4 */
#define LL_DMA_BURST_8_ITEMS              0x02U /**< Number of data items to be transfered is 8 */
#define LL_DMA_BURST_16_ITEMS             0x03U /**< Number of data items to be transfered is 16 */
#define LL_DMA_BURST_32_ITEMS             0x04U /**< Number of data items to be transfered is 32 */
#define LL_DMA_BURST_64_ITEMS             0x05U /**< Number of data items to be transfered is 64 */
#define LL_DMA_BURST_128_ITEMS            0x06U /**< Number of data items to be transfered is 128 */
#define LL_DMA_BURST_256_ITEMS            0x07U /**< Number of data items to be transfered is 256 */
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_SRC_GATHER_MANAGEMENT DMA Source Gather Management
 * @brief DMA Source Gather Management
 * @{
 */
#define LL_DMA_GATHER_DISABLE             0x00U /**< Source gather is disabled */
#define LL_DMA_GATHER_ENABLE              0x01U /**< Source gather is enabled  */
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_DST_SCATTER_MANAGEMENT DMA Destination Scatter Management
 * @brief DMA Destination Scatter Management
 * @{
 */
#define LL_DMA_SCATTER_DISABLE            0x00U /**< Destination scatter is disabled */
#define LL_DMA_SCATTER_ENABLE             0x01U /**< Destination scatter is enabled  */
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_TT_FC DMA Transfer Type and Flow Controller
 * @brief DMA Trnasfer Type and Flow Controller
 * @{
 */
#define LL_DMA_TT_M2M_FC_DMA              0x00U /**< Transfer type: Memory to Memory;         Flow Controller: DMA */
#define LL_DMA_TT_M2P_FC_DMA              0x01U /**< Transfer type: Memory to Peripheral;     Flow Controller: DMA */
#define LL_DMA_TT_P2M_FC_DMA              0x02U /**< Transfer type: Peripheral to Memory;     Flow Controller: DMA */
#define LL_DMA_TT_P2P_FC_DMA              0x03U /**< Transfer type: Peripheral to Peripheral; Flow Controller: DMA */
#define LL_DMA_TT_P2M_FC_SRCPER           0x04U /**< Transfer type: Peripheral to Memory;     Flow Controller: Source Peripheral */
#define LL_DMA_TT_P2P_FC_SRCPER           0x05U /**< Transfer type: Peripheral to Peripheral; Flow Controller: Source Peripheral */
#define LL_DMA_TT_M2P_FC_DSTPER           0x06U /**< Transfer type: Memory to Peripheral;     Flow Controller: Destination Peripheral */
#define LL_DMA_TT_P2P_FC_DSTPER           0x07U /**< Transfer type: Peripheral to Peripheral; Flow Controller: Destination Peripheral */
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_LINK_LIST_MANAGEMENT DMA Linked-list Management
 * @brief DMA Linked-list Management
 * @{
 */
#define LL_DMA_LINKLIST_DISABLE           0x00U /**< DMA Linked-list is disabled */
#define LL_DMA_LINKLIST_ENABLE            0x01U /**< DMA Linked-list is enabled  */
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_CHANNEL_PRIORITY DMA Channel Priority
 * @brief DMA Channel Priority. A priority of 7 is the highest priority, and 0 is the lowest.
 * @{
 */
#define LL_DMA_CHANNEL_PRIORITY_0         0U /**< lowest priority             */
#define LL_DMA_CHANNEL_PRIORITY_1         1U
#define LL_DMA_CHANNEL_PRIORITY_2         2U
#define LL_DMA_CHANNEL_PRIORITY_3         3U
#define LL_DMA_CHANNEL_PRIORITY_4         4U
#define LL_DMA_CHANNEL_PRIORITY_5         5U
#define LL_DMA_CHANNEL_PRIORITY_6         6U
#define LL_DMA_CHANNEL_PRIORITY_7         7U /**< highest priority            */
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_Handshake_Type DMA Handshake Type
 * @brief DMA Handshake Type
 * @{
 */
#define LL_DMA_HANDSHAKE_TYPE_HARDWARE    0U
#define LL_DMA_HANDSHAKE_TYPE_SOFTWARE    1U
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_Handshake_Interface DMA Handshake Interface
 * @brief DMA Handshake Interface
 * @{
 */
#define LL_DMA_HANDSHAKE_IF_UART0_TX      0x00U /**< DMA hardware handshake interface: UART0 TX */
#define LL_DMA_HANDSHAKE_IF_UART0_RX      0x01U /**< DMA hardware handshake interface: UART0 RX */
#define LL_DMA_HANDSHAKE_IF_UART1_TX      0x02U /**< DMA hardware handshake interface: UART1 TX */
#define LL_DMA_HANDSHAKE_IF_UART1_RX      0x03U /**< DMA hardware handshake interface: UART1 RX */
#define LL_DMA_HANDSHAKE_IF_UART2_TX      0x04U /**< DMA hardware handshake interface: UART2 TX */
#define LL_DMA_HANDSHAKE_IF_UART2_RX      0x05U /**< DMA hardware handshake interface: UART2 RX */
#define LL_DMA_HANDSHAKE_IF_SPI0_TX       0x06U /**< DMA hardware handshake interface: SPI0  TX */
#define LL_DMA_HANDSHAKE_IF_SPI0_RX       0x07U /**< DMA hardware handshake interface: SPI0  RX */
#define LL_DMA_HANDSHAKE_IF_SPI1_TX       0x08U /**< DMA hardware handshake interface: SPI1  TX */
#define LL_DMA_HANDSHAKE_IF_SPI1_RX       0x09U /**< DMA hardware handshake interface: SPI1  RX */
#define LL_DMA_HANDSHAKE_IF_RADAR         0x0AU /**< DMA hardware handshake interface: RADAR    */
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_Handshake_Polarity DMA Handshake Polarity
 * @brief DMA Handshake Polarity
 * @{
 */
#define LL_DMA_HANDSHAKE_POLARITY_HIGH    0U /**< Handshaking interface Polarity is active high */
#define LL_DMA_HANDSHAKE_POLARITY_LOW     1U /**< Handshaking interface Polarity is active low  */
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_RELOAD_MANAGEMENT DMA Reload Management
 * @brief DMA Reload Management
 * @{
 */
#define LL_DMA_RELOAD_DISABLE             0x00U /**< Reload is disabled */
#define LL_DMA_RELOAD_ENABLE              0x01U /**< Reload is enbled   */
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_FIFO_MODE_MANAGEMENT DMA FIFO Mode Management
 * @brief DMA FIFO Mode Management
 * @{
 */
#define LL_DMA_FIFO_MODE_REDUCE_LATENCY   0x00U /**< Reduce transfer latency  */
#define LL_DMA_FIFO_MODE_IMPROVE_BUS      0x01U /**< Improve bus utilization  */
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_FC_MODE_MANAGEMENT DMA Flow Control Mode Management
 * @brief DMA Flow Control Mode Management
 * @{
 */
#define LL_DMA_FC_MODE_PREFETCH_ENABLE    0x00U /**< Data pre-fetching is enabled  */
#define LL_DMA_FC_MODE_PREFETCH_DISABLE   0x01U /**< Data pre-fetching is disabled */
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EC_INT_TYPE DMA Interrupt Type
 * @brief DMA Interrupt Type
 * @{
 */
#define LL_DMA_IT_STATUS_TFR              0x01
#define LL_DMA_IT_STATUS_BLOCK            0x02
#define LL_DMA_IT_STATUS_SRC_TRAN         0x04
#define LL_DMA_IT_STATUS_DST_TRAN         0x08
#define LL_DMA_IT_STATUS_ERR              0x10
/**
 * @}
 */

// DMA_LL_Exported_Constants
/**
 * @}
 */

/* Exported macros -----------------------------------------------------------*/

/**
 * @defgroup DMA_LL_Exported_Macros DMA Exported Macros
 * @{
 */

/**
 * @brief Convert LL_DMA_TR_WIDTH_xxx_BITS to byte number
 * @param tr @ref DMA_LL_EC_Transfer_Width
 * @retval byte number
 */
#define LL_DMA_CONV_TRWIDTH_TO_BYTES(tr)      \
	(((tr) == LL_DMA_TR_WIDTH_8_BITS) ? 1   : \
	((tr) == LL_DMA_TR_WIDTH_16_BITS) ? 2   : \
	((tr) == LL_DMA_TR_WIDTH_32_BITS) ? 4   : \
	((tr) == LL_DMA_TR_WIDTH_64_BITS) ? 8   : \
	((tr) == LL_DMA_TR_WIDTH_128_BITS) ? 16 : \
	32)

/**
 * @}
 */

/* Exported functions --------------------------------------------------------*/
/**
 * @defgroup DMA_LL_Exported_Functions DMA Exported Functions
 * @{
 */

/**
 * @defgroup DMA_LL_EF_Addr_Management DMA Address Management Functions
 * @brief DMA Address Management Functions
 * @{
 */

/**
 * @brief Set Initial Source Address for Channel x
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param addr Initial Source Address.
 * @return None
 */
void     dma_set_src_addr(dma_base_t dmax, uint32_t chanx, uint32_t addr);

/**
 * @brief Get Current Source Address for Channel x
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return Current Source Address, updated after each source transfer.
 */
uint32_t dma_get_src_addr(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set Initial Destination Address for Channel x
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param addr Initial Destination Address
 * @return None
 */
void     dma_set_dst_addr(dma_base_t dmax, uint32_t chanx, uint32_t addr);

/**
 * @brief Get Current Destination Address for Channel x
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return Current Destination Address, updated after each destination transfer.
 */
uint32_t dma_get_dst_addr(dma_base_t dmax, uint32_t chanx);
// DMA_LL_EF_Addr_Management
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EF_LinkedList_Management DMA Linked List Management
 * @brief DMA Linked List Management
 * @{
 */

/**
 * @brief Set Linked List Pointer for channel x. Note that the last 2bits are forced to 2'b00.
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param addr Linked list pointer in memory
 */
void     dma_set_linked_list_loc(dma_base_t dmax, uint32_t chanx, uint32_t addr);

/**
 * @brief Get Linked List Pointer of Channel x. Note that the last 2bits are always 2'b00.
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @retval Linked list pointer in memory
 */
uint32_t dma_get_linked_list_loc(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Enable/Disable block chaining by linked list on source side.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param en 0 -- disable; 1 -- enable.
 */
void     dma_set_src_linked_list_en(dma_base_t dmax, uint32_t chanx, uint32_t en);

/**
 * @brief Get block chaining by linked list state on source side.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @retval state of bit (1 or 0)
 */
uint32_t dma_get_src_linked_list_en(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Enable/Disable block chaining by linked list on destination side.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param en 0 -- disable; 1 -- enable.
 */
void     dma_set_dst_linked_list_en(dma_base_t dmax, uint32_t chanx, uint32_t en);

/**
 * @brief Get block chaining by linked list state on destination side.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @retval state of bit (1 or 0)
 */
uint32_t dma_get_dst_linked_list_en(dma_base_t dmax, uint32_t chanx);

// DMA_LL_EF_LinkedList_Management
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EF_Control DMA Control
 * @brief DMA Control
 * @{
 */

/**
 * @brief Clear block done flag before trasfer start.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return None
 */
void     dma_clear_block_done_flag(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Check if block transfer done.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @retval State of bit (1 or 0)
 */
uint32_t dma_is_block_done(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set the block transfer size if the DMA is the flow controller.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param block_ts
 */
void     dma_set_block_transfer_size(dma_base_t dmax, uint32_t chanx, uint32_t block_ts);


/**
 * @brief Get the block transfer size.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return uint32_t
 */
uint32_t dma_get_block_transfer_size(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set Transfer type and flow control.
 * @details Flow control can be assigned to DMA controller, the source peripheral or the destination peripheral.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param tt_fc This parameter can be one of the following values:
 *        @arg @ref LL_DMA_TT_M2M_FC_DMA
 *        @arg @ref LL_DMA_TT_M2P_FC_DMA
 *        @arg @ref LL_DMA_TT_P2M_FC_DMA
 *        @arg @ref LL_DMA_TT_P2P_FC_DMA
 *        @arg @ref LL_DMA_TT_P2M_FC_SRCPER
 *        @arg @ref LL_DMA_TT_P2P_FC_SRCPER
 *        @arg @ref LL_DMA_TT_M2P_FC_DSTPER
 *        @arg @ref LL_DMA_TT_P2P_FC_DSTPER
 */
void     dma_set_flowctrl(dma_base_t dmax, uint32_t chanx, uint32_t tt_fc);

/**
 * @brief Get Transfer type and Flow control.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_TT_FC
 */
uint32_t dma_get_flowctrl(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set Source burst transaction length.
 * @details Number of data items, each of width "source transaction width", to be read from the
 * source every time a burst transferred request is made from either the corresponding hardware or
 * software handshaking interface.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param src_msize This parameter can be one of the following values:
 *        @arg @ref LL_DMA_BURST_1_ITEMS
 *        @arg @ref LL_DMA_BURST_4_ITEMS
 *        @arg @ref LL_DMA_BURST_8_ITEMS
 *        @arg @ref LL_DMA_BURST_16_ITEMS
 *        @arg @ref LL_DMA_BURST_32_ITEMS
 *        @arg @ref LL_DMA_BURST_64_ITEMS
 *        @arg @ref LL_DMA_BURST_128_ITEMS
 *        @arg @ref LL_DMA_BURST_256_ITEMS
 */
void     dma_set_src_burst_length(dma_base_t dmax, uint32_t chanx, uint32_t src_msize);

/**
 * @brief Get source burst transaction length.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_BURST_TRANSACTION_LENGTH
 */
uint32_t dma_get_src_burst_length(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set Destination burst length.
 * @details Number of data items, each of width "destination transaction width", to be written to the
 * destination every time a burst transferred request is made from either the corresponding hardware or
 * software handshaking interface.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param dest_msize This parameter can be one of the following values:
 *        @arg @ref LL_DMA_BURST_1_ITEMS
 *        @arg @ref LL_DMA_BURST_4_ITEMS
 *        @arg @ref LL_DMA_BURST_8_ITEMS
 *        @arg @ref LL_DMA_BURST_16_ITEMS
 *        @arg @ref LL_DMA_BURST_32_ITEMS
 *        @arg @ref LL_DMA_BURST_64_ITEMS
 *        @arg @ref LL_DMA_BURST_128_ITEMS
 *        @arg @ref LL_DMA_BURST_256_ITEMS
 */
void     dma_set_dst_burst_length(dma_base_t dmax, uint32_t chanx, uint32_t dest_msize);

/**
 * @brief Get Destination burst length.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_BURST_TRANSACTION_LENGTH
 */
uint32_t dma_get_dst_burst_length(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set source address increment type.
 * @details Indicates whether to increment or decrement the source address on every source transfer.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param sinc This parameter can be one of the following values:
 *        @arg @ref LL_DMA_ADDR_INCREMENT
 *        @arg @ref LL_DMA_ADDR_DECREMENT
 *        @arg @ref LL_DMA_ADDR_NO_CHANGE
 */
void     dma_set_src_addr_inc(dma_base_t dmax, uint32_t chanx, uint32_t sinc);

/**
 * @brief Get source address increment type.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_ADDRESS_INCREMENT_TYPE
 */
uint32_t dma_get_src_addr_inc(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set destination address increment type.
 * @details Indicates whether to increment or decrement the destination address on every destination transfer.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param dinc This parameter can be one of the following values:
 *        @arg @ref LL_DMA_ADDR_INCREMENT
 *        @arg @ref LL_DMA_ADDR_DECREMENT
 *        @arg @ref LL_DMA_ADDR_NO_CHANGE
 */
void     dma_set_dst_addr_inc(dma_base_t dmax, uint32_t chanx, uint32_t dinc);

/**
 * @brief Get destination address increment type.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_ADDRESS_INCREMENT_TYPE
 */
uint32_t dma_get_dst_addr_inc(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set source transfer width.
 * @details For a non-memory peripheral, typically the peripheral (source) FIFO width.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param width This parameter can be one of the following values:
 *        @arg @ref LL_DMA_TR_WIDTH_8_BITS
 *        @arg @ref LL_DMA_TR_WIDTH_16_BITS
 *        @arg @ref LL_DMA_TR_WIDTH_32_BITS
 *        @arg @ref LL_DMA_TR_WIDTH_64_BITS
 *        @arg @ref LL_DMA_TR_WIDTH_128_BITS
 *        @arg @ref LL_DMA_TR_WIDTH_256_BITS
 */
void     dma_set_src_tr_width(dma_base_t dmax, uint32_t chanx, uint32_t width);

/**
 * @brief Get source transfer width.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_Transfer_Width
 */
uint32_t dma_get_src_tr_width(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set destination transfer width.
 * @details For a non-memory peripheral, typically the peripheral (destination) FIFO width.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param width This parameter can be one of the following values:
 *        @arg @ref LL_DMA_TR_WIDTH_8_BITS
 *        @arg @ref LL_DMA_TR_WIDTH_16_BITS
 *        @arg @ref LL_DMA_TR_WIDTH_32_BITS
 *        @arg @ref LL_DMA_TR_WIDTH_64_BITS
 *        @arg @ref LL_DMA_TR_WIDTH_128_BITS
 *        @arg @ref LL_DMA_TR_WIDTH_256_BITS
 */
void     dma_set_dst_tr_width(dma_base_t dmax, uint32_t chanx, uint32_t width);

/**
 * @brief Get destination transfer width.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_Transfer_Width
 */
uint32_t dma_get_dst_tr_width(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set interrupt enable bit.
 * @details If set, then all interrupt-generating sources are enabled. Functions as a global mask
 * bit for all interrupts for the channel.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param en 0 -- disable; 1 -- enable.
 */
void     dma_set_int_en(dma_base_t dmax, uint32_t chanx, uint32_t en);

/**
 * @brief Get interrupt enable bit value.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return state of bit (1 or 0)
 */
uint32_t dma_get_int_en(dma_base_t dmax, uint32_t chanx);

// DMA_LL_EF_Control
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EF_Src_Gather DMA Source Gather
 * @brief DMA Source Gather
 * @{
 */

/**
 * @brief Enable/Disable Source Gather.
 * @details Gather on the source side is applicable only when the source address
 * type is incrementing or decrementing.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param en 0 -- disable; 1 -- enable.
 */
void     dma_set_src_gather_en(dma_base_t dmax, uint32_t chanx, uint32_t en);

/**
 * @brief Check whether the source gather is enabled.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @retval state of bit (1 or 0)
 */
uint32_t dma_get_src_gather_en(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set source gather count.
 * @details Source contiguous transfer count between successive gather boundaries.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param count must <= 0xFFFU
 */
void     dma_set_src_gather_cnt(dma_base_t dmax, uint32_t chanx, uint32_t count);

/**
 * @brief Set source gather interval.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return uint32_t
 */
uint32_t dma_get_src_gather_cnt(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set source gather interval.
 * @details Specifies the source address increment/decrement in multiples of "source transfer width" on a gather
 * boundary when gather mode is enabled for the source transfer.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param interval must <= 0xFFFFFU
 */
void     dma_set_src_gather_interval(dma_base_t dmax, uint32_t chanx, uint32_t interval);


/**
 * @brief Get source gather interval
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return uint32_t
 */
uint32_t dma_get_src_gather_interval(dma_base_t dmax, uint32_t chanx);

// DMA_LL_EF_Src_Gather
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EF_Dst_Scatter DMA Destination Scatter
 * @brief DMA Destination Scatter
 * @{
 */

/**
 * @brief Enable/Disable Destination Scatter.
 * @details Scatter on the destination side is applicable only when the destination address type
 * is incrementing or decrementing.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param en 0 -- disable; 1 -- enable.
 */
void     dma_set_dst_scatter_en(dma_base_t dmax, uint32_t chanx, uint32_t en);

/**
 * @brief Check whether the destination scatter is enabled.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @retval state of bit (1 or 0)
 */
uint32_t dma_get_dst_scatter_en(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set destination scatter count.
 * @details Specifies the number of contiguous destination transfers of "destination transfer width" between
 * successive scatter boundaries.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param count must <= 0xFFFU
 */
void     dma_set_dst_scatter_cnt(dma_base_t dmax, uint32_t chanx, uint32_t count);

/**
 * @brief Get destination scatter count.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return uint32_t
 */
uint32_t dma_get_dst_scatter_cnt(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set destination scatter interval.
 * @details Specifies the destination address increment/decrement in multiples of "destination transfer width" on a scatter
 * boundary when scatter mode is enabled for the destination transfer.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param interval must <= 0xFFFFFU
 */
void     dma_set_dst_scatter_interval(dma_base_t dmax, uint32_t chanx, uint32_t interval);

/**
 * @brief Get destination scatter interval.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return uint32_t
 */
uint32_t dma_get_dst_scatter_interval(dma_base_t dmax, uint32_t chanx);

// DMA_LL_EF_Dst_Scatter
/**
 * @}
 */


/**
 * @defgroup DMA_LL_EF_Source_Status_Management DMA Source Status Management
 * @brief DMA Source Status Management
 * @{
 */

/**
 * @brief Enable/Disable source status update.
 * @details Source status information is fetched only from the location pointed by the "source status address register",
 * stored in the "source status register" and written out to the "source status" location of the LLI, if this feature is enabled.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param en 0 -- disable; 1 -- enable.
 */
void     dma_set_src_status_update_en(dma_base_t dmax, uint32_t chanx, uint32_t en);

/**
 * @brief Check whether the source status update is enabled.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return state of bit (1 or 0)
 */
uint32_t dma_get_src_status_update_en(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set source status address.
 * @details After each block transfer completes, hardware can retrieve the source status information
 * from the address set by this function.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param addr
 */
void     dma_set_src_status_addr(dma_base_t dmax, uint32_t chanx, uint32_t addr);

/**
 * @brief Get source status address.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return
 */
uint32_t dma_get_src_status_addr(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Source status information retrieved by hardware from the address set by @ref dma_set_src_status_addr().
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return
 */
uint32_t dma_get_src_status(dma_base_t dmax, uint32_t chanx);

// DMA_LL_EF_Source_Status_Management
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EF_Destination_Status_Management DMA Destination Status Management
 * @brief DMA Destination Status Management
 * @{
 */

/**
 * @brief Enable/Disable destination status update.
 * @details Destination status information is fetched only from the location pointed to by the
 * "destination status address register", stored in the "destination status register" and written
 * out to the "destination status" location of the LLI, if this feature is enabled.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param en 0 -- disable; 1 -- enable.
 */
void     dma_set_dst_status_update_en(dma_base_t dmax, uint32_t chanx, uint32_t en);

/**
 * @brief Check whether destination status update is enabled.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return stats of bit (1 or 0)
 */
uint32_t dma_get_dst_status_update_en(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set destination status address.
 * @details After completion of each block transfer, hardware can retrieve the destination status information
 * from the user-defined address set by this function to which the contents of the destination status register point.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param addr
 */
void     dma_set_dst_status_addr(dma_base_t dmax, uint32_t chanx, uint32_t addr);

/**
 * @brief Get destination status address.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return
 */
uint32_t dma_get_dst_status_addr(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Get destination status.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return
 */
uint32_t dma_get_dst_status(dma_base_t dmax, uint32_t chanx);


// DMA_LL_EF_Destination_Status_Management
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EF_Handshake_Management DMA Handshake Management
 * @brief DMA Handshake Management
 * @{
 */

/**
 * @brief Select handshake type (hardware or software) on source side.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param software This parameter can be one of the following values:
 *        @arg @ref LL_DMA_HANDSHAKE_TYPE_HARDWARE
 *        @arg @ref LL_DMA_HANDSHAKE_TYPE_SOFTWARE
 */
void     dma_set_src_handshake_type(dma_base_t dmax, uint32_t chanx, uint32_t software);

/**
 * @brief Get handshake type (hardware or software) on source side.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_Handshake_Type
 */
uint32_t dma_get_src_handshake_type(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Select handshake type (hardware or software) on destination side.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param software This parameter can be one of the following values:
 *        @arg @ref LL_DMA_HANDSHAKE_TYPE_HARDWARE
 *        @arg @ref LL_DMA_HANDSHAKE_TYPE_SOFTWARE
 */
void     dma_set_dst_handshake_type(dma_base_t dmax, uint32_t chanx, uint32_t software);

/**
 * @brief Get handshake type (hardware or software) on destination side.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_Handshake_Type
 */
uint32_t dma_get_dst_handshake_type(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set Source hardware interface. The cannel can then communicate with the source peripheral
 * connected to that interface through the assigned hardware handshaking interface.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param per This parameter can be one of the following values:
 *        @arg @ref LL_DMA_HANDSHAKE_IF_UART0_TX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_UART0_RX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_UART1_TX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_UART1_RX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_UART2_TX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_UART2_RX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_SPI0_TX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_SPI0_RX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_SPI1_TX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_SPI1_RX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_RADAR
 * @return None
 */
void     dma_set_src_handshake_if(dma_base_t dmax, uint32_t chanx, uint32_t per);

/**
 * @brief Get source handshake interface.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_Handshake_Interface
 */
uint32_t dma_get_src_handshake_if(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set Destination hardware interface. The channel can then communicate with the destination
 * peripheral connected to that interface through the assigned hardware handshaking interface.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param per This parameter can be one of the following values:
 *        @arg @ref LL_DMA_HANDSHAKE_IF_UART0_TX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_UART0_RX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_UART1_TX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_UART1_RX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_UART2_TX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_UART2_RX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_SPI0_TX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_SPI0_RX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_SPI1_TX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_SPI1_RX
 *        @arg @ref LL_DMA_HANDSHAKE_IF_RADAR
 * @return None
 */
void     dma_set_dst_handshake_if(dma_base_t dmax, uint32_t chanx, uint32_t per);

/**
 * @brief Get destination handshake interface.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_Handshake_Interface
 */
uint32_t dma_get_dst_handshake_if(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set handshake interface polarity on source side.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param polarity This parameter can be one of the following values:
 *        @arg @ref LL_DMA_HANDSHAKE_POLARITY_HIGH
 *        @arg @ref LL_DMA_HANDSHAKE_POLARITY_LOW
 */
void     dma_set_src_handshake_polarity(dma_base_t dmax, uint32_t chanx, uint32_t polarity);

/**
 * @brief Get handshake interface polarity on source side.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_Handshake_Polarity
 */
uint32_t dma_get_src_handshake_polarity(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set handshake interface polarity on destination side.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param polarity This parameter can be one of the following values:
 *        @arg @ref LL_DMA_HANDSHAKE_POLARITY_HIGH
 *        @arg @ref LL_DMA_HANDSHAKE_POLARITY_LOW
 */
void     dma_set_dst_handshake_polarity(dma_base_t dmax, uint32_t chanx, uint32_t polarity);

/**
 * @brief Get handshake interface polarity on destination side.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_Handshake_Polarity
 */
uint32_t dma_get_dst_handshake_polarity(dma_base_t dmax, uint32_t chanx);

// DMA_LL_EF_Handshake_Management
/**
 * @}
 */


/**
 * @defgroup DMA_LL_EF_Configuration  DMA Configuration
 * @{
 */

void     dma_set_protection_control(dma_base_t dmax, uint32_t chanx, uint32_t ctrl);
uint32_t dma_get_protection_control(dma_base_t dmax, uint32_t chanx);

/**
 * @brief FIFO Mode Select. Determines how much space or data needs to be available in the FIFO
 * before a burst transaction request is serviced.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @param mode This parameter can be one of the following values:
 *        @arg @ref LL_DMA_FIFO_MODE_REDUCE_LATENCY
 *        @arg @ref LL_DMA_FIFO_MODE_IMPROVE_BUS
 */
void     dma_set_fifo_mode(dma_base_t dmax, uint32_t chanx, uint32_t mode);

/**
 * @brief Get FIFO Mode.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_FIFO_MODE_MANAGEMENT
 */
uint32_t dma_get_fifo_mode(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Flow Control Mode Select. Determines when source transaction requests are serviced when
 * the Destination peripheral is the flow controller.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @param mode This parameter can be one of the following values:
 *        @arg @ref LL_DMA_FC_MODE_PREFETCH_ENABLE
 *        @arg @ref LL_DMA_FC_MODE_PREFETCH_DISABLE
 */
void     dma_set_flow_control_mode(dma_base_t dmax, uint32_t chanx, uint32_t mode);

/**
 * @brief Get Flow Control Mode.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_FC_MODE_MANAGEMENT
 */
uint32_t dma_get_flow_control_mode(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Automatic Destination Reload Enable/Disable. The DARx register can be automatically
 * reloaded from its initial value at the end of every block for multi-block transfers. A new block
 * transfer is then initiated.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @param reload 0 -- disable; 1 -- enable.
 */
void     dma_set_dst_reload_en(dma_base_t dmax, uint32_t chanx, uint32_t reload);

/**
 * @brief Check whether automatic destination reload is enabled or not.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_dst_reload_en(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Automatic Source Reload Enable/Disable. The SARx register can be automatically reloaded
 * from its initial value at the end of every block for multi-block transfers. A new block transfer
 * is then initiated.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @param reload 0 -- disable; 1 -- enable.
 */
void     dma_set_src_realod_en(dma_base_t dmax, uint32_t chanx, uint32_t reload);

/**
 * @brief Check whether automatic source reload is enabled or not.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_src_realod_en(dma_base_t dmax, uint32_t chanx);

void     dma_set_lock_bus(dma_base_t dmax, uint32_t chanx, uint32_t lock);
uint32_t dma_get_lock_bus(dma_base_t dmax, uint32_t chanx);

void     dma_set_lock_channel(dma_base_t dmax, uint32_t chanx, uint32_t lock);
uint32_t dma_get_lock_channel(dma_base_t dmax, uint32_t chanx);

void     dma_set_lock_bus_level(dma_base_t dmax, uint32_t chanx, uint32_t level);
uint32_t dma_get_lock_bus_level(dma_base_t dmax, uint32_t chanx);

void     dma_set_lock_channel_level(dma_base_t dmax, uint32_t chanx, uint32_t level);
uint32_t dma_get_lock_channel_level(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Check Channel FIFO status. Indicates if there is data left in the channel FIFO.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_is_fifo_empty(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Suspends all DMA data transfers from the source until this bit is cleared.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @param susp 0 -- not suspend; 1 -- suspend DMA transfer from the source.
 */
void     dma_set_channel_suspend(dma_base_t dmax, uint32_t chanx, uint32_t susp);

/**
 * @brief Check whether channel is suspended.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_channel_suspend(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Set DMA channel priority. A priority of 7 is the highest priority, and 0 is the lowest.
 *
 * @param dmax DMAx instance
 * @param channel @ref DMA_LL_EC_Channel
 * @param priority This parameter can be one of the following values:
 *        @arg @ref LL_DMA_CHANNEL_PRIORITY_0
 *        @arg @ref LL_DMA_CHANNEL_PRIORITY_1
 *        @arg @ref LL_DMA_CHANNEL_PRIORITY_2
 *        @arg @ref LL_DMA_CHANNEL_PRIORITY_3
 *        @arg @ref LL_DMA_CHANNEL_PRIORITY_4
 *        @arg @ref LL_DMA_CHANNEL_PRIORITY_5
 *        @arg @ref LL_DMA_CHANNEL_PRIORITY_6
 *        @arg @ref LL_DMA_CHANNEL_PRIORITY_7
 * @return None
 */
void     dma_set_channel_priority(dma_base_t dmax, uint32_t chanx, uint32_t priority);

/**
 * @brief Get channel priority.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return @ref DMA_LL_EC_CHANNEL_PRIORITY
 */
uint32_t dma_get_channel_priority(dma_base_t dmax, uint32_t chanx);


/**
 * @brief Enable DMAx controller.
 *
 * @param dmax DMAx Instance
 * @return None
 */
void     dma_enable_global(dma_base_t dmax);

/**
 * @brief Check if DMAx controller is enabled or not.
 *
 * @param dmax DMAx instance
 * @return state of bit (0 or 1)
 */
uint32_t dma_is_enable_global(dma_base_t dmax);

/**
 * @brief Disable DMAx controller.
 *
 * @param dmax DMAx Instance.
 * @return None
 */
void     dma_disable_global(dma_base_t dmax);

/**
 * @brief Enable a DMA channel.
 *
 * @param dmax DMAx Instance.
 * @param channel @ref DMA_LL_EC_Channel
 * @return None
 */
void     dma_enable_channel(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Disable a DMA channel.
 *
 * @param dmax DMAx Instance.
 * @param channel @ref DMA_LL_EC_Channel
 * @return None
 */
void     dma_disable_channel(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Check if DMA channel is active or idle.
 *
 * @param dmax DMAx Instance.
 * @param channel @ref DMA_LL_EC_Channel
 * @return State of bit (1 or 0).
 */
u8      dma_is_active_channel(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Get an available DMA channel
 *
 * @param dmax DMAx Instance.
 * @param chanx address to save dma channel @ref DMA_LL_EC_Channel
 * @return u8 @ref AT_TYPE_ERROR
 */
u8 dma_get_free_channel(dma_base_t dmax, u32 *chanx);

// DMA_LL_EF_Configuration
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EF_Interrupt_Management DMA Interrupt Management
 * @{
 */

/**
 * @brief Raw status for IntTfr interrupt
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_raw_tfr(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Raw status for IntBlock interrupt
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_raw_block(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Raw status for IntSrcTran interrupt
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_raw_src_tran(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Raw status for IntDstTran interrupt
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_raw_dst_tran(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Raw status for IntErr interrupt
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_raw_error(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Status for IntTfr interrupt after masking.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_status_tfr(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Status for IntBlock interrupt after masking.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_status_block(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Status for IntSrcTran interrupt after masking.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_status_src_tran(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Status for IntDstTran interrupt after masking.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_status_dst_tran(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Status for IntErr interrupt after masking.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_get_status_error(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Enable IntTfr interrupt
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_enable_it_tfr(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Disable IntTfr interrupt
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_disable_it_tfr(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Check whether IntTfr interrupt is enabled or not.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_is_enabled_it_tfr(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Enable IntBlock interrupt.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return None
 */
void     dma_enable_it_block(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Disable IntBlock interrupt.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return None
 */
void     dma_disable_it_block(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Check whether IntBlock interrupt is enabled or not.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_is_enabled_it_block(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Enable IntSrcTran interrupt.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return None
 */
void     dma_enable_it_src_tran(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Disable IntSrcTran interrupt.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return None
 */
void     dma_disable_it_src_tran(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Check if IntSrcTran interrupt is enabled.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_is_enabled_it_src_tran(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Enable IntDstTran interrupt.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return None
 */
void     dma_enable_it_dst_tran(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Disable IntDstTran interrupt
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return None
 */
void     dma_disable_it_dst_tran(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Check whether IntDstTran interrupt is enabled or not.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_is_enabled_it_dst_tran(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Enable IntErr interrupt
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return None
 */
void     dma_enable_it_error(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Disable IntErr interrupt
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return None
 */
void     dma_disable_it_error(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Check if IntErr interrupt is enabled.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 * @return state of bit (0 or 1)
 */
uint32_t dma_is_enabled_it_error(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Clear IntTfr interrupt flag.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_clear_it_tfr(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Clear IntBlock interrupt flag.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_clear_it_block(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Clear IntSrcTran interrupt flag.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_clear_it_src_tran(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Clear IntDstTran interrupt flag.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_clear_it_dst_tran(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Clear IntErr interrupt flag.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_clear_it_error(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Get combined interrupt status, each bit indicates one kind of interrupt type.
 *
 * @param dmax DMAx instance
 * @retval BIT4 means @ref LL_DMA_IT_STATUS_ERR
 * @retval BIT3 means @ref LL_DMA_IT_STATUS_DST_TRAN
 * @retval BIT2 means @ref LL_DMA_IT_STATUS_SRC_TRAN
 * @retval BIT1 means @ref LL_DMA_IT_STATUS_BLOCK
 * @retval BIT0 means @ref LL_DMA_IT_STATUS_TFR
 */
uint32_t dma_get_int_status(dma_base_t dmax);

// DMA_LL_EF_Interrupt_Management
/**
 * @}
 */

/**
 * @defgroup DMA_LL_EF_Software_Request_Management DMA Software Request Management
 * @{
 */

/**
 * @brief Source software transaction request. Note that this request is ignored if software handshaking is not enabled
 * for the source of chanx.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_src_sw_request(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Destination software transaction request. Note that this request is ignored if software handshaking is not
 * enabled for the destination of chanx.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_dst_sw_request(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Source single transaction request. Note that this request is ignored when software handshaking is not enabled
 * for the source of chanx.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_src_single_sw_request(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Destination single transaction request. Note that this request is ignored when software handshaking is not
 * enabled for the destination of chanx.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_dst_single_sw_request(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Source last transaction request. Note that this request is ignored when software handshaking is not enabled
 * for the source of chanx, or when the source of chanx is not a flow controller.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_src_last_sw_request(dma_base_t dmax, uint32_t chanx);

/**
 * @brief Destination last transaction request. Note that this request is ignored when software handshaking is not
 * enabled for the destination of chanx, or when the destination of chanx is not a flow controller.
 *
 * @param dmax DMAx instance
 * @param chanx @ref DMA_LL_EC_Channel
 */
void     dma_dst_last_sw_request(dma_base_t dmax, uint32_t chanx);

// DMA_LL_EF_Software_Request_Management
/**
 * @}
 */

// DMA_LL_Exported_Functions
/**
 * @}
 */

// DMA_LL
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

#endif /* !__DMA_H__ */
