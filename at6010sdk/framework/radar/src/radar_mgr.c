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
#include "adc.h"
#include "atlog.h"
#include "hal_radar_fmcw.h"
#include "if_sample.h"
#include "osal.h"
#include "radar.h"
#include "radar_frame.h"
#include "radar_msg.h"
#include "radar_profile_init.h"
#include "hal_radar.h"

typedef void (*profile_func_t)(void);

typedef struct {
	u8 radar_onoff;
	u8 is_hw_support_bhr;   // only for fmcw radar mode
	u8 mot_cfg_val;
	u8 mot_sen_lev;
	u16 mdet_delta_val;
	u16 mdet_undet_delta_val;
	profile_func_t profile_init;
#if defined(CFG_PRESENCE_DET) || defined(CFG_APP_BRHR)
	u8 bhr_cfg_val;
	u8 bhr_sen_lev;
	u16 bhr_delta_val;
	u16 bhr_undet_delta_val;
#endif
}radar_mgr_env_t;

typedef enum {
	RADAR_RF_PWR_SET,
	RADAR_RF_FMCW_INIT,
	RADAR_RF_FMCW_STA_FREQ_SET,
	RADAR_RF_FMCW_TX_GAIN_SET
} radar_rf_cfg_t;

static radar_mgr_env_t m_radmgr_env = {
	.mot_cfg_val = 0xFF,
	.mot_sen_lev = 0xFF,
	.mdet_delta_val = 0xFFFF,
	.mdet_undet_delta_val =  0xFFFF,
#if defined(CFG_PRESENCE_DET) || defined(CFG_APP_BRHR)
	.bhr_cfg_val = 0xFF,
	.bhr_sen_lev = 0xFF,
	.bhr_delta_val = 0xFFFF,
#endif
	.radar_onoff = 1,
	.is_hw_support_bhr = 0,
};

static osal_mutex_t m_radar_mgr_mutex;
static const mot_dist_para_t m_mot_dist_paras[RAD_DIST_LV_MAX] = RAD_MOT_DIST_PARAS;
#if defined(CFG_PRESENCE_DET) || defined(CFG_APP_BRHR)
static const bhr_dist_para_t m_bhr_dist_paras[RAD_BHR_DIST_LV_MAX] = RAD_BHR_DIST_PARAS;
#endif

//#define DBG_LOG_RADAR_MGR

#ifdef DBG_LOG_RADAR_MGR
#define dbg_radar_mgr        logi
#else
#define dbg_radar_mgr(...)
#endif


static void radar_mgr_dev_init(void)
{
	//fmcw_obj_param *cfg;

	// hal_fmcw_init();              /*init some default parameter*/
	// cfg = hal_fmcw_get_tgt_cfg(); /*get fmcw radar target configure parameter*/
	// hal_fmcw_set_cfg(cfg);        /*cfg take effect*/
	// hal_fmcw_pa_on();

	/* radar profile configuration. */
	radar_profile_init();
	hal_radar_lp_start();
	m_radmgr_env.profile_init = radar_profile_init;
}

static void radar_mgr_if_start(void)
{
	hal_radar_start();
}

static void radar_mgr_if_stop(void)
{
	hal_radar_stop();
}

static void radar_mgr_pwr_onoff(u8 onoff)
{
	if (m_radmgr_env.radar_onoff == onoff) {
		return;
	}

	if (onoff > 0U) {
		m_radmgr_env.profile_init();
		radar_mgr_reset();
		radar_mgr_if_start();
	} else {
		radar_mgr_if_stop();
		radar_rf_pwr_off();
	}

	m_radmgr_env.radar_onoff = onoff;
}

void radar_mgr_init(void)
{
	osal_mutex_create(&m_radar_mgr_mutex);

	radar_mgr_dev_init();

	mdet_init(NULL);

	m_radmgr_env.is_hw_support_bhr = mdet_is_hw_support_bhr();
}

u8 radar_mgr_is_bhr_support(void)
{
	return m_radmgr_env.is_hw_support_bhr;
}

void radar_mgr_skip_num(u16 num)
{
	mdet_skip_num(num);
}

void radar_mgr_reset(void)
{
	mdet_proc_reset(1);
}

void radar_mgr_profile_set(void *profile_init_func)
{
	m_radmgr_env.profile_init = (profile_func_t)profile_init_func;
}

s32 radar_mgr_mdet_info_get(fmcw_det_info_t *inf)
{
	s32 ret = -1;

	if (radar_mgr_get_pwr_status() == 1) {
		mdet_get_det_info(inf);
		ret = 0;
	}

	return ret;
}

s32 radar_mgr_mdet_auxinfo_get(void *inf)
{
	s32 ret = -1;

	if (radar_mgr_get_pwr_status() == 1) {
		mdet_get_det_auxinfo(inf);
		ret = 0;
	}

	return ret;
}

void radar_mgr_mdet_pwr_get(u32 *cap)
{
	if (radar_mgr_get_pwr_status() == 1) {
		mdet_get_det_pwr(cap);
	}
}

u8 radar_mgr_mdet_sen_lev_get(void)
{
	return m_radmgr_env.mot_sen_lev;
}

void radar_mgr_mdet_sen_lev_set(u8 lev)
{
	if (m_radmgr_env.mot_sen_lev == lev) {
		return;
	}

	osal_mutex_lock(&m_radar_mgr_mutex,OSAL_WAIT_FOREVER);

	m_radmgr_env.mot_sen_lev = lev;
	mdet_set_sensitivity(MDET_MOTION_DET_MOD,lev);

	osal_mutex_unlock(&m_radar_mgr_mutex);;
}

void radar_mgr_mdet_delta_set(u16 val)
{
	if (m_radmgr_env.mdet_delta_val == val) {
		return;
	}

	dbg_radar_mgr("%s: val = %d\r\n", __func__, val);

	osal_mutex_lock(&m_radar_mgr_mutex,OSAL_WAIT_FOREVER);

	m_radmgr_env.mdet_delta_val = val;
	mdet_set_delta(val);

	osal_mutex_unlock(&m_radar_mgr_mutex);
}

u16 radar_mgr_mdet_delta_get(void)
{
	dbg_radar_mgr("%s: val = %d\r\n", __func__, m_radmgr_env.mdet_delta_val);

	return m_radmgr_env.mdet_delta_val;
}

void radar_mgr_mdet_undet_delta_set(u16 val)
{
	if (m_radmgr_env.mdet_undet_delta_val == val) {
		return;
	}

	osal_mutex_lock(&m_radar_mgr_mutex,OSAL_WAIT_FOREVER);

	m_radmgr_env.mdet_undet_delta_val = val;
	mdet_set_undet_delta(val);

	osal_mutex_unlock(&m_radar_mgr_mutex);
}

u16 radar_mgr_mdet_undet_delta_get(void)
{
	return m_radmgr_env.mdet_undet_delta_val;
}

void radar_mgr_mdet_user_cfg_set(u8 lev)
{
	if ((lev >= RAD_DIST_LV_MAX) || (m_radmgr_env.mot_cfg_val == lev)) {
		return;
	}
	dbg_radar_mgr("%s: val = %d\r\n", __func__, lev);

	osal_mutex_lock(&m_radar_mgr_mutex,OSAL_WAIT_FOREVER);

	m_radmgr_env.mot_cfg_val = lev;

	m_radmgr_env.mdet_undet_delta_val = m_mot_dist_paras[lev].delta_sta;
	m_radmgr_env.mdet_delta_val = m_mot_dist_paras[lev].delta_sto;
	m_radmgr_env.mot_sen_lev = m_mot_dist_paras[lev].sensitivity;

	mdet_set_undet_delta(m_radmgr_env.mdet_undet_delta_val);
	mdet_set_delta(m_radmgr_env.mdet_delta_val);
	mdet_set_sensitivity(MDET_MOTION_DET_MOD, m_mot_dist_paras[lev].sensitivity);

	osal_mutex_unlock(&m_radar_mgr_mutex);
}

u8 radar_mgr_mdet_user_cfg_get(void)
{
	dbg_radar_mgr("%s: val = %d\r\n", __func__, m_radmgr_env.mot_cfg_val);

	return m_radmgr_env.mot_cfg_val;
}

#if defined(CFG_PRESENCE_DET) || defined(CFG_APP_BRHR)
u16 radar_mgr_bhr_undet_delta_get(void)
{
	return m_radmgr_env.bhr_undet_delta_val;
}

void radar_mgr_bhr_undet_delta_set(u16 val)
{
	if (m_radmgr_env.bhr_undet_delta_val == val || m_radmgr_env.is_hw_support_bhr == 0) {
		return;
	}

	osal_mutex_lock(&m_radar_mgr_mutex,OSAL_WAIT_FOREVER);

	m_radmgr_env.bhr_undet_delta_val = val;
	mdet_set_bhr_undet_delta(val);

	osal_mutex_unlock(&m_radar_mgr_mutex);
}

void radar_mgr_bhr_delta_set(u16 val)
{
	if (m_radmgr_env.bhr_delta_val == val || m_radmgr_env.is_hw_support_bhr == 0) {
		return;
	}

	osal_mutex_lock(&m_radar_mgr_mutex,OSAL_WAIT_FOREVER);

	m_radmgr_env.bhr_delta_val = val;
	mdet_set_bhr_delta(val);

	osal_mutex_unlock(&m_radar_mgr_mutex);
}

u16 radar_mgr_bhr_delta_get(void)
{
	return m_radmgr_env.bhr_delta_val;
}

void radar_mgr_bhr_sen_lev_set(u8 lev)
{
	if (m_radmgr_env.bhr_sen_lev == lev || m_radmgr_env.is_hw_support_bhr == 0) {
		return;
	}

	osal_mutex_lock(&m_radar_mgr_mutex,OSAL_WAIT_FOREVER);

	m_radmgr_env.bhr_sen_lev = lev;
	mdet_set_sensitivity(MDET_BRTH_DET_MOD,lev);

	osal_mutex_unlock(&m_radar_mgr_mutex);
}

u8 radar_mgr_bhr_sen_lev_get(void)
{
	return m_radmgr_env.bhr_sen_lev;
}

void radar_mgr_bhr_user_cfg_set(u8 lev)
{
	if ((lev >= RAD_BHR_DIST_LV_MAX) || (m_radmgr_env.bhr_cfg_val == lev) ||
		m_radmgr_env.is_hw_support_bhr == 0) {
		return;
	}

	dbg_radar_mgr("%s: val = %d\r\n", __func__, lev);

	osal_mutex_lock(&m_radar_mgr_mutex,OSAL_WAIT_FOREVER);

	m_radmgr_env.bhr_cfg_val = lev;
	m_radmgr_env.bhr_delta_val       = m_bhr_dist_paras[lev].delta_sto;
	m_radmgr_env.bhr_undet_delta_val = m_bhr_dist_paras[lev].delta_sta;
	m_radmgr_env.bhr_sen_lev         = m_bhr_dist_paras[lev].sensitivity;

	mdet_set_bhr_delta(m_radmgr_env.bhr_delta_val);
	mdet_set_bhr_undet_delta(m_radmgr_env.bhr_undet_delta_val);
	mdet_set_sensitivity(MDET_BRTH_DET_MOD,m_radmgr_env.bhr_sen_lev);

	osal_mutex_unlock(&m_radar_mgr_mutex);
}

u8 radar_mgr_bhr_user_cfg_get(void)
{
	dbg_radar_mgr("%s: val = %d\r\n", __func__, m_radmgr_env.bhr_cfg_val);

	return m_radmgr_env.bhr_cfg_val;
}
#endif


u8 radar_mgr_get_pwr_status(void)
{
	return m_radmgr_env.radar_onoff;
}

static void radar_mgr_rf_param_set(u16 type ,u16 value)
{
	u32 para = type << 16 | (u16)value;

	osal_msg_t msg = {
		.id = MSG_RAD_RF_SET,
		.para = para
	};

	osal_task_send_msg(MOD_RAD, &msg, OSAL_WAIT_FOREVER);

	return;
}

void radar_mgr_rf_pwr_set(u8 value)
{
	radar_mgr_rf_param_set(RADAR_RF_PWR_SET,value);
}

void radar_mgr_rf_fmcw_init(void)
{
	radar_mgr_rf_param_set(RADAR_RF_FMCW_INIT,0);
}

void radar_mgr_rf_fmcw_freq_set(u32 freq)
{
	radar_mgr_rf_param_set(RADAR_RF_FMCW_STA_FREQ_SET,freq);
}

void radar_mgr_rf_fmcw_tx_gain_set(u32 lev)
{
	radar_mgr_rf_param_set(RADAR_RF_FMCW_TX_GAIN_SET,lev);
}

void radar_mgr_worker_run(void)
{
	radar_mgr_reset();

	osal_msg_t msg = {
		.id = MSG_RAD_START,
		.para = 0
	};

	osal_task_send_msg(MOD_RAD, &msg, OSAL_WAIT_FOREVER);
}

void radar_mgr_worker_halt(void)
{
	osal_msg_t msg = {
		.id = MSG_RAD_STOP,
		.para = 0
	};

	osal_task_send_msg(MOD_RAD, &msg, OSAL_WAIT_FOREVER);
	mdet_stop_proc_data();
}

static void radar_mgr_rf_set(u32 value)
{
	u16 type = (value & 0xFFFF0000) >> 16, param = (value & 0xFFFF);
	fmcw_obj_param *cfg;

	switch (type) {
	case RADAR_RF_PWR_SET:
		radar_mgr_pwr_onoff(param);
		break;

	case RADAR_RF_FMCW_INIT:
		radar_mgr_dev_init();
		break;

	case RADAR_RF_FMCW_STA_FREQ_SET:
		// cfg = hal_fmcw_get_tgt_cfg();
		cfg->chirp_param.chirp_sta_freq = param;
		// hal_fmcw_set_cfg(cfg);
		break;
	case RADAR_RF_FMCW_TX_GAIN_SET:
		if (param < 4U) {
			// cfg = hal_fmcw_get_tgt_cfg();
			cfg->tx_gain_level = param;
			// hal_fmcw_set_cfg(cfg);
		}
		break;

	default:
		logi("invalid fft param type.\n\r");
		break;
	};
}

bool radar_mgr_msg_handler(osal_msg_t *msg)
{
	u32 ret = TRUE;

	switch (msg->id) {
	case MSG_RAD_START:
		radar_mgr_if_start();
		break;

	case MSG_RAD_STOP:
		radar_mgr_if_stop();
		break;

	case MSG_RAD_RF_SET:
		radar_mgr_rf_set(msg->para);
		break;

	default:
		ret = FALSE;
		break;
	}

	return ret;
}
