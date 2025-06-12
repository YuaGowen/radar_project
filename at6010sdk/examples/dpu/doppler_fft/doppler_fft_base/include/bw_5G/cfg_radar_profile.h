/**
 *******************************************************************************
 * @file    cfg_radar_profile.h
 * @author  Airtouch Software Team
 * @brief   Radar Profile Configurations for the current project
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

#ifndef __CFG_RADAR_PROFILE_H__
#define __CFG_RADAR_PROFILE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RADAR_PROFILE_CFG Radar Profile Configurations
 * @details Radar Profile Configurations
 * @{
 */

/**
 * @brief RFE tx gain level definition (@ref tx_gain_t)
 */
#define RADAR_PRF_TX_GAIN       TX_GAIN_MAX_MINUS_6dB

/**
 * @brief RFE rx antenna number definition (@ref radar_rx_num_t)
 */
#define RADAR_PRF_RFE_RX_NUM    RADAR_RX_NUM_2

/**
 * @brief RFE High-pass filter 1 configurations (@ref rx_cfg_t.hpf1)
 */
#define RADAR_PRF_RFE_HPF1                 \
{                                          \
	.gain  = HPF1_GAIN_MAX_MINUS_2dB,      \
	.corner_freq = HPF_CORNER_FREQ_350kHz  \
}

/**
 * @brief RFE High-pass filter 2 configurations (@ref rx_cfg_t.hpf2)
 */
#define RADAR_PRF_RFE_HPF2                 \
{                                          \
	.gain =  HPF2_GAIN_18dB,               \
	.corner_freq =  HPF_CORNER_FREQ_175kHz \
}

/**
 * @brief RFE Low-pass filter configurations (@ref rx_cfg_t.lpf)
 */
#define RADAR_PRF_RFE_LPF                  \
{                                          \
	.gain =  LPF_GAIN_12dB,                \
	.corner_freq =  LPF_CORNER_FREQ_2MHz   \
}

/**
 * @defgroup CHIRP_PROFILES 8 chirp profile configurations
 * @{
 */

/**
 * @brief chirp 0 profile, see @ref radar_chirp_profile_t
 */
#define RADAR_PRF_CHIRP_0       { 0U }

/**
 * @brief chirp 1 profile, see @ref radar_chirp_profile_t
 */
#define RADAR_PRF_CHIRP_1       { 0U }
/**
 * @brief chirp 2 profile, see @ref radar_chirp_profile_t
 */
#define RADAR_PRF_CHIRP_2       { 0U }
/**
 * @brief chirp 3 profile, see @ref radar_chirp_profile_t
 */
#define RADAR_PRF_CHIRP_3       { 0U }
/**
 * @brief chirp 4 profile, see @ref radar_chirp_profile_t
 */
#define RADAR_PRF_CHIRP_4       { 0U }

/**
 * @brief chirp 5 profile, see @ref radar_chirp_profile_t
 */
#define RADAR_PRF_CHIRP_5       { 0U }

/**
 * @brief chirp 6 profile, see @ref radar_chirp_profile_t
 */
#define RADAR_PRF_CHIRP_6       { 0U }

/**
 * @brief chirp 7 profile, see @ref radar_chirp_profile_t
 */
#define RADAR_PRF_CHIRP_7         \
{                                 \
	.start_freq_mhz     = 59000U, \
	.chirp_num          = 16U,    \
	.rise_step_time     = 1U,     \
	.rise_step_freq_khz = 1136U,  \
	.rise_step_num      = 4400U,  \
	.gap_time           = 1010U   \
}

/**
 * @}
 */


/**
 * @defgroup SUBFRAME_PROFILES 8 sub-frame profile configurations
 * @details 8 sub-frame profile configurations, each one is @ref radar_subframe_profile_t
 * @{
 */

/**
 * @brief sub frame 0 profile, see @ref radar_subframe_profile_t
 */
#define RADAR_PRF_SUBFRM_0      { 0U }

/**
 * @brief sub frame 1 profile, see @ref radar_subframe_profile_t
 */
#define RADAR_PRF_SUBFRM_1        \
{                                 \
	.chirp_start_id = 7U,         \
	.chirp_end_id   = 7U,         \
	.tx_start_id    = 2U,         \
	.tx_end_id      = 2U,         \
	.repeat_times   = 1U,         \
	.subfrm_delay_us= 100U        \
}

/**
 * @brief sub frame 2 profile, see @ref radar_subframe_profile_t
 */
#define RADAR_PRF_SUBFRM_2      { 0U }

/**
 * @brief sub frame 3 profile, see @ref radar_subframe_profile_t
 */
#define RADAR_PRF_SUBFRM_3      { 0U }

/**
 * @brief sub frame 4 profile, see @ref radar_subframe_profile_t
 */
#define RADAR_PRF_SUBFRM_4      { 0U }

/**
 * @brief sub frame 5 profile, see @ref radar_subframe_profile_t
 */
#define RADAR_PRF_SUBFRM_5      { 0U }

/**
 * @brief sub frame 6 profile, see @ref radar_subframe_profile_t
 */
#define RADAR_PRF_SUBFRM_6      { 0U }

/**
 * @brief sub frame 7 profile, see @ref radar_subframe_profile_t
 */
#define RADAR_PRF_SUBFRM_7      { 0U }

/**
 * @}
 */

/**
 * @brief Frame profile definition (see @ref radar_frame_profile_t)
 */
#define RADAR_PRF_FRAME                               \
{                                                     \
	.repeat_times = 1U,                           \
	.interval_us  = 1U,                          \
	.subfrm_id    = {1U, 0U, 0U, 0U, 0U, 0U, 0U, 0U}, \
	.subfrm_num   =  1U                               \
}

/**
 * @brief DSP filter profile definition (@ref radar_filter_profile_t)
 */
#define RADAR_PRF_FILTER                \
{                                       \
	.rate         = RADAR_ADC_RATE_10M, \
	.chirp_size   = 512,                \
	.sample_delay = 1,                 \
	.filter_delay = 152                 \
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__CFG_RADAR_PROFILE_H__ */
