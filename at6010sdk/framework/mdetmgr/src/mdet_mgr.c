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

#include "mdet_mgr.h"
#include "radar_msg.h"
#include "app_msg.h"
#include "osal.h"
#include "uart.h"
#include "gpio.h"
#include "atlog.h"
#include "light_ctrl.h"
#include "radar_mgr.h"
#include "hal_radar.h"
#include "cfg_radar_profile.h"
#include "output_config.h"

#ifdef CFG_APP_HTM
#define PARA_RB_NUM                           (50)
#else
#define PARA_RB_NUM                           (70)
#endif

typedef struct {
	void *md_hdl;
	u8 det_output_en; /**< detection result output, 1: enable, 0: disalbe */
} mdet_mgr_env_t;

/* Motion detection handler definition. */
static mdet_mgr_env_t m_mdet_env = {
	.md_hdl = NULL,
#ifdef CFG_DET_RESULT_OUTPUT
	.det_output_en = 1,
#else
	.det_output_en = 0,
#endif
};

/**
 * @brief Motion detection algorithm paramter configuration.
 *
 * @param md motion detection handler.
 */
static void mdet_init_default_para(void* md)
{
#if defined(CFG_APP_HTM) || defined(CFG_APP_LIGHT) || defined(CFG_APP_BRHR)
	radar_fmcw_mv_range_conf_set(14);
	radar_fmcw_bhr_range_conf_set(12);
	radar_fmcw_trigger_mode_set(MD_MV_TRIGGER); /* move triggered, breath keep active */

	radar_fmcw_md_param_special_cfg(MD_PARA_RESP_TRACK_COUNT, 650); /* keep breath alive time 650*25ms ,unit :25ms */

	radar_fmcw_md_param_special_cfg(MD_PARA_SHUTDOWN_PROT_TIME, 1); /* protect time after shutdown ,unit: 1s */
	radar_fmcw_md_param_special_cfg(MD_PARA_WIFI_SUPRESSION_RANK, 1); /* 5.8g wifi noise suppression level ,level = 0~128 */
	radar_fmcw_md_param_special_cfg(MD_PARA_MOVING_ENTER_FRAME_COUNT, 40); /* dithering elimination time to avoid error trigger, set default to 1s */

	radar_fmcw_md_param_special_cfg(MD_PARA_MOVING_ACCURACY_VS_REALTIME, 0); /* value = 0~7, 0:the best accuracy */
	radar_fmcw_md_param_special_cfg(MD_PARA_MOVING_RANING_BARYCENTER_VS_NEAREST, 0); /* 0：BARYCENTER,1：NEAREST */
	radar_fmcw_md_param_special_cfg(MD_PARA_RESPIRATION_FUNC_BITMAP, (1<<3) | (1<<1));
	radar_fmcw_active_time_keep(10); /* keep active time = 10*100ms,unit:100ms */
#ifdef PHASE_SHIFT_180DEGREE_EN
	radar_fmcw_rx1_phase_180_degree_shift(); /* enable rx1 phase shift 180 */
#endif

#ifdef CFG_APP_HTM
	radar_fmcw_htm_offset_set(md,(u8)PARA_HTM_RANGE_OFFSET);
#ifdef CFG_HTM_STATIC_PROBE
	radar_fmcw_htm_mode_set(HTM_MODE_STATIC_PROBE);  /* specific dyncmic or static height measure  */
#else
	radar_fmcw_htm_mode_set(HTM_MODE_DYNA_PROBE);    /* specific dyncmic or static height measure  */
	radar_fmcw_htm_stat_ret_cfg(md,5,4,1);           /* total time:5, repeat time:4 */
#endif
#endif

#if defined(CFG_RADAR_STAGE_SUPPORT)
	radar_fmcw_mv_pwr_dump_cfg(md,PARA_RB_NUM);
#endif

#elif defined(CFG_APP_POD)
	pod_cfg_t cfg = {
		.threshold_DB = 85,
		.rb_sta = 3,
		.rb_num = 40,
		.num_frame_acc = 32,
		.ref_rb_cnt = 15,
		.ref_rb_threshold = 7,
		.report_delay = 33,
		.report_mode = POD_REPORT_SCORE_CNT_RGE,
	};

	radar_fmcw_pod_param_set(md,&cfg);
#endif
}

static void mdet_param_dispatch(u32 value)
{
#ifndef CFG_APP_POD
	fmcw_md_para_set(m_mdet_env.md_hdl, value);
#endif
}

static void mdet_param_set(u16 type, u16 value)
{
	u32 para = type << 16 | (u16)value;
	osal_msg_t msg = {
		.id = MSG_RAD_MDET_PARA_SET,
		.para = para
	};

	osal_task_send_msg(MOD_RAD, &msg, OSAL_WAIT_FOREVER);
}

static void output_det_result(void)
{
	fmcw_det_info_t det_info = {0};
	radar_frame_profile_t frame = RADAR_PRF_FRAME;
	u32 fps = 1000000/frame.interval_us;
	static u32 frame_pre_idx = 0;

	radar_mgr_mdet_info_get(&det_info);
	if ((det_info.frame_idx - frame_pre_idx) > fps) {
		logi("frame=%d, det=%d, range=%dmm, angle=%ddeg\r\n",
			det_info.frame_idx, det_info.det_result, (det_info.range_val), det_info.angle_val);
		frame_pre_idx = det_info.frame_idx;
	}
}

void mdet_init(void *p)
{
	u8 profile = MD_PROFILE_BASELINE, max_rb_bl = 0, max_rb_high = 0, bw = 0;
	u8 mode = DET_MOD_NORMAL;
	u32 dist_mm = RAD_DET_MOT_MAX*10;
	usr_calib_cfg_h ca_conf = {
#ifdef CFG_DUMP_FMCW_DATA
		.port_sel = 0, /* 0:uart0,1:spi*/
#else
		.port_sel = -1,
#endif
	};

#ifdef CFG_APP_LIGHT
	/* mode: normal */
	bw= MD_SWEEP_BW_1GHZ;
	mode = DET_MOD_NORMAL;
	max_rb_bl = (dist_mm + 88)/176;

#if defined(CFG_PRESENCE_DET)
	profile = MD_PROFILE_HIGH;
	dist_mm = RAD_DET_BHR_MAX*10;
	max_rb_high = (dist_mm + 88)/176;
#else
	profile = MD_PROFILE_BASELINE;
#endif

#elif defined(CFG_APP_HTM)
	/* mode: height measurement */
	bw= MD_SWEEP_BW_5GHZ;
	mode = DET_MOD_HTM;
	max_rb_bl = (dist_mm + 16)/32;
#elif defined(CFG_APP_POD)
	/* mode: Presence Occupancy Detection  */
	bw= MD_SWEEP_BW_5GHZ;
	mode = DET_MOD_POD;
	max_rb_bl =   0; /* invalid param for pod */
	max_rb_high = 0;  /* invalid param for pod */
	profile     = 0;  /* invalid param for pod */
#elif defined(CFG_APP_BRHR)
	bw= MD_SWEEP_BW_1GHZ;
	mode = DET_MOD_BHR;
	max_rb_bl = 0;   /* invalid param for brhr */
	profile = 0;     /* invalid param for brhr */
	max_rb_high = 0; /* invalid param for brhr */
#endif /* !CFG_APP_HTM */

	m_mdet_env.md_hdl = radar_fmcw_md_init(profile, max_rb_bl, max_rb_high, bw, mode);

	/* radar algorithm params configuration. */
	mdet_init_default_para(m_mdet_env.md_hdl);

#if defined(ULP_PROG_1) && defined(CFG_EXTI_ULP)
	// NO calibration
#else
#ifdef CFG_DUMP_FMCW_DATA
	output_dma_config();
	radar_fmcw_md_ca_cfg(m_mdet_env.md_hdl,output_port_send_data);
#endif
#endif

#ifndef CFG_DUMP_FMCW_DATA
	/**
	 * @brief enable or disable fmcw detect log.
	 *
	 * log format: param0 DET or NOT DET param1 dist or range param2
	 * param0: frame number
	 * param1: moving or breath
	 * param2: moving distance or the detected range of breath
	*/
	radar_fmcw_md_log_onoff(m_mdet_env.md_hdl, DISABLE_FMCW_LOG);
#endif
}

void mdet_algo_det_log_onoff(u8 en)
{
	radar_fmcw_md_log_onoff(m_mdet_env.md_hdl, en);
}

void mdet_set_det_result_output(u8 val)
{
	m_mdet_env.det_output_en = val;
}

u8 mdet_is_hw_support_bhr(void)
{
	u8 ret = 0;

	ret = radar_fmcw_is_support_bhr(m_mdet_env.md_hdl);

	return ret;
}

void mdet_get_det_info(fmcw_det_info_t *inf)
{
	radar_fmcw_md_det_info(m_mdet_env.md_hdl, inf);
}

void mdet_get_det_auxinfo(void *inf)
{
#if defined(ULP_PROG_1) && defined(CFG_EXTI_ULP)
	// do nothing
#else
	radar_fmcw_md_det_auxinfo(m_mdet_env.md_hdl, inf);
#endif
}

void mdet_get_det_pwr(u32 *addr)
{
#ifdef CFG_RADAR_STAGE_SUPPORT
	radar_fmcw_mv_pwr_dump(m_mdet_env.md_hdl, addr);
#endif
}

void mdet_pod_param_set(pod_cfg_t *cfg)
{
	osal_msg_t msg = {
		.id = MSG_RAD_POD_PARA_SET,
		.para = (u32)cfg
	};

	osal_task_send_msg(MOD_RAD, &msg, OSAL_WAIT_FOREVER);
}

void mdet_set_undet_delta(u16 delta)
{
	mdet_param_set(MD_FMCW_PARA_BL_UNDET_DELTA, delta);
}

void mdet_set_delta(u16 delta)
{
	mdet_param_set(MD_FMCW_PARA_BL_DET_DELTA, delta);
}

void mdet_set_sensitivity(u8 mode, u8 level)
{
	switch (mode) {
	case MDET_MOTION_DET_MOD:
		mdet_param_set(MD_FMCW_PARA_BL_SEN, level);
		break;

	case MDET_MICRO_DET_MOD:
		mdet_param_set(MD_FMCW_PARA_EXT_SEN, level);
		break;

	case MDET_BRTH_DET_MOD:
		mdet_param_set(MD_FMCW_PARA_HIGH_SEN, level);
		break;

	default :
		loge("set_sensitivity:invalid parameter \n\r");
	}
}

#if defined(CFG_PRESENCE_DET) || defined(CFG_APP_BRHR)
void mdet_set_bhr_undet_delta(u16 delta)
{
	mdet_param_set(MD_FMCW_PARA_HIGH_UNDET_DELTA, delta);
}

void mdet_set_bhr_delta(u16 delta)
{
	mdet_param_set(MD_FMCW_PARA_HIGH_DET_DELTA, delta);
}
#endif

void mdet_proc_reset(u8 is_hard)
{
	radar_fmcw_md_reset(m_mdet_env.md_hdl, is_hard);
}

void mdet_stop_proc_data(void)
{
	radar_fmcw_md_pause(m_mdet_env.md_hdl, 0);
}

void mdet_start_proc_data(void)
{
	radar_fmcw_md_resume(m_mdet_env.md_hdl, 0);
}

void mdet_set_data_cb(mdet_data_cb hdl)
{
}

void mdet_skip_num(u16 num)
{
	mdet_param_set(MD_FMCW_PARA_SKIP_NUM, num);
}

u8 mdet_proc(void)
{
	u8 is_mdet = 0;
	u8 is_bhrdet = 0;
	u8 is_microdet = 0;
	u8 ret_val = 0;

	ret_val = radar_fmcw_md_perform(m_mdet_env.md_hdl);

#ifdef CFG_MDET_POSTPROC
	ret_val = mdet_postproc(m_mdet_env.md_hdl, ret_val);
#endif

	/* convert fmcw det result to mdet mgr det result. */
	if (ret_val) {
		if (ret_val & MD_RESULT_MICRO) {
			is_microdet = 1;
		} else if (ret_val & MD_RESULT_PRESENCE) {
			is_bhrdet = 1;
		} else {
			is_mdet = 1;
		}

		/* output processing of detection results. */
		if (m_mdet_env.det_output_en != 0) {
			output_det_result();
		}

		ret_val = ((is_microdet << MDET_BIT_POS_MICRO_DET) | (is_bhrdet << MDET_BIT_POS_BHR_DET) | is_mdet);
	}
	return ret_val;
}

bool mdet_msg_handler(osal_msg_t *msg)
{
	u32 ret = TRUE;
	u32 dat = 0;

	switch (msg->id) {
	case MSG_RAD_DAT_RDY:
		dat = (u32)mdet_proc();
		if (dat) {
			osal_msg_t msg = {
				.id = MSG_APP_COMMON_MDET_RET,
				.para = dat
			};

			osal_task_send_msg(MOD_APP, &msg, OSAL_WAIT_FOREVER);
		}
		break;

	case MSG_RAD_MDET_PARA_SET:
		mdet_param_dispatch(msg->para);
		break;
#ifdef CFG_APP_POD
	case MSG_RAD_POD_PARA_SET:
		radar_fmcw_pod_param_set(m_mdet_env.md_hdl, (pod_cfg_t *)msg->para);
		break;
#endif
	default:
		ret = FALSE;
		break;
	}

	return ret;
}
