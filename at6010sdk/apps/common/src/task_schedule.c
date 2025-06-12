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

#include "jiffies.h"
#include "hal_ls.h"
#include "hal_light.h"
#include "dip_mgr.h"
#include "aux_adc_key.h"
#include "osal_task.h"
#include "osal_time.h"
#include "k_msg.h"
#include "task_schedule.h"

#ifdef CFG_OS
#include "osal.h"
#endif

#ifdef CFG_APP_LIGHT
#include "app_light.h"
#endif

#define TASK_20MS_JITTER        (2u)
#define TASK_200MS_JITTER       (8u)
#define TASK_500MS_JITTER       (12u)
#define TASK_2000MS_JITTER      (16u)

#define TASK_20MS_PERIOD        (20u)
#define TASK_200MS_PERIOD       (200u)
#define TASK_500MS_PERIOD       (500u)
#define TASK_2000MS_PERIOD      (2000u)

typedef void (*task_process_t)(void);

typedef struct {
	u16 task_jitter;
	u16 task_period;
	task_process_t process;
} task_para_t;

static void task_20ms(void);
static void task_200ms(void);
static void task_500ms(void);
static void task_2000ms(void);
osal_task_t *g_app_default_task = NULL;

static const task_para_t m_task_para[] = {
	{TASK_20MS_JITTER, TASK_20MS_PERIOD, task_20ms},
	{TASK_200MS_JITTER, TASK_200MS_PERIOD, task_200ms},
	{TASK_500MS_JITTER, TASK_500MS_PERIOD, task_500ms},
	{TASK_2000MS_JITTER, TASK_2000MS_PERIOD, task_2000ms}
};

static u16 m_task_cnt[(sizeof(m_task_para) / sizeof(task_para_t))];
static void app_task_evt_loop(void *arg)
{
	while (1) {
		k_msg_loop();
		task_schedule_proc();
		osal_msleep(1U);
	}
}

osal_status_t app_task_schedule_init(void)
{
	osal_status_t ret = 0;

	for (u8 i = 0u; i < (sizeof(m_task_para) / sizeof(task_para_t)); ++i) {
		m_task_cnt[i] = m_task_para[i].task_jitter;
	}

	g_app_default_task = osal_task_create(MOD_USER_AUTO, "default_task", app_task_evt_loop, NULL,
								OSAL_PRIORITY_NORMAL, 512U, 0);
	if (g_app_default_task == NULL) {
		loge("osal_task_create failed.\r\n");
		ret = OSAL_ERROR;
	}

	return ret;
}

void task_schedule_proc(void)
{
	static u32 jiff_old = 0u;
	u32 jiff, diff;

#ifndef CFG_OS
	jiff = jiffies();
#else
	jiff = osal_time_get_tick();
#endif

	if (jiff != jiff_old) {
		diff = (jiff < jiff_old) ? ((0xFFFFFFFFu - jiff_old) + jiff) : (jiff - jiff_old);
		jiff_old = jiff;

		for (u8 i = 0u; i < (sizeof(m_task_para) / sizeof(task_para_t)); ++i) {
			m_task_cnt[i] = (m_task_cnt[i] > diff) ? (u16)(m_task_cnt[i] - diff) : 0u;
			if (0u == m_task_cnt[i]) {
				m_task_para[i].process();
				m_task_cnt[i] = m_task_para[i].task_period;
			}
		}
	}
}

static void task_20ms(void)
{
#if (defined(CFG_LED_PWM) && (PARA_PWM_LUMACYCLE != LED_LUMACYCLE_LIGHT_NONE))
	hal_led_lumacycle_proc();
#endif
}

static void task_200ms(void)
{
#ifdef CFG_LS
	(void)hal_ls_detect();
#endif
}

static void task_500ms(void)
{
#if (defined(CFG_LS) && defined(CFG_LED_PWM) && (PARA_PWM_OFF_DAYTIME == 1))
	app_light_off_daytime_proc();
#endif
}

static void task_2000ms(void)
{
#ifdef CFG_AUX_ADC_KEY
	aux_adc_key_proc();
#endif
}
