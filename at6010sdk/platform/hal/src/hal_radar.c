/**
 *******************************************************************************
 * @file    hal_radar.c
 * @author  Airtouch Software Team
 * @brief   Radar HAL driver
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


#include "hal_radar.h"
#include "at6010.h"
#include "clock.h"
#include "hal_timer.h"

typedef struct {
	void (*setter)(u32 value);
	u32 (*getter)(void);
} radar_param_func_t;

/* CAUTION: map this func to radar_param list, do not change sequence! */
static const radar_param_func_t radar_param_func[] = {
        {radar_ant_rx_num_set,         radar_ant_rx_num_get        },
        {radar_param_hpf1_gain_set,    radar_param_hpf1_gain_get   },
        {radar_param_hpf1_freq_set,    radar_param_hpf1_freq_get   },
        {radar_param_hpf2_gain_set,    radar_param_hpf2_gain_get   },
        {radar_param_hpf2_freq_set,    radar_param_hpf2_freq_get   },
        {radar_param_lpf_gain_set,     radar_param_lpf_gain_get    },
        {radar_param_lpf_freq_set,     radar_param_lpf_freq_get    },
        {radar_frame_repeat_times_set, radar_frame_repeat_times_get},
        {radar_frame_interval_set,     radar_frame_interval_get    }
};

static bool is_rfe_1g_range(const radar_profile_t *profile)
{
	bool ret = TRUE;
	for (u8 i = 0; i < CHIRP_PROFILE_NUM; i++) {
		if ((profile->chirp[i].start_freq_mhz > 60000U) ||
		    (((profile->chirp[i].start_freq_mhz * 1000U) +
		      (profile->chirp[i].rise_step_freq_khz * profile->chirp[i].rise_step_num)) > 60000000U)) {
			ret = FALSE;
			break;
		}
	}
	return ret;
}

__STATIC_INLINE void get_rfe_cfg(const radar_profile_t *profile, rfe_cfg_t *rfe_cfg)
{
	rfe_cfg->mode = (is_rfe_1g_range(profile) == TRUE) ? RFE_BW_1G : RFE_BW_5G;
	rfe_cfg->tx_gain = profile->rfe.tx_gain;
	rfe_cfg->rx_cfg = profile->rfe.rx_cfg;
}

__STATIC_INLINE void get_tx_cfg(u8 idx, const radar_profile_t *profile, tx_profile_t *tx_cfg)
{
	/* The tx profile parameters are not accessible, set fixed value. */
	(void)idx;
	(void)profile;
	tx_cfg->chirp_num = 1;
	tx_cfg->tx_bpm = TX_BPM_PHASE_0_DEGREE;
	tx_cfg->tx_phase = TX_PHASE_START_0_DEGREE;
}

__STATIC_INLINE void get_filter_cfg(const radar_profile_t *profile, radar_filter_cfg_t *filter_cfg)
{
	filter_cfg->rate = profile->filter.rate;
	filter_cfg->rx_num = profile->rfe.rx_cfg.rx_num;
	filter_cfg->chirp_size = profile->filter.chirp_size;
	filter_cfg->sample_delay = profile->filter.sample_delay;
	filter_cfg->filter_delay = profile->filter.filter_delay;

	/* The intf parameters are not accessible, set fixed value. */
	filter_cfg->intf_mode = NOISE_INTF_NA;
	filter_cfg->rx1_phase = RADAR_RX_PHASE_0_DEGREE;
	filter_cfg->rx2_phase = RADAR_RX_PHASE_0_DEGREE;
	filter_cfg->intf_para.count_diff_thA = 120;
	filter_cfg->intf_para.count_diff_thB = 120;
	filter_cfg->intf_para.count_value_thA = 4000;
	filter_cfg->intf_para.elim_value_thA = 2000;
	filter_cfg->intf_para.elim_diff_thB = 1000;
	filter_cfg->intf_para.elim_value_thC = 8000;
	filter_cfg->intf_para.elim_value_thD = 2000;
}

__STATIC_INLINE void get_chirp_cfg(u8 idx, const radar_profile_t *profile, chirp_profile_t *chirp_cfg)
{
	chirp_cfg->start_freq_mhz = profile->chirp[idx].start_freq_mhz;
	chirp_cfg->chirp_num = profile->chirp[idx].chirp_num;
	chirp_cfg->rise_step_time = profile->chirp[idx].rise_step_time;
	chirp_cfg->rise_step_freq_khz = profile->chirp[idx].rise_step_freq_khz;
	chirp_cfg->rise_step_num = profile->chirp[idx].rise_step_num;

	/* NOTE: Default half fall, linear fall is deprecated */
	/* The fall and jump settings are based on the appropriate values derived from tests */
	chirp_cfg->fall_step_freq_khz = 0U;
	chirp_cfg->fall_mode = FALL_MODE_HALF;
	chirp_cfg->fall_step_time = 80U;
	chirp_cfg->fall_step_num = 6U;

	if ((chirp_cfg->rise_step_num * chirp_cfg->rise_step_freq_khz) > 2000000U) {
		chirp_cfg->jump_to_freq_mhz = chirp_cfg->start_freq_mhz + 2000U;
		chirp_cfg->jump_wait_time = 130U;
	} else {
		chirp_cfg->jump_to_freq_mhz = 0U;
		chirp_cfg->jump_wait_time = 0U;
	}
	u32 fall_time = (chirp_cfg->fall_step_time * chirp_cfg->fall_step_num) + chirp_cfg->jump_wait_time;
	u32 gap_time_min = fall_time + IDLE_TIME_TICK_MIN;
	if (0U == profile->chirp[idx].gap_time) {
		chirp_cfg->idle_time = 0U;
	} else if (profile->chirp[idx].gap_time > gap_time_min) {
		chirp_cfg->idle_time = profile->chirp[idx].gap_time - fall_time;
	} else {
		chirp_cfg->idle_time = IDLE_TIME_TICK_MIN;
	}
}

__STATIC_INLINE void get_subfrm_cfg(u8 idx, const radar_profile_t *profile, subframe_profile_t *subfrm_cfg)
{
	subfrm_cfg->chirp_start_id = profile->subfrm[idx].chirp_start_id;
	subfrm_cfg->chirp_end_id = profile->subfrm[idx].chirp_end_id;
	subfrm_cfg->tx_start_id = profile->subfrm[idx].tx_start_id;
	subfrm_cfg->tx_end_id = profile->subfrm[idx].tx_end_id;
	subfrm_cfg->repeat_times = profile->subfrm[idx].repeat_times;
	subfrm_cfg->subfrm_delay_us = profile->subfrm[idx].subfrm_delay_us;

	/* The bpm parameters are not accessible, set fixed value. */
	subfrm_cfg->bpm_en = 0U;
	subfrm_cfg->bpm_rst = 0U;
	subfrm_cfg->bpm_seed = 0U;
}

__STATIC_INLINE void get_frame_cfg(const radar_profile_t *profile, frame_cfg_t *frm_cfg)
{
	u32 i;

	frm_cfg->repeat_times = profile->frame.repeat_times;
	frm_cfg->interval_us = profile->frame.interval_us;
	for (i = 0U; i < SUB_FRAME_NUM; i++) {
		frm_cfg->subfrm_id[i] = profile->frame.subfrm_id[i];
	}
	frm_cfg->subfrm_num = profile->frame.subfrm_num;

	/* The bpm parameters are not accessible, set fixed value. */
	frm_cfg->bpm_seed = 0U;
}

__STATIC_INLINE u8 get_faston_time(u32 idle_time)
{
	u8 faston_time;
	idle_time >>= 3; /* convert unit from 12.ns to 0.1us */
	if (idle_time < IDLE_TIME_MIN) {
		faston_time = FASTON_TIME_MIN;
	} else if (idle_time > (0xFFU + IF_SETUP_TIME)) {
		faston_time = 0xFFU;
	} else {
		faston_time = (u8)(idle_time - IF_SETUP_TIME);
	}
	return faston_time;
}

u8 hal_radar_init(const radar_profile_t *profile)
{
	rfe_cfg_t rfe_cfg;
	tx_profile_t tx_cfg;
	radar_filter_cfg_t filter_cfg;
	chirp_profile_t chirp_cfg;
	subframe_profile_t subfrm_cfg;
	frame_cfg_t frm_cfg;
	radar_filter_dma_cfg_t filter_dma_cfg = {0};
	u8 i;
	u32 min_chirp_idle = 0xFFFFFFFFU;

	sys_clk_enable_radar();

	get_rfe_cfg(profile, &rfe_cfg);
	radar_init(&rfe_cfg);

	for (i = 0U; i < TX_PROFILE_NUM; i++) {
		get_tx_cfg(i, profile, &tx_cfg);
		radar_tx_profile_config(i, &tx_cfg);
	}

	for (i = 0U; i < CHIRP_PROFILE_NUM; i++) {
		get_chirp_cfg(i, profile, &chirp_cfg);
		radar_chirp_profile_config(i, &chirp_cfg);
		min_chirp_idle = ((chirp_cfg.idle_time > 0U) && (chirp_cfg.idle_time < min_chirp_idle))
		                         ? chirp_cfg.idle_time
		                         : min_chirp_idle;
	}

	for (i = 0U; i < SUB_FRAME_NUM; i++) {
		get_subfrm_cfg(i, profile, &subfrm_cfg);
		radar_subframe_profile_config(i, &subfrm_cfg);
	}

	get_frame_cfg(profile, &frm_cfg);
	radar_frame_config(&frm_cfg);

	radar_faston_time_set(get_faston_time(min_chirp_idle));

	get_filter_cfg(profile, &filter_cfg);
	radar_filter_cfg(&filter_cfg);

	filter_dma_cfg.dma_en = 0U;
	radar_filter_dma_cfg(&filter_dma_cfg);

	return E_OK;
}

u8 hal_radar_burst_init(radar_rx_num_t rx_num)
{
	sys_clk_enable_radar();
	radar_burst_init(rx_num);
	return E_OK;
}

void hal_radar_stop(void)
{
	radar_frame_stop();
	radar_rf_pwr_off();
}

void hal_radar_start(void)
{
	radar_rf_pwr_on();
	radar_frame_start();
}

u8 hal_radar_deinit(void)
{
	sys_clk_disable_radar();
	return E_OK;
}

void hal_radar_lp_start(void)
{
	timer_id_t timer_id;
	timer_cfg_t timer_cfg = {0U};

	timer_id = radar_lp_timer_get();

	if(timer_id == TIMER_ID_INVALID) {
		(void)hal_timer_get_free(&timer_id);
		timer_cfg.source = TIMER_SOURCE_SYSTEM;
		timer_cfg.value_mode = TIMER_VALUE_DEC;
		timer_cfg.start = 0U;
		timer_cfg.irq_en = 1U;
		hal_timer_init(timer_id, &timer_cfg);
	}

	radar_lp_init((u8)timer_id);
}

void hal_radar_lp_stop(void)
{
	radar_lp_pause();
}

void hal_radar_ulp_start(void)
{
	radar_ulp_init();
}

void hal_radar_ulp_stop(void)
{
	radar_ulp_deinit();
}

u8 hal_radar_param_set(radar_param_t param, u32 value)
{
	u8 ret = E_OK;

	if (NULL != radar_param_func[param].setter) {
		radar_param_func[param].setter(value);
	} else {
		ret = E_ACCES;
	}

	return ret;
}

u8 hal_radar_param_get(radar_param_t param, u32 *value)
{
	u8 ret = E_OK;

	if (NULL != radar_param_func[param].getter) {
		*value = radar_param_func[param].getter();
	} else {
		ret = E_ACCES;
	}

	return ret;
}
