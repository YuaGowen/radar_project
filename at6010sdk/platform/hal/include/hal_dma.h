/**
 *******************************************************************************
 * @file    hal_dma.h
 * @author  Airtouch Software Team
 * @brief   DMA HAL Driver
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


#ifndef __HAL_DMA_H__
#define __HAL_DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at6010.h"
#include "hal_def.h"
#include "dma.h"

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @addtogroup HAL_DMA
 * @{
 */


/* Exported Types ------------------------------------------------------------*/
/**
 * @defgroup DMA_Exported_Types DMA Exported Types
 * @brief DMA Exported Types
 * @{
 */

/**
 * @brief DMA Control Struction Definition
 */
typedef struct _dma_ctrl_t {
	u32 int_en         : 1; /**< Interrupt Enable (global mask)                                   */

	u32 dst_tr_width   : 3; /**< Destination transfer width  @ref DMA_Transfer_Width              */
	u32 src_tr_width   : 3; /**< Source      transfer width  @ref DMA_Transfer_Width              */

	u32 dst_addr_inc   : 2; /**< Destination address increment type  @ref DMA_Addr_Increment_Type */
	u32 src_addr_inc   : 2; /**< Source      address increment type  @ref DMA_Addr_Increment_Type */

	u32 dst_burst_size : 3; /**< Destination burst transaction length @ref DMA_Burst_Size         */
	u32 src_burst_size : 3; /**< Source      burst transaction length @ref DMA_Burst_Size         */

	u32 src_gather_en  : 1; /**< Source      gather enable   @ref DMA_Gather_Management           */
	u32 dst_scatter_en : 1; /**< Destination scatter enable  @ref DMA_Scatter_Management          */

	u32 rsvd1          : 1;

	u32 tt_fc          : 3; /**< Transfer type (direction) and flow control @ref DMA_Flow_Control */

	u32 rsvd2          : 4;

	u32 dst_llp_en     : 1; /**< Linked List enable on destination side @ref DMA_Link_List_Management */
	u32 src_llp_en     : 1; /**< Linked List enable on source      side @ref DMA_Link_List_Management */

	u32 rsvd3          : 3;

	u32 block_size     : 12; /**< Block transfer size                                             */
	u32 done           : 1;  /**< Block transfer done status                                      */

	u32 rsvd4          : 19;
} dma_ctrl_t;

/**
 * @brief DMA Configuration Struction Definition
 */
typedef struct _dma_cfg_t {
	uint32_t channel_priority         : 3; /**< Specifies channel priority @ref DMA_Channel_Priority                                       */

	uint32_t src_handshake_type       : 1; /**< Specifies handshake type on source      side: Hardware or Software @ref DMA_Handshake_Type */
	uint32_t dst_handshake_type       : 1; /**< Specifies handshake type on destination side: Hardware or Software @ref DMA_Handshake_Type */
	uint32_t reserved1                : 11;

	uint32_t src_handshake_interface  : 16;/**< Specifies hardware handshake interface on source      side: @ref DMA_Handshake_Interface   */
	uint32_t dst_handshake_interface  : 16;/**< Specifies handware handshake interface on destination side: @ref DMA_Handshake_Interface   */

	uint32_t src_handshake_polarity   : 1; /**< Specifies hardware handshake polarity on source      side: @ref DMA_Handshake_Polarity     */
	uint32_t dst_handshake_polarity   : 1; /**< Specifies hardware handshake polarity on destination side: @ref DMA_Handshake_Polarity     */

	uint32_t src_reload_en            : 1; /**< Specifies address reload on source      side: @ref DMA_Reload_Management                   */
	uint32_t dst_realod_en            : 1; /**< Specifies address reload on destination side: @ref DMA_Reload_Management                   */

	uint32_t src_status_update_enable : 1; /* IGNORE */
	uint32_t dst_status_update_enable : 1; /* IGNORE */

	uint32_t fifo_mode                : 1; /**< Determines how much space or data needs to be available in the FIFO before
                                                a burst transaction request is serviced. @ref DMA_FIFO_Mode_Management */

	uint32_t fcmode                   : 1; /**< Special mode that only applies when the destination peripheral is the flow controller.
                                                It controls the data pre-fetching from the source peripheral  @ref DMA_FC_MODE_Management */

	uint32_t rsvd_lock_bus            : 1; /* IGNORE */
	uint32_t rsvd_lock_chan           : 1; /* IGNORE */
	uint32_t rsvd_lock_bus_level      : 2; /* IGNORE */
	uint32_t rsvd_lock_chan_level     : 2; /* IGNORE */
	uint32_t reserved2                : 2;
} dma_cfg_t;


/**
 * @brief DMA Linked List Item
 */
typedef struct _dma_link_list_t {
	uint32_t                    src_addr;   /**< source address, could be memory buffer address or hardware data register address      */

	uint32_t                    dst_addr;   /**< destination address, could be memory buffer address or hardware data register address */

	struct _dma_link_list_t    *llp_next;   /**< address of next linked list                                                           */

	dma_ctrl_t                  control;    /**< control information for one block transfer                                            */

	uint32_t                    src_status; /**< write back for source      status (user read this field or just ignore)               */

	uint32_t                    dst_status; /**< write back for destination status (user read this field or just ignore)               */

} dma_link_list_t;


/**
 * @brief DMA_Init_Struct DMA Initial Struction Definition
 */
typedef struct {
	uint32_t      channel;              /**< Specifies the DMA channel @ref DMA_Channel_Selection */

	uint32_t      transfer_type;        /**< Specifies the transfer type @ref DMA_Transfer_Type   */

	dma_ctrl_t    control;              /**< Specifies the control information for the channel    */

	dma_cfg_t     config;               /**< Specifies the configuration for the channel          */

	uint32_t      llp_loc;              /**< Specifies the Linked list position if multi-block transfer by linked list */

	uint32_t      src_gather_cnt;       /**< source gather count                                  */
	uint32_t      src_gather_interval;  /**< source gather interval                               */

	uint32_t      dst_scatter_cnt;      /**< destination scatter count                            */
	uint32_t      dst_scatter_interval; /**< destination scatter interval                         */

	uint32_t      src_status_addr;      /**< source status address for write back                 */
	uint32_t      dst_status_addr;      /**< destination status address for write back            */
} dma_init_t;


/**
 * @brief HAL DMA State Enum Definition
 */
typedef enum {
	HAL_DMA_STATE_RESET                   = 0x00U, /**< DMA not yet initialized or disabled       */
	HAL_DMA_STATE_READY                   = 0x01U, /**< DMA initialized and ready for use         */
	HAL_DMA_STATE_BUSY                    = 0x02U, /**< DMA process is ongoing                    */
	HAL_DMA_STATE_TIMEOUT                 = 0x03U, /**< DMA timeout state                         */
	HAL_DMA_STATE_ERROR                   = 0x04U, /**< DMA error state                           */
	HAL_DMA_STATE_ABORT                   = 0x05U, /**< DMA abort state                           */
} hal_dma_state_t;

/**
 * @brief HAL DMA Callback ID Enum Definitions
 */
typedef enum {
	HAL_DMA_XFER_CPLT_CB_ID               = 0x00U, /**< Complete Transfer callback ID             */
	HAL_DMA_XFER_ERROR_CB_ID              = 0x01U, /**< Error callback ID                         */
	HAL_DMA_XFER_BLOCK_CB_ID              = 0x02U, /**< Block Transfer Callback ID                */
	HAL_DMA_XFER_SRCTRAN_CB_ID            = 0x03U, /**< Source Transfer Complete Callback ID      */
	HAL_DMA_XFER_DSTTRAN_CB_ID            = 0x04U, /**< Destination Transfer Complete Callback ID */
	HAL_DMA_ABORT_CB_ID                   = 0x05U, /**< Abort callback ID                         */
	HAL_DMA_ALL_CB_ID                     = 0x06U, /**< All callback ID                           */
} hal_dma_callback_id_t;


/**
 * @brief DMA handle structure definition
 */
typedef struct __dma_handle_t {
	dma_base_t           instance;                                              /**< DMA base address                 */

	dma_init_t           init;                                                  /**< DMA communication parameters     */

	// NOTE: User should not touch the following members directly.
	uint32_t             block_total;                                           /**< How many blocks will be transferred */
	uint32_t             block_cnt;                                             /**< How many blocks have been transferred */

	hal_lock_t           lock;                                                  /**< DMA locking object               */

	__IO hal_dma_state_t state;                                                 /**< DMA transfer state               */

	void                *parent;                                                /**< Parent Object state              */

	void                 (* xfer_cplt_callback)   (struct __dma_handle_t *hdma);/**< DMA transfer complete callback   */
	void                 (* xfer_error_callback)  (struct __dma_handle_t *hdma);/**< DMA transfer Error callback      */
	void                 (* xfer_block_callback)  (struct __dma_handle_t *hdma);/**< DMA transfer Block callback      */
	void                 (* xfer_srctran_callback)(struct __dma_handle_t *hdma);/**< DMA transfer SrcTran callback    */
	void                 (* xfer_dsttran_callback)(struct __dma_handle_t *hdma);/**< DMA transfer DstTran callback    */
	void                 (* xfer_abort_callback)  (struct __dma_handle_t *hdma);/**< DMA transfer abort callback      */

	__IO uint32_t        error_code;                                            /**< DMA error code                   */

} dma_handle_t;

/**
 * @}
 */


/* Exported Constants --------------------------------------------------------*/

/**
 * @defgroup DMA_Exported_Constants DMA Exported Constants
 * @brief DMA Exported Constants
 * @{
 */

/**
 * @defgroup DMA_Error_Code DMA Error Code
 * @brief DMA Error Code
 * @{
 */
#define HAL_DMA_ERROR_NONE                0x00U /**< No error */
#define HAL_DMA_ERROR_TE                  0x01U /**< Transfer error */
#define HAL_DMA_ERROR_FE                  0x02U /**< FIFO error */
#define HAL_DMA_ERROR_TIMEOUT             0x04U /**< Timeout error */
#define HAL_DMA_ERROR_PARAM               0x08U /**< Parameter error */
#define HAL_DMA_ERROR_NO_XFER             0x10U /**< Abort requested with no Xfer ongoing */
#define HAL_DMA_ERROR_NOT_SUPPORTED       0x20U /**< Not supported mode */
/**
 * @}
 */


/**
 * @defgroup DMA_Channel_Selection DMA Channel Selection
 * @brief DMA Channel Selection
 * @{
 */
#define DMA_CHANNEL_0                     0x00U /**< DMA Channel 0 */
#define DMA_CHANNEL_1                     0x01U /**< DMA Channel 1 */
#define DMA_CHANNEL_2                     0x02U /**< DMA Channel 2 */
#define DMA_CHANNEL_3                     0x03U /**< DMA Channel 3 */
/**
 * @}
 */


/**
 * @defgroup DMA_Transfer_Type DMA Transfer Type
 * @brief DMA Transfer Type
 * @{
 */
typedef enum {
	DMA_TRANSFER_TYPE_ROW1                = 0x01U, /**< Single block or last multi-block, no write back                */
	DMA_TRANSFER_TYPE_ROW2                = 0x02U, /**< Multi-block, SAR contiguous,  DAR auto-reload, no write back   */
	DMA_TRANSFER_TYPE_ROW3                = 0x03U, /**< Multi-block, SAR auto-reload, DAR contiguous,  no write back   */
	DMA_TRANSFER_TYPE_ROW4                = 0x04U, /**< Multi-block, SAR auto-reload, DAR auto-reload, no write back   */
	DMA_TRANSFER_TYPE_ROW5                = 0x05U, /**< Single block or last multi-block, with write back              */
	DMA_TRANSFER_TYPE_ROW6                = 0x06U, /**< Multi-block, SAR contiguous,  DAR linked list, with write back */
	DMA_TRANSFER_TYPE_ROW7                = 0x07U, /**< Multi-block, SAR auto-reload, DAR linked list, with write back */
	DMA_TRANSFER_TYPE_ROW8                = 0x08U, /**< Multi-block, SAR linked list, DAR contiguous,  with write back */
	DMA_TRANSFER_TYPE_ROW9                = 0x09U, /**< Multi-block, SAR linked list, DAR auto-reload, with write back */
	DMA_TRANSFER_TYPE_ROW10               = 0x0AU, /**< Multi-block, SAR linked list, DAR linked list, with write back */
} dma_transfer_type_t;
/**
 * @}
 */


/**
 * @defgroup DMA_Transfer_Width DMA Transfer Width
 * @brief DMA Transfer Width
 * @details For a non-memory peripheral, typically the peripheral FIFO width.
 * @{
 */
#define DMA_TR_WIDTH_8_BITS               0x00U /**< Transfer width is 8 bits */
#define DMA_TR_WIDTH_16_BITS              0x01U /**< Transfer width is 16 bits */
#define DMA_TR_WIDTH_32_BITS              0x02U /**< Transfer width is 32 bits */
#define DMA_TR_WIDTH_64_BITS              0x03U /**< Transfer width is 64 bits */
#define DMA_TR_WIDTH_128_BITS             0x04U /**< Transfer width is 128 bits */
#define DMA_TR_WIDTH_256_BITS             0x05U /**< Transfer width is 256 bits */
/**
 * @}
 */


/**
 * @defgroup DMA_Addr_Increment_Type DMA Address Increment Type
 * @brief DMA Address Increment Type
 * @details Indicates whether to increment or decrement the source/destination
 * address on every source/destination transfer.
 * If the device is fetching data from a source peripheral FIFO with a fixed address,
 * then set to "DMA_ADDR_NO_CHANGE".
 * If the device is writing data to a destination peripheral FIFO with a fixed address,
 * then set to "DMA_ADDR_NO_CHANGE".
 * @{
 */
#define DMA_ADDR_INCREMENT                0x00U /**< Increments the address */
#define DMA_ADDR_DECREMENT                0x01U /**< Decrements the address */
#define DMA_ADDR_NO_CHANGE                0x02U /**< No change in the address */
/**
 * @}
 */


/**
 * @defgroup DMA_Burst_Size DMA Burst Size
 * @brief DMA Burst Size
 * @details Number of data items, each of width @ref DMA_Transfer_Width, to be
 *          read from the source or write to the destination every time a burst
 *          transferred request is made from either the corresponding hardware
 *          or software handshaking interface.
 * @{
 */
#define DMA_BURST_SIZE_1_ITEMS            0x00U /**< Number of data items to be transferred is 1 */
#define DMA_BURST_SIZE_4_ITEMS            0x01U /**< Number of data items to be transferred is 4 */
#define DMA_BURST_SIZE_8_ITEMS            0x02U /**< Number of data items to be transferred is 8 */
#define DMA_BURST_SIZE_16_ITEMS           0x03U /**< Number of data items to be transferred is 16 */
#define DMA_BURST_SIZE_32_ITEMS           0x04U /**< Number of data items to be transferred is 32 */
#define DMA_BURST_SIZE_64_ITEMS           0x05U /**< Number of data items to be transferred is 64 */
#define DMA_BURST_SIZE_128_ITEMS          0x06U /**< Number of data items to be transferred is 128 */
#define DMA_BURST_SIZE_256_ITEMS          0x07U /**< Number of data items to be transferred is 256 */
/**
 * @}
 */


/**
 * @defgroup DMA_Gather_Management DMA Gather Management
 * @brief DMA Gather Management
 * @details Gather on the source side is applicable only when the @ref DMA_Addr_Increment_Type
 *          indicates an incrementing or decrementing address control.
 * @{
 */
#define DMA_GATHER_DISABLE                0x00U /**< Source gather is disabled */
#define DMA_GATHER_ENABLE                 0x01U /**< Source gather is enabled  */
/**
 * @}
 */

/**
 * @defgroup DMA_Scatter_Management DMA Scatter Management
 * @brief DMA Scatter Management
 * @details Scatter on the destination side is applicable only when the @ref DMA_Addr_Increment_Type
 *          indicates an incrementing or decrementing address control.
 * @{
 */
#define DMA_SCATTER_DISABLE               0x00U /**< Destination Scatter is disabled */
#define DMA_SCATTER_ENABLE                0x01U /**< Destination Scatter is enabled  */
/**
 * @}
 */


/**
 * @defgroup DMA_Flow_Control DMA Flow Control
 * @brief DMA Flow Control
 * @{
 */
#define DMA_TT_M2M_FC_DMA                 0x00U /**< Transfer type: Memory to Memory;         Flow control: DMA */
#define DMA_TT_M2P_FC_DMA                 0x01U /**< Transfer type: Memory to Peripheral;     Flow contorl: DMA */
#define DMA_TT_P2M_FC_DMA                 0x02U /**< Transfer type: Peripheral to Memory;     Flow contorl: DMA */
#define DMA_TT_P2P_FC_DMA                 0x03U /**< Transfer type: Peripheral to Peripheral; Flow contorl: DMA */
#define DMA_TT_P2M_FC_SRC_PER             0x04U /**< Transfer type: Peripheral to Memory;     Flow contorl: Source Peripheral */
#define DMA_TT_P2P_FC_SRC_PER             0x05U /**< Transfer type: Peripheral to Peripheral; Flow contorl: Source Peripheral */
#define DMA_TT_M2P_FC_DST_PER             0x06U /**< Transfer type: Memory to Peripheral;     Flow contorl: Destination Peripheral */
#define DMA_TT_P2P_FC_DST_PER             0x07U /**< Transfer type: Peripheral to Peripheral; Flow contorl: Destination Peripheral */
/**
 * @}
 */


/**
 * @defgroup DMA_Link_List_Management DMA Link List Management
 * @brief DMA Link List Management
 * @{
 */
#define DMA_LINKLIST_DISABLE              0x00U /**< DMA Linked List Disable  */
#define DMA_LINKLIST_ENABLE               0x01U /**< DMA Linked List Enable   */
/**
 * @}
 */


/**
 * @defgroup DMA_Channel_Priority DMA Channel Priority
 * @brief DMA Channel Priority
 * @{
 */
#define DMA_CHAN_PRIORITY_0               0x00U /**< Lowest priority */
#define DMA_CHAN_PRIORITY_1               0x01U
#define DMA_CHAN_PRIORITY_2               0x02U
#define DMA_CHAN_PRIORITY_3               0x03U
#define DMA_CHAN_PRIORITY_4               0x04U
#define DMA_CHAN_PRIORITY_5               0x05U
#define DMA_CHAN_PRIORITY_6               0x06U
#define DMA_CHAN_PRIORITY_7               0x07U /**< Highest priority */
/**
 * @}
 */



/**
 * @defgroup DMA_Handshake_Type DMA Handshake Type
 * @brief DMA Handshake Type
 * @details Select the handshaking interface is hardware or software.
 * @{
 */
#define DMA_HANDSHAKE_TYPE_HARDWARE       0x00U /**< Hardware handshaking interface. Software initiated transaction requests are ignored. */
#define DMA_HANDSHAKE_TYPE_SOFTWARE       0x01U /**< Software handshaking interface. Hardware initiated transaction requests are ignored. */
/**
 * @}
 */

/**
 * @defgroup DMA_Handshake_Interface DMA Handshake Interface
 * @brief DMA Handshake Interface
 * @{
 */
#define DMA_HANDSHAKE_IF_UART0_TX         0x00U /**< DMA hardware handshake interface: UART0 TX */
#define DMA_HANDSHAKE_IF_UART0_RX         0x01U /**< DMA hardware handshake interface: UART0 RX */
#define DMA_HANDSHAKE_IF_UART1_TX         0x02U /**< DMA hardware handshake interface: UART1 TX */
#define DMA_HANDSHAKE_IF_UART1_RX         0x03U /**< DMA hardware handshake interface: UART1 RX */
#define DMA_HANDSHAKE_IF_UART2_TX         0x04U /**< DMA hardware handshake interface: UART2 TX */
#define DMA_HANDSHAKE_IF_UART2_RX         0x05U /**< DMA hardware handshake interface: UART2 RX */
#define DMA_HANDSHAKE_IF_SPI0_TX          0x06U /**< DMA hardware handshake interface: SPI0  TX */
#define DMA_HANDSHAKE_IF_SPI0_RX          0x07U /**< DMA hardware handshake interface: SPI0  RX */
#define DMA_HANDSHAKE_IF_SPI1_TX          0x08U /**< DMA hardware handshake interface: SPI1  TX */
#define DMA_HANDSHAKE_IF_SPI1_RX          0x09U /**< DMA hardware handshake interface: SPI1  RX */
#define DMA_HANDSHAKE_IF_RADAR            0x0AU /**< DMA hardware handshake interface: RADAR    */
/**
 * @}
 */


/**
 * @defgroup DMA_Handshake_Polarity DMA Handshake Polarity
 * @brief DMA Handshake Polarity
 * @{
 */
#define DMA_HANDSHAKE_POLARITY_HIGH       0x00U /**< Handshaking interface Polarity is active high */
#define DMA_HANDSHAKE_POLARITY_LOW        0x01U /**< Handshaking interface Polarity is active low  */
/**
 * @}
 */


/**
 * @defgroup DMA_Reload_Management DMA Reload Management
 * @brief DMA Reload Management
 * @details Controls the source/destination address register automatically loads
 *          the initial value at the end of every block for multi-block transfers.
 * @{
 */
#define DMA_RELOAD_DISABLE                0x00U /**< Reload disabled */
#define DMA_RELOAD_ENABLE                 0x01U /**< Realod enabled  */
/**
 * @}
 */

/**
 * @defgroup DMA_FIFO_Mode_Management DMA FIFO Mode Management
 * @brief DMA FIFO Mode Management
 * @{
 */
#define DMA_FIFO_MODE_REDUCE_LATENCY      0x00U /**< Reduce transfer latency.    */
#define DMA_FIFO_MODE_IMPROVE_BUS         0x01U /**< Improve bus utilization.    */
/**
 * @}
 */

/**
 * @defgroup DMA_FC_MODE_Management DMA Flow Control Mode Management
 * @brief DMA Flow Control Mode Management
 * @{
 */
#define DMA_FC_MODE_PREFETCH_ENABLE       0x00U /**< Data pre-fetching is enabled  */
#define DMA_FC_MODE_PREFETCH_DISABLE      0x01U /**< Data pre-fetching is disabled */
/**
 * @}
 */

/**
 * @}
 */

/* Exported Macros -----------------------------------------------------------*/

/**
 * @brief Reset DMA handle state
 * @param hdl specifies the DMA handle
 * @retval None
 */
#define __HAL_DMA_RESET_HANDLE_STATE(hdl)   ((hdl)->state = HAL_DMA_STATE_RESET)


/* Exported Functions --------------------------------------------------------*/
/**
 * @defgroup DMA_Exported_Functions DMA Exported Functions
 * @brief DMA Exported Functions
 * @{
 */

/* Exported Functions Group1 -------------------------------------------------*/
/**
 * @defgroup DMA_Exported_Functions_Group1 Initialization and de-initialization functions
 * @brief Initialization and de-initialization functions
 * @{
 */

hal_status_t hal_dma_init(dma_handle_t *hdma);
hal_status_t hal_dma_deinit(dma_handle_t *hdma);

hal_status_t hal_dma_init_struct_init(dma_init_t *init);
hal_status_t hal_dma_lli_struct_init(dma_link_list_t lli[], uint32_t cnt);
/**
 * @}
 */

/* Exported Functions Group2 -------------------------------------------------*/
/**
 * @defgroup DMA_Exported_Functions_Group2 I/O Operation Functions
 * @brief I/O Operation Functions
 * @{
 */
hal_status_t hal_dma_start(dma_handle_t *hdma, uint32_t src_addr, uint32_t dst_addr, uint32_t data_length, uint32_t block_total);
hal_status_t hal_dma_abort(dma_handle_t *hdma);
hal_status_t hal_dma_poll_transfer(dma_handle_t *hdma, uint32_t timeout);

hal_status_t hal_dma_start_it(dma_handle_t *hdma, uint32_t src_addr, uint32_t dst_addr, uint32_t data_length, uint32_t block_total);
hal_status_t hal_dma_aboart_it(dma_handle_t *hdma);
void         hal_dma_irq_handler(dma_handle_t *hdma);

hal_status_t hal_dma_register_callbacks(dma_handle_t *hdma, hal_dma_callback_id_t id, void (*callback)(dma_handle_t *hdma));
hal_status_t hal_dma_unregister_callbacks(dma_handle_t *hdma, hal_dma_callback_id_t id);

/**
 * @}
 */

/* Exported Functions Group3 -------------------------------------------------*/
/**
 * @defgroup DMA_Exported_Functions_Group3 Peripheral State Functions
 * @brief Peripheral State Functions
 * @{
 */
hal_dma_state_t hal_dma_get_state(dma_handle_t *hdma);
uint32_t        hal_dma_get_error(dma_handle_t *hdma);
/**
 * @}
 */


/**
 * @}
 */


/* Private Macros ------------------------------------------------------------*/
/**
 * @defgroup DMA_Private_Macros DMA Private Macros
 * @brief DMA Private Macros
 * @{
 */

#define IS_DMA_INSTANCE(dma)            ((dma) == DMA1)

#define IS_DMA_CHANNEL(chan)            (((chan) == DMA_CHANNEL_0)        || \
                                        ((chan) == DMA_CHANNEL_1)         || \
                                        ((chan) == DMA_CHANNEL_2)         || \
                                        ((chan) == DMA_CHANNEL_3))

#define IS_DMA_TRANSFER_TYPE(v)         (((v) == DMA_TRANSFER_TYPE_ROW1)  || \
                                        ((v) == DMA_TRANSFER_TYPE_ROW2)   || \
                                        ((v) == DMA_TRANSFER_TYPE_ROW3)   || \
                                        ((v) == DMA_TRANSFER_TYPE_ROW4)   || \
                                        ((v) == DMA_TRANSFER_TYPE_ROW5)   || \
                                        ((v) == DMA_TRANSFER_TYPE_ROW6)   || \
                                        ((v) == DMA_TRANSFER_TYPE_ROW7)   || \
                                        ((v) == DMA_TRANSFER_TYPE_ROW8)   || \
                                        ((v) == DMA_TRANSFER_TYPE_ROW9)   || \
                                        ((v) == DMA_TRANSFER_TYPE_ROW10))

#define IS_DMA_TR_WIDTH(v)              (((v) == DMA_TR_WIDTH_8_BITS)     || \
                                        ((v) == DMA_TR_WIDTH_16_BITS)     || \
                                        ((v) == DMA_TR_WIDTH_32_BITS)     || \
                                        ((v) == DMA_TR_WIDTH_64_BITS)     || \
                                        ((v) == DMA_TR_WIDTH_128_BITS)    || \
                                        ((v) == DMA_TR_WIDTH_256_BITS))


#define IS_DMA_ADDR_INC_TYPE(v)         (((v) == DMA_ADDR_INCREMENT)      || \
                                        ((v) == DMA_ADDR_DECREMENT)       || \
                                        ((v) == DMA_ADDR_NO_CHANGE))

#define IS_DMA_BURST_SIZE(v)            (((v) == DMA_BURST_SIZE_1_ITEMS)  || \
                                        ((v) == DMA_BURST_SIZE_4_ITEMS)   || \
                                        ((v) == DMA_BURST_SIZE_8_ITEMS)   || \
                                        ((v) == DMA_BURST_SIZE_16_ITEMS)  || \
                                        ((v) == DMA_BURST_SIZE_32_ITEMS)  || \
                                        ((v) == DMA_BURST_SIZE_64_ITEMS)  || \
                                        ((v) == DMA_BURST_SIZE_128_ITEMS) || \
                                        ((v) == DMA_BURST_SIZE_256_ITEMS))

#define IS_DMA_SRC_GATHER(v)            (((v) == DMA_GATHER_DISABLE)      || ((v) == DMA_GATHER_ENABLE))

#define IS_DMA_DST_SCATTER(v)           (((v) == DMA_SCATTER_DISABLE)     || ((v) == DMA_SCATTER_ENABLE))

#define IS_DMA_TT_FC(v)                 (((v) == DMA_TT_M2M_FC_DMA)       || \
                                        ((v) == DMA_TT_M2P_FC_DMA)        || \
                                        ((v) == DMA_TT_P2M_FC_DMA)        || \
                                        ((v) == DMA_TT_P2P_FC_DMA)        || \
                                        ((v) == DMA_TT_P2M_FC_SRC_PER)    || \
                                        ((v) == DMA_TT_P2P_FC_SRC_PER)    || \
                                        ((v) == DMA_TT_M2P_FC_DST_PER)    || \
                                        ((v) == DMA_TT_P2P_FC_DST_PER))

#define IS_DMA_LINKLIST(v)              (((v) == DMA_LINKLIST_DISABLE)    || ((v) == DMA_LINKLIST_ENABLE))

#define IS_DMA_BLOCK_XFER_SIZE(v)       ((v) <= 0xFFFU)

#define IS_DMA_CHAN_PRIORITY(v)         (((v) == DMA_CHAN_PRIORITY_0)     || \
                                        ((v) == DMA_CHAN_PRIORITY_1)      || \
                                        ((v) == DMA_CHAN_PRIORITY_2)      || \
                                        ((v) == DMA_CHAN_PRIORITY_3)      || \
                                        ((v) == DMA_CHAN_PRIORITY_4)      || \
                                        ((v) == DMA_CHAN_PRIORITY_5)      || \
                                        ((v) == DMA_CHAN_PRIORITY_6)      || \
                                        ((v) == DMA_CHAN_PRIORITY_7))

#define IS_DMA_HANDSHAKE_TYPE(v)        (((v) == DMA_HANDSHAKE_TYPE_HARDWARE)    || ((v) == DMA_HANDSHAKE_TYPE_SOFTWARE))

#define IS_DMA_HANDSHAKE_IF(v)          (((v) == DMA_HANDSHAKE_IF_UART0_TX) || \
                                        ((v) == DMA_HANDSHAKE_IF_UART0_RX)  || \
                                        ((v) == DMA_HANDSHAKE_IF_UART1_TX)  || \
                                        ((v) == DMA_HANDSHAKE_IF_UART1_RX)  || \
                                        ((v) == DMA_HANDSHAKE_IF_UART2_TX)  || \
                                        ((v) == DMA_HANDSHAKE_IF_UART2_RX)  || \
                                        ((v) == DMA_HANDSHAKE_IF_SPI0_TX)   || \
                                        ((v) == DMA_HANDSHAKE_IF_SPI0_RX)   || \
                                        ((v) == DMA_HANDSHAKE_IF_SPI1_TX)   || \
                                        ((v) == DMA_HANDSHAKE_IF_SPI1_RX)   || \
                                        ((v) == DMA_HANDSHAKE_IF_RADAR))

#define IS_DMA_HANDSHAKE_POLARITY(v)    (((v) == DMA_HANDSHAKE_POLARITY_HIGH)  || ((v) == DMA_HANDSHAKE_POLARITY_LOW))

#define IS_DMA_FIFO_MODE(v)             (((v) == DMA_FIFO_MODE_REDUCE_LATENCY) || ((v) == DMA_FIFO_MODE_IMPROVE_BUS))

#define IS_DMA_FCMODE(v)                (((v) == DMA_FC_MODE_PREFETCH_ENABLE)  || ((v) == DMA_FC_MODE_PREFETCH_DISABLE))

#define IS_DMA_LINKLIST_LOC(v)          (((v) & 0x3) == 0x00)

#define IS_DMA_SRC_GATHER_CNT(v)        ((v) <= 0xFFFU)

#define IS_DMA_SRC_GATHER_INTERVAL(v)   ((v) <= 0xFFFFFU)

#define IS_DMA_DST_SCATTER_CNT(v)       ((v) <= 0xFFFU)

#define IS_DMA_DST_SCATTER_INTERVAL(v)  ((v) <= 0xFFFFFU)

/**
 * @}
 */


/* Private Functions ---------------------------------------------------------*/
/**
 * @defgroup DMA_Private_Functions DMA Private Functions
 * @brief DMA Private Functions
 * @{
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

#endif /* !__HAL_DMA_H__ */
