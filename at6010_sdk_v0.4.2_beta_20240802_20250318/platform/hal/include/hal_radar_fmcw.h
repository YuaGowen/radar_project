/**
 *******************************************************************************
 * @file    hal_radar_fmcw.h
 * @author  Airtouch Software Team
 * @brief   Radar FMCW Mode
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

#ifndef __HAL_RADAR_FMCW_H__
#define __HAL_RADAR_FMCW_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_RADAR_FMCW RADAR FMCW
 * @brief RADAR FMCW HAL Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"

/* Data Types ----------------------------------------------------------------*/

enum fmcw_chirp_dir {
	CHIRP_DIR_UP,
	CHIRP_DIR_DN,
};
typedef struct {
	u32 chirp_freq_step;
	u16 chirp_freq_step_duration;
	u16 chirp_freq_step_count;
} chirp_sweep_param;

/**
 * @brief RX gain level
 */
typedef enum  {
	FMCW_ANA_RX_GAIN_LEVL_0 = 0, /**< max radar rx gain level, decrease 3db per level             */
	FMCW_ANA_RX_GAIN_LEVL_1,
	FMCW_ANA_RX_GAIN_LEVL_2,
	FMCW_ANA_RX_GAIN_LEVL_3,
	FMCW_ANA_RX_GAIN_LEVL_4,
	FMCW_ANA_RX_GAIN_LEVL_5,
	FMCW_ANA_RX_GAIN_LEVL_6,
	FMCW_ANA_RX_GAIN_LEVL_7,
	FMCW_ANA_RX_GAIN_LEVL_8,
	FMCW_ANA_RX_GAIN_LEVL_9,
	FMCW_ANA_RX_GAIN_LEVL_10,
	FMCW_ANA_RX_GAIN_LEVL_11,
	FMCW_ANA_RX_GAIN_LEVL_12,
	FMCW_ANA_RX_GAIN_LEVL_13,
	FMCW_ANA_RX_GAIN_LEVL_14,
	FMCW_ANA_RX_GAIN_LEVL_15,
	FMCW_ANA_RX_GAIN_LEVL_16,
	FMCW_ANA_RX_GAIN_LEVL_17,/**< min radar rx gain level                                         */
} fmcw_rx_gain_lev;

/**
 * @brief TX gain level
 */
typedef enum {
	FMCW_ANA_TX_GAIN_LEVL_0 = 0, /**< max radar tx gain level,decrease 6db per level              */
	FMCW_ANA_TX_GAIN_LEVL_1,
	FMCW_ANA_TX_GAIN_LEVL_2,
	FMCW_ANA_TX_GAIN_LEVL_3,     /**< min radar tx gain level                                     */
} fmcw_tx_gain_lev;

typedef enum {
	FMCW_IF_PORT_MODE_OUTPUT,
	FMCW_IF_PORT_MODE_INPUT ,
} fmcw_if_port_t;

/* rc hp theoretical value -3db value */
typedef enum {
	FMCW_RC_FLT_HP_9_HZ     ,    /* only for dc */
	FMCW_RC_FLT_HP_904_HZ   ,
	FMCW_RC_FLT_HP_1809_HZ  ,
	FMCW_RC_FLT_HP_3619_HZ  ,
	FMCW_RC_FLT_HP_7238_HZ  ,    /* measured value:-3db@3khz */
	FMCW_RC_FLT_HP_14476_HZ ,
	FMCW_RC_FLT_HP_28952_HZ ,
	FMCW_RC_FLT_HP_57903_HZ ,
} fmcw_flt_hp_cf_t;

typedef enum {
	FMCW_RC_FLT_LP_159236_HZ,
} fmcw_flt_lp_cf_t;

typedef struct {
	u16 chirp_sta_freq; /**< uint: MHZ                                        */
	u16 sweep_bandwidth;/**<  uint: MHZ                                       */
	chirp_sweep_param chirp_up_param;
	chirp_sweep_param chirp_down_param;
} fmcw_chirp_param;

typedef struct {
	u8  tx_gain_level;
	u8  rx_gain_level;
	u16 intvl_per_chirp;
	u16 intvl_per_frame;
	u16 chirps_per_frame;
	fmcw_chirp_param chirp_param;
} fmcw_obj_param;

/* Function Declarations -----------------------------------------------------*/

void hal_fmcw_init(void);
void hal_fmcw_set_cfg(fmcw_obj_param *param);
fmcw_obj_param* hal_fmcw_get_tgt_cfg(void);

void hal_fmcw_power_on(void);
void hal_fmcw_power_off(void);
void hal_fmcw_pa_on(void);
void hal_fmcw_pa_off(void);

u16  hal_fmcw_get_chirp_start_freq(void);
u16  hal_fmcw_get_chirp_sweep_band(void);
u16  hal_fmcw_get_chirps_per_frame(void);

void hal_fmcw_set_chirp_start_freq(u32 start_freq,u32 sweep_band);
void hal_fmcw_set_chirp_bw_time(u32 step_freq, u8 step_duration, u16 step_cnt, u8 dir);
void hal_fmcw_chirp_frame_setup(u32 frm_chirp_interval,u16 frm_chirp_cnt,u32 frm_interval);
void hal_fmcw_chirp_signal_trigger(void);
void hal_fmcw_chirp_signal_halt(void);

void hal_fmcw_set_rx_gain(fmcw_rx_gain_lev level);
void hal_fmcw_set_tx_gain(fmcw_tx_gain_lev level);

void hal_fmcw_ana_if_port_cfg(fmcw_if_port_t type);

void hal_fmcw_set_hp_rc_filter(fmcw_flt_hp_cf_t cf);
void hal_fmcw_set_lp_rc_filter(fmcw_flt_lp_cf_t cf);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_RADAR_FMCW_H__ */
