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

/* Includes ------------------------------------------------------------------*/
#include "hal_dma.h"
#include "at_assert.h"

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_DMA DMA
 * @brief DMA HAL Driver
 * @{
 */

/* Private Types -------------------------------------------------------------*/

/* Private Variables ---------------------------------------------------------*/

/* Private Constants ---------------------------------------------------------*/
/**
 * @addtogroup DMA_Private_Constants
 * @{
 */

#define HAL_TIMEOUT_DMA_ABORT             (50U) /* 50ms */

/**
 * @}
 */

/* Private Macros ------------------------------------------------------------*/

/* Private Functions ---------------------------------------------------------*/
/**
 * @addtogroup DMA_Private_Functions
 * @{
 */

static void     dma_setconfig(dma_handle_t *hdma, uint32_t src_addr, uint32_t dst_addr, uint32_t data_length);
static int      dma_set_transfer_type(dma_base_t dmax, uint32_t chanx, uint32_t  transfer);
static uint32_t dma_get_transfer_type(dma_base_t dmax, uint32_t chanx);

/**
 * @}
 */


/* Exported Functions --------------------------------------------------------*/
/**
 * @addtogroup DMA_Exported_Functions
 * @{
 */

/* Exported Functions Group1 -------------------------------------------------*/
/**
 * @addtogroup DMA_Exported_Functions_Group1
 * @{
 */

/**
 * @brief Initialize the DMA according to the specified parameters in the @ref dma_handle_t.
 *
 * @param hdma Pointer to a dma_handle_t structure that contains the configuration information for the specified DMA channel.
 * @return HAL status
 */
hal_status_t hal_dma_init(dma_handle_t *hdma)
{
	if (NULL == hdma) {
		return HAL_ERROR;
	}

	dma_base_t dmax = hdma->instance;
	uint32_t   chanx = hdma->init.channel;

	assert(IS_DMA_INSTANCE(dmax));
	assert(IS_DMA_CHANNEL(chanx));

	assert(IS_DMA_TR_WIDTH(hdma->init.control.dst_tr_width));
	assert(IS_DMA_TR_WIDTH(hdma->init.control.src_tr_width));
	assert(IS_DMA_ADDR_INC_TYPE(hdma->init.control.dst_addr_inc));
	assert(IS_DMA_ADDR_INC_TYPE(hdma->init.control.src_addr_inc));
	assert(IS_DMA_BURST_SIZE(hdma->init.control.dst_burst_size));
	assert(IS_DMA_BURST_SIZE(hdma->init.control.src_burst_size));
	assert(IS_DMA_SRC_GATHER(hdma->init.control.src_gather_en));
	assert(IS_DMA_DST_SCATTER(hdma->init.control.dst_scatter_en));
	assert(IS_DMA_TT_FC(hdma->init.control.tt_fc));
	assert(IS_DMA_LINKLIST(hdma->init.control.dst_llp_en));
	assert(IS_DMA_LINKLIST(hdma->init.control.src_llp_en));
	assert(IS_DMA_CHAN_PRIORITY(hdma->init.config.channel_priority));

	hdma->state = HAL_DMA_STATE_BUSY;

	__HAL_UNLOCK(hdma);

	// Disable this channel first
	dma_disable_channel(dmax, chanx);

	// Wait until this channel is inactive
	while(dma_is_active_channel(dmax, chanx));

	// Disable this channel global interrupt first
	dma_set_int_en(dmax, chanx, 0);

	// Mask this channel from all interrupts
	dma_disable_it_tfr(dmax, chanx);
	dma_disable_it_block(dmax, chanx);
	dma_disable_it_error(dmax, chanx);
	dma_disable_it_src_tran(dmax, chanx);
	dma_disable_it_dst_tran(dmax, chanx);

	// Clear previous interrupt flags of this channel
	dma_clear_it_tfr(dmax, chanx);
	dma_clear_it_block(dmax, chanx);
	dma_clear_it_error(dmax, chanx);
	dma_clear_it_src_tran(dmax, chanx);
	dma_clear_it_dst_tran(dmax, chanx);

	assert(IS_DMA_TRANSFER_TYPE(hdma->init.transfer_type));
	dma_set_transfer_type(dmax, chanx, hdma->init.transfer_type);
	dma_set_linked_list_loc(dmax, chanx, hdma->init.llp_loc);

	dma_set_src_tr_width(dmax, chanx, hdma->init.control.src_tr_width);
	dma_set_dst_tr_width(dmax, chanx, hdma->init.control.dst_tr_width);

	dma_set_src_addr_inc(dmax, chanx, hdma->init.control.src_addr_inc);
	dma_set_dst_addr_inc(dmax, chanx, hdma->init.control.dst_addr_inc);

	dma_set_src_burst_length(dmax, chanx, hdma->init.control.src_burst_size);
	dma_set_dst_burst_length(dmax, chanx, hdma->init.control.dst_burst_size);

	dma_set_src_gather_en(dmax, chanx, hdma->init.control.src_gather_en);
	assert(IS_DMA_SRC_GATHER_CNT(hdma->init.src_gather_cnt));
	assert(IS_DMA_SRC_GATHER_INTERVAL(hdma->init.src_gather_interval));
	dma_set_src_gather_cnt(dmax, chanx, hdma->init.src_gather_cnt);
	dma_set_src_gather_interval(dmax, chanx, hdma->init.src_gather_interval);

	dma_set_dst_scatter_en(dmax, chanx, hdma->init.control.dst_scatter_en);
	assert(IS_DMA_DST_SCATTER_CNT(hdma->init.dst_scatter_cnt));
	assert(IS_DMA_DST_SCATTER_INTERVAL(hdma->init.dst_scatter_interval));
	dma_set_dst_scatter_cnt(dmax, chanx, hdma->init.dst_scatter_cnt);
	dma_set_dst_scatter_interval(dmax, chanx, hdma->init.dst_scatter_interval);

	dma_set_flowctrl(dmax, chanx, hdma->init.control.tt_fc);

	assert(IS_DMA_CHAN_PRIORITY(hdma->init.config.channel_priority));
	dma_set_channel_priority(dmax, chanx, hdma->init.config.channel_priority);

	assert(IS_DMA_HANDSHAKE_TYPE(hdma->init.config.src_handshake_type));
	assert(IS_DMA_HANDSHAKE_TYPE(hdma->init.config.dst_handshake_type));
	dma_set_src_handshake_type(dmax, chanx, hdma->init.config.src_handshake_type);
	dma_set_dst_handshake_type(dmax, chanx, hdma->init.config.dst_handshake_type);

	if (DMA_HANDSHAKE_TYPE_HARDWARE == hdma->init.config.src_handshake_type) {
		// hardware
		dma_set_src_handshake_if(dmax, chanx, hdma->init.config.src_handshake_interface);
		dma_set_src_handshake_polarity(dmax, chanx, hdma->init.config.src_handshake_polarity);
	} else {
		// software
		//TODO
	}

	if (DMA_HANDSHAKE_TYPE_HARDWARE == hdma->init.config.dst_handshake_type) {
		// hardware
		dma_set_dst_handshake_if(dmax, chanx, hdma->init.config.dst_handshake_interface);
		dma_set_dst_handshake_polarity(dmax, chanx, hdma->init.config.dst_handshake_polarity);
	} else {
		// software
		//TODO
	}

	dma_set_src_status_update_en(dmax, chanx, hdma->init.config.src_status_update_enable);
	dma_set_dst_status_update_en(dmax, chanx, hdma->init.config.dst_status_update_enable);
	if (hdma->init.config.src_status_update_enable) {
		dma_set_src_status_addr(dmax, chanx, hdma->init.src_status_addr);
	} else {
		dma_set_src_status_addr(dmax, chanx, 0);
	}

	if (hdma->init.config.dst_status_update_enable) {
		dma_set_dst_status_addr(dmax, chanx, hdma->init.dst_status_addr);
	} else {
		dma_set_dst_status_addr(dmax, chanx, 0);
	}

	assert(IS_DMA_FIFO_MODE(hdma->init.config.fifo_mode));
	dma_set_fifo_mode(dmax, chanx, hdma->init.config.fifo_mode);

	// fc mode
	assert(IS_DMA_FCMODE(hdma->init.config.fcmode));
	dma_set_flow_control_mode(dmax, chanx, hdma->init.config.fcmode);

	// Enable DMA controller
	dma_enable_global(dmax);

	hdma->state = HAL_DMA_STATE_READY;

	return HAL_OK;
}

/**
 * @brief DeInitializes the DMA peripheral.
 *
 * @param hdma Pointer to a dma_handle_t structure that contains the configuration information for the specified DMA channel.
 * @return HAL status
 */
hal_status_t hal_dma_deinit(dma_handle_t *hdma)
{
	if (NULL == hdma) {
		return HAL_ERROR;
	}

	dma_base_t dmax = hdma->instance;
	uint32_t   chanx = hdma->init.channel;

	if (HAL_DMA_STATE_BUSY == hdma->state) {
		return HAL_BUSY;
	}

	assert(IS_DMA_CHANNEL(hdma->init.channel));

	dma_disable_channel(dmax, chanx);

	// Reset source address
	dma_set_src_addr(dmax, chanx, 0);

	// Reset destination address
	dma_set_dst_addr(dmax, chanx, 0);

	// Reset control
	dma_set_block_transfer_size(dmax, chanx, 2);

	dma_set_src_linked_list_en(dmax, chanx, 0);
	dma_set_dst_linked_list_en(dmax, chanx, 0);

	dma_set_flowctrl(dmax, chanx, LL_DMA_TT_P2P_FC_DMA);

	dma_set_dst_scatter_en(dmax, chanx, 0);
	dma_set_src_gather_en(dmax, chanx, 0);

	dma_set_src_burst_length(dmax, chanx, LL_DMA_BURST_4_ITEMS);
	dma_set_dst_burst_length(dmax, chanx, LL_DMA_BURST_4_ITEMS);

	dma_set_src_addr_inc(dmax, chanx, LL_DMA_ADDR_INCREMENT);
	dma_set_dst_addr_inc(dmax, chanx, LL_DMA_ADDR_INCREMENT);

	dma_set_src_tr_width(dmax, chanx, LL_DMA_TR_WIDTH_8_BITS);
	dma_set_dst_tr_width(dmax, chanx, LL_DMA_TR_WIDTH_8_BITS);

	dma_set_int_en(dmax, chanx, 1);

	// Reset configuration
	dma_set_src_handshake_if(dmax, chanx, LL_DMA_HANDSHAKE_IF_UART0_TX);
	dma_set_dst_handshake_if(dmax, chanx, LL_DMA_HANDSHAKE_IF_UART0_TX);

	dma_set_src_status_update_en(dmax, chanx, 0);
	dma_set_dst_status_update_en(dmax, chanx, 0);

	dma_set_protection_control(dmax, chanx, 1);

	dma_set_fifo_mode(dmax, chanx, LL_DMA_FIFO_MODE_REDUCE_LATENCY);

	dma_set_flow_control_mode(dmax, chanx, LL_DMA_FC_MODE_PREFETCH_ENABLE);

	dma_set_dst_reload_en(dmax, chanx, 0);
	dma_set_src_realod_en(dmax, chanx, 0);

	dma_set_src_handshake_polarity(dmax, chanx, LL_DMA_HANDSHAKE_POLARITY_HIGH);
	dma_set_dst_handshake_polarity(dmax, chanx, LL_DMA_HANDSHAKE_POLARITY_HIGH);

	dma_set_lock_bus(dmax, chanx, 0);
	dma_set_lock_channel(dmax, chanx, 0);

	dma_set_lock_bus_level(dmax, chanx, 0);
	dma_set_lock_channel_level(dmax, chanx, 0);

	dma_set_src_handshake_type(dmax, chanx, LL_DMA_HANDSHAKE_TYPE_SOFTWARE);
	dma_set_dst_handshake_type(dmax, chanx, LL_DMA_HANDSHAKE_TYPE_SOFTWARE);

	dma_set_channel_suspend(dmax, chanx, 0);
	dma_set_channel_priority(dmax, chanx, 0);

	// Reset source gather
	dma_set_src_gather_cnt(dmax, chanx, 0);
	dma_set_src_gather_interval(dmax, chanx, 0);

	// Reset destination scatter
	dma_set_dst_scatter_cnt(dmax, chanx, 0);
	dma_set_dst_scatter_interval(dmax, chanx, 0);

	// Disable all interrupts
	dma_disable_it_tfr(dmax, chanx);
	dma_disable_it_block(dmax, chanx);
	dma_disable_it_src_tran(dmax, chanx);
	dma_disable_it_dst_tran(dmax, chanx);
	dma_disable_it_error(dmax, chanx);

	// Clean all callbacks
	hdma->xfer_cplt_callback = NULL;
	hdma->xfer_error_callback = NULL;
	hdma->xfer_block_callback = NULL;
	hdma->xfer_srctran_callback = NULL;
	hdma->xfer_dsttran_callback = NULL;
	hdma->xfer_abort_callback = NULL;

	/* Clear all interrupt flags */
	dma_clear_it_tfr(dmax, chanx);
	dma_clear_it_block(dmax, chanx);
	dma_clear_it_src_tran(dmax, chanx);
	dma_clear_it_dst_tran(dmax, chanx);
	dma_clear_it_error(dmax, chanx);

	// Reset the error code
	hdma->error_code = HAL_DMA_ERROR_NONE;

	// Reset the DMA state
	hdma->state = HAL_DMA_STATE_RESET;

	// Release lock
	__HAL_UNLOCK(hdma);

	return HAL_OK;
}

/**
 * @brief Init DMA HAL initial structure
 *
 * @param init DMA init structure @ref DMA_Init_Struct
 * @return HAL status
 */
hal_status_t hal_dma_init_struct_init(dma_init_t *init)
{
	if (NULL == init) {
		return HAL_ERROR;
	}

	init->channel = DMA_CHANNEL_0;

	// DMA control settings
	init->control.int_en         = 0; // Disable all DMA interrupts for this channel
	init->control.dst_tr_width   = DMA_TR_WIDTH_8_BITS;
	init->control.src_tr_width   = DMA_TR_WIDTH_8_BITS;
	init->control.dst_addr_inc   = DMA_ADDR_NO_CHANGE;
	init->control.src_addr_inc   = DMA_ADDR_NO_CHANGE;
	init->control.dst_burst_size = DMA_BURST_SIZE_1_ITEMS;
	init->control.src_burst_size = DMA_BURST_SIZE_1_ITEMS;
	init->control.src_gather_en  = DMA_GATHER_DISABLE;
	init->control.dst_scatter_en = DMA_SCATTER_DISABLE;
	init->control.tt_fc          = DMA_TT_M2M_FC_DMA;
	init->control.dst_llp_en     = DMA_LINKLIST_DISABLE;
	init->control.src_llp_en     = DMA_LINKLIST_DISABLE;
	init->control.block_size     = 0;
	init->control.done           = 0;

	// DMA config settings
	init->config.channel_priority         = DMA_CHAN_PRIORITY_0;
	init->config.src_handshake_type       = DMA_HANDSHAKE_TYPE_SOFTWARE;
	init->config.dst_handshake_type       = DMA_HANDSHAKE_TYPE_SOFTWARE;
	init->config.src_handshake_interface  = DMA_HANDSHAKE_IF_SPI1_TX;
	init->config.dst_handshake_interface  = DMA_HANDSHAKE_IF_SPI1_RX;
	init->config.src_reload_en            = DMA_RELOAD_DISABLE;
	init->config.dst_realod_en            = DMA_RELOAD_DISABLE;
	init->config.src_status_update_enable = 0;
	init->config.dst_status_update_enable = 0;
	init->config.fifo_mode                = DMA_FIFO_MODE_REDUCE_LATENCY;
	init->config.fcmode                   = DMA_FC_MODE_PREFETCH_ENABLE;
	init->config.rsvd_lock_bus            = 0; // IGNORE
	init->config.rsvd_lock_chan           = 0; // IGNORE
	init->config.rsvd_lock_bus_level      = 0; // IGNORE
	init->config.rsvd_lock_chan_level     = 0; // IGNOER

	// Misc settings
	init->transfer_type        = DMA_TRANSFER_TYPE_ROW1;
	init->llp_loc              = 0;

	init->src_gather_cnt       = 0;
	init->src_gather_interval  = 0;

	init->dst_scatter_cnt      = 0;
	init->dst_scatter_interval = 0;

	init->src_status_addr      = 0;
	init->dst_status_addr      = 0;

	return HAL_OK;
}

/**
 * @brief Init DMA Linked List structure
 *
 * @param lli Linked List Array
 * @param cnt Linked List Item Count
 * @return HAL status
 */
hal_status_t hal_dma_lli_struct_init(dma_link_list_t lli[], uint32_t cnt)
{
	if (NULL == lli) {
		return HAL_ERROR;
	}

	for (uint32_t i = 0; i < cnt; i++) {
		dma_link_list_t *item        = (dma_link_list_t *)(lli + i);

		item->src_addr               = (uint32_t)0;
		item->dst_addr               = (uint32_t)0;
		item->llp_next               = (struct _dma_link_list_t *)0;

		item->control.int_en         = 1;
		item->control.dst_tr_width   = DMA_TR_WIDTH_8_BITS;
		item->control.src_tr_width   = DMA_TR_WIDTH_8_BITS;
		item->control.dst_addr_inc   = DMA_ADDR_INCREMENT;
		item->control.src_addr_inc   = DMA_ADDR_INCREMENT;
		item->control.dst_burst_size = DMA_BURST_SIZE_1_ITEMS;
		item->control.src_burst_size = DMA_BURST_SIZE_1_ITEMS;
		item->control.src_gather_en  = DMA_GATHER_DISABLE;
		item->control.dst_scatter_en = DMA_SCATTER_DISABLE;
		item->control.tt_fc          = DMA_TT_M2M_FC_DMA;
		item->control.dst_llp_en     = DMA_LINKLIST_DISABLE;
		item->control.src_llp_en     = DMA_LINKLIST_DISABLE;
		item->control.block_size     = 0;
		item->control.done           = 0;

		item->src_status             = 0;
		item->dst_status             = 0;

		if (i < (cnt - 1)) {
			item->llp_next = (dma_link_list_t *)(lli + i + 1);
		}
	}

	return HAL_OK;
}

/**
 * @}
 */


/* Exported Functions Group2 -------------------------------------------------*/
/**
 * @defgroup DMA_Exported_Functions_Group2 I/O Operation Functions
 * @brief I/O Operation Functions
 * @{
 */

/**
 * @brief Starts the DMA transfer.
 *
 * @param hdma         Pointer to a dma_handl_t strucutre that contains the configuration information for the specified DMA channel.
 * @param src_addr     The source memory buffer address
 * @param dst_addr     The destination memory buffer address
 * @param data_length  The length of data to be transferred from source to destination
 * @param block_total  How many blocks will be transferred.
 * @return HAL status
 */
hal_status_t hal_dma_start(dma_handle_t *hdma, uint32_t src_addr, uint32_t dst_addr, uint32_t data_length, uint32_t block_total)
{
	hal_status_t status = HAL_OK;
	dma_base_t dmax = hdma->instance;
	uint32_t   chanx = hdma->init.channel;

	__HAL_LOCK(hdma);

	if (HAL_DMA_STATE_READY == hdma->state) {
		/* Change DMA peripheral state */
		hdma->state = HAL_DMA_STATE_BUSY;

		/* Initialize the error code */
		hdma->error_code = HAL_DMA_ERROR_NONE;

		/* Configure the source, destination address and the data length */
		dma_setconfig(hdma, src_addr, dst_addr, data_length);

		dma_clear_block_done_flag(dmax, chanx);

		hdma->block_total = block_total;
		hdma->block_cnt = 0;

		// Disable channel interrupts
		dma_set_int_en(dmax, chanx, 0);

		// Enable DMA channel
		dma_enable_channel(dmax, chanx);
	} else {
		__HAL_UNLOCK(hdma);

		status = HAL_BUSY;
	}

	return status;
}

/**
 * @brief Aborts the DMA transfer.
 *
 * @param hdma
 * @return HAL status
 */
hal_status_t hal_dma_abort(dma_handle_t *hdma)
{
	uint32_t timeout_cnt = HAL_TIMEOUT_DMA_ABORT;

	if (NULL == hdma) {
		return HAL_ERROR;
	}

	dma_base_t dmax  = hdma->instance;
	uint32_t   chanx = hdma->init.channel;

	if (hdma->state != HAL_DMA_STATE_BUSY) {
		hdma->error_code = HAL_DMA_ERROR_NO_XFER;

		__HAL_UNLOCK(hdma);

		return HAL_ERROR;
	} else {
		dma_disable_it_tfr(dmax, chanx);
		dma_disable_it_block(dmax, chanx);
		dma_disable_it_src_tran(dmax, chanx);
		dma_disable_it_block(dmax, chanx);
		dma_disable_it_error(dmax, chanx);

		dma_disable_channel(dmax, chanx);

		while (dma_is_active_channel(dmax, chanx) != RESET) {
			if (--timeout_cnt == 0) {
				hdma->error_code = HAL_DMA_ERROR_TIMEOUT;
				hdma->state = HAL_DMA_STATE_TIMEOUT;

				__HAL_UNLOCK(hdma);
				return HAL_TIMEOUT;
			}
		}

		dma_clear_it_tfr(dmax, chanx);
		dma_clear_it_block(dmax, chanx);
		dma_clear_it_src_tran(dmax, chanx);
		dma_clear_it_dst_tran(dmax, chanx);
		dma_clear_it_error(dmax, chanx);

		hdma->state = HAL_DMA_STATE_READY;

		__HAL_UNLOCK(hdma);
	}

	return HAL_OK;
}

/**
 * @brief Polling for transfer complete.
 *
 * @param hdma
 * @param timeout
 * @return HAL status
 */
hal_status_t hal_dma_poll_transfer(dma_handle_t *hdma, uint32_t timeout)
{
	hal_status_t status = HAL_OK;

	dma_base_t dmax = hdma->instance;
	uint32_t   chanx = hdma->init.channel;

	while(dma_is_active_channel(dmax, chanx));

	if (dma_get_raw_tfr(dmax, chanx)) {
		hdma->state = HAL_DMA_STATE_READY;
		hdma->error_code = HAL_DMA_ERROR_NONE;

		status = HAL_OK;
	} else {
		hdma->state = HAL_DMA_STATE_ERROR;
		hdma->error_code = HAL_DMA_ERROR_TE;

		status = HAL_ERROR;
	}

	__HAL_UNLOCK(hdma);

	return status;
}

/**
 * @brief Start the DMA transfer with interrupt enabled.
 *
 * @param hdma         Pointer to a dma_handle_t structure that contains the configuration information for the specified DMA channel
 * @param src_addr     The source memory buffer address
 * @param dst_addr     The destination memory buffer address
 * @param data_length  The length of data to be transferred from source to destination
 * @param block_total  How many blocks will be transferred
 * @return HAL status
 */
hal_status_t hal_dma_start_it(dma_handle_t *hdma, uint32_t src_addr, uint32_t dst_addr, uint32_t data_length, uint32_t block_total)
{
	hal_status_t status = HAL_OK;

	/* Check the parameters */

	dma_base_t dmax = hdma->instance;
	uint32_t   chanx = hdma->init.channel;

	/* Process locked */
	__HAL_LOCK(hdma);

	if (HAL_DMA_STATE_READY == hdma->state) {
		/* Change DMA Peripheral state */
		hdma->state = HAL_DMA_STATE_BUSY;

		/* Initialize the error code */
		hdma->error_code = HAL_DMA_ERROR_NONE;

		/* Configure the source, destination address and the data length */
		dma_setconfig(hdma, src_addr, dst_addr, data_length);

		dma_clear_block_done_flag(dmax, chanx);

		hdma->block_total = block_total;
		hdma->block_cnt = 0;

		/* Clear all interrupt flags */
		dma_clear_it_error(dmax, chanx);
		dma_clear_it_tfr(dmax, chanx);
		dma_clear_it_block(dmax, chanx);
		dma_clear_it_src_tran(dmax, chanx);
		dma_clear_it_dst_tran(dmax, chanx);

		/* Enable common interrupts */
		dma_enable_it_error(dmax, chanx);
		dma_enable_it_block(dmax, chanx);
		dma_enable_it_tfr(dmax, chanx);

		if (DMA_HANDSHAKE_TYPE_HARDWARE == hdma->init.config.src_handshake_type) {
			// hardware
			dma_set_src_handshake_if(dmax, chanx, hdma->init.config.src_handshake_interface);
			dma_set_src_handshake_polarity(dmax, chanx, hdma->init.config.src_handshake_polarity);
		} else {
			// software
			if (hdma->init.control.tt_fc == DMA_TT_P2M_FC_DMA ||
				hdma->init.control.tt_fc == DMA_TT_P2M_FC_SRC_PER ||
				hdma->init.control.tt_fc == DMA_TT_P2P_FC_SRC_PER ||
				hdma->init.control.tt_fc == DMA_TT_P2P_FC_DST_PER) {
				dma_enable_it_src_tran(dmax, chanx);
			}
		}

		if (DMA_HANDSHAKE_TYPE_HARDWARE == hdma->init.config.dst_handshake_type) {
			// hardware
			dma_set_dst_handshake_if(dmax, chanx, hdma->init.config.dst_handshake_interface);
			dma_set_dst_handshake_polarity(dmax, chanx, hdma->init.config.dst_handshake_polarity);
		} else {
			// software
			if (hdma->init.control.tt_fc == DMA_TT_M2P_FC_DMA ||
				hdma->init.control.tt_fc == DMA_TT_M2P_FC_DST_PER ||
				hdma->init.control.tt_fc == DMA_TT_P2P_FC_SRC_PER ||
				hdma->init.control.tt_fc == DMA_TT_P2P_FC_DST_PER) {
				dma_enable_it_dst_tran(dmax, chanx);
			}
		}

		dma_set_int_en(dmax, chanx, 1);
		dma_enable_channel(dmax, chanx);
	} else {
		/* Process unlocked */
		__HAL_UNLOCK(hdma);

		/* Return error status */
		status = HAL_BUSY;
	}

	return status;
}

/**
 * @brief Aborts the DMA transfer in interrupt mode.
 *
 * @param hdma
 * @return HAL status
 */
hal_status_t hal_dma_aboart_it(dma_handle_t *hdma)
{
	if (NULL == hdma) {
		return HAL_ERROR;
	}

	dma_base_t dmax  = hdma->instance;
	uint32_t   chanx = hdma->init.channel;

	if (hdma->state != HAL_DMA_STATE_BUSY) {
		hdma->error_code = HAL_DMA_ERROR_NO_XFER;
		return HAL_ERROR;
	} else {
		hdma->state = HAL_DMA_STATE_ABORT;

		dma_disable_channel(dmax, chanx);
	}

	return HAL_OK;
}

/**
 * @brief Handles the DMA interrupt request
 *
 * @param hdma Pointer to a dma_handl_t structure that contains the configuration information for the specified DMA channel
 */
void hal_dma_irq_handler(dma_handle_t *hdma)
{
	dma_base_t dmax  = hdma->instance;
	uint32_t   chanx = hdma->init.channel;

	/* Transfer Error */
	if (dma_get_status_error(dmax, chanx) && dma_is_enabled_it_error(dmax, chanx)) {
		/* Disable the Transfer Error interrupt */
		dma_disable_it_error(dmax, chanx);

		/* Clear the Trasfer Error flag */
		dma_clear_it_error(dmax, chanx);

		/* Update the error code */
		hdma->error_code = HAL_DMA_ERROR_TE;
	}

	/* Transfer Complete */
	if (dma_get_status_tfr(dmax, chanx) && dma_is_enabled_it_tfr(dmax, chanx)) {

		hdma->block_cnt++;

		if (hdma->init.transfer_type == DMA_TRANSFER_TYPE_ROW4) {
			if (hdma->block_cnt >= hdma->block_total) {
				if (hdma->xfer_cplt_callback != NULL) {
					hdma->xfer_cplt_callback(hdma);
				}

				dma_set_int_en(dmax, chanx, 0);
				dma_clear_it_tfr(dmax, chanx);
				dma_clear_it_src_tran(dmax, chanx);
				dma_clear_it_dst_tran(dmax, chanx);
				dma_clear_it_block(dmax,chanx);
			}
			return;
		}

		if (hdma->xfer_cplt_callback != NULL) {
			hdma->xfer_cplt_callback(hdma);
		}

		/* Clear the transfer complete flag */
		dma_clear_it_tfr(dmax, chanx);
		dma_clear_it_block(dmax, chanx);
		dma_clear_it_src_tran(dmax, chanx);
		dma_clear_it_dst_tran(dmax, chanx);

		/* Disable all the transfer interrupts */
		dma_disable_it_tfr(dmax, chanx);
		dma_disable_it_block(dmax, chanx);
		dma_disable_it_src_tran(dmax, chanx);
		dma_disable_it_dst_tran(dmax, chanx);
		dma_disable_it_error(dmax, chanx);

		if (HAL_DMA_STATE_ABORT == hdma->state) {
			if (hdma->xfer_abort_callback != NULL) {
				hdma->xfer_abort_callback(hdma);
			}

			hdma->state = HAL_DMA_STATE_READY;
			__HAL_UNLOCK(hdma);
			return;
		}

		hdma->state = HAL_DMA_STATE_READY;
		__HAL_UNLOCK(hdma);
	}

	// Block transfer complete
	if (dma_get_status_block(dmax, chanx) && dma_is_enabled_it_block(dmax, chanx)) {
		hdma->block_cnt++;
		if (hdma->block_cnt == (hdma->block_total - 1)) {
			switch (hdma->init.transfer_type)
			{
			case DMA_TRANSFER_TYPE_ROW2:
			case DMA_TRANSFER_TYPE_ROW9:
				dma_set_dst_reload_en(dmax, chanx, 0);
				break;

			case DMA_TRANSFER_TYPE_ROW3:
			case DMA_TRANSFER_TYPE_ROW7:
				dma_set_src_realod_en(dmax, chanx, 0);
				break;

			case DMA_TRANSFER_TYPE_ROW4:
				dma_set_src_realod_en(dmax, chanx, 0);
				dma_set_dst_reload_en(dmax, chanx, 0);
				break;

			default:
				break;
			}
		}

		if (hdma->xfer_block_callback != NULL) {
			hdma->xfer_block_callback(hdma);
		}

		dma_clear_it_src_tran(dmax, chanx);
		dma_clear_it_dst_tran(dmax, chanx);
		dma_clear_it_block(dmax, chanx);
	}


	// Source Transaction Complete
	if (dma_get_status_src_tran(dmax, chanx) && dma_is_enabled_it_src_tran(dmax, chanx)) {
		if (hdma->xfer_srctran_callback != NULL) {
			hdma->xfer_srctran_callback(hdma);
		}

		dma_clear_it_src_tran(dmax, chanx);
	}

	// Destination Transaction Complete
	if (dma_get_status_dst_tran(dmax, chanx) && dma_is_enabled_it_dst_tran(dmax, chanx)) {
		if (hdma->xfer_dsttran_callback != NULL) {
			hdma->xfer_dsttran_callback(hdma);
		}

		dma_clear_it_dst_tran(dmax, chanx);
	}

	if (hdma->error_code != HAL_DMA_ERROR_NONE) {
		if (hdma->error_code & HAL_DMA_ERROR_TE) {
			hdma->state = HAL_DMA_STATE_ABORT;
			dma_disable_channel(dmax, chanx);

			while (dma_is_active_channel(dmax, chanx));

			hdma->state = HAL_DMA_STATE_READY;

			__HAL_UNLOCK(hdma);
		}

		if (hdma->xfer_error_callback != NULL) {
			hdma->xfer_error_callback(hdma);
		}
	}
}

/**
 * @brief Register callbacks
 *
 * @param hdma pointer to dma_handle_t structure that contains the configuration
 *             information for the specified DMA channel.
 * @param id   User callback identifier
 * @param callback pointer to private callback funtion which has pointer to a
 *             dma_handle_t structure as parameter
 * @return HAL status
 */
hal_status_t hal_dma_register_callbacks(dma_handle_t *hdma, hal_dma_callback_id_t id, void (*callback)(dma_handle_t *hdma))
{
	hal_status_t status = HAL_OK;

	__HAL_LOCK(hdma);

	if (HAL_DMA_STATE_READY == hdma->state) {
		switch (id) {
		case HAL_DMA_XFER_CPLT_CB_ID:
			hdma->xfer_cplt_callback = callback;
			break;

		case HAL_DMA_XFER_ERROR_CB_ID:
			hdma->xfer_error_callback = callback;
			break;

		case HAL_DMA_XFER_BLOCK_CB_ID:
			hdma->xfer_block_callback = callback;
			break;

		case HAL_DMA_XFER_SRCTRAN_CB_ID:
			hdma->xfer_srctran_callback = callback;
			break;

		case HAL_DMA_XFER_DSTTRAN_CB_ID:
			hdma->xfer_dsttran_callback = callback;
			break;

		case HAL_DMA_ABORT_CB_ID:
			hdma->xfer_abort_callback = callback;
			break;
		default:
			status = HAL_ERROR;
			break;
		}
	} else {
		status = HAL_ERROR;
	}

	__HAL_UNLOCK(hdma);

	return status;
}

/**
 * @brief Unregister callbacks
 *
 * @param hdma pointer to a dma_handle_t structure that contains the configuration
 *             information for the specified DMA channel
 * @param id   User callback identifier
 * @return HAL status
 */
hal_status_t hal_dma_unregister_callbacks(dma_handle_t *hdma, hal_dma_callback_id_t id)
{
	hal_status_t status = HAL_OK;

	__HAL_LOCK(hdma);

	if (HAL_DMA_STATE_READY == hdma->state) {
		switch (id)
		{
		case HAL_DMA_XFER_CPLT_CB_ID:
			hdma->xfer_cplt_callback = NULL;
			break;

		case HAL_DMA_XFER_ERROR_CB_ID:
			hdma->xfer_error_callback = NULL;
			break;

		case HAL_DMA_XFER_BLOCK_CB_ID:
			hdma->xfer_block_callback = NULL;
			break;

		case HAL_DMA_XFER_SRCTRAN_CB_ID:
			hdma->xfer_srctran_callback = NULL;
			break;

		case HAL_DMA_XFER_DSTTRAN_CB_ID:
			hdma->xfer_dsttran_callback = NULL;
			break;

		case HAL_DMA_ABORT_CB_ID:
			hdma->xfer_abort_callback = NULL;
			break;

		case HAL_DMA_ALL_CB_ID:
			hdma->xfer_cplt_callback = NULL;
			hdma->xfer_error_callback = NULL;
			hdma->xfer_block_callback = NULL;
			hdma->xfer_srctran_callback = NULL;
			hdma->xfer_dsttran_callback = NULL;
			hdma->xfer_abort_callback = NULL;
			break;

		default:
			status = HAL_ERROR;
			break;
		}
	}

	__HAL_UNLOCK(hdma);

	return status;
}

/**
 * @}
 */

/* Exported Functions Group3 -------------------------------------------------*/
/**
 * @addtogroup DMA_Exported_Functions_Group3 Peripheral State Functions
 * @{
 */

/**
 * @brief Returns the DMA state
 *
 * @param hdma  pointer to dma_handle_t structure that contains the configuration
 *              information for the specified DMA channel
 * @retval HAL state
 */
hal_dma_state_t hal_dma_get_state(dma_handle_t *hdma)
{
	return hdma->state;
}

/**
 * @brief Return the DMA error code
 *
 * @param hdma  pointer to a dma_handle_t structure that contains the configuration
 *              information for the specified DMA channel
 * @retval DMA error code
 */
uint32_t hal_dma_get_error(dma_handle_t *hdma)
{
	return hdma->error_code;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @addtogroup DMA_Private_Functions
 * @{
 */

/**
 * @brief Sets the DMA transfer parameter.
 *
 * @param hdma
 * @param src_addr    The source memory buffer address
 * @param dst_addr    The destination memory buffer address
 * @param data_length The length of data to be transferred from source to destination
 */
static void dma_setconfig(dma_handle_t *hdma, uint32_t src_addr, uint32_t dst_addr, uint32_t data_length)
{
	dma_base_t dmax = hdma->instance;
	uint32_t   chanx = hdma->init.channel;

	dma_set_src_addr(dmax, chanx, src_addr);
	dma_set_dst_addr(dmax, chanx, dst_addr);
	dma_set_block_transfer_size(dmax, chanx, data_length);
}

/**
 * @brief Set Multi-block transfer type.
 *
 * @param dmax DMAx instance
 * @param chanx DMA channel, @ref DMA_Channel_Selection
 * @param transfer DMA transfer type, @ref DMA_Transfer_Type
 * @retval HAL status
 */
static int dma_set_transfer_type(dma_base_t dmax, uint32_t chanx, uint32_t transfer)
{
	if (dma_is_active_channel(dmax, chanx)) {
		return HAL_ERROR;
	}

	switch (transfer)
	{
	case DMA_TRANSFER_TYPE_ROW1:
		dma_set_linked_list_loc(dmax, chanx, 0);

		dma_set_src_linked_list_en(dmax, chanx, 0);
		dma_set_dst_linked_list_en(dmax, chanx, 0);
		dma_set_src_realod_en(dmax, chanx, 0);
		dma_set_dst_reload_en(dmax, chanx, 0);
		break;

	case DMA_TRANSFER_TYPE_ROW2:
		dma_set_linked_list_loc(dmax, chanx, 0);

		dma_set_src_linked_list_en(dmax, chanx, 0);
		dma_set_dst_linked_list_en(dmax, chanx, 0);
		dma_set_src_realod_en(dmax, chanx, 0);
		dma_set_dst_reload_en(dmax, chanx, 1);
		break;

	case DMA_TRANSFER_TYPE_ROW3:
		dma_set_linked_list_loc(dmax, chanx, 0);

		dma_set_src_linked_list_en(dmax, chanx, 0);
		dma_set_dst_linked_list_en(dmax, chanx, 0);
		dma_set_src_realod_en(dmax, chanx, 1);
		dma_set_dst_reload_en(dmax, chanx, 0);
		break;

	case DMA_TRANSFER_TYPE_ROW4:
		dma_set_linked_list_loc(dmax, chanx, 0);

		dma_set_src_linked_list_en(dmax, chanx, 0);
		dma_set_dst_linked_list_en(dmax, chanx, 0);
		dma_set_src_realod_en(dmax, chanx, 1);
		dma_set_dst_reload_en(dmax, chanx, 1);
		break;

	case DMA_TRANSFER_TYPE_ROW5:
		dma_set_src_linked_list_en(dmax, chanx, 0);
		dma_set_dst_linked_list_en(dmax, chanx, 0);
		dma_set_src_realod_en(dmax, chanx, 0);
		dma_set_dst_reload_en(dmax, chanx, 0);
		break;

	case DMA_TRANSFER_TYPE_ROW6:
		dma_set_src_linked_list_en(dmax, chanx, 0);
		dma_set_dst_linked_list_en(dmax, chanx, 1);
		dma_set_src_realod_en(dmax, chanx, 0);
		dma_set_dst_reload_en(dmax, chanx, 0);
		break;

	case DMA_TRANSFER_TYPE_ROW7:
		dma_set_src_linked_list_en(dmax, chanx, 0);
		dma_set_dst_linked_list_en(dmax, chanx, 1);
		dma_set_src_realod_en(dmax, chanx, 1);
		dma_set_dst_reload_en(dmax, chanx, 0);
		break;

	case DMA_TRANSFER_TYPE_ROW8:
		dma_set_src_linked_list_en(dmax, chanx, 1);
		dma_set_dst_linked_list_en(dmax, chanx, 0);
		dma_set_src_realod_en(dmax, chanx, 0);
		dma_set_dst_reload_en(dmax, chanx, 0);
		break;

	case DMA_TRANSFER_TYPE_ROW9:
		dma_set_src_linked_list_en(dmax, chanx, 1);
		dma_set_dst_linked_list_en(dmax, chanx, 0);
		dma_set_src_realod_en(dmax, chanx, 0);
		dma_set_dst_reload_en(dmax, chanx, 1);
		break;

	case DMA_TRANSFER_TYPE_ROW10:
		dma_set_src_linked_list_en(dmax, chanx, 1);
		dma_set_dst_linked_list_en(dmax, chanx, 1);
		dma_set_src_realod_en(dmax, chanx, 0);
		dma_set_dst_reload_en(dmax, chanx, 0);
		break;

	default:
		break;
	}

	return HAL_OK;
}

/**
 * @brief
 *
 * @param dmax
 * @param chanx
 * @return
 */
static uint32_t dma_get_transfer_type(dma_base_t dmax, uint32_t chanx)
{
	uint32_t row = 0;
	dma_transfer_type_t retval = DMA_TRANSFER_TYPE_ROW1;

	if (dma_get_dst_reload_en(dmax, chanx)) {
		row |= 0x01;
	}

	if (dma_get_dst_linked_list_en(dmax, chanx)) {
		row |= 0x02;
	}

	if (dma_get_src_realod_en(dmax, chanx)) {
		row |= 0x04;
	}

	if (dma_get_src_linked_list_en(dmax, chanx)) {
		row |= 0x08;
	}

	if (dma_get_linked_list_loc(dmax, chanx) != 0U) {
		row |= 0x10;
	}

	switch (row) {
	case 0x00: retval = DMA_TRANSFER_TYPE_ROW1;  break;
	case 0x01: retval = DMA_TRANSFER_TYPE_ROW2;  break;
	case 0x04: retval = DMA_TRANSFER_TYPE_ROW3;  break;
	case 0x05: retval = DMA_TRANSFER_TYPE_ROW4;  break;
	case 0x10: retval = DMA_TRANSFER_TYPE_ROW5;  break;
	case 0x12: retval = DMA_TRANSFER_TYPE_ROW6;  break;
	case 0x16: retval = DMA_TRANSFER_TYPE_ROW7;  break;
	case 0x18: retval = DMA_TRANSFER_TYPE_ROW8;  break;
	case 0x19: retval = DMA_TRANSFER_TYPE_ROW9;  break;
	case 0x1A: retval = DMA_TRANSFER_TYPE_ROW10; break;
	default:
		break;
	}

	return (uint32_t)retval;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
