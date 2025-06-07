/**
 *******************************************************************************
 * @file    radar.h
 * @author  Airtouch Software Team
 * @brief   Radar Low-Level Driver Header
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

#ifndef __RADAR_H__
#define __RADAR_H__

#ifdef __cpluscplus
extern "C" {
#endif

#include "at_types.h"

/**
 * @addtogroup AT6010_LL_DRIVER
 * @{
 */

/**
 * @defgroup RADAR_LL Radar
 * @brief Radar Low-Level Driver
 * @{
 */

/** @defgroup HPF1_GAIN_SEL HPF1 gain select
 * @{
 */
#define HPF1_GAIN_MAX_MINUS_7dB 0U //19dB
#define HPF1_GAIN_MAX_MINUS_6dB 1U //20dB
#define HPF1_GAIN_MAX_MINUS_5dB 2U //21dB
#define HPF1_GAIN_MAX_MINUS_4dB 3U //22dB
#define HPF1_GAIN_MAX_MINUS_3dB 4U //23dB
#define HPF1_GAIN_MAX_MINUS_2dB 5U //24dB
#define HPF1_GAIN_MAX_MINUS_1dB 6U //25dB
#define HPF1_GAIN_MAX           7U //26dB
/** @} */

/** @defgroup HPF_CORNER_FREQ_SEL HPF corner frequency select
 * @{
 */
#define HPF_CORNER_FREQ_43_75kHz 0U
#define HPF_CORNER_FREQ_87_5kHz  1U
#define HPF_CORNER_FREQ_175kHz   2U
#define HPF_CORNER_FREQ_350kHz   3U
#define HPF_CORNER_FREQ_700kHz   4U
/** @} */

/** @defgroup HPF2_GAIN_SEL HPF2 gain select
 * @{
 */
#define HPF2_GAIN_0dB  0U
#define HPF2_GAIN_3dB  1U
#define HPF2_GAIN_6dB  2U
#define HPF2_GAIN_9dB  3U
#define HPF2_GAIN_12dB 4U
#define HPF2_GAIN_15dB 5U
#define HPF2_GAIN_18dB 6U
/** @} */

/** @defgroup LPF_GAIN_SEL LPF gain select
 * @{
 */
#define LPF_GAIN_0dB  0U
#define LPF_GAIN_3dB  1U
#define LPF_GAIN_6dB  2U
#define LPF_GAIN_9dB  3U
#define LPF_GAIN_12dB 4U
/** @} */

/** @defgroup LPF_CORNER_FREQ_SEL LPF corner frequency select
 * @{
 */
#define LPF_CORNER_FREQ_2MHz 0U
#define LPF_CORNER_FREQ_1MHz 1U
/** @} */

/** @defgroup RFE tx gain level select
 * @{
 */
typedef enum {
	TX_GAIN_MAX,
	TX_GAIN_MAX_MINUS_1p5dB,
	TX_GAIN_MAX_MINUS_3dB,
	TX_GAIN_MAX_MINUS_6dB,
	TX_GAIN_QTY
} tx_gain_t;
/** @} */

/** @defgroup RFE rx antenna number select
 * @{
 */
#define RADAR_RX_NUM_1           0
#define RADAR_RX_NUM_2           1
typedef unsigned char            radar_rx_num_t;


typedef enum {
	DIR_UP,
	DIR_DOWN
} chirp_dir_t;

/** @defgroup RFE bandwidth mode select
 * @{
 */
typedef enum {
	RFE_BW_5G = 0,
	RFE_BW_1G = 1
} rfe_bw_t;
/** @} */

typedef struct {
	u8 gain;
	u8 corner_freq;
} channel_pf_cfg_t;

typedef struct {
	radar_rx_num_t rx_num;
	channel_pf_cfg_t hpf1;
	channel_pf_cfg_t hpf2;
	channel_pf_cfg_t lpf;
} rx_cfg_t;

typedef struct {
	rfe_bw_t mode;
	tx_gain_t tx_gain;
	rx_cfg_t rx_cfg;
} rfe_cfg_t;

void radar_init(const rfe_cfg_t *cfg);
void radar_burst_init(radar_rx_num_t rx_num);

void radar_lp_init(u8 timer_id);

u8 radar_lp_timer_get(void);


/**
 * @brief stop radar lp timer, this function must be called when radar is stopped in lp mode
 */
void radar_lp_pause(void);
/**
 * @brief resume radar lp timer, this function must be called when radar is started in lp mode
 */
void radar_lp_resume(void);

void radar_ulp_init(void);
void radar_ulp_deinit(void);

void radar_deinit(radar_rx_num_t rx_num);

void radar_tx_gain_set(tx_gain_t level);
void radar_rx_pf_cfg(const rx_cfg_t *cfg);
void radar_drv_get_uuid(u8 *uuid);

/**
 * @brief set afc frequency
 *
 * @param mode 1g/5g mode
 * @return u8 vco_cap
 */
u8 fmcw_afc_freq_set(rfe_bw_t mode);

/**
 * @brief get saved vco_cap after initialization
 *
 * @return u8 vco_cap
 */
u8 radar_afc_vco_cap_get(void);


void radar_tx_pa_on(void);
void radar_tx_pa_off(void);

void radar_rf_pwr_on(void);
void radar_rf_pwr_off(void);

void radar_isr(void);

/**
 * @defgroup RADAR_PARAM_CFG RADAR CONFIG
 * @brief Config functions for radar related parameters
 * @{
 */

u32 radar_ant_rx_num_get(void);
void radar_ant_rx_num_set(u32 rx_num);

u32 radar_param_hpf1_gain_get(void);
void radar_param_hpf1_gain_set(u32 value);
u32 radar_param_hpf1_freq_get(void);
void radar_param_hpf1_freq_set(u32 value);

u32 radar_param_hpf2_gain_get(void);
void radar_param_hpf2_gain_set(u32 value);
u32 radar_param_hpf2_freq_get(void);
void radar_param_hpf2_freq_set(u32 value);

u32 radar_param_lpf_gain_get(void);
void radar_param_lpf_gain_set(u32 value);
u32 radar_param_lpf_freq_get(void);
void radar_param_lpf_freq_set(u32 value);
/** @} */


/**
 * @defgroup RADAR_DEBUG RADAR DEBUG
 * @brief Debug functions for radar related configurations
 * @{
 */

void pll_1g_fast_mode_enable(void);
/** @} */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cpluscplus
}
#endif

#endif /* !__RADAR_H__ */
