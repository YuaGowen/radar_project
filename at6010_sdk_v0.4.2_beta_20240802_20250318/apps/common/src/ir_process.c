/**
 *******************************************************************************
 * @file    ir_process.c
 * @author  Airtouch Software Team
 * @brief   target configuration of the current project
 *******************************************************************************
 * @copyright Copyright (c) 2024, Airtouching Intelligence Technology.
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

#include "ir_process.h"
#include "hal_common.h"
#include "hal_light.h"
#include "ir_para.h"
#include "radar_mgr.h"
#include "osal.h"
#include "hal_ir.h"

#ifdef CFG_OS
#include "sys_msg.h"
#include "sys_task.h"
#include "osal.h"
#endif

#ifdef CFG_APP_LIGHT
#include "app_light.h"
#endif

#ifdef CFG_APP_HTM
#include "app_htm.h"
#endif

#ifdef CFG_IR

//#define IR_TIMER_EN
#define IR_MSG_POST_EN

#define IR_PARA_DUTY_PWR_HIGH (80U)
#define IR_PARA_DUTY_PWR_LOW  (50U)
#define IR_DUTY_SCALE         (10U)

#ifndef IR_MSG_POST_EN
static u16 m_ir_code;
#endif

static s32 ir_dist_key_proc(u8 ircode)
{
	s32 ret = 0;
	if (IR_MODE_AUTO == get_ir_para(IR_PARA_MODE)) {
		switch (ircode) {
		case IR_CODE_DIST_L1:
			radar_mgr_mdet_user_cfg_set(PARA_IR_DIST_L1);
			set_ir_para(IR_PARA_DETECTION_RANGE, PARA_IR_DIST_L1);
			break;
		case IR_CODE_DIST_L2:
			radar_mgr_mdet_user_cfg_set(PARA_IR_DIST_L2);
			set_ir_para(IR_PARA_DETECTION_RANGE, PARA_IR_DIST_L2);
			break;
		case IR_CODE_DIST_L3:
			radar_mgr_mdet_user_cfg_set(PARA_IR_DIST_L3);
			set_ir_para(IR_PARA_DETECTION_RANGE, PARA_IR_DIST_L3);
			break;
		case IR_CODE_DIST_L4:
			radar_mgr_mdet_user_cfg_set(PARA_IR_DIST_L4);
			set_ir_para(IR_PARA_DETECTION_RANGE, PARA_IR_DIST_L4);
			break;
		default:
			ret = -1;
			break;
		}
	} else {
		ret = -1;
	}
	return ret;
}

static s32 ir_std_key_proc(u8 ircode)
{
	s32 ret = 0;
	if (IR_MODE_AUTO == get_ir_para(IR_PARA_MODE)) {
		switch (ircode) {
		case IR_CODE_STD_L1:
			app_light_set_standby_level((u16)PARA_STD_L1 * IR_DUTY_SCALE);
			set_ir_para(IR_PARA_STANDBY_LEVEL, ircode);
			break;
		case IR_CODE_STD_L2:
			app_light_set_standby_level((u16)PARA_STD_L2 * IR_DUTY_SCALE);
			set_ir_para(IR_PARA_STANDBY_LEVEL, ircode);
			break;
		case IR_CODE_STD_L3:
			app_light_set_standby_level((u16)PARA_STD_L3 * IR_DUTY_SCALE);
			set_ir_para(IR_PARA_STANDBY_LEVEL, ircode);
			break;
		case IR_CODE_STD_L4:
			app_light_set_standby_level((u16)PARA_STD_L4 * IR_DUTY_SCALE);
			set_ir_para(IR_PARA_STANDBY_LEVEL, ircode);
			break;
		default:
			ret = -1;
			break;
		}
	} else {
		ret = -1;
	}
	return ret;
}

static s32 ir_set_keep_time1(u8 ircode)
{
	u32 time = 0;
	s32 ret = -1;
	if (IR_MODE_AUTO == get_ir_para(IR_PARA_MODE)) {
		switch (ircode) {
		case IR_CODE_KEEP_TIME1_L1:
			time = (u32)PARA_KEEP_TIME1_L1*1000u;
			set_ir_para(IR_PARA_ACTIVE_TIMER, ircode);
			break;
		case IR_CODE_KEEP_TIME1_L2:
			time = (u32)PARA_KEEP_TIME1_L2*1000u;
			set_ir_para(IR_PARA_ACTIVE_TIMER, ircode);
			break;
		case IR_CODE_KEEP_TIME1_L3:
			time = (u32)PARA_KEEP_TIME1_L3*1000u;
			set_ir_para(IR_PARA_ACTIVE_TIMER, ircode);
			break;
		case IR_CODE_KEEP_TIME1_L4:
			time = (u32)PARA_KEEP_TIME1_L4*1000u;
			set_ir_para(IR_PARA_ACTIVE_TIMER, ircode);
			break;
		default:
			ret = -1;
			break;
		}

		if (time != 0u) {
#ifdef CFG_APP_LIGHT
			app_light_set_lot(time);
#endif
			ret = 0;
		}
	}
	return ret;
}

#ifdef CFG_LS
static s32 ir_lux_key_proc(u8 ircode)
{
	s32 ret = 0;
	if (IR_MODE_AUTO == get_ir_para(IR_PARA_MODE)) {
		switch (ircode) {
		case IR_CODE_LUX_L1:
			hal_ls_set_threshold(PARA_IR_LUX_L1);
			set_ir_para(IR_PARA_LUX_LEVEL, ircode);
			break;
		case IR_CODE_LUX_L2:
			hal_ls_set_threshold(PARA_IR_LUX_L2);
			set_ir_para(IR_PARA_LUX_LEVEL, ircode);
			break;
		case IR_CODE_LUX_L3:
			hal_ls_set_threshold(PARA_IR_LUX_L3);
			set_ir_para(IR_PARA_LUX_LEVEL, ircode);
			break;
		case IR_CODE_LUX_L4:
			hal_ls_set_threshold(PARA_IR_LUX_L4);
			set_ir_para(IR_PARA_LUX_LEVEL, ircode);
			break;
		default:
			ret = -1;
			break;
		}
	} else {
		ret = -1;
	}
	return ret;
}
#endif

static s32 ir_scene_key_proc(u8 ircode)
{
	s32 ret = 0;

	switch (ircode) {
	case IR_CODE_SC1:
		set_ir_para(IR_PARA_MODE, IR_MODE_SC1);
		radar_mgr_mdet_user_cfg_set(PARA_IR_DIST_L4);
#ifdef CFG_APP_LIGHT
		app_light_set_standby_level((u16)PARA_STD_L4 * IR_DUTY_SCALE);
		app_light_set_lot((u32)PARA_KEEP_TIME1_L4*1000u);
#endif
#ifdef CFG_LS
		hal_ls_set_threshold(0);
#endif

		break;
	case IR_CODE_SC2:
		set_ir_para(IR_PARA_MODE, IR_MODE_SC2);
		radar_mgr_mdet_user_cfg_set(PARA_IR_DIST_L4);
#ifdef CFG_APP_LIGHT
		app_light_set_standby_level((u16)PARA_STD_L4 * IR_DUTY_SCALE);
		app_light_set_lot((u32)PARA_KEEP_TIME1_L1*1000u);
#endif
#ifdef CFG_LS
		hal_ls_set_threshold(PARA_IR_LUX_L2);
#endif
		break;
	case IR_CODE_SC3:
		set_ir_para(IR_PARA_MODE, IR_MODE_SC3);
		radar_mgr_mdet_user_cfg_set(PARA_IR_DIST_L4);
#ifdef CFG_APP_LIGHT
		app_light_set_standby_level((u16)PARA_STD_L3 * IR_DUTY_SCALE);
		app_light_set_lot((u32)PARA_KEEP_TIME1_L2*1000u);
#endif
#ifdef CFG_LS
		hal_ls_set_threshold(PARA_IR_LUX_L2);
#endif
		break;
	case IR_CODE_SC4:
		set_ir_para(IR_PARA_MODE, IR_MODE_SC4);
		radar_mgr_mdet_user_cfg_set(PARA_IR_DIST_L4);
#ifdef CFG_APP_LIGHT
		app_light_set_standby_level((u16)PARA_STD_L3 * IR_DUTY_SCALE);
		app_light_set_lot((u32)PARA_KEEP_TIME1_L3*1000u);
#endif
#ifdef CFG_LS
		hal_ls_set_threshold(PARA_IR_LUX_L2);
#endif
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
}

static s32 ir_misc_key_proc(u8 ircode)
{
	s32 ret = 0;

	switch (ircode) {
	case IR_CODE_ALWAYS_ON:
		set_ir_para(IR_PARA_MODE, IR_MODE_ON);
		app_light_always_on();
		break;

	case IR_CODE_ALWAYS_OFF:
		set_ir_para(IR_PARA_MODE, IR_MODE_OFF);
		app_light_always_off();
		break;

	case IR_CODE_TEST_MODE:
		set_ir_para(IR_PARA_MODE, IR_MODE_TEST);
#ifdef CFG_APP_LIGHT
		app_light_set_lot(2000u);
#endif
#ifdef CFG_LS
		hal_ls_set_threshold(PARA_IR_LUX_L2);
#endif
		radar_mgr_mdet_user_cfg_set(PARA_IR_DIST_L4);
		app_light_set_standby_level(100u);
		break;

	case IR_CODE_AUTO_MODE:
		set_ir_para(IR_PARA_MODE, IR_MODE_AUTO);
		app_light_set_intensity(PARA_PWM_LON_DUTY);
		(void)ir_dist_key_proc(get_ir_para(IR_PARA_DETECTION_RANGE));
		(void)ir_std_key_proc(get_ir_para(IR_PARA_STANDBY_LEVEL));
		(void)ir_set_keep_time1(get_ir_para(IR_PARA_ACTIVE_TIMER));
#ifdef CFG_LS
		(void)ir_lux_key_proc(get_ir_para(IR_PARA_LUX_LEVEL));
#endif
		break;

	case IR_CODE_PWR_HIGHT:
		if (IR_MODE_ON == get_ir_para(IR_PARA_MODE)) {
			set_ir_para(IR_PARA_INTENSITY, IR_PARA_DUTY_PWR_HIGH);
			app_light_set_intensity((u16)IR_PARA_DUTY_PWR_HIGH * IR_DUTY_SCALE);
		}
		break;

	case IR_CODE_PWR_LOW:
		if (IR_MODE_ON == get_ir_para(IR_PARA_MODE)) {
			set_ir_para(IR_PARA_INTENSITY, IR_PARA_DUTY_PWR_LOW);
			app_light_set_intensity((u16)IR_PARA_DUTY_PWR_LOW * IR_DUTY_SCALE);
		}
		break;

	case IR_CODE_DIM_ADD:
		if (IR_MODE_ON == get_ir_para(IR_PARA_MODE)) {
			u8 intensity = get_ir_para(IR_PARA_INTENSITY);
			intensity = (intensity < IR_PARA_MAX_INTENSITY) ? (intensity + 5u) : IR_PARA_MAX_INTENSITY;
			intensity = (intensity > 100u) ? 100u : intensity;
			set_ir_para(IR_PARA_INTENSITY, intensity);
			app_light_set_intensity((u16)intensity * IR_DUTY_SCALE);
		}
		break;

	case IR_CODE_DIM_SUB:
		if (IR_MODE_ON == get_ir_para(IR_PARA_MODE)) {
			u8 intensity = get_ir_para(IR_PARA_INTENSITY);
			intensity = (intensity > IR_PARA_MIN_INTENSITY) ? (intensity - 5u) : IR_PARA_MIN_INTENSITY;
			intensity = (intensity < IR_PARA_MIN_INTENSITY) ? IR_PARA_MIN_INTENSITY : intensity;
			set_ir_para(IR_PARA_INTENSITY, intensity);
			app_light_set_intensity((u16)intensity * IR_DUTY_SCALE);
		}
		break;

	default:
		ret = -1;
		break;
	}

	return ret;
}

#ifdef IR_TIMER_EN
static void ir_timer_cb(void *arg)
{
	UNUSED(arg);
	ir_main_proc();
}

static void ir_period_timer_init(void)
{
	osal_timer_t ir_tm = {NULL};
	OSAL_ASSERT_ERR(
	        osal_timer_create(&ir_tm, OSAL_TIMER_MODE_PERIODIC, ir_timer_cb, NULL, 100U),
	        "create ir timer failed!\r\n");
	OSAL_ASSERT_ERR(osal_timer_start(&ir_tm), "start ir timer failed!\r\n");
}
#endif

static void ir_code_proc(u8 ircode)
{
	if (ircode != 0xffu) {
		/* assume the value 0xff is invalid ir code. */
		logi("%s, ircode=0x%x\r\n", __func__, ircode);

		if (ir_dist_key_proc(ircode) == 0) {
		} else if (ir_set_keep_time1(ircode) == 0) {
#ifdef CFG_LS
		} else if (ir_lux_key_proc(ircode) == 0) {
#endif
		} else if (ir_scene_key_proc(ircode) == 0) {
		} else if (ir_misc_key_proc(ircode) == 0) {
		} else if (ir_std_key_proc(ircode) == 0) {
		} else {
			logi("[%s]ircode not exists.\r\n", __func__);
		}
	} else {
		logw("ircode invalid!\r\n");
	}
}

#ifndef IR_MSG_POST_EN
static u8 ir_take_code(void)
{
	u8 ircode = (u8)m_ir_code & 0xFFU;
	m_ir_code = 0xFFFFU;
	return ircode;
}

void ir_main_proc(void)
{
	u8 ircode;

	ircode = ir_take_code();
	ir_code_proc(ircode);
}
#endif /* IR_MSG_POST_EN */

#ifdef CFG_OS
static void ir_post_msg_hdl(u16 scancode)
{
	osal_msg_t msg;

	msg.id = (u32)MSG_SYS_IR_PRESSED;
	msg.para = scancode;
	OSAL_ASSERT_ERR(osal_task_send_msg(MOD_SYS, &msg, OSAL_WAIT_FOREVER),
	                "ir send msg failed!\r\n");
}

bool ir_main_msg_handler(const osal_msg_t *msg)
{
	bool ret = FALSE;
	if ((u32)MSG_SYS_IR_PRESSED == msg->id) {
		ir_code_proc((u8)msg->para);
		ret = TRUE;
	}
	return ret;
}
#endif /* CFG_OS */

void app_ir_init(void)
{
	logi("%s\r\n", __func__);

#ifdef IR_TIMER_EN
	ir_period_timer_init();
#else
	hal_ir_nec_init(ir_post_msg_hdl);
	sys_task_reg_msg(MSG_SYS_GRP_IR, ir_main_msg_handler);
#endif
}

#endif /* CFG_IR */
