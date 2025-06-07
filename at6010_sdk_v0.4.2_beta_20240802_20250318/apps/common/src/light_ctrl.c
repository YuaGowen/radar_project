/**
 *******************************************************************************
 * @file    light_ctrl.c
 * @author  Airtouch Software Team
 * @brief   Light control
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

#include "light_ctrl.h"
#include "osal.h"
#include "hal_light.h"
#include "atlog.h"
#include "mdet_mgr.h"
#include "radar_mgr.h"
#include "jiffies.h"
#ifdef CFG_OS
#include "app_msg.h"
#endif

//#define DBG_LOG_LIGHT_CTRL
#define LIGHT_TIMER_INITIAL_PERIOD_MS (1000U)

#ifdef CFG_PRESENCE_DET
#define LIGHT_PWR_OFF_PROT_TIME_MS (2000u)
#else
#define LIGHT_PWR_OFF_PROT_TIME_MS (1500u)
#endif

/**
 * @defgroup LIGHT_EVENT Light event set-bit
 * @brief Light event set-bit used in OS event
 * @{
 */
#define LIGHT_EVENT_LOT_EXPIRED    (1U)
#define LIGHT_EVENT_SILENT_EXPIRED (1U << 1)
#define LIGHT_EVENT_DIM_EXPIRED    (1U << 2)
#define LIGHT_EVENT_SWITCH_ON      (1U << 3)
#define LIGHT_EVENT_SWITCH_OFF     (1U << 4)
#define LIGHT_EVENT_BLINK          (1U << 5)
/**
 * @}
 */

/**
 * @brief Current light state
 */
typedef enum {
	LIGHT_STATE_ON,
	LIGHT_STATE_SILENT,
#if (defined(CFG_LED_PWM) && (PARA_PWM_DIM_OFF == 1))
	LIGHT_STATE_DIM,
#endif
	LIGHT_STATE_BLINK,
	LIGHT_STATE_OFF
} light_state_t;

typedef struct {
	u8 id;
	light_state_t state;
	osal_timer_t tm_lot;
#ifdef CFG_OS
	u32 ms;
	osal_event_t event;
	osal_task_t *task;
#if (PARA_PWM_LUMACYCLE != LED_LUMACYCLE_LIGHT_NONE)
	osal_timer_t tm_luma;
#endif
#endif /* CFG_OS */
} light_ctrl_t;

static light_ctrl_t m_light_ctrl_dev[LIGHT_NUM];

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/

static void enter_silent_state(u8 id);
#if (defined(CFG_LED_PWM) && (PARA_PWM_DIM_OFF == 1))
static void enter_dim_state(u8 id);
#endif

static void tm_lot_expired_hdl(void *arg);

/*******************************************************************************
**                      Private Function Definitions                          **
*******************************************************************************/

__STATIC_INLINE void set_light_state(u8 id, light_state_t state)
{
#if (PARA_PWM_LUMACYCLE != LED_LUMACYCLE_LIGHT_NONE)
	(void)osal_timer_start(&m_light_ctrl_dev[id].tm_luma);
#endif
	m_light_ctrl_dev[id].state = state;
}

static void tm_blink_hdl(void *arg)
{
	u8 id = *(u8 *)arg;
	hal_light_set(id, 1U ^ hal_light_get_status(id));
}

/**
 * * Light_ctrl events
 */

static void event_lot_expired(u8 id)
{
#ifdef DBG_LOG_LIGHT_CTRL
	logi("%s.\r\n", __func__);
#endif

#if (defined(CFG_LED_PWM) && (PARA_PWM_DIM_OFF == 1))
	enter_dim_state(id);
#else
	enter_silent_state(id);
#endif
}

static void event_silent_expired(u8 id)
{
#ifdef DBG_LOG_LIGHT_CTRL
	logi("%s.\r\n", __func__);
#endif

	mdet_start_proc_data();
	osal_timer_change_callback(&m_light_ctrl_dev[id].tm_lot, tm_lot_expired_hdl);
	set_light_state(id, LIGHT_STATE_OFF);
}

#if (defined(CFG_LED_PWM) && (PARA_PWM_DIM_OFF == 1))
static void event_dim_expired(u8 id)
{
	enter_silent_state(id);
}
#endif /* (defined(CFG_LED_PWM) && (PARA_PWM_DIM_OFF == 1)) */

static void event_light_blink(u8 id, u32 ms)
{
	mdet_stop_proc_data();
	hal_light_set(id, HAL_LIGHT_SET_ON);
	osal_timer_change_callback(&m_light_ctrl_dev[id].tm_lot, tm_blink_hdl);
	osal_timer_change_mode(&m_light_ctrl_dev[id].tm_lot, OSAL_TIMER_MODE_PERIODIC);
	(void)osal_timer_change_period(&m_light_ctrl_dev[id].tm_lot, ms);
	set_light_state(id, LIGHT_STATE_BLINK);
}

/**
 * * Light_ctrl timer callbacks
 */

static void tm_lot_expired_hdl(void *arg)
{
	u8 id = *(u8 *)arg;
#ifdef CFG_OS
	(void)osal_event_set(&m_light_ctrl_dev[id].event, LIGHT_EVENT_LOT_EXPIRED);
#else
	event_lot_expired(id);
#endif
}

static void tm_silent_expired_hdl(void *arg)
{
	u8 id = *(u8 *)arg;
#ifdef CFG_OS
	(void)osal_event_set(&m_light_ctrl_dev[id].event, LIGHT_EVENT_SILENT_EXPIRED);
#else
	event_silent_expired(id);
#endif
}

#if (defined(CFG_LED_PWM) && (PARA_PWM_DIM_OFF == 1))
static void tm_dim_expired_hdl(void *arg)
{
	u8 id = *(u8 *)arg;
#ifdef CFG_OS
	(void)osal_event_set(&m_light_ctrl_dev[id].event, LIGHT_EVENT_DIM_EXPIRED);
#else
	event_dim_expired(id);
#endif
}
#endif /* (defined(CFG_LED_PWM) && (PARA_PWM_DIM_OFF == 1)) */

static void restart_tm_lot(u8 id, u32 ms, osal_timer_cb_t hdl)
{
	if (hdl != NULL) {
		osal_timer_change_callback(&m_light_ctrl_dev[id].tm_lot, hdl);
	}
	if (OSAL_OK != osal_timer_change_period(&m_light_ctrl_dev[id].tm_lot, ms)) {
		loge("Update timer lot failed!\r\n");
	}
	if (OSAL_OK != osal_timer_start(&m_light_ctrl_dev[id].tm_lot)) {
		loge("Start timer lot failed!\r\n");
	}
}

static void enter_silent_state(u8 id)
{
	radar_mgr_skip_num(PARA_SKIP_NUM_LIGHT_ONOFF);

	hal_light_set(id, HAL_LIGHT_SET_OFF);

	/* set light off time(silent time). */
	restart_tm_lot(id, LIGHT_PWR_OFF_PROT_TIME_MS, tm_silent_expired_hdl);
	mdet_stop_proc_data();
	mdet_proc_reset(0);
	set_light_state(id, LIGHT_STATE_SILENT);
}

#if (defined(CFG_LED_PWM) && (PARA_PWM_DIM_OFF == 1))
__STATIC_INLINE u32 light_lumacycle_dim_time(u8 id)
{
#if (PARA_PWM_LUMACYCLE & LED_LUMACYCLE_LIGHT_OFF)
	return (hal_pwm_duty_get(id) - (u32)PARA_PWM_LDIM_DUTY) * LED_LUMACYCLE_STEP_TIME_MS /
	       LED_LUMACYCLE_STEP;
#else
	UNUSED(id);
	return 0u;
#endif
}

static void enter_dim_state(u8 id)
{
	hal_light_set(id, HAL_LIGHT_SET_DIM);
	restart_tm_lot(id, (u32)PARA_LIGHT_DIM_TIME_MS + light_lumacycle_dim_time(id),
	               tm_dim_expired_hdl);
	set_light_state(id, LIGHT_STATE_DIM);
}
#endif /* (defined(CFG_LED_PWM) && (PARA_PWM_DIM_OFF == 1)) */

__STATIC_INLINE u32 light_lumacycle_on_time(u8 id)
{
#if (PARA_PWM_LUMACYCLE & LED_LUMACYCLE_LIGHT_ON)
	return ((u32)PARA_PWM_LON_DUTY - hal_pwm_duty_get(id)) * LED_LUMACYCLE_STEP_TIME_MS /
	       LED_LUMACYCLE_STEP;
#else
	(void)id;
	return 0u;
#endif
}

#if (PARA_PWM_LUMACYCLE != LED_LUMACYCLE_LIGHT_NONE)
static void tm_luma_hdl(void *arg)
{
	u8 id = *(u8 *)arg;
	hal_led_lumacycle_proc();
	if (0U == hal_led_lumacycle_get_en(id)) {
		(void)osal_timer_stop(&m_light_ctrl_dev[id].tm_luma);
	}
}
#endif /* (PARA_PWM_LUMACYCLE != LED_LUMACYCLE_LIGHT_NONE) */

static void event_light_on(u8 id, u32 ms)
{
	switch (m_light_ctrl_dev[id].state) {
	case LIGHT_STATE_ON:
		hal_light_set(id, HAL_LIGHT_SET_ON);
		if (ms != 0xffffffffU) {
			restart_tm_lot(id, ms, NULL);
		}
		break;
	case LIGHT_STATE_SILENT:
	case LIGHT_STATE_BLINK:
		/* Keep, do nothing */
		break;
	default:
		radar_mgr_skip_num(PARA_SKIP_NUM_LIGHT_ONOFF);
		hal_light_set(id, HAL_LIGHT_SET_ON);
		set_light_state(id, LIGHT_STATE_ON);
		if (ms != 0xffffffffU) { /* paras 0xffffffff means light on always. */
			restart_tm_lot(id, ms + light_lumacycle_on_time(id), tm_lot_expired_hdl);
		}
		break;
	}
}

static void event_light_off(u8 id)
{
	switch (m_light_ctrl_dev[id].state) {
	case LIGHT_STATE_ON:
		radar_mgr_skip_num(PARA_SKIP_NUM_LIGHT_ONOFF);
		hal_light_set(id, HAL_LIGHT_SET_OFF);
		(void)osal_timer_stop(&m_light_ctrl_dev[id].tm_lot);
		set_light_state(id, LIGHT_STATE_OFF);
		break;
#if (defined(CFG_LED_PWM) && (PARA_PWM_DIM_OFF == 1))
	case LIGHT_STATE_DIM:
		enter_silent_state(id);
		break;
#endif
	case LIGHT_STATE_SILENT:
	case LIGHT_STATE_OFF:
		hal_light_set(id, HAL_LIGHT_SET_OFF);
		break;
	case LIGHT_STATE_BLINK:
		if (0U != hal_light_get_status(id)) {
			radar_mgr_skip_num(PARA_SKIP_NUM_LIGHT_ONOFF);
			hal_light_set(id, HAL_LIGHT_SET_OFF);
		}
		mdet_start_proc_data();
		(void)osal_timer_stop(&m_light_ctrl_dev[id].tm_lot);
		osal_timer_change_mode(&m_light_ctrl_dev[id].tm_lot, OSAL_TIMER_MODE_SINGLE);
		set_light_state(id, LIGHT_STATE_OFF);
		break;
	default:
		loge("[%s]state not exists!\n\r", __func__);
		break;
	}
}

__STATIC_INLINE void light_ctrl_shift(u8 id, u8 onoff, u32 ms)
{
	if (1u == onoff) {
#ifdef CFG_OS
		m_light_ctrl_dev[id].ms = ms;
		(void)osal_event_set(&m_light_ctrl_dev[id].event, LIGHT_EVENT_SWITCH_ON);
#else
		event_light_on(id, ms);
#endif /* CFG_OS */
	} else {
#ifdef CFG_OS
		(void)osal_event_set(&m_light_ctrl_dev[id].event, LIGHT_EVENT_SWITCH_OFF);
#else
		event_light_off(id);
#endif /* CFG_OS */
	}
}

#ifdef CFG_OS
void light_proc(void *arg)
{
	u8 id = *(u8 *)arg;
	u32 events = 0U;
	osal_event_wait_t event_cfg;

	event_cfg.wait_bits = 0x00FFFFFFU;
	event_cfg.wait_ms = OSAL_WAIT_FOREVER;
	event_cfg.clear = EVENT_WAIT_CLEAR_ON_EXIT;
	event_cfg.mode = EVENT_WAIT_ANY_BITS;

	for (;;) {
		if (0U == events) {
			(void)osal_event_wait(&m_light_ctrl_dev[id].event, &event_cfg, &events);
		}

		if (0U != (events & LIGHT_EVENT_SWITCH_ON)) {
			event_light_on(id, m_light_ctrl_dev[id].ms);
			/* NOTE: in case of received both */
			events &= ~(LIGHT_EVENT_SWITCH_ON | LIGHT_EVENT_LOT_EXPIRED);
		} else if (0U != (events & LIGHT_EVENT_SWITCH_OFF)) {
			event_light_off(id);
			events ^= LIGHT_EVENT_SWITCH_OFF;
		} else if (0U != (events & LIGHT_EVENT_LOT_EXPIRED)) {
			event_lot_expired(id);
			events ^= LIGHT_EVENT_LOT_EXPIRED;
		} else if (0U != (events & LIGHT_EVENT_SILENT_EXPIRED)) {
			event_silent_expired(id);
			events ^= LIGHT_EVENT_SILENT_EXPIRED;
#if (defined(CFG_LED_PWM) && (PARA_PWM_DIM_OFF == 1))
		} else if (0U != (events & LIGHT_EVENT_DIM_EXPIRED)) {
			event_dim_expired(id);
			events ^= LIGHT_EVENT_DIM_EXPIRED;
#endif
		} else if (0U != (events & LIGHT_EVENT_BLINK)) {
			event_light_blink(id, m_light_ctrl_dev[id].ms);
			events ^= LIGHT_EVENT_BLINK;
		} else {
			logw("Invalid light events!\r\n");
		}
	}
}
#endif /* CFG_OS */

/*******************************************************************************
**                      Public Function Definitions                           **
*******************************************************************************/

void light_ctrl_init(void)
{
	for (u8 i = 0u; i < LIGHT_NUM; i++) {
		m_light_ctrl_dev[i].state = LIGHT_STATE_OFF;
		m_light_ctrl_dev[i].id = i;
		osal_assert_create("tm-lot",
		                   osal_timer_create(&m_light_ctrl_dev[i].tm_lot,
		                                     OSAL_TIMER_MODE_SINGLE, tm_lot_expired_hdl,
		                                     &m_light_ctrl_dev[i].id,
		                                     LIGHT_TIMER_INITIAL_PERIOD_MS));
#ifdef CFG_OS
		(void)osal_event_create(&m_light_ctrl_dev[i].event);
		m_light_ctrl_dev[i].task = osal_task_create(
		        MOD_USER_AUTO, "light", light_proc, &m_light_ctrl_dev[i].id,
		        OSAL_PRIORITY_BELOW_NORMAL, 512U, OASL_CTF_NONE);
#if (PARA_PWM_LUMACYCLE != LED_LUMACYCLE_LIGHT_NONE)
		osal_assert_create("tm-luma",
		                   osal_timer_create(&m_light_ctrl_dev[i].tm_luma,
		                                     OSAL_TIMER_MODE_PERIODIC, tm_luma_hdl,
		                                     &m_light_ctrl_dev[i].id,
		                                     LED_LUMACYCLE_STEP_TIME_MS));
#endif
#endif /* CFG_OS */
	}
}

void light_ctrl_switch(u8 onoff, u32 ms)
{
	light_ctrl_shift(LIGHT_PRIMARY, onoff, ms);
}

u8 light_ctrl_blink(u32 ms)
{
#ifdef CFG_OS
	m_light_ctrl_dev[LIGHT_PRIMARY].ms = ms;
	(void)osal_event_set(&m_light_ctrl_dev[LIGHT_PRIMARY].event, LIGHT_EVENT_BLINK);
#else
	event_light_blink(LIGHT_PRIMARY, ms);
#endif /* CFG_OS */
	return E_OK;
}

u8 light_ctrl_on(u32 ms)
{
	light_ctrl_shift(LIGHT_PRIMARY, 1U, ms);
	return E_OK;
}

u8 light_ctrl_off(void)
{
	light_ctrl_shift(LIGHT_PRIMARY, 0U, 0U);
	return E_OK;
}

#ifdef CFG_AUX_LIGHT_EN
void light_ctrl_switch_aux(u8 onoff, u32 ms)
{
	light_ctrl_shift(LIGHT_AUXILIARY, onoff, ms);
}
#endif

#ifdef CFG_SWITCH_ONOFF_MODE
void light_ctrl_switch_onoff(u8 mod)
{
	UNUSED(mod);

	u8 light_state;
	u32 now;
	static u32 last = 0;

	now = jiffies();

	if ((now - last) < 1500U) {
		return;
	}

	light_state = 1U - get_light_state();
	light_ctrl_shift(LIGHT_PRIMARY, light_state, 0xffffffffU);
	last = jiffies();
}
#endif

u8 get_light_state(void)
{
	u8 sta = 0;

	switch (m_light_ctrl_dev[LIGHT_PRIMARY].state) {
	case LIGHT_STATE_ON:
		sta = 1;
		break;

	default:
		break;
	}

	return sta;
}

void light_ctrl_event(u32 para)
{
	u8 id = (u8)(para >> 8);
	u8 event = (u8)para;
	switch (event) {
	case LIGHT_EVENT_LOT_EXPIRED:
		event_lot_expired(id);
		break;
	case LIGHT_EVENT_SILENT_EXPIRED:
		event_silent_expired(id);
		break;
#if (defined(CFG_LED_PWM) && (PARA_PWM_DIM_OFF == 1))
	case LIGHT_EVENT_DIM_EXPIRED:
		event_dim_expired(id);
		break;
#endif
	default:
		/* Invalid event */
		break;
	}
}
