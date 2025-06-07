/**
 *******************************************************************************
 * @file    hal_radar.h
 * @author  Airtouch Software Team
 * @brief   Firmware Software version
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

#ifndef __HAL_RADAR_H__
#define __HAL_RADAR_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_RADAR RADAR
 * @brief RADAR HAL Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "radar.h"
#include "radar_adc.h"
#include "radar_frame.h"

/* Data Types ----------------------------------------------------------------*/

/**
 * @brief Radar Parameter Type Definition
 */
typedef enum {
	RADAR_PRM_RX_NUM            = 0,
	RADAR_PRM_HPF1_GAIN         = 1,
	RADAR_PRM_HPF1_FREQ         = 2,
	RADAR_PRM_HPF2_GAIN         = 3,
	RADAR_PRM_HPF2_FREQ         = 4,
	RADAR_PRM_LPF_GAIN          = 5,
	RADAR_PRM_LPF_FREQ          = 6,
	RADAR_PRM_FRM_REP_TIMES     = 7,
	RADAR_PRM_FRM_INTL_US       = 8,
	RADAR_PRM_SUBFRM_NUM        = 9,
	RADAR_PRM_ADC_RATE          = 10,
	RADAR_PRM_SAMPLE_PER_CHIRP  = 11,
	RADAR_PRM_SAMPLE_DELAY      = 12,
	RADAR_PRM_FILTER_DELAY      = 13,
	RADAR_PRM_INTF_MODE         = 14,
	RADAR_PRM_RX1_PHASE         = 15,
	RADAR_PRM_RX2_PHASE         = 16,
	RADAR_PRM_COUNT_DIFF_THA    = 17,
	RADAR_PRM_COUNT_DIFF_THB    = 18,
	RADAR_PRM_COUNT_VALUE_THA   = 19,
	RADAR_PRM_ELIM_VALUE_THA    = 20,
	RADAR_PRM_ELIM_DIFF_THB     = 21,
	RADAR_PRM_ELIM_VALUE_THC    = 22,
	RADAR_PRM_ELIM_VALUE_THD    = 23,
	RADAR_PRM_DMA_EN            = 24,
	RADAR_PRM_DATA_SEL          = 25,
	RADAR_PRM_FILTER_EN         = 26
} radar_param_t;

/**
 * @brief RFE profile structure definition
 */
typedef struct {
	tx_gain_t tx_gain;
	rx_cfg_t rx_cfg;
} radar_rfe_profile_t;

/**
 * @brief Chirp profile structure definition
 */
typedef struct {
	u32 start_freq_mhz;     /**< Sweep start frequency                        */
	u32 chirp_num;          /**< Number of chirps                             */
	u32 rise_step_time;     /**< Settling time for each rise step, unit 12.5ns */
	u32 rise_step_freq_khz; /**< Step frequency of rising */
	u32 rise_step_num;      /**< Step number of rising */
	u32 gap_time; /**< Gap time between sweep end and next sweep start, unit 12.5ns. chirp time = rise_step_time *
	                 rise_step_num + gap_time. The minimum gap_time is 880 when sweep band is no more than 2GHz and
	                 1010 when sweep band is greater than 2GHz */
} radar_chirp_profile_t;

/**
 * @brief Sub frame profile structure definition
 */
typedef struct {
	u8 chirp_start_id;
	u8 chirp_end_id;
	u8 tx_start_id;
	u8 tx_end_id;
	u16 repeat_times;
	u32 subfrm_delay_us;
} radar_subframe_profile_t;

/**
 * @brief Frame profile structure definition
 */
typedef struct {
	u32 repeat_times;
	u32 interval_us; /**< value range: 204 ~ 13421568 */
	u8 subfrm_id[SUB_FRAME_NUM];
	u8 subfrm_num;
} radar_frame_profile_t;

/**
 * @brief Filter profile structure definition
 */
typedef struct {
	radar_adc_rate_t rate;
	u16 chirp_size;   /**< Shall be same as dsp                                 */
	u8 sample_delay;  /**< To reduce the influence of group delay, Unit:16*12.5 */
	u8 filter_delay;  /**< Unit: 12.5                                           */
} radar_filter_profile_t;

/**
 * @brief Filter DMA profile structure definition
 */
typedef struct {
	u8 dma_en;
	dma_data_t data_sel;
	u8 filter_en;
} radar_filter_dma_profile_t;

/**
 * @brief Radar profile structure definition
 */
typedef struct {
	radar_rfe_profile_t rfe; /**< RFE Profile                                 */

	radar_chirp_profile_t chirp[CHIRP_PROFILE_NUM]; /**< WGEN - Chirp Profile */

	radar_subframe_profile_t subfrm[SUB_FRAME_NUM]; /**< WGEN - Sub-Frame Profile */

	radar_frame_profile_t frame;   /**< WGEN - Frame Profile                  */

	radar_filter_profile_t filter; /**< DSP - Filter Profile                  */
} radar_profile_t;

/* Function Declarations -----------------------------------------------------*/

u8 hal_radar_init(const radar_profile_t *profile);
u8 hal_radar_burst_init(radar_rx_num_t rx_num);
u8 hal_radar_deinit(void);
void hal_radar_stop(void);
void hal_radar_start(void);

void hal_radar_lp_start(void);
void hal_radar_lp_stop(void);

void hal_radar_ulp_start(void);
void hal_radar_ulp_stop(void);

u8 hal_radar_param_set(radar_param_t param, u32 value);
u8 hal_radar_param_get(radar_param_t param, u32 *value);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_RADAR_H__ */
