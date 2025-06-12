/**
 *******************************************************************************
 * @file    app_light.c
 * @author  Airtouch Software Team
 * @brief   light application
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
 ******************************************************************************/

#include "app_light.h"
#include "hal_light.h"
#include "hal_ls.h"
#include "adc_key.h"
#include "atlog.h"
#include "radar_mgr.h"
#include "if_sample.h"
#include "sys_timer.h"
#include "light_ctrl.h"
#include "mdet_mgr.h"
#include "jiffies.h"
#include "fifo.h"
#include "ir_para.h"
#include "ir_decode.h"
#include "dip_mgr.h"
#include "diagnose.h"
#include "osal.h"
#include "app_msg.h"

#if defined(CFG_CI) && defined(CFG_USER_SETTING)
#include "ci_cmd.h"
#endif

#ifdef CFG_PWRON_SELF_CHECK
#include "self_check.h"
#endif

#ifdef CFG_ATMESH
#include "atmesh_api.h"
#endif

#ifdef CFG_APP_LIGHT

#ifdef APP_LIGHT_LOG_EN
#define APP_LIGHT_LOG(...) logw(__VA_ARGS__)
#else
#define APP_LIGHT_LOG(...)
#endif

typedef enum {
	RUN_STATUS_STARTUP = 0, /* 0: startup */
	RUN_STATUS_SELF_CHECK = 1, /* 1: self check mode */
	RUN_STATUS_NORMAL = 2, /* 2: normal work mode */
	RUN_STATUS_SWITCH_TM1 = 3, /* 3: switch test mode1 */
	RUN_STATUS_SWITCH_TM2 = 4, /* 4: switch test mode2 */

	RUN_STATUS_INVALID = 0xff
} run_status_t;

typedef enum {
	APP_LIGHT_MODE_SELF_CHECK,
	APP_LIGHT_MODE_DIAGNOSE,
	APP_LIGHT_MODE_NORMAL
} app_light_mode_t;

typedef struct {
	u32 lot_ms;
	run_status_t run_status;
	u32 stop_read_adc: 1;
} app_light_dev_t;

#ifdef CFG_ATMESH
typedef enum {
	DET_STATUS_IDLE,
	DET_STATUS_DETECTED,
	DET_STATUS_DETECT_CONTINUE
} det_status_t;

static det_status_t m_det_status;
static osal_timer_t m_det_post_tm;
#endif /* CFG_ATMESH */

static app_light_dev_t m_app_light = {
	.lot_ms = 5000u,   /* lot_ms */
	.run_status = RUN_STATUS_STARTUP,      /* run_status */
	.stop_read_adc = 0u,
};

#if defined(CFG_OS) && defined(CFG_LS)
static osal_timer_t m_ls_tm;
#endif

#ifdef CFG_SWITCH_TM

#include "switch_tm.h"

static osal_timer_t m_tim_flash;

/* switch on/off times to entry tm. */
#define SWITCH_TM1_CNT_MIN              (5u)
#define SWITCH_TM1_CNT_MAX              (7u)

#define SWITCH_TM2_CNT_MIN              (10u)
#define SWITCH_TM2_CNT_MAX              (12u)

#define SWITCH_TM_CLR_TO_MS             (15 * 1000)
#define SWITCH_TM_FLASH_MS              (400)           /* Light blinking interval */
#define SWITCH_TM_LON_MS                (2 * 1000)      /* Light on duration in tm2 */
#define SWITCH_TM_LOFF_MS               (1000)          /* Light off duration after blink */

#ifdef DBG_LOG_SWITCH_TM
#define dbg_tm_log        logi
#else
#define dbg_tm_log(...)
#endif

static void switch_tm_flash_on1_hdl(void *arg);

static void switch_tm_flash_end_hdl(void *arg)
{
	u32 tm_id = (u32)arg;

	dbg_tm_log("switch_tm_flash_end_hdl\r\n");

	if (1u == tm_id) {
		hal_light_set(LIGHT_PRIMARY, HAL_LIGHT_SET_ON);
	} else if (2u == tm_id) {
		m_app_light.run_status = RUN_STATUS_NORMAL;
		(void)osal_timer_stop(&m_tim_flash);
		(void)osal_timer_delete(&m_tim_flash);
		app_light_set_lot(SWITCH_TM_LON_MS);
	} else {
		/* not support now */
	}
}

static void switch_tm_flash_off1_hdl(void *arg)
{
	UNUSED(arg);
	dbg_tm_log("switch_tm_flash_off1_hdl\r\n");

	hal_light_set(LIGHT_PRIMARY, HAL_LIGHT_SET_ON);
	osal_timer_change_callback(&m_tim_flash, switch_tm_flash_on1_hdl);
	(void)osal_timer_change_period(&m_tim_flash, SWITCH_TM_FLASH_MS);
	(void)osal_timer_start(&m_tim_flash);
}

/* light on timeout, then turn off light. */
static void switch_tm_flash_on1_hdl(void *arg)
{
	static u8 flash_time = 0;

	dbg_tm_log("switch_tm_flash_on1_hdl\r\n");

	hal_light_set(LIGHT_PRIMARY, HAL_LIGHT_SET_OFF);
	flash_time++;
	if (flash_time >= (u32)arg) {
		osal_timer_change_callback(&m_tim_flash, switch_tm_flash_end_hdl);
		(void)osal_timer_change_period(&m_tim_flash, SWITCH_TM_LOFF_MS);
	} else {
		osal_timer_change_callback(&m_tim_flash, switch_tm_flash_off1_hdl);
		(void)osal_timer_change_period(&m_tim_flash, SWITCH_TM_FLASH_MS);
	}
	(void)osal_timer_start(&m_tim_flash);
}

/**
 * @brief test mode 1: flash once then keep on
 *        test mode 2: flash twice then detecting
 * @param id test mode id
 */
static void entry_switch_tm(u8 id)
{
	void *arg = NULL;

	if (1U == id) {
		logi("tm1...\r\n");
		m_app_light.run_status = RUN_STATUS_SWITCH_TM1;
		arg = (void *)1U;
	} else if (2U == id) {
		logi("tm2...\r\n");
		m_app_light.run_status = RUN_STATUS_SWITCH_TM2;
		arg = (void *)2U;
	} else {
		/* Invalid test mode */
	}

	hal_light_set(LIGHT_PRIMARY, HAL_LIGHT_SET_ON);
	OSAL_ASSERT_ERR(osal_timer_create(&m_tim_flash, OSAL_TIMER_MODE_SINGLE,
	                                  switch_tm_flash_on1_hdl, arg, SWITCH_TM_FLASH_MS),
	                "Create tm timer failed!\r\n");
	OSAL_ASSERT_ERR(osal_timer_start(&m_tim_flash), "Start tm timer failed!\r\n");
}

static void switch_tm_entry(void)
{
	u8 tm_cnt;

	logi("%s\r\n", __func__);

	/* get stored data from nv. */
	tm_cnt = switch_tm_proc(SWITCH_TM_CLR_TO_MS);

	if ((tm_cnt >= SWITCH_TM1_CNT_MIN) && (tm_cnt <= SWITCH_TM1_CNT_MAX)) {
#ifdef CFG_PWRON_SELF_CHECK
		self_check_exit();
#endif
		entry_switch_tm(1U);
	} else if ((tm_cnt >= SWITCH_TM2_CNT_MIN) && (tm_cnt <= SWITCH_TM2_CNT_MAX)) {
#ifdef CFG_PWRON_SELF_CHECK
		self_check_exit();
#endif
		entry_switch_tm(2U);
	} else if (tm_cnt > SWITCH_TM2_CNT_MAX) {
		(void)tm_clr_powerup_time();
		dbg_tm_log("clr nv data\r\n");
	}
}
#endif /* CFG_SWITCH_TM */

#ifdef CFG_IR
void app_light_always_on(void)
{
	light_ctrl_switch(1u, 0x7fffffffu);
}

void app_light_always_off(void)
{
	app_light_set_standby_level(0u);
	light_ctrl_switch(0u, 0u);
}

void app_light_set_intensity(u16 intensity)
{
#ifdef CFG_LED_PWM
	hal_pwm_duty_set(LIGHT_PRIMARY, intensity);
#endif
	if (1u == hal_light_get_status(LIGHT_PRIMARY)) {
		light_ctrl_switch(1u, 0x7fffffffu);
	}
}

void app_light_set_standby_level(u16 level)
{
#ifdef CFG_LED_PWM
	hal_pwm_standby_duty_set(LIGHT_PRIMARY, level);
#endif
	if (0u == hal_light_get_status(LIGHT_PRIMARY)) {
		light_ctrl_switch(0u, 0u);		/* flash standby level if light is off */
	}
}
#endif

#if (defined(CFG_LS) && defined(CFG_LED_PWM) && (PARA_PWM_OFF_DAYTIME == 1))
void app_light_off_daytime_proc(void)
{
	u16 duty;
#if (PARA_PWM_LUMACYCLE != LED_LUMACYCLE_LIGHT_NONE)
	if (1u == hal_led_lumacycle_get_en(LIGHT_PRIMARY)) {
		return;
	}
#endif
	if ((hal_light_get_status(LIGHT_PRIMARY) == 0u)) {
		duty = (hal_ls_get_status() == 0u) ? 0u : (u16)PARA_PWM_LOFF_DUTY;
		hal_pwm_set(LIGHT_PRIMARY, duty);
	}
}
#endif

#if defined(CFG_OS) && defined(CFG_LS)
static void light_status_detect(void *arg)
{
	UNUSED(arg);
	(void)hal_ls_detect();
}
#endif

#ifdef FUNC_RAD_DET_BLINK
static void blink_timeout_hdl(void)
{
	hal_blink_set(0);
	rad_timer_stop(RAD_TIMER_BLINK);
}

static void rad_detect_blink(void)
{
	hal_blink_set(1);
	rad_set_timer(RAD_TIMER_BLINK, 500, blink_timeout_hdl);
}
#endif

#if defined(CFG_PRESENCE_DET) && defined(CFG_PRINT_BHR_DATA)
static void bhr_data_cb_func(u16 *buf, u16 size)
{
	u16 i = 0;
	u16 val = 0;

	if (size == 0 ) {
		return;
	}

	do {
		val = *(buf+i);
		at_printf("\r\n%04x", (u16)val);
	} while(++i < size);
}
#endif

static void light_para_init(void)
{
	app_light_set_lot(PARA_LIGHT_ON_TIME_MS);
}

#if defined(CFG_BHR_DET_IND) && defined(BHR_DET_IND_PERIOD)
static sys_timer_t *m_bhr_ind_tim = NULL;
static u8 m_bhr_ind_flag = 0;

static void bhr_ind_set_flag(u8 flag)
{
	m_bhr_ind_flag = flag;
}

static void bhr_ind_time_hdl(u32 arg)
{
	if (m_bhr_ind_flag) {
		light_ctrl_switch_aux(1u, AUX_LIGHT_TIME_MS);
		bhr_ind_set_flag(0);
	}
}
#endif

static bool check_mdet_pre_condition(void)
{
#ifdef CFG_LS
#if (PARA_LS_TYPE_UVL == 1)
	if (hal_ls_get_status() == 0) {
		return FALSE;
	}
#elif (PARA_LS_DETECT_INVALID == 0)
	if ((hal_light_get_status(LIGHT_PRIMARY) == 0u) && (hal_ls_get_status() == 0u)) {
		return FALSE;
	}
#endif /* (PARA_LS_TYPE_UVL == 1) */

#ifdef LS_CHECK_DITHER
	if (hal_ls_get_dither_flag() == 1) {
		hal_ls_clr_dither_flag();
		return FALSE;
	}
#endif

#endif /* CFG_LS */

#ifdef CFG_IR
	u8 light_mode = get_ir_para(IR_PARA_MODE);
	if ((light_mode == (u8)IR_MODE_OFF) || (light_mode == (u8)IR_MODE_ON)) {
		return FALSE;
	}
#endif

	if (radar_mgr_get_pwr_status() == 0u) {
		return FALSE;
	}

#ifdef CFG_SWITCH_TM
	if ((m_app_light.run_status == RUN_STATUS_SWITCH_TM1)
	    || (m_app_light.run_status == RUN_STATUS_SWITCH_TM2)) {
		return FALSE;
	}
#endif

	return TRUE;
}

static app_light_mode_t check_app_light_mode(void)
{
#ifdef CFG_PWRON_SELF_CHECK
	if (self_check_get_status() == SC_STATUS_RUNNING) {
		return APP_LIGHT_MODE_SELF_CHECK;
	}
#endif

#ifdef CFG_DIAGNOSE_EN
	if (is_diagnosing()) {
		return APP_LIGHT_MODE_DIAGNOSE;
	}
#endif

	return APP_LIGHT_MODE_NORMAL;
}

#ifdef CFG_ATMESH
__STATIC_INLINE void post_det_msg(void)
{
	osal_msg_t msg;

	msg.id = (u32)APP_RADAR_LIGHT_ONOFF_IND;
	msg.para = 1U;
	OSAL_ASSERT_ERR(osal_task_send_msg(MOD_WCN, &msg, OSAL_WAIT_FOREVER),
	                "send detection status failed!\r\n");
}

static void event_detect_timeout(void *arg)
{
	UNUSED(arg);

	switch (m_det_status) {
	case DET_STATUS_DETECTED:
		m_det_status = DET_STATUS_IDLE;
		break;
	case DET_STATUS_DETECT_CONTINUE:
		m_det_status = DET_STATUS_DETECTED;
		post_det_msg();
		light_ctrl_switch(1U, m_app_light.lot_ms);
		OSAL_ASSERT_ERR(osal_timer_start(&m_det_post_tm),
		                "start post det timer failed!\r\n");
		break;
	default:
		/* Nothing */
		break;
	}
}
#endif /* CFG_ATMESH */

static void event_detected(void)
{
#ifdef RAD_DET_ONE_SHOT
	if (get_light_state() == 1U) {
		return;
	}
#endif

#ifdef CFG_SWITCH_ONOFF_MODE
	light_ctrl_switch_onoff(1u);
#else
	light_ctrl_switch(1u, m_app_light.lot_ms);
#endif

#ifdef CFG_ATMESH
	switch (m_det_status) {
	case DET_STATUS_IDLE:
		m_det_status = DET_STATUS_DETECTED;
		post_det_msg();
		OSAL_ASSERT_ERR(osal_timer_start(&m_det_post_tm),
		                "start post det timer failed!\r\n");
		break;
	case DET_STATUS_DETECTED:
		m_det_status = DET_STATUS_DETECT_CONTINUE;
		OSAL_ASSERT_ERR(osal_timer_restart(&m_det_post_tm),
		                "start post det timer failed!\r\n");
		break;
	default:
		/* Nothing */
		break;
	}
#endif /* CFG_ATMESH */
}

static void mdet_ret_process(u8 dat)
{
	APP_LIGHT_LOG("%s, mdet result = 0x%x\n", __func__, dat);

	if (((dat >> MDET_BIT_POS_MOTION_DET) & 0x1u) /* motion detection. */
#ifdef CFG_PRESENCE_DET
		|| ((dat >> MDET_BIT_POS_BHR_DET) & 0x1u) /* breath detection. */
#endif
#ifdef CFG_MICRO_DET
		|| ((dat >> MDET_BIT_POS_MICRO_DET) & 0x1u) /* micro-detection. */
#endif
	) {
		event_detected();
	}

	/* breath detection indication. */
#if defined(CFG_PRESENCE_DET) && defined(CFG_BHR_DET_IND)
	if ((det >> MDET_BIT_POS_BHR_DET_RT) & 0x1u) {
#ifdef BHR_DET_IND_PERIOD
		bhr_ind_set_flag(1);
#else
		light_ctrl_switch_aux(1u, AUX_LIGHT_TIME_MS);
#endif
	}
#endif
}

static void app_light_mode_normal(void)
{
	u8 det = mdet_proc();

	mdet_ret_process(det);
}

void app_light_init(void)
{
	light_ctrl_init();

	light_para_init();

	radar_mgr_mdet_user_cfg_set(PARA_MOT_DIST_LV);

#ifdef CFG_PRESENCE_DET
	radar_mgr_bhr_user_cfg_set(PARA_BHR_DIST_LV);
#endif

#ifdef CFG_PWRON_SELF_CHECK
	/* entry self check process after hardware init. */
	self_check_entry(1, PARA_SELF_CHECK_TIME_MS);
#endif

#ifdef CFG_SWITCH_TM
	switch_tm_entry();
#endif

#ifdef CFG_ADC_KEY
	adc_key_func_reg();
#endif

#ifdef CFG_DIAGNOSE_EN
	diagnose_entry();                        /* Start self-diagnosis */
#endif

#if defined(CFG_BHR_DET_IND) && defined(BHR_DET_IND_PERIOD)
	m_bhr_ind_tim = create_timer("bhr_ind_tim", TIMER_OPT_PERIODIC, bhr_ind_time_hdl, 0);
	start_timer(m_bhr_ind_tim, 1000*BHR_DET_IND_PERIOD_TIME);
#endif

	radar_mgr_reset();

#ifdef CFG_ATMESH
	OSAL_ASSERT_ERR(osal_timer_create(&m_det_post_tm, OSAL_TIMER_MODE_SINGLE, event_detect_timeout,
	                                  NULL, 2000U),
	                "create post det timer failed!\r\n");
#endif /* CFG_ATMESH */

#if defined(CFG_OS) && defined(CFG_LS)
	osal_assert_create("tm-ls", osal_timer_create(&m_ls_tm, OSAL_TIMER_MODE_PERIODIC,
	                                              light_status_detect, NULL, 200U));
	(void)osal_timer_start(&m_ls_tm);
#endif
	radar_mgr_worker_run();

}

bool app_light_msg_handler(osal_msg_t *msg)
{
	bool ret = TRUE;

	switch (msg->id) {
	case MSG_APP_COMMON_MDET_RET:
		if (FALSE == check_mdet_pre_condition()) {
			m_app_light.stop_read_adc = 1;
			break;
		}
#ifdef CFG_PWRON_SELF_CHECK
		if (self_check_get_status() == SC_STATUS_RUNNING) {
			/* self-check running, keep adc but do not control light */
			break;
		}
#endif
		mdet_ret_process((u8)msg->para);
		break;
	case MSG_APP_LIGHT_ON:
		light_ctrl_switch(1U, m_app_light.lot_ms);
		break;
	case MSG_APP_LIGHT_OFF:
		light_ctrl_switch(0U, m_app_light.lot_ms);
		break;
	case MSG_APP_LIGHT_CTRL:
		light_ctrl_event(msg->para);
		break;
	default:
		ret = FALSE;
		break;
	}

	return ret;
}

/**
 * app_light_set_lot() - Set the keep time of light on, light will
 *   be turn off if the set time has expired.
 * @param time: The light on keep time in ms.
 * @return: none.
 */
void app_light_set_lot(u32 time)
{
	if (m_app_light.lot_ms != time) {
		m_app_light.lot_ms = time;
		if (1u == get_light_state()) {
			light_ctrl_switch(1u, time);  /* Update timer if light is on */
		}
	}
}

u32 app_light_get_lot(void)
{
	return m_app_light.lot_ms;
}

/**
 * app_light_proc - the main program for light application.
 */
void app_light_proc(void *arg)
{
	UNUSED(arg);

	if (FALSE == check_mdet_pre_condition()) {
		m_app_light.stop_read_adc = 1;
		return;
	}

	if (m_app_light.stop_read_adc == 1u) {
		m_app_light.stop_read_adc = 0u;

		/* reset radar mdet. */
		radar_mgr_reset();
	}

	switch (check_app_light_mode()) {
	case APP_LIGHT_MODE_SELF_CHECK:
		break;

	case APP_LIGHT_MODE_DIAGNOSE:
#ifdef CFG_DIAGNOSE_EN
		if (mdet_proc() > 0U) {
			diagnose_trigger_hdl();
		}
#endif
		break;

	case APP_LIGHT_MODE_NORMAL:
		app_light_mode_normal();
		break;
	default:
		/* Invalid mode */
		break;
	}
}

#endif /* !CFG_APP_LIGHT */
