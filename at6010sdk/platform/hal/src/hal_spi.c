/**
 *******************************************************************************
 * @file    hal_spi.c
 * @author  Airtouch Software Team
 * @brief   SPI HAL driver
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


#include "hal_spi.h"
#include "clock.h"
#include "at_assert.h"

/* Private Typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

#define SPI_DEFAULT_TIMEOUT               1000U
#define SPI_BSY_FLAG_WORKAROUND_TIMEOUT   1000U /* Timeout 1000 µs            */


/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void spi_dma_transmit_cplt(dma_handle_t *hdma);

static void spi_dma_receive_cplt(dma_handle_t *hdma);

static void spi_dma_transmit_receive_cplt(dma_handle_t *hdma);

static void spi_dma_error(dma_handle_t *hdma);

static hal_status_t spi_wait_flags_state_until_timeout(spi_handle_t *hspi, uint32_t flag, flag_state_t state, uint32_t timeout, uint32_t tick_start);

static void spi_tx_isr_8bit(spi_handle_t *hspi);
static void spi_tx_isr_16bit(spi_handle_t *hspi);
static void spi_tx_isr_32bit(spi_handle_t *hspi);

static void spi_rx_isr_8bit(spi_handle_t *hspi);
static void spi_rx_isr_16bit(spi_handle_t *hspi);
static void spi_rx_isr_32bit(spi_handle_t *hspi);

static void spi_close_tx_isr(spi_handle_t *hspi);
static void spi_close_rx_isr(spi_handle_t *hspi);
static void spi_close_txrx_isr(spi_handle_t *hspi);

static hal_status_t spi_end_rx_transaction(spi_handle_t *hspi, uint32_t timeout, uint32_t tick_start);
static hal_status_t spi_end_rxtx_transaction(spi_handle_t *hspi, uint32_t timeout, uint32_t tick_start);

/**
 * @brief Initializes the SPI according to the specified parameters.
 *
 * @param hspi
 * @return HAL status
 */
hal_status_t hal_spi_init(spi_handle_t *hspi)
{
	if (NULL == hspi) {
		return HAL_ERROR;
	}

	spi_base_t spix = hspi->instance;

	/* check the parameters */
	assert(IS_SPI_ALL_INSTANCE(spix));
	assert(IS_SPI_MODE(hspi->init.mode));
	assert(IS_SPI_BAUDRATE(hspi->init.baudrate));
	assert(IS_SPI_CPOL(hspi->init.clk_pol));
	assert(IS_SPI_CPHA(hspi->init.clk_pha));
	assert(IS_SPI_DFS(hspi->init.dfs));
	assert(IS_SPI_FRAME_PROTOCOL(hspi->init.frame_protocol));
	assert(IS_SPI_FRF(hspi->init.spi_frf));
	assert(IS_SPI_TMOD(hspi->init.tmod));

	if (HAL_SPI_STATE_RESET == hspi->state) {
		__HAL_UNLOCK(hspi);

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
		hspi->tx_cplt_callback = hal_spi_tx_cplt_callback;
		hspi->rx_cplt_callback = hal_spi_rx_cplt_callback;
		hspi->txrx_cplt_callback = hal_spi_txrx_cplt_callback;
		hspi->error_callback = hal_spi_error_callback;
		hspi->abort_cplt_callback = hal_spi_abort_cplt_callback;

		if (NULL == hspi->msp_init_callback) {
			hspi->msp_init_callback = hal_spi_msp_init;
		}
		/* Init the low level hardware: CLOCK, GPIO, NVIC */
		hspi->msp_init_callback(hspi);
#else
	hal_spi_msp_init(hspi);
#endif
	}

	hspi->state = HAL_SPI_STATE_BUSY;

	/* disable all SPI interrupt first */
	spi_disable_it_all(spix);

	spi_disable(spix);

	spi_set_work_mode(spix, hspi->init.mode);
	spi_set_clock_polarity(spix, hspi->init.clk_pol);
	spi_set_clock_phase(spix, hspi->init.clk_pha);
	spi_set_data_frame_size(spix, hspi->init.dfs);
	spi_set_frame_protocol(spix, hspi->init.frame_protocol);
	spi_set_spi_frame_format(spix, hspi->init.spi_frf);
	spi_set_transfer_mode(spix, hspi->init.tmod);

	uint32_t pclk = clk_mcu_get();
	uint32_t baud = pclk / hspi->init.baudrate;
	spi_set_sckdiv(spix, baud >> 1);

	spi_set_slave_select_toggle_enable(spix, hspi->init.sste);

	if (hspi->init.tx_dma_en) {
		spi_tx_dma_enable(spix, hspi->init.tx_dma_en);
		spi_set_tx_dma_data_lvl(spix, hspi->init.tx_dma_lvl);
	}

	if (hspi->init.rx_dma_en) {
		spi_rx_dma_enable(spix, hspi->init.rx_dma_en);
		spi_set_rx_dma_data_lvl(spix, hspi->init.rx_dma_lvl);
	}

	hspi->error_code = HAL_SPI_ERROR_NONE;
	hspi->state = HAL_SPI_STATE_READY;

	return HAL_OK;
}

/**
 * @brief De-Initializes the SPI peripheral.
 *
 * @param hspi
 * @return HAL status
 */
hal_status_t hal_spi_deinit(spi_handle_t *hspi)
{
	if (NULL == hspi) {
		return HAL_ERROR;
	}

	spi_base_t spix = hspi->instance;

	assert(IS_SPI_ALL_INSTANCE(spix));

	spi_disable(spix);
	spi_disable_slave(spix);

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
	if (hspi->msp_deinit_callback == NULL) {
		hspi->msp_deinit_callback = hal_spi_msp_deinit;
	}
	/* deinit the low level hardware: GPIO, CLK, NVIC... */
	hspi->msp_deinit_callback(hspi);
#else
	/* deinit the low level hardware: GPIO, CLK, NVIC... */
	hal_spi_msp_deinit(hspi);
#endif

	hspi->error_code = HAL_SPI_ERROR_NONE;
	hspi->state = HAL_SPI_STATE_RESET;

	__HAL_UNLOCK(hspi);

	return HAL_OK;
}

/**
 * @brief Initialize the SPI MSP.
 *
 * @param hspi
 * @return None
 */
__WEAK void hal_spi_msp_init(spi_handle_t *hspi)
{
	UNUSED(hspi);

	/* NOTE: This function should not be modified.                 */
	/*       This function should be implemented in the user file. */
}

/**
 * @brief De-Initialize the SPI MSP.
 *
 * @param hspi
 * @return None
 */
__WEAK void hal_spi_msp_deinit(spi_handle_t *hspi)
{
	UNUSED(hspi);

	/* NOTE: This function should not be modified.                 */
	/*       This function should be implemented in the user file. */
}

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
/**
 * @brief Register a User SPI callback to be used instead of the weak predefined callback.
 *
 * @param hspi
 * @param id ID of the callback to be registered
 * @param cb pointer to the callback function
 * @return HAL status
 */
hal_status_t hal_spi_register_callback(spi_handle_t *hspi, hal_spi_callback_id_t id, spi_callback_func_t cb)
{
	hal_status_t status = HAL_OK;

	if (NULL == hspi) {
		hspi->error_code |= HAL_SPI_ERROR_INVALID_CALLBACK;
		return HAL_ERROR;
	}

	__HAL_LOCK(hspi);

	if (HAL_SPI_STATE_READY == hspi->state) {
		switch (id) {
		case HAL_SPI_TX_COMPLETE_CB_ID:
			hspi->tx_cplt_callback = cb;
			break;

		case HAL_SPI_RX_COMPLETE_CB_ID:
			hspi->rx_cplt_callback = cb;
			break;

		case HAL_SPI_TX_RX_COMPLETE_CB_ID:
			hspi->txrx_cplt_callback = cb;
			break;

		case HAL_SPI_ERROR_CB_ID:
			hspi->error_callback = cb;
			break;

		case HAL_SPI_ABORT_CB_ID:
			hspi->abort_cplt_callback = cb;
			break;

		case HAL_SPI_MSPINIT_CB_ID:
			hspi->msp_init_callback = cb;
			break;

		case HAL_SPI_MSPDEINIT_CB_ID:
			hspi->msp_deinit_callback = cb;
			break;

		default:
			hspi->error_code |= HAL_SPI_ERROR_INVALID_CALLBACK;
			status = HAL_ERROR;
			break;
		}
	} else if (HAL_SPI_STATE_RESET == hspi->state) {
		switch (id)
		{
		case HAL_SPI_MSPINIT_CB_ID:
			hspi->msp_init_callback = cb;
			break;

		case HAL_SPI_MSPDEINIT_CB_ID:
			hspi->msp_deinit_callback = cb;
			break;

		default:
			hspi->error_code |= HAL_SPI_ERROR_INVALID_CALLBACK;
			status = HAL_ERROR;
			break;
		}
	} else {
		hspi->error_code |= HAL_SPI_ERROR_INVALID_CALLBACK;
		status = HAL_ERROR;
	}

	__HAL_UNLOCK(hspi);

	return status;
}

/**
 * @brief Unregister the SPI callback.
 *
 * @param hspi
 * @param id ID of the callback to be unregistered.
 * @return HAL status
 */
hal_status_t hal_spi_unregister_callback(spi_handle_t *hspi, hal_spi_callback_id_t id)
{
	hal_status_t status = HAL_OK;

	__HAL_LOCK(hspi);

	if (HAL_SPI_STATE_READY == hspi->state) {
		switch (id)
		{
		case HAL_SPI_TX_COMPLETE_CB_ID:
			hspi->tx_cplt_callback = hal_spi_tx_cplt_callback;
			break;

		case HAL_SPI_RX_COMPLETE_CB_ID:
			hspi->rx_cplt_callback = hal_spi_rx_cplt_callback;
			break;

		case HAL_SPI_TX_RX_COMPLETE_CB_ID:
			hspi->txrx_cplt_callback = hal_spi_txrx_cplt_callback;
			break;

		case HAL_SPI_ERROR_CB_ID:
			hspi->error_callback = hal_spi_error_callback;
			break;

		case HAL_SPI_ABORT_CB_ID:
			hspi->abort_cplt_callback = hal_spi_abort_cplt_callback;
			break;

		case HAL_SPI_MSPINIT_CB_ID:
			hspi->msp_init_callback = hal_spi_msp_init;
			break;

		case HAL_SPI_MSPDEINIT_CB_ID:
			hspi->msp_deinit_callback = hal_spi_msp_deinit;
			break;

		default:
			hspi->error_code |= HAL_SPI_ERROR_INVALID_CALLBACK;
			status = HAL_ERROR;
			break;
		}
	} else if (HAL_SPI_STATE_RESET == hspi->state) {
		switch (id)
		{
		case HAL_SPI_MSPINIT_CB_ID:
			hspi->msp_init_callback = hal_spi_msp_init;
			break;

		case HAL_SPI_MSPDEINIT_CB_ID:
			hspi->msp_deinit_callback = hal_spi_msp_deinit;
			break;

		default:
			hspi->error_code |= HAL_SPI_ERROR_INVALID_CALLBACK;
			status = HAL_ERROR;
			break;
		}
	} else {
		hspi->error_code |= HAL_SPI_ERROR_INVALID_CALLBACK;
		status = HAL_ERROR;
	}

	__HAL_UNLOCK(hspi);

	return status;
}

#endif /* USE_HAL_SPI_REGISTER_CALLBACKS == 1U */

/**
 * @brief Transmit an amount of data in blocking mode.
 *
 * @param hspi
 * @param pdata pointer to data buffer
 * @param size amount of data to be sent
 * @param timeout timeout duration
 * @return HAL status
 */
hal_status_t hal_spi_transmit(spi_handle_t *hspi, uint8_t *pdata, uint16_t size, uint32_t timeout)
{
	uint32_t tick_start = 0u;
	hal_status_t errcode = HAL_OK;
	uint16_t initial_xfer_count;
	spi_base_t spix;

	if (NULL == hspi) {
		errcode = HAL_ERROR;
		goto error;
	}

	spix = hspi->instance;

	__HAL_LOCK(hspi);

	initial_xfer_count = size;
	if (hspi->state != HAL_SPI_STATE_READY) {
		errcode = HAL_BUSY;
		goto error;
	}

	if ((NULL == pdata) || (0U == size)) {
		errcode = HAL_ERROR;
		goto error;
	}

	hspi->state = HAL_SPI_STATE_BUSY_TX;
	hspi->error_code = HAL_SPI_ERROR_NONE;

	hspi->tx_buf_ptr = (uint8_t *)pdata;
	hspi->tx_xfer_size = size;
	hspi->tx_xfer_count = size;

	hspi->rx_buf_ptr = (uint8_t *)NULL;
	hspi->rx_xfer_size = 0U;
	hspi->rx_xfer_count = 0U;

	hspi->tx_isr = NULL;
	hspi->rx_isr = NULL;

	if (!spi_is_enable(spix)) {
		spi_enable(spix);
	}

	if (SPI_MODE_MASTER == hspi->init.mode) {
		spi_enable_slave(spix);
	}

	if (IS_SPI_DFS_IN_4_8_BIT(hspi->init.dfs)) {
		/* 8BIT */
		if ((hspi->init.mode == SPI_MODE_SLAVE) || (initial_xfer_count == 0x01U)) {
			spi_write_data(spix, *((uint8_t *)hspi->tx_buf_ptr));
			hspi->tx_buf_ptr += sizeof(uint8_t);
			hspi->tx_xfer_count--;
		}

		while (hspi->tx_xfer_count > 0U) {
			if (spi_get_status_tx_fifo_not_full(spix)) {
				spi_write_data(spix, *((uint8_t *)hspi->tx_buf_ptr));
				hspi->tx_buf_ptr += sizeof(uint8_t);
				hspi->tx_xfer_count--;
			} else {
				/* TODO: timeout management */
			}
		}
	} else if (IS_SPI_DFS_IN_9_16_BIT(hspi->init.dfs)) {
		/* 16BIT */
		if ((hspi->init.mode == SPI_MODE_SLAVE) || (initial_xfer_count == 0x01U)) {
			spi_write_data(spix, *((uint16_t *)hspi->tx_buf_ptr));
			hspi->tx_buf_ptr += sizeof(uint16_t);
			hspi->tx_xfer_count--;
		}

		while (hspi->tx_xfer_count > 0U) {
			if (spi_get_status_tx_fifo_not_full(spix)) {
				spi_write_data(spix, *((uint16_t *)hspi->tx_buf_ptr));
				hspi->tx_buf_ptr += sizeof(uint16_t);
				hspi->tx_xfer_count--;
			} else {
				// TODO: timeout management
			}
		}
	} else {
		// 32BIT
		if ((hspi->init.mode == SPI_MODE_SLAVE) || (initial_xfer_count == 0x01U)) {
			spi_write_data(spix, *((uint32_t *)hspi->tx_buf_ptr));
			hspi->tx_buf_ptr += sizeof(uint32_t);
			hspi->tx_xfer_count--;
		}

		while (hspi->tx_xfer_count > 0U) {
			if (spi_get_status_tx_fifo_not_full(spix)) {
				spi_write_data(spix, *((uint32_t *)hspi->tx_buf_ptr));
				hspi->tx_buf_ptr += sizeof(uint32_t);
				hspi->tx_xfer_count--;
			} else {
				// TODO: timeout management
			}
		}
	}

	// Check the end of the transaction
	if (spi_end_rxtx_transaction(hspi, timeout, tick_start) != HAL_OK) {
		hspi->error_code = HAL_SPI_ERROR_FLAG;
	}

	if (hspi->error_code != HAL_SPI_ERROR_NONE) {
		errcode = HAL_ERROR;
	}

error:
	hspi->state = HAL_SPI_STATE_READY;
	__HAL_UNLOCK(hspi);
	return errcode;
}


/**
 * @brief Receive an amout of data in blocking mode.
 *
 * @param hspi
 * @param pdata pointer to data buffer
 * @param size  amount of data to be received
 * @param timeout timeout duration
 * @return HAL status
 */
hal_status_t hal_spi_receive(spi_handle_t *hspi, uint8_t *pdata, uint16_t size, uint32_t timeout)
{
	hal_status_t error_code = HAL_OK;
	uint32_t     tick_start = 0u;

	if (NULL == hspi) {
		return HAL_ERROR;
	}

	spi_base_t spix = hspi->instance;

	__HAL_LOCK(hspi);

	if (hspi->state != HAL_SPI_STATE_READY) {
		error_code = HAL_BUSY;
		goto error;
	}

	if ((NULL == pdata) || (0U == size)) {
		error_code = HAL_ERROR;
		goto error;
	}

	hspi->state = HAL_SPI_STATE_BUSY_RX;
	hspi->error_code = HAL_SPI_ERROR_NONE;

	hspi->rx_buf_ptr = (uint8_t *)pdata;
	hspi->rx_xfer_size = size;
	hspi->rx_xfer_count = size;

	hspi->tx_buf_ptr = (uint8_t *)NULL;
	hspi->tx_xfer_size = 0;
	hspi->tx_xfer_count = 0;

	hspi->tx_isr = NULL;
	hspi->rx_isr = NULL;

	// now start in receive mode
	if (hspi->init.mode == SPI_MODE_MASTER) {
		spi_set_number_of_data_frames(spix, size);
	}

	if (!spi_is_enable(spix)) {
		spi_enable(spix);
	}

	if (SPI_MODE_MASTER == hspi->init.mode) {
		spi_enable_slave(spix);
	}

	// write a dummy data first
	spi_write_data(spix, 0xFF);

	if (IS_SPI_DFS_IN_4_8_BIT(hspi->init.dfs)) {
		// DFS 8bit
		while (hspi->rx_xfer_count > 0U) {
			if (spi_get_status_rx_fifo_not_empty(spix)) {
				*((uint8_t *)hspi->rx_buf_ptr) = spi_read_data(spix);
				hspi->rx_buf_ptr += sizeof(uint8_t);
				hspi->rx_xfer_count--;
			} else {
				// TODO: timeout management
			}
		}
	} else if (IS_SPI_DFS_IN_9_16_BIT(hspi->init.dfs)) {
		// DFS 16bit
		while (hspi->rx_xfer_count > 0U) {
			if (spi_get_status_rx_fifo_not_empty(spix)) {
				*((uint16_t *)hspi->rx_buf_ptr) = spi_read_data(spix);
				hspi->rx_buf_ptr += sizeof(uint16_t);
				hspi->rx_xfer_count--;
			} else {
				// TODO: timeout management
			}
		}
	} else {
		// DFS 32bit
		while (hspi->rx_xfer_count > 0U) {
			if (spi_get_status_rx_fifo_not_empty(spix)) {
				*((uint32_t *)hspi->rx_buf_ptr) = spi_read_data(spix);
				hspi->rx_buf_ptr += sizeof(uint32_t);
				hspi->rx_xfer_count--;
			} else {
				// TODO: timeout management
			}
		}
	}


	if (spi_end_rx_transaction(hspi, timeout, tick_start) != HAL_OK) {
		hspi->error_code = HAL_SPI_ERROR_FLAG;
	}

	if (hspi->error_code != HAL_SPI_ERROR_NONE) {
		error_code = HAL_ERROR;
	}

error:
	hspi->state = HAL_SPI_STATE_READY;
	__HAL_UNLOCK(hspi);

	return error_code;
}

/**
 * @brief Transmit and Receive an amount of data in blocking mode.
 *
 * @param hspi
 * @param tx_data pointer to transmission data buffer
 * @param rx_data pointer to reception data buffer
 * @param size amount of data to be sent and received
 * @param timeout timeout duration
 * @return HAL status
 */
hal_status_t hal_spi_transmit_receive(spi_handle_t *hspi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint32_t timeout)
{
	uint16_t initial_tx_xfer_count = 0;
	uint32_t tmp_mode;
	uint32_t tick_start = 0u;
	hal_spi_state_t tmp_state;

	uint32_t tx_allowed = 1U;
	hal_status_t error_code = HAL_OK;

	if (NULL == hspi) {
		return HAL_ERROR;
	}

	spi_base_t spix = hspi->instance;

	__HAL_LOCK(hspi);

	tmp_state = hspi->state;
	tmp_mode = hspi->init.mode;
	initial_tx_xfer_count = size;

	if (!((tmp_state == HAL_SPI_STATE_READY) || \
        ((tmp_mode == SPI_MODE_MASTER) && (tmp_state == HAL_SPI_STATE_BUSY_RX)))) {
		error_code = HAL_BUSY;
		goto error;
	}

	if ((tx_data == NULL) || (rx_data == NULL) || (size == 0U)) {
		error_code = HAL_ERROR;
		goto error;
	}

	if (hspi->state != HAL_SPI_STATE_BUSY_RX) {
		hspi->state = HAL_SPI_STATE_BUSY_TX_RX;
	}

	hspi->tx_buf_ptr = (uint8_t *)tx_data;
	hspi->tx_xfer_size = size;
	hspi->tx_xfer_count = size;

	hspi->rx_buf_ptr = (uint8_t *)rx_data;
	hspi->rx_xfer_size = size;
	hspi->rx_xfer_count = size;

	hspi->error_code = HAL_SPI_ERROR_NONE;

	hspi->tx_isr = NULL;
	hspi->rx_isr = NULL;

	if (!spi_is_enable(spix)) {
		spi_enable(spix);
	}

	if (SPI_MODE_MASTER == hspi->init.mode) {
		spi_enable_slave(spix);
	}

	// Transmit and receive data in 8bit mode
	if (IS_SPI_DFS_IN_4_8_BIT(hspi->init.dfs)) {
		if ((hspi->init.mode == SPI_MODE_SLAVE) || (initial_tx_xfer_count == 1U)) {
			spi_write_data(spix, *hspi->tx_buf_ptr);
			hspi->tx_buf_ptr += sizeof(uint8_t);
			hspi->tx_xfer_count--;
		}

		while ((hspi->tx_xfer_count > 0U) || (hspi->rx_xfer_count > 0U)) {
			// check for tx
			if ((spi_get_status_tx_fifo_not_full(spix)) && (hspi->tx_xfer_count > 0U) && (tx_allowed == 1U)) {
				spi_write_data(spix, *hspi->tx_buf_ptr);
				hspi->tx_buf_ptr++;
				hspi->tx_xfer_count--;

				tx_allowed = 0U;
			}

			// check for rx
			if ((spi_get_status_rx_fifo_not_empty(spix)) && (hspi->rx_xfer_count > 0U)) {
				*((uint8_t *)hspi->rx_buf_ptr) = spi_read_data(spix);
				hspi->rx_buf_ptr += sizeof(uint8_t);
				hspi->rx_xfer_count--;

				tx_allowed = 1U;
			}

			// TODO: timeout
		}
	} else if (IS_SPI_DFS_IN_9_16_BIT(hspi->init.dfs)) {
		if ((hspi->init.mode == SPI_MODE_SLAVE) || (initial_tx_xfer_count == 1U)) {
			spi_write_data(spix, *((uint16_t *)hspi->tx_buf_ptr));
			hspi->tx_buf_ptr += sizeof(uint8_t);
			hspi->tx_xfer_count--;
		}

		while ((hspi->tx_xfer_count > 0U) || (hspi->rx_xfer_count > 0U)) {
			// check for tx
			if ((tx_allowed == 1U) && (spi_get_status_tx_fifo_not_full(spix)) && (hspi->tx_xfer_count > 0U)) {
				spi_write_data(spix, *((uint16_t *)hspi->tx_buf_ptr));
				hspi->tx_buf_ptr += sizeof(uint16_t);
				hspi->tx_xfer_count--;

				tx_allowed = 0U;
			}

			// check for rx
			if ((spi_get_status_rx_fifo_not_empty(spix)) && (hspi->rx_xfer_count > 0U)) {
				*((uint16_t *)hspi->rx_buf_ptr) = spi_read_data(spix);
				hspi->rx_buf_ptr += sizeof(uint16_t);
				hspi->rx_xfer_count--;

				tx_allowed = 1U;
			}

			// TODO: timeout management
		}
	} else {

	}

	// check the end of the transaction
	if (spi_end_rxtx_transaction(hspi, timeout, tick_start) != HAL_OK) {
		error_code = HAL_ERROR;
		hspi->error_code = HAL_SPI_ERROR_FLAG;
		goto error;
	}

error:

	spi_disable_slave(spix);
	hspi->state = HAL_SPI_STATE_READY;
	__HAL_UNLOCK(hspi);

	return error_code;
}

/**
 * @brief Transmit an amount of data in non-blocking mode with interrupt.
 *
 * @param hspi
 * @param pdata pointer to data buffer
 * @param size  amount of data to be sent
 * @return HAL status
 */
hal_status_t hal_spi_transmit_it(spi_handle_t *hspi, uint8_t *pdata, uint16_t size)
{
	hal_status_t error_code = HAL_OK;

	if (NULL == hspi) {
		return HAL_ERROR;
	}

	spi_base_t spix = hspi->instance;

	assert((hspi->init.tmod == SPI_TMOD_TX_RX) || (hspi->init.tmod == SPI_TMOD_TX_ONLY));

	__HAL_LOCK(hspi);

	if ((pdata == NULL) || (size == 0U)) {
		error_code = HAL_ERROR;
		goto error;
	}

	if (hspi->state != HAL_SPI_STATE_READY) {
		error_code = HAL_BUSY;
		goto error;
	}

	hspi->state = HAL_SPI_STATE_BUSY_TX;
	hspi->error_code = HAL_SPI_ERROR_NONE;

	hspi->tx_buf_ptr = (uint8_t *)pdata;
	hspi->tx_xfer_size = size;
	hspi->tx_xfer_count = size;

	hspi->rx_buf_ptr = (uint8_t *)NULL;
	hspi->rx_xfer_size = 0U;
	hspi->rx_xfer_count = 0U;
	hspi->rx_isr = NULL;

	if (IS_SPI_DFS_IN_4_8_BIT(hspi->init.dfs)) {
		hspi->tx_isr = spi_tx_isr_8bit;
	} else if (IS_SPI_DFS_IN_9_16_BIT(hspi->init.dfs)) {
		hspi->tx_isr = spi_tx_isr_16bit;
	} else {
		hspi->tx_isr = spi_tx_isr_32bit;
	}

	spi_enable_it_tx_fifo_empty(spix);
	spi_enable_it_tx_fifo_overflow(spix);
	spi_enable_it_spi_tx_err(spix);

	if (!spi_is_enable(spix)) {
		spi_enable(spix);
	}

	if (SPI_MODE_MASTER == hspi->init.mode) {
		spi_enable_slave(spix);
	}

error:
	__HAL_UNLOCK(hspi);
	return error_code;
}

/**
 * @brief Receive an amount of data in non-blocking mode with interrupt.
 *
 * @param hspi
 * @param pdata pointer to data buffer
 * @param size amount of data to be received
 * @return hal_status_t
 */
hal_status_t hal_spi_receive_it(spi_handle_t *hspi, uint8_t *pdata, uint16_t size)
{
	hal_status_t error_code = HAL_OK;

	if (NULL == hspi) {
		return HAL_ERROR;
	}

	spi_base_t spix = hspi->instance;

	if ((hspi->init.mode == SPI_MODE_MASTER) && (hspi->init.tmod == SPI_TMOD_TX_RX)) {
		hspi->state = HAL_SPI_STATE_BUSY_RX;
		return hal_spi_transmit_receive_it(hspi, pdata, pdata, size);
	}

	__HAL_UNLOCK(hspi);

	if (hspi->state != HAL_SPI_STATE_READY) {
		error_code = HAL_BUSY;
		goto error;
	}

	if ((pdata == NULL) || (size == 0U)) {
		error_code = HAL_ERROR;
		goto error;
	}

	hspi->state = HAL_SPI_STATE_BUSY_RX;
	hspi->error_code = HAL_SPI_ERROR_NONE;

	hspi->rx_buf_ptr = (uint8_t *)pdata;
	hspi->rx_xfer_size = size;
	hspi->rx_xfer_count = size;

	hspi->tx_buf_ptr = (uint8_t *)NULL;
	hspi->tx_xfer_size = 0U;
	hspi->tx_xfer_count = 0U;

	hspi->tx_isr = NULL;
	if (IS_SPI_DFS_IN_4_8_BIT(hspi->init.dfs)) {
		hspi->rx_isr = spi_rx_isr_8bit;
	} else if (IS_SPI_DFS_IN_9_16_BIT(hspi->init.dfs)) {
		hspi->rx_isr = spi_rx_isr_16bit;
	} else {
		hspi->rx_isr = spi_rx_isr_32bit;
	}

	spi_disable_it_all(spix);
	spi_enable_it_rx_fifo_full(spix);
	spi_enable_it_rx_fifo_overflow(spix);
	spi_enable_it_rx_fifo_underflow(spix);

	if ((hspi->init.mode == SPI_MODE_MASTER) && (hspi->init.tmod == SPI_TMOD_RX_ONLY)) {
		spi_set_number_of_data_frames(spix, size);
	}

	if (!spi_is_enable(spix)) {
		spi_enable(spix);
	}

	if (SPI_MODE_MASTER == hspi->init.mode) {
		spi_enable_slave(spix);
	}
	spi_write_data(spix, 0xFF);

error:
	__HAL_UNLOCK(hspi);
	return error_code;
}

/**
 * @brief Transmit and Receive an amount of data in non-blocking mode with interrupt.
 *
 * @param hspi
 * @param tx_data pointer to transmission data buffer
 * @param rx_data pointer to reception dta buffer
 * @param size amount of data to be sent and received
 * @return HAL status
 */
hal_status_t hal_spi_transmit_receive_it(spi_handle_t *hspi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size)
{
	uint32_t        tmp_mode;
	hal_spi_state_t tmp_state;
	hal_status_t    error_code = HAL_OK;

	if (NULL == hspi) {
		return HAL_ERROR;
	}

	spi_base_t spix = hspi->instance;

	__HAL_LOCK(hspi);
	tmp_state = hspi->state;
	tmp_mode = hspi->init.mode;

	if (!((tmp_state == HAL_SPI_STATE_READY) || \
	((tmp_mode == SPI_MODE_MASTER) || (tmp_state == HAL_SPI_STATE_BUSY_RX)))) {
		error_code = HAL_BUSY;
		goto error;
	}

	if ((tx_data == NULL) || (rx_data == NULL) || (size == 0U)) {
		error_code = HAL_ERROR;
		goto error;
	}

	if (hspi->state == HAL_SPI_STATE_BUSY_RX) {
		hspi->state = HAL_SPI_STATE_BUSY_TX_RX;
	}

	hspi->error_code = HAL_SPI_ERROR_NONE;

	hspi->tx_buf_ptr = (uint8_t *)tx_data;
	hspi->tx_xfer_size = size;
	hspi->tx_xfer_count = size;

	hspi->rx_buf_ptr = (uint8_t *)rx_data;
	hspi->rx_xfer_size = size;
	hspi->rx_xfer_count = size;

	if (IS_SPI_DFS_IN_4_8_BIT(hspi->init.dfs)) {
		hspi->tx_isr = spi_tx_isr_8bit;
		hspi->rx_isr = spi_rx_isr_8bit;
	} else if (IS_SPI_DFS_IN_9_16_BIT(hspi->init.dfs)) {
		hspi->tx_isr = spi_tx_isr_16bit;
		hspi->rx_isr = spi_rx_isr_16bit;
	} else {
		hspi->tx_isr = spi_tx_isr_32bit;
		hspi->rx_isr = spi_rx_isr_32bit;
	}

	spi_disable_it_all(spix);
	spi_enable_it_tx_fifo_empty(spix);
	spi_enable_it_tx_fifo_overflow(spix);
	spi_enable_it_rx_fifo_full(spix);
	spi_enable_it_spi_tx_err(spix);

	if (!spi_is_enable(spix)) {
		spi_enable(spix);
	}

	if (SPI_MODE_MASTER == hspi->init.mode) {
		spi_enable_slave(spix);
	}

error:
	__HAL_UNLOCK(hspi);
	return error_code;
}

/**
 * @brief Transmit an amount of data in non-blocking mode with DMA.
 *
 * @param hspi
 * @param pdata pointer to data buffer
 * @param size amount of data to be sent
 * @return HAL status
 */
hal_status_t hal_spi_transmit_dma(spi_handle_t *hspi, uint8_t *pdata, uint16_t size)
{
	hal_status_t error_code = HAL_OK;
	spi_base_t spix;

	assert(IS_SPI_DMA_HANDLE(hspi->hdmatx));

	__HAL_LOCK(hspi);

	if (hspi->state != HAL_SPI_STATE_READY) {
		error_code = HAL_BUSY;
		goto error;
	}

	if ((pdata == NULL) || (size == 0U)) {
		error_code = HAL_ERROR;
		goto error;
	}

	spix = hspi->instance;

	hspi->state = HAL_SPI_STATE_BUSY_TX;
	hspi->error_code = HAL_SPI_ERROR_NONE;

	hspi->tx_buf_ptr = (uint8_t *)pdata;
	hspi->tx_xfer_size = size;
	hspi->tx_xfer_count = size;

	hspi->rx_buf_ptr = (uint8_t *)NULL;
	hspi->rx_xfer_size = 0U;
	hspi->rx_xfer_count = 0U;

	hspi->tx_isr = NULL;
	hspi->rx_isr = NULL;

	hspi->hdmatx->xfer_cplt_callback = spi_dma_transmit_cplt;
	hspi->hdmatx->xfer_error_callback = spi_dma_error;
	hspi->hdmatx->xfer_abort_callback = NULL;

	if (!spi_is_enable(spix)) {
		spi_enable(spix);
	}

	if (SPI_MODE_MASTER == hspi->init.mode) {
		spi_enable_slave(spix);
	}

	if (HAL_OK != hal_dma_start_it(hspi->hdmatx, (uint32_t)hspi->tx_buf_ptr, (uint32_t)spi_get_dr_addr(spix), hspi->tx_xfer_count, 1)) {
		hspi->error_code |= HAL_SPI_ERROR_DMA;
		error_code = HAL_ERROR;

		hspi->state = HAL_SPI_STATE_READY;
		goto error;
	}

error:
	__HAL_UNLOCK(hspi);
	return error_code;
}

/**
 * @brief Receive an amount of data in non-blocking mode with DMA.
 *
 * @param hspi
 * @param pdata pointer to data buffer
 * @param size amount of data to be sent
 * @return HAL status
 */
hal_status_t hal_spi_receive_dma(spi_handle_t *hspi, uint8_t *pdata, uint16_t size)
{
	hal_status_t error_code = HAL_OK;

	if (NULL == hspi) {
		return HAL_ERROR;
	}

	spi_base_t spix = hspi->instance;

	assert(IS_SPI_DMA_HANDLE(hspi->hdmarx));

	if ((hspi->init.tmod == SPI_TMOD_TX_RX) && (hspi->init.mode == SPI_MODE_MASTER)) {
		hspi->state = HAL_SPI_STATE_BUSY_RX;

		assert(IS_SPI_DMA_HANDLE(hspi->hdmatx));

		return hal_spi_transmit_receive_dma(hspi, pdata, pdata, size);
	} else if ((hspi->init.tmod == SPI_TMOD_RX_ONLY) && (hspi->init.mode == SPI_MODE_MASTER)) {
		spi_set_number_of_data_frames(spix, size);
	}

	__HAL_LOCK(hspi);

	if (hspi->state != HAL_SPI_STATE_READY) {
		error_code = HAL_BUSY;
		goto error;
	}

	if ((NULL == pdata) || (0U == size)) {
		error_code = HAL_ERROR;
		goto error;
	}

	hspi->state = HAL_SPI_STATE_BUSY_RX;
	hspi->error_code = HAL_SPI_ERROR_NONE;

	hspi->rx_buf_ptr = (uint8_t *)pdata;
	hspi->rx_xfer_size = size;
	hspi->rx_xfer_count = size;

	hspi->tx_buf_ptr = (uint8_t *)NULL;
	hspi->tx_xfer_size = 0U;
	hspi->tx_xfer_count = 0U;

	hspi->rx_isr = NULL;
	hspi->tx_isr = NULL;

	hspi->hdmarx->xfer_cplt_callback = spi_dma_receive_cplt;
	hspi->hdmarx->xfer_error_callback = spi_dma_error;
	hspi->hdmarx->xfer_abort_callback = NULL;

	// enable SPI Rx DMA request
	spi_rx_dma_enable(spix, 1);
	spi_enable_it_spi_tx_err(spix);
	if (!spi_is_enable(spix)) {
		spi_enable(spix);
	}

	if (SPI_MODE_MASTER == hspi->init.mode) {
		spi_enable_slave(spix);
	}
	spi_write_data(spix, 0xFF);

	if (HAL_OK != hal_dma_start_it(hspi->hdmarx,
					(uint32_t)spi_get_dr_addr(spix), (uint32_t)hspi->rx_buf_ptr,
					hspi->rx_xfer_size, 1)) {
		hspi->error_code |= HAL_SPI_ERROR_DMA;
		error_code = HAL_ERROR;

		hspi->state = HAL_SPI_STATE_READY;
		goto error;
	}


error:
	__HAL_UNLOCK(hspi);
	return error_code;
}

/**
 * @brief Transmit and receive an amount of data in non-blocking mode with DMA.
 *
 * @param hspi
 * @param tx_data pointer to transmission data buffer
 * @param rx_data pointer to reception data buffer
 * @param size amount of data to be sent
 * @return HAL statuss
 */
hal_status_t hal_spi_transmit_receive_dma(spi_handle_t *hspi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size)
{
	uint32_t temp_mode;
	hal_spi_state_t temp_state;
	hal_status_t error_code = HAL_OK;

	if (NULL == hspi) {
		return HAL_ERROR;
	}

	spi_base_t spix = hspi->instance;

	assert(IS_SPI_DMA_HANDLE(hspi->hdmarx));
	assert(IS_SPI_DMA_HANDLE(hspi->hdmatx));

	assert(hspi->init.tmod == SPI_TMOD_TX_RX);

	__HAL_LOCK(hspi);

	temp_state = hspi->state;
	temp_mode = hspi->init.mode;

	if (!((temp_state == HAL_SPI_STATE_READY) ||
		((temp_mode == SPI_MODE_MASTER) && (hspi->init.tmod == SPI_TMOD_TX_RX) && (temp_state == HAL_SPI_STATE_BUSY_RX)))) {
		error_code = HAL_BUSY;
		goto error;
	}

	if ((NULL == tx_data) || (NULL == rx_data) || (0U == size)) {
		error_code = HAL_ERROR;
		goto error;
	}

	if (hspi->state != HAL_SPI_STATE_BUSY_RX) {
		hspi->state = HAL_SPI_STATE_BUSY_TX_RX;
	}

	hspi->error_code = HAL_SPI_ERROR_NONE;

	hspi->tx_buf_ptr = (uint8_t *)tx_data;
	hspi->tx_xfer_size = size;
	hspi->tx_xfer_count = size;

	hspi->rx_buf_ptr = (uint8_t *)rx_data;
	hspi->rx_xfer_size = size;
	hspi->rx_xfer_count = size;

	hspi->rx_isr = NULL;
	hspi->tx_isr = NULL;

	if (hspi->state == HAL_SPI_STATE_BUSY_RX) {
		hspi->hdmarx->xfer_cplt_callback = spi_dma_receive_cplt;
	} else {
		hspi->hdmarx->xfer_cplt_callback = spi_dma_transmit_receive_cplt;
	}

	hspi->hdmarx->xfer_error_callback = spi_dma_error;
	hspi->hdmarx->xfer_abort_callback = NULL;

	hspi->hdmatx->xfer_cplt_callback = NULL;
	hspi->hdmatx->xfer_error_callback = NULL;
	hspi->hdmatx->xfer_abort_callback = NULL;

	spi_rx_dma_enable(spix, 1);
	spi_tx_dma_enable(spix, 1);
	spi_enable_it_spi_tx_err(spix);

	if (!spi_is_enable(spix)) {
		spi_enable(spix);
	}

	if (SPI_MODE_MASTER == hspi->init.mode) {
		spi_enable_slave(spix);
	}

	if (HAL_OK != hal_dma_start_it(hspi->hdmarx, (uint32_t)spi_get_dr_addr(spix),
						(uint32_t)hspi->rx_buf_ptr, hspi->rx_xfer_size, 1)) {

		hspi->error_code |= HAL_SPI_ERROR_DMA;
		error_code = HAL_ERROR;

		hspi->state = HAL_SPI_STATE_READY;
		goto error;
	}

	if (HAL_OK != hal_dma_start_it(hspi->hdmatx, (uint32_t)hspi->tx_buf_ptr,
						(uint32_t)spi_get_dr_addr(spix), hspi->tx_xfer_size, 1)) {

		hspi->error_code |= HAL_SPI_ERROR_DMA;
		error_code = HAL_ERROR;

		hspi->state = HAL_SPI_STATE_READY;
		goto error;
	}

error:
	__HAL_UNLOCK(hspi);
	return error_code;
}

/**
 * @brief Handle SPI interrupt request.
 *
 * @param hspi
 */
void hal_spi_irq_handler(spi_handle_t *hspi)
{
	spi_base_t spix = hspi->instance;
	uint32_t spi_status = spi_get_status(spix);
	uint32_t spi_it_status = spi_get_it_status(spix);

	// SPI Receive
	if ( ((spi_is_active_it(spix, LL_SPI_IT_RX_FIFO_FULL) != RESET) && spi_is_enable_it(spix, LL_SPI_IT_RX_FIFO_FULL) != RESET) && \
		((spi_get_status_rx_fifo_not_empty(spix) != RESET) || (spi_get_status_rx_fifo_full(spix) != RESET) ) ) {
		hspi->rx_isr(hspi);
		return;
	}

	if ( ((spi_get_status_tx_fifo_empty(spix) != RESET) || (spi_get_status_tx_fifo_not_full(spix) != RESET)) && \
	( (spi_is_active_it(spix, SPI_IT_TX_FIFO_EMPTY) != RESET) && (spi_is_enable_it(spix, SPI_IT_TX_FIFO_EMPTY) != RESET) )) {
		hspi->tx_isr(hspi);
		return;
	}

	// TODO: error management
	if ((spi_status & (LL_SPI_STATUS_DCOL_ERR | LL_SPI_STATUS_TX_ERR)) || \
		spi_is_active_it(spix, LL_SPI_IT_SPI_TX_ERR | LL_SPI_IT_MULTI_MASTER_CONT | LL_SPI_IT_RX_FIFO_OVERFLOW | LL_SPI_IT_RX_FIFO_UNDERFLOW | LL_SPI_IT_TX_FIFO_OVERFLOW)) {

		if (spi_status & LL_SPI_STATUS_DCOL_ERR) {
			hspi->error_code |= HAL_SPI_ERROR_DCOL;
		}

		if (spi_status & LL_SPI_STATUS_TX_ERR) {
			// only valid in slave mode
			hspi->error_code |= HAL_SPI_ERROR_TXERR;
		}

		if ((spi_it_status & LL_SPI_IT_MULTI_MASTER_CONT) != RESET) {
			// only valid in master mode
			hspi->error_code |= HAL_SPI_ERROR_MULTIMASTER;
		}

		if ((spi_it_status & LL_SPI_IT_RX_FIFO_OVERFLOW) != RESET) {
			hspi->error_code |= HAL_SPI_ERROR_RXOVERFLOW;
			spi_clear_rx_overflow(spix);
		}

		if ((spi_it_status & LL_SPI_IT_RX_FIFO_UNDERFLOW) != RESET) {
			hspi->error_code |= HAL_SPI_ERROR_RXUNDERFLOW;
			spi_clear_rx_underflow(spix);
		}

		if ((spi_it_status & LL_SPI_IT_TX_FIFO_OVERFLOW) != RESET) {
			hspi->error_code |= HAL_SPI_ERROR_TXOVERFLOW;
			spi_clear_it_tx_fifo_err(spix);
		}

		if (hspi->error_code != HAL_SPI_ERROR_NONE) {
			// disable all SPI interrupts
			spi_disable_it_all(spix);

			spi_disable(spix);
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
			hspi->error_callback(hspi);
#else
			hal_spi_error_callback(hspi);
#endif
		}
		return;
	}
}

/**
 * @brief Tx transfer completed callback.
 *
 * @param hspi
 */
__WEAK void hal_spi_tx_cplt_callback(spi_handle_t *hspi)
{
	UNUSED(hspi);

	// NOTE: This function should not be modified, when the callback is needed,
	//       this function should be implemented in the user file.
}

/**
 * @brief Rx transfer completed callback.
 *
 * @param hspi
 */
__WEAK void hal_spi_rx_cplt_callback(spi_handle_t *hspi)
{
	UNUSED(hspi);

	// NOTE: This function should not be modified, when the callback is needed,
	//       this function should be implemented in the user file.
}

/**
 * @brief Tx and Rx transfer completed callback.
 *
 * @param hspi
 */
__WEAK void hal_spi_txrx_cplt_callback(spi_handle_t *hspi)
{
	UNUSED(hspi);

	// NOTE: This function should not be modified, when the callback is needed,
	//       this function should be implemented in the user file.
}

/**
 * @brief SPI error callback.
 *
 * @param hspi
 */
__WEAK void hal_spi_error_callback(spi_handle_t *hspi)
{
	UNUSED(hspi);

	// NOTE: This function should not be modified, when the callback is needed,
	//       this function should be implemented in the user file.
}

/**
 * @brief SPI abort completed callback.
 *
 * @param hspi
 */
__WEAK void hal_spi_abort_cplt_callback(spi_handle_t *hspi)
{
	UNUSED(hspi);

	// TODO: 删除 abort 回调函数

	// NOTE: This function should not be modified, when the callback is needed,
	//       this function should be implemented in the user file.
}

/**
 * @brief Return the SPI handle state.
 *
 * @param hspi
 * @return SPI state
 */
hal_spi_state_t hal_spi_get_state(spi_handle_t *hspi)
{
	return hspi->state;
}

/**
 * @brief Return the SPI error code.
 *
 * @param hspi
 * @return SPI error code in bitmap format
 */
uint32_t        hal_spi_get_error(spi_handle_t *hspi)
{
	return hspi->error_code;
}

static void spi_dma_transmit_cplt(dma_handle_t *hdma)
{
	spi_handle_t *hspi = (spi_handle_t *)(((dma_handle_t *)hdma)->parent);
	spi_base_t spix = hspi->instance;
	uint32_t tick_start = 0u;

	if (spi_end_rxtx_transaction(hspi, SPI_DEFAULT_TIMEOUT, tick_start) != HAL_OK) {
		hspi->error_code |= HAL_SPI_ERROR_FLAG;
	}

	spi_disable(spix);
	spi_tx_dma_enable(spix, 0);

	hspi->tx_xfer_count = 0U;
	hspi->state = HAL_SPI_STATE_READY;

	if (hspi->error_code != HAL_SPI_ERROR_NONE) {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
		hspi->error_callback(hspi);
#else
		hal_spi_error_callback(hspi);
#endif
		return;
	}

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
	hspi->tx_cplt_callback(hspi);
#else
	hal_spi_tx_cplt_callback(hspi);
#endif
}

/**
 * @brief DMA SPI receive process complete callback.
 *
 * @param hdma
 */
static void spi_dma_receive_cplt(dma_handle_t *hdma)
{
	spi_handle_t *hspi = (spi_handle_t *)(hdma->parent);
	spi_base_t spix = hspi->instance;
	uint32_t tick_start = 0;


	spi_disable_it_spi_tx_err(spix);

	if (spi_end_rx_transaction(hspi, SPI_DEFAULT_TIMEOUT, tick_start) != HAL_OK) {
		hspi->error_code = HAL_SPI_ERROR_FLAG;
	}

	if (spi_is_enable(spix)) {
		spi_disable(spix);
	}

	if ((hspi->init.tmod == SPI_TMOD_TX_RX) && (hspi->init.mode == SPI_MODE_MASTER)) {
		spi_tx_dma_enable(spix, 0);
		spi_rx_dma_enable(spix, 0);
	} else {
		spi_rx_dma_enable(spix, 0);
	}

	hspi->rx_xfer_count = 0U;
	hspi->state = HAL_SPI_STATE_READY;

	if (hspi->error_code != HAL_SPI_ERROR_NONE) {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
		hspi->error_callback(hspi);
#else
		hal_spi_error_callback(hspi);
#endif
		return;
	}

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
		hspi->rx_cplt_callback(hspi);
#else
		hal_spi_rx_cplt_callback(hspi);
#endif
}

/**
 * @brief DMA SPI transmit receive process complete callback.
 *
 * @param hdma
 */
static void spi_dma_transmit_receive_cplt(dma_handle_t *hdma)
{
	spi_handle_t *hspi = (spi_handle_t *)(((dma_handle_t *)hdma)->parent);
	spi_base_t spix = hspi->instance;
	uint32_t tick_start = 0;

	spi_disable_it_spi_tx_err(spix);

	if (spi_end_rxtx_transaction(hspi, SPI_DEFAULT_TIMEOUT, tick_start) != HAL_OK) {
		hspi->error_code |= HAL_SPI_ERROR_FLAG;
	}

	if (spi_is_enable(spix)) {
		spi_disable(spix);
	}
	spi_tx_dma_enable(spix, 0);
	spi_rx_dma_enable(spix, 0);

	hspi->tx_xfer_count = 0;
	hspi->rx_xfer_count = 0;
	hspi->state = HAL_SPI_STATE_READY;

	// call user error callback
	if (hspi->error_code!= HAL_SPI_ERROR_NONE) {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
		hspi->error_callback(hspi);
#else
		hal_spi_error_callback(hspi);
#endif
		return;
	}

	// TxRx complete
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
	hspi->txrx_cplt_callback(hspi);
#else
	hal_spi_txrx_cplt_callback(hspi);
#endif
}

/**
 * @brief DMA SPI communication error callback.
 *
 * @param hdma
 */
static void spi_dma_error(dma_handle_t *hdma)
{
	spi_handle_t *hspi = (spi_handle_t *)(((dma_handle_t *)hdma)->parent);
	spi_base_t spix = hspi->instance;

	if (spi_is_enable(spix)) {
		spi_disable(spix);
		spi_tx_dma_enable(spix, 0);
		spi_rx_dma_enable(spix, 0);
	}

	hspi->error_code |= HAL_SPI_ERROR_DMA;
	hspi->state = HAL_SPI_STATE_READY;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
	hspi->error_callback(hspi);
#else
	hal_spi_error_callback(hspi);
#endif
}

/**
 * @brief
 *
 * @param hspi    Pointer to a spi_handle_t structure
 * @param flag    SPI flag to check. This parameter can be one of the following values:
 *        @arg @ref SPI_STATUS_BUSY
 *        @arg @ref SPI_STATUS_TX_FIFO_NOT_FULL
 *        @arg @ref SPI_STATUS_TX_FIFO_EMPTY
 *        @arg @ref SPI_STATUS_RX_FIFO_NOT_EMPTY
 *        @arg @ref SPI_STATUS_RX_FIFO_FULL
 *        @arg @ref SPI_STATUS_TX_ERR
 *        @arg @ref SPI_STATUS_DCOL_ERR
 * @param state   Flag state to check. This parameter can be one of the following values:
 *        @arg @ref RESET
 *        @arg @ref SET
 * @param timeout Timeout duration
 * @param tick_start tick start value
 * @return HAL status
 */
static hal_status_t spi_wait_flags_state_until_timeout(spi_handle_t *hspi, uint32_t flag, flag_state_t state, uint32_t timeout, uint32_t tick_start)
{
	(void)tick_start;
	uint32_t      tmp_timeout = SPI_BSY_FLAG_WORKAROUND_TIMEOUT;
	spi_base_t    spix = hspi->instance;

	while ((spi_get_status(hspi->instance) & flag) != state) {
		if (timeout != HAL_MAX_DELAY) {
			// TODO: timeout management
			if (tmp_timeout-- == 0U) {
				spi_disable_it(spix, SPI_IT_TX_FIFO_EMPTY | SPI_IT_TX_FIFO_OVERFLOW | SPI_IT_SPI_TX_ERR);
				if ((hspi->init.mode == SPI_MODE_MASTER) && (hspi->init.tmod == SPI_TMOD_RX_ONLY)) {
					spi_disable(spix);
				}

				hspi->state = HAL_SPI_STATE_READY;
				__HAL_UNLOCK(hspi);
				return HAL_TIMEOUT;
			}
		}
	}

	return HAL_OK;
}

/**
 * @brief
 *
 * @param hspi
 */
static void spi_tx_isr_8bit(spi_handle_t *hspi)
{
	spi_base_t spix = hspi->instance;

	spi_write_data(spix, *((uint8_t *)hspi->tx_buf_ptr));
	hspi->tx_buf_ptr += sizeof(uint8_t);
	hspi->tx_xfer_count--;

	if (hspi->tx_xfer_count == 0U) {
		if (hspi->init.tmod == SPI_TMOD_TX_RX) {
			// Full-duplex
			spi_disable_it_tx_fifo_empty(spix);
			spi_disable_it_tx_fifo_overflow(spix);

			if (hspi->rx_xfer_count == 0U) {
				spi_close_txrx_isr(hspi);
			}
		} else {
			// Half-duplex
			spi_close_tx_isr(hspi);
		}
	}
}

static void spi_tx_isr_16bit(spi_handle_t *hspi)
{
	spi_base_t spix = hspi->instance;

	spi_write_data(spix, *((uint16_t *)hspi->tx_buf_ptr));
	hspi->tx_buf_ptr += sizeof(uint16_t);
	hspi->tx_xfer_count--;

	if (hspi->tx_xfer_count == 0U) {
		if (hspi->init.tmod == SPI_TMOD_TX_RX) {
			// Full-duplex
			spi_disable_it_tx_fifo_empty(spix);
			spi_disable_it_tx_fifo_overflow(spix);

			if (hspi->rx_xfer_count == 0U) {
				spi_close_txrx_isr(hspi);
			}
		} else {
			// Half-duplex
			spi_close_tx_isr(hspi);
		}
	}
}

static void spi_tx_isr_32bit(spi_handle_t *hspi)
{
	spi_base_t spix = hspi->instance;

	spi_write_data(spix, *((uint32_t *)hspi->tx_buf_ptr));
	hspi->tx_buf_ptr += sizeof(uint32_t);
	hspi->tx_xfer_count--;

	if (hspi->tx_xfer_count == 0U) {
		if (hspi->init.tmod == SPI_TMOD_TX_RX) {
			// Full-duplex
			spi_disable_it_tx_fifo_empty(spix);
			spi_disable_it_tx_fifo_overflow(spix);

			if (hspi->rx_xfer_count == 0U) {
				spi_close_txrx_isr(hspi);
			}
		} else {
			// Ha-duplex
			spi_close_tx_isr(hspi);
		}
	}
}

/**
 * @brief Manage the receive 8bit in interrupt context.
 *
 * @param hspi
 */
static void spi_rx_isr_8bit(spi_handle_t *hspi)
{
	spi_base_t spix = hspi->instance;

	*((uint8_t *)hspi->rx_buf_ptr) = spi_read_data(spix);
	hspi->rx_buf_ptr += sizeof(uint8_t);
	hspi->rx_xfer_count--;

	if (hspi->rx_xfer_count == 0U) {
		if (hspi->init.tmod == SPI_TMOD_TX_RX) {
			// full-duplex
			spi_disable_it_rx_fifo_full(spix);
			spi_disable_it_rx_fifo_overflow(spix);
			spi_disable_it_rx_fifo_underflow(spix);

			if (hspi->tx_xfer_count == 0U) {
				spi_close_txrx_isr(hspi);
			}
		} else {
			// half-duplex
			spi_close_rx_isr(hspi);
		}
	}
}

static void spi_rx_isr_16bit(spi_handle_t *hspi)
{
	spi_base_t spix = hspi->instance;

	*((uint16_t *)hspi->rx_buf_ptr) = (uint16_t)spi_read_data(spix);
	hspi->rx_buf_ptr += sizeof(uint16_t);
	hspi->rx_xfer_count--;

	if (hspi->rx_xfer_count == 0U) {
		if (hspi->init.tmod == SPI_TMOD_TX_RX) {
			// Full-duplex
			spi_disable_it(spix, SPI_IT_RX_FIFO_FULL);
			spi_disable_it(spix, SPI_IT_RX_FIFO_OVERFLOW);
			spi_disable_it(spix, SPI_IT_RX_FIFO_UNDERFLOW);

			if (hspi->tx_xfer_count == 0U) {
				spi_close_txrx_isr(hspi);
			}
		} else {
			// Half-duplex
			spi_close_rx_isr(hspi);
		}
	}
}

static void spi_rx_isr_32bit(spi_handle_t *hspi)
{
	spi_base_t spix = hspi->instance;

	*((uint32_t *)hspi->rx_buf_ptr) = (uint32_t)spi_read_data(spix);
	hspi->rx_buf_ptr += sizeof(uint32_t);
	hspi->rx_xfer_count--;

	if (hspi->rx_xfer_count == 0U) {
		if (hspi->init.tmod == SPI_TMOD_TX_RX) {
			// full-duplex
			spi_disable_it_rx_fifo_full(spix);
			spi_disable_it_rx_fifo_overflow(spix);
			spi_disable_it_rx_fifo_underflow(spix);

			if (hspi->tx_xfer_count == 0U) {
				spi_close_txrx_isr(hspi);
			}
		} else {
			// half-duplex
			spi_close_rx_isr(hspi);
		}
	}
}

static void spi_close_tx_isr(spi_handle_t *hspi)
{
	spi_base_t spix = hspi->instance;
	uint32_t   tick_start = 0u;
	__IO uint32_t count = SPI_DEFAULT_TIMEOUT;

	do {
		if (count == 0U) {
			hspi->error_code |= HAL_SPI_ERROR_FLAG;
			break;
		}
		count--;
	} while (spi_get_status_tx_fifo_empty(spix) == RESET);

	spi_disable_it(spix, SPI_IT_TX_FIFO_EMPTY | SPI_IT_TX_FIFO_OVERFLOW | SPI_IT_SPI_TX_ERR);

	if (spi_end_rxtx_transaction(hspi, SPI_DEFAULT_TIMEOUT, tick_start) != HAL_OK) {
		hspi->error_code |= HAL_SPI_ERROR_FLAG;
	}

	hspi->state = HAL_SPI_STATE_READY;
	if (hspi->error_code != HAL_SPI_ERROR_NONE) {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
		hspi->error_callback(hspi);
#else
		hal_spi_error_callback(hspi);
#endif
	} else {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
		hspi->tx_cplt_callback(hspi);
#else
		hal_spi_tx_cplt_callback(hspi);
#endif
	}
}

/**
 * @brief Handle the end of the RX transaction.
 *
 * @param hspi
 */
static void spi_close_rx_isr(spi_handle_t *hspi)
{
	spi_base_t spix = hspi->instance;
	uint32_t   tick_start = 0;

	spi_disable_it_rx_fifo_full(spix);
	spi_disable_it_rx_fifo_overflow(spix);
	spi_disable_it_rx_fifo_underflow(spix);

	if (spi_end_rx_transaction(hspi, SPI_DEFAULT_TIMEOUT, tick_start) != HAL_OK) {
		hspi->error_code |= HAL_SPI_ERROR_FLAG;
	}

	hspi->state = HAL_SPI_STATE_READY;

	if (hspi->error_code == HAL_SPI_ERROR_NONE) {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
		hspi->rx_cplt_callback(hspi);
#else
		hal_spi_rx_cplt_callback(hspi);
#endif
	} else {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
		hspi->error_callback(hspi);
#else
		hal_spi_error_callback(hspi);
#endif
	}
}

/**
 * @brief Handle the end of the TX && RX transaction.
 *
 * @param hspi
 */
static void spi_close_txrx_isr(spi_handle_t *hspi)
{
	spi_base_t spix = hspi->instance;
	uint32_t tick_start = 0u;
	__IO uint32_t count = SPI_DEFAULT_TIMEOUT;

	spi_disable_it_spi_tx_err(spix);

	do {
		if (count == 0U) {
			hspi->error_code |= HAL_SPI_ERROR_FLAG;
			break;
		}
		count--;
	} while (spi_get_status_tx_fifo_empty(spix) == RESET);

	if (spi_end_rxtx_transaction(hspi, SPI_DEFAULT_TIMEOUT, tick_start) != HAL_OK) {
		hspi->error_code |= HAL_SPI_ERROR_FLAG;
	}

	if (hspi->init.tmod == SPI_TMOD_TX_RX) {
		spi_clear_it_tx_fifo_err(spix);
	}

	if (hspi->error_code == HAL_SPI_ERROR_NONE) {
		if (hspi->state == HAL_SPI_STATE_BUSY_RX) {
			hspi->state = HAL_SPI_STATE_READY;
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
				hspi->rx_cplt_callback(hspi);
#else
				hal_spi_rx_cplt_callback(hspi);
#endif
		} else {
			hspi->state = HAL_SPI_STATE_READY;
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
			hspi->txrx_cplt_callback(hspi);
#else
			hal_spi_txrx_cplt_callback(hspi);
#endif
		}
	} else {
		hspi->state = HAL_SPI_STATE_READY;
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
		hspi->error_callback(hspi);
#else
		hal_spi_error_callback(hspi);
#endif
	}
}

/**
 * @brief
 *
 * @param hspi
 * @param timeout
 * @param tick_start
 * @return hal_status_t
 */
static hal_status_t spi_end_rx_transaction(spi_handle_t *hspi, uint32_t timeout, uint32_t tick_start)
{
	spi_base_t spix = hspi->instance;

	if ((hspi->init.mode == SPI_MODE_MASTER) && (hspi->init.tmod == SPI_TMOD_RX_ONLY)) {
		// Master, RX_ONLY
		spi_disable(spix);
	}

	if (hspi->init.mode == SPI_MODE_MASTER) {
		// Master
		if (hspi->init.tmod != SPI_TMOD_RX_ONLY) {
			if (spi_wait_flags_state_until_timeout(hspi, SPI_STATUS_BUSY, RESET, timeout, tick_start) != HAL_OK) {
				hspi->error_code |= HAL_SPI_ERROR_FLAG;
				return HAL_TIMEOUT;
			}
		} else {
			if (spi_wait_flags_state_until_timeout(hspi, SPI_STATUS_RX_FIFO_NOT_EMPTY, RESET, timeout, tick_start) != HAL_OK) {
				hspi->error_code |= HAL_SPI_ERROR_FLAG;
				return HAL_TIMEOUT;
			}
		}
	} else {
		// Slave
		if (spi_wait_flags_state_until_timeout(hspi, SPI_STATUS_RX_FIFO_NOT_EMPTY, RESET, timeout, tick_start) != HAL_OK) {
			hspi->error_code |= HAL_SPI_ERROR_FLAG;
			return HAL_TIMEOUT;
		}
	}

	return HAL_OK;
}

static hal_status_t spi_end_rxtx_transaction(spi_handle_t *hspi, uint32_t timeout, uint32_t tick_start)
{
	__IO uint32_t count = SPI_BSY_FLAG_WORKAROUND_TIMEOUT;

	if (SPI_MODE_MASTER == hspi->init.mode) {
		if (spi_wait_flags_state_until_timeout(hspi, SPI_STATUS_BUSY, RESET, timeout, tick_start) != HAL_OK) {
			hspi->error_code |= HAL_SPI_ERROR_FLAG;
			return HAL_TIMEOUT;
		}
	} else {
		do {
			if (0U == count) {
				break;
			}
			count--;
		} while (spi_get_status_busy(hspi->instance) != RESET);
	}

	return HAL_OK;
}
