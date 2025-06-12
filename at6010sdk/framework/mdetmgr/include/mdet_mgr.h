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

#ifndef __MDET_MGR_H__
#define __MDET_MGR_H__

#ifdef __cpluscplus
extern "C" {
#endif

#include "at_types.h"
#include "mdet_fmcw.h"
/**
 * Motion detection/Breath detection/Micro-detection bit flag position.
 */
#define MDET_BIT_POS_MOTION_DET        0
#define MDET_BIT_POS_BHR_DET           1
#define MDET_BIT_POS_BHR_DET_RT        2
#define MDET_BIT_POS_MICRO_DET         3

#define MDET_MOTION_DET_MOD    0
#define MDET_MICRO_DET_MOD     1
#define MDET_BRTH_DET_MOD      2

typedef void (*mdet_data_cb)(int *buf, u16 size);

void mdet_init(void *p);
bool mdet_filter_proc(s16 *adc_val);
void mdet_proc_reset(u8 is_hard);
void mdet_stop_proc_data(void);
void mdet_start_proc_data(void);
void mdet_set_data_cb(mdet_data_cb hdl);
u8 mdet_proc(void);
void mdet_set_delta(u16 delta);
void mdet_set_undet_delta(u16 delta);
u8 mdet_is_hw_support_bhr(void);
void mdet_get_det_info(fmcw_det_info_t *inf);
void mdet_get_det_pwr(u32 *addr);
void mdet_set_sensitivity(u8 mode, u8 level);
void mdet_set_bhr_delta(u16 delta);
void mdet_set_bhr_undet_delta(u16 delta);
void mdet_set_microdet_delta(u16 delta);
void mdet_set_micro_undet_delta(u16 delta);
void mdet_skip_num(u16 num);
void mdet_pod_param_set(pod_cfg_t *cfg);
void mdet_algo_det_log_onoff(u8 en);
void mdet_set_det_result_output(u8 val);
void mdet_get_det_auxinfo(void *inf);
#ifdef __cpluscplus
}
#endif

#endif

