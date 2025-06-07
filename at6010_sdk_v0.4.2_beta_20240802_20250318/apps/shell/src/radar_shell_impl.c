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

#include "radar_mgr.h"
#include "mdet_mgr.h"
#include <stdio.h>
#include "string.h"
#include "at_types.h"
#include "atlog.h"
#include "mdet_fmcw.h"
#include "target_config.h"
#include "osal.h"

#if defined(CFG_HTM)
#include "app_htm.h"
#endif

#define DBG_LOG_RADAR_MGR
#ifdef DBG_LOG_RADAR_MGR
#define dbg_radar_shell        logi
#else
#define dbg_radar_shell(...)
#endif

#ifdef CFG_RADAR_SHELL

/**
 * @brief radar set_pwr 0/1
 *
 * @param argc 2
 * @param argv argv[1] can be 0 (power off) or 1 (power on).
 * @return s32
 */
s32 radar_shell_power_set(size_t argc, char **argv)
{
	u8 onoff = 0;

	if (argc > 1) {
		onoff = str2num(argv[1]);
		logi("set power:%d\n\r", onoff);

		radar_mgr_rf_pwr_set(onoff);
	}

	return 0;
}

/**
 * @brief FMCW initialize with no arguments.
 *
 * @param argc
 * @param argv
 * @return s32
 */
s32 radar_shell_rf_fmcw_init(size_t argc, char **argv)
{
	radar_mgr_rf_fmcw_init();

	return 0;
}

/**
 * @brief Set Chirp start frenquency with unit MHz.
 *
 * @param argc
 * @param argv
 * @return s32
 */
s32 radar_shell_rf_fmcw_freq_set(size_t argc, char **argv)
{
	u32 freq = 0;

	if (argc > 1) {
		freq = str2num(argv[1]);
		logi("set freq:%dMHz\n\r", freq);

		radar_mgr_rf_fmcw_freq_set(freq);
	}

	return 0;
}

s32 radar_shell_rf_fmcw_tx_gain_set(size_t argc, char **argv)
{
	u32 level = 0;

	if (argc > 1) {
		level = str2num(argv[1]);
		logi("set tx level:%d\n\r", level);

		radar_mgr_rf_fmcw_tx_gain_set(level);
	}

	return 0;
}

s32 radar_shell_detect_stop(size_t argc, char **argv)
{
#ifdef CFG_APP_HTM
	app_htm_det_stop();
#else
	radar_mgr_worker_halt();

#endif
	return 0;
}

s32 radar_shell_detect_start(size_t argc, char **argv)
{
#ifdef CFG_APP_HTM
	app_htm_det_start();
#else
	radar_mgr_worker_run();

#endif
	return 0;
}

s32 radar_shell_pod_param_set(size_t argc, char **argv)
{
	pod_cfg_t cfg = {0};

	if (argc > 8) {
		cfg.num_frame_acc = str2num(argv[1]);
		cfg.rb_sta = str2num(argv[2]);
		cfg.rb_num = str2num(argv[3]);
		cfg.threshold_DB = str2num(argv[4]);
		cfg.ref_rb_cnt = str2num(argv[5]);
		cfg.ref_rb_threshold = str2num(argv[6]);
		cfg.report_delay = str2num(argv[7]);
		cfg.report_mode = str2num(argv[8]);

		mdet_pod_param_set(&cfg);
	} else {
		logw("pod param set failed !!\n\r");
	}

	return 0;
}

s32 radar_shell_pod_det_log_en(size_t argc, char **argv)
{
	u8 en = 0;

	if (argc > 1) {
		en = str2num(argv[1]);
		mdet_algo_det_log_onoff(en);
	} else {
		logw("pod log en failed !!\n\r");
	}

	return 0;
}

s32 radar_shell_user_md_set(size_t argc, char **argv)
{
	u8 lev = 0;

	if (argc > 1) {
		lev = str2num(argv[1]);
		logi("set mdet lev :%d\n\r", lev);

		radar_mgr_mdet_user_cfg_set(lev);
	}

	return 0;
}
s32 radar_shell_user_md_get(size_t argc, char **argv)
{
	u8 lev = 0;

	lev = radar_mgr_mdet_user_cfg_get();
	logi("get mdet lev:%d\n\r", lev);

	return 0;
}

s32 radar_shell_user_bhr_set(size_t argc, char **argv)
{
#ifdef CFG_PRESENCE_DET
	u8 lev = 0;

	if (argc > 1) {
		lev = str2num(argv[1]);
		logi("set breathe lev:%d\n\r", lev);

		radar_mgr_bhr_user_cfg_set(lev);
	}
#endif
	return 0;
}

s32 radar_shell_user_bhr_get(size_t argc, char **argv)
{
#ifdef CFG_PRESENCE_DET
	u8 lev = 0;

	lev = radar_mgr_bhr_user_cfg_get();

	logi("get breathe lev:%d\n\r", lev);
#endif
	return 0;
}

s32 radar_shell_mdet_sen_get(size_t argc, char **argv)
{
	u8 lev = 0;

	lev = radar_mgr_mdet_sen_lev_get();

	logi("get mdet sensitivity:%d\n\r", lev);

	return 0;
}

s32 radar_shell_mdet_sen_set(size_t argc, char **argv)
{
	u8 lev = 0;

	if (argc > 1) {
		lev = str2num(argv[1]);
		logi("set mdet sensitivity:%d\n\r", lev);

		radar_mgr_mdet_sen_lev_set(lev);
	}

	return 0;
}

s32 radar_shell_bhr_sen_get(size_t argc, char **argv)
{
#ifdef CFG_PRESENCE_DET
	u8 lev = 0;

	lev = radar_mgr_bhr_sen_lev_get();

	logi("get breathe sensitivity:%d\n\r", lev);
#endif
	return 0;
}

s32 radar_shell_bhr_sen_set(size_t argc, char **argv)
{
#ifdef CFG_PRESENCE_DET
	u8 lev = 0;

	if (argc > 1) {
		lev = str2num(argv[1]);
		logi("set breathe sensitivity:%d\n\r", lev);

		radar_mgr_bhr_sen_lev_set(lev);
	}
#endif
	return 0;
}

s32 radar_shell_bhr_det_start(size_t argc, char **argv)
{
#ifdef CFG_PRESENCE_DET
	radar_mgr_bhr_undet_delta_set(0);
#endif
	return 0;
}

s32 radar_shell_bhr_det_stop(size_t argc, char **argv)
{
#ifdef CFG_PRESENCE_DET
	radar_mgr_bhr_undet_delta_set(RAD_DET_BHR_MAX);

#endif
	return 0;
}


static osal_timer_t m_radar_shell = {0};

static void radar_shell_timer_timeout(void *pArgs)
{
	fmcw_det_info_t inf;
	radar_mgr_mdet_info_get(&inf);
	s8 det_type = -1;

	char string_array[3][10] = {"isMov@", "isBrh@", "isMicro@"};

	if (inf.is_detected) {
		if(inf.det_result & (MD_RESULT_MOT_CLOSE |MD_RESULT_MOT_FAR | MD_RESULT_MOT_STAY)) {
			//logi("isMov@%dmm,%d°\n\r",inf.range_val,inf.angle_val);
			det_type = 0;
		} else if(inf.det_result & MD_RESULT_PRESENCE){
			//logi("isBrh@%dmm,%d°\n\r",inf.range_val,inf.angle_val);
			det_type = 1;
		} else if(inf.det_result & MD_RESULT_MICRO) {
			//logi("isMicro@%dmm,%d°\n\r",inf.range_val,inf.angle_val);
			det_type = 2;
		}
	}

	if(det_type != -1 ) {
		if(inf.angle_val != 0) {
			logi("%s%dmm,%d°\n\r",string_array[det_type],inf.range_val,inf.angle_val);
		} else {
			logi("%s%dmm\n\r",string_array[det_type],inf.range_val);
		}
	}

	return;
}

s32 radar_shell_md_log_en(size_t argc, char **argv)
{
	u32 time = 800;

	if (argc > 1) {
		time = str2num(argv[1]);

	}

	if(m_radar_shell.handle == NULL) {
		osal_timer_create(&m_radar_shell, OSAL_TIMER_MODE_PERIODIC, radar_shell_timer_timeout, NULL, time);
		osal_timer_start(&m_radar_shell);
	}

	return 0;
}

s32 radar_shell_md_log_disable(size_t argc, char **argv)
{
	if(m_radar_shell.handle != NULL) {
		osal_timer_stop(&m_radar_shell);
		osal_timer_delete(&m_radar_shell);
	}

	return 0;
}

#endif /* !CFG_RADAR_SHELL */
