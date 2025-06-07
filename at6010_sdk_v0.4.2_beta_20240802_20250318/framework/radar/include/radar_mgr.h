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

#ifndef __RADAR_MGR_H__
#define __RADAR_MGR_H__

#include "at_types.h"
#include "mdet_mgr.h"

#define RAD_DIST_LV_MAX            32
#define RAD_BHR_DIST_LV_MAX        13
#define RAD_MICRO_DIST_LV_MAX      13

typedef struct {
	u16 delta_sta;
	u8 sensitivity;
	u16 delta_sto;
} mot_dist_para_t;

typedef struct {
	u16 delta_sta;
	u8 sensitivity;
	u16 delta_sto;
} bhr_dist_para_t;

typedef struct {
	u16 delta;
} micro_dist_para_t;

void radar_mgr_init(void);
void radar_mgr_skip_num(u16 num);
void radar_mgr_reset(void);
void radar_mgr_profile_set(void *profile_init_func);

void radar_mgr_rf_pwr_set(u8 value);
void radar_mgr_rf_fmcw_init(void);
void radar_mgr_rf_fmcw_freq_set(u32 freq);
void radar_mgr_rf_fmcw_tx_gain_set(u32 lev);

u8 radar_mgr_get_pwr_status(void);
u8 radar_mgr_is_bhr_support(void);
void radar_mgr_worker_run(void);
void radar_mgr_worker_halt(void);
s32 radar_mgr_mdet_info_get(fmcw_det_info_t *inf);

void radar_mgr_mdet_pwr_get(u32 *cap);

void radar_mgr_mdet_delta_set(u16 val);
u16 radar_mgr_mdet_delta_get(void);
void radar_mgr_mdet_undet_delta_set(u16 val);
u16 radar_mgr_mdet_undet_delta_get(void);
void radar_mgr_mdet_sen_lev_set(u8 lev);
u8 radar_mgr_mdet_sen_lev_get(void);
void radar_mgr_mdet_user_cfg_set(u8 lev);
u8 radar_mgr_mdet_user_cfg_get(void);

u16 radar_mgr_bhr_undet_delta_get(void);
void radar_mgr_bhr_undet_delta_set(u16 val);
void radar_mgr_bhr_delta_set(u16 val);
u16 radar_mgr_bhr_delta_get(void);
u8 radar_mgr_bhr_sen_lev_get(void);
void radar_mgr_bhr_sen_lev_set(u8 lev);
void radar_mgr_bhr_user_cfg_set(u8 lev);
u8 radar_mgr_bhr_user_cfg_get(void);
s32 radar_mgr_mdet_auxinfo_get(void *inf);

#endif

