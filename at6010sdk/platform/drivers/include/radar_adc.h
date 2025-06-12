/**
 * Copyright (c) 2022, Airtouching Intelligence Technology.
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
#ifndef __RADAR_FILTER_H__
#define __RADAR_FILTER_H__

#include "at_common.h"
#include "radar.h"

typedef enum {
	RADAR_ADC_RATE_5M,
	RADAR_ADC_RATE_10M
} radar_adc_rate_t;

typedef enum {
	NOISE_INTF_NA,
	NOISE_INTF_ZF,
	NOISE_INTF_LIMIT,
	NOISE_INTF_AVG
} noise_intf_mode_t;

typedef enum {
	RADAR_RX_PHASE_0_DEGREE,
	RADAR_RX_PHASE_180_DEGREE
} radar_rx_phase_t;

typedef enum {
	DMA_DATA_RAW,
	DMA_DATA_FILTER
} dma_data_t;

typedef enum {
	RADAR_RX_CH_1,
	RADAR_RX_CH_2
} radar_rx_ch_t;

typedef struct {
	u16 count_diff_thA;  /* For count Dual Side */
	u16 count_diff_thB;  /* For count Single Side */
	u16 count_value_thA; /* For count */
	u16 elim_value_thA;  /* For elimination, case 1 abs value */
	u16 elim_diff_thB;   /* for elimination, case 2 diff */
	u16 elim_value_thC;  /* For Elimination, case 2 abs */
	u16 elim_value_thD;  /* For NoiseLim, threshold */
} radar_intf_para_t;

typedef struct {
	radar_rx_num_t rx_num; /* Number of rx antennas */
	radar_adc_rate_t rate;
	noise_intf_mode_t intf_mode;
	u8 sample_delay; /* To reduce the influence of group delay, Unit 16*12.5 */
	u8 filter_delay; /* Unit 12.5 */
	u16 chirp_size;  /* Shall be same as dsp */
	radar_rx_phase_t rx1_phase;
	radar_rx_phase_t rx2_phase;
	radar_intf_para_t intf_para;
} radar_filter_cfg_t;

typedef struct {
	u8 dma_en;
	dma_data_t data_sel;
	radar_rx_num_t rx_num;
} radar_filter_dma_cfg_t;

typedef struct {
	u32 sum;
	u32 diff_max;
	u32 diff_acc;
	u32 vsum;
} intf_info_t;

typedef struct {
	u8 intf_dma_en;
	u32 intf_addr;
} radar_intf_cfg_t;

void radar_filter_cfg(const radar_filter_cfg_t *cfg);
void radar_filter_chirp_size_set(u16 chirp_size);
void radar_intf_mode_set(noise_intf_mode_t intf_mode);
void radar_intf_para_set(const radar_intf_para_t *paras);
void radar_filter_dma_cfg(const radar_filter_dma_cfg_t *cfg);
void radar_filter_intf_info_get(radar_rx_ch_t ch, intf_info_t *info);

void radar_dump_intf_status_cfg(const radar_intf_cfg_t *cfg);

void radar_sdm_enable(radar_rx_ch_t ch);
void radar_sdm_disable(radar_rx_ch_t ch);

/**
 * @brief This function is used for debug
 */
void radar_filter_disable(void);

#endif
