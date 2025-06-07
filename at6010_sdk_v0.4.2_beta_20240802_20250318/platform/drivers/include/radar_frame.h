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
#ifndef __RADAR_FRAME_H__
#define __RADAR_FRAME_H__

#include "at_common.h"

#define CHIRP_PROFILE_NUM (8U)
#define TX_PROFILE_NUM    (8U)
#define SUB_FRAME_NUM     (8U)

#define FASTON_TIME_MIN    (20U)                             /**< unit 0.1us */
#define IF_SETUP_TIME      (30U)                             /**< unit 0.1us */
#define IDLE_TIME_MIN      (FASTON_TIME_MIN + IF_SETUP_TIME) /**< unit 0.1us */
#define IDLE_TIME_TICK_MIN (IDLE_TIME_MIN << 3)              /**< unit 12.5ns */

typedef enum {
	TX_BPM_PHASE_SHIFTER_RANDOM = 0,
	TX_BPM_PHASE_SHIFTER_NOT_RANDOM = 1,
	TX_BPM_PHASE_0_DEGREE = 2,
	TX_BPM_STOP = 3
} tx_bpm_ctrl_t;

typedef enum {
	TX_PHASE_START_0_DEGREE = 0,
	TX_PHASE_START_180_DEGREE = 1
} tx_phase_shift_start_t;

typedef enum {
	TRIGGER_MODE_INTERNAL,
	TRIGGER_MODE_EXTERNAL
} trigger_mode_t;

typedef enum {
	FALL_MODE_LINEAR = 0,
	FALL_MODE_HALF = 1
} fall_mode_t;

typedef struct {
	u32 start_freq_mhz;     /**< Start frequency */
	u32 chirp_num;          /**< Chirps number in this profile */
	u32 rise_step_time;     /**< Settling time for each rise step, unit 12.5ns */
	u32 rise_step_freq_khz; /**< Step frequency of rising */
	u32 rise_step_num;      /**< Step number of rising */
	fall_mode_t fall_mode;  /**< Falling mode selection */
	u32 fall_step_time;     /**< Settling time for each fall step, unit 12.5ns */
	u32 fall_step_freq_khz; /**< Step frequency of falling, invalid in FALL_MODE_HALF */
	u32 fall_step_num;      /**< Step number of falling */
	u32 idle_time;          /**< Idle time between chirps, unit 12.5ns */
	u32 jump_to_freq_mhz;   /**< Frequency jump to before falling, 0 or invalid value will disable jump */
	u32 jump_wait_time;     /**< Waiting time after jump, unit 12.5ns */
} chirp_profile_t;

typedef struct {
	u32 chirp_num;
	tx_bpm_ctrl_t tx_bpm;
	tx_phase_shift_start_t tx_phase;
} tx_profile_t;

typedef struct {
	u8 chirp_start_id;
	u8 chirp_end_id;
	u8 tx_start_id;
	u8 tx_end_id;
	u16 repeat_times;
	u8 bpm_en;
	u8 bpm_rst;
	u32 bpm_seed;
	u32 subfrm_delay_us;
} subframe_profile_t;

typedef struct {
	u32 repeat_times;
	u32 interval_us;
	u32 bpm_seed;
	u8 subfrm_id[SUB_FRAME_NUM];
	u8 subfrm_num;
} frame_cfg_t;

void radar_chirp_profile_config(u8 id, const chirp_profile_t *cfg);
void radar_tx_profile_config(u8 id, const tx_profile_t *cfg);
void radar_subframe_profile_config(u8 id, const subframe_profile_t *cfg);
void radar_frame_config(const frame_cfg_t *cfg);

void radar_chirp_profile_start_freq_set(u8 id, u32 start_freq_mhz);

void radar_frame_trigger_mode_set(trigger_mode_t mode);
void radar_frame_start(void);
void radar_frame_stop(void);

/**
 * @brief Set pa-on time before idle over.The larger value leads to later pa-on, which will reduce
 * some power consumption.
 *
 * @param ticks Delay time after falling over, unit 0.1 us
 */
void radar_faston_time_set(u8 ticks);

/**
 * @brief get current frame interval
 *
 * @return u32 frame interval(us)
 */
u32 radar_frame_interval_get(void);
void radar_frame_interval_set(u32 us);

void radar_frame_repeat_times_set(u32 value);
u32 radar_frame_repeat_times_get(void);

#endif
