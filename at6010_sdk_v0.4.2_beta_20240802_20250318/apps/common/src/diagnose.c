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

#include "sys_timer.h"
#include "gpio.h"
#include "light_ctrl.h"
#include "mdet_mgr.h"
#include "if_sample.h"
#include "radar_mgr.h"
#include "diagnose.h"

#ifdef CFG_APP_LIGHT
#include "app_light.h"
#endif

#ifdef CFG_SELF_DIAGNOSE

#define GPIO_TOGGLE_MS              (22u)                   /* toggle peroid */
#define DIAGNOSE_ON_MS              (2000u)                 /* maximum diagnose time of one cycle. */
#define DIAGNOSE_OFF_MS             (5000u)                 /* stop diagnose time, it's radar normal working time. */
#define DIAGNOSE_NG_MS              (DIAGNOSE_ON_MS * 2)    /* The trigger time of radar failure, it must be longer than the diagnosis time. */
#define DIAGNOSE_SKIPS              (50)                    /* skip some adc points before motion detection. */

typedef struct {
	u8 state;
	u16 save_delta;
	sys_timer_t *heartbeat_tm;   /* heartbeat toggle timer. */
	sys_timer_t *diag_tm;        /* heartbeat diagnose timer. */
} self_diag_t;

typedef enum {
	SYS_DIAGNOSE_STATE_STOPPED = 0,
	SYS_DIAGNOSE_STATE_RUNNING = 1,
	SYS_DIAGNOSE_STATE_FAILED = 2,
	SYS_DIAGNOSE_STATE_OK = 3
} sys_diagnose_state_t;

static self_diag_t m_diag_dev = { 0 };


/**
 * Initialize the toggle pin to driver heartbeat LED.
 */
static void diagnose_init_hook (void)
{
	gpio_toggle_init(GPIO_DIAGNOSE_PIN);
	gpio_set_pull_down(GPIO_DIAGNOSE_PIN, 0);
}

/**
 * re-config parameters for the motion detection before start diagnosis.
 */
static void diagnose_start_hook (void)
{
	mdet_stop_proc_data();
	mdet_proc_reset(0);
	m_diag_dev.save_delta = mdet_time_get_delta();
	mdet_time_set_delta(DIAGNOSE_ADC_DELTA);
	mdet_time_set_skip_num(DIAGNOSE_SKIPS);
	mdet_start_proc_data();
}

/**
 * restore parameters for the normal motion detection.
 */
static void diagnose_stop_hook (void)
{
	/* set diagnose pin to low.  */
	gpio_set_value(GPIO_DIAGNOSE_PIN, (s32)0);

	mdet_stop_proc_data();
	mdet_proc_reset(0);
	mdet_time_set_delta(m_diag_dev.save_delta); /* restore delta. */
	mdet_time_set_skip_num(DIAGNOSE_SKIPS);
	mdet_start_proc_data();
}

/**
 * processing for radar failure.
 */
static void diagnose_failed_hook (uint32 ms)
{
	/* light on if radar failure. */
	light_ctrl_switch(1u, ms);

	/* stop motion detection and re-initialize radar. */
	mdet_stop_proc_data();
	radar_mgr_init();
}

/**
 * toggle gpio to drive LED for diagnosis.
 */
static void diagnose_heartbeat_hook (void)
{
	gpio_toggle(GPIO_DIAGNOSE_PIN);
}

/**
 * Change diagnose state.
 */
static void set_diagnose_state(sys_diagnose_state_t state)
{
	m_diag_dev.state = state;
}

/**
 * Radar failure handling function.
 */
static void diagnose_failed_hdl(u32 delay)
{
	diagnose_failed_hook(delay);
	diagnose_start_hook();
	start_timer(m_diag_dev.diag_tm, DIAGNOSE_ON_MS);
}

/**
 * Diagnose timer expired handler, include noraml mdet and diagnose mdet.
 */
static void tm_diagnose_expired_hdl(u32 arg)
{
	(void)arg;
	if (m_diag_dev.state == SYS_DIAGNOSE_STATE_RUNNING) {
		/* set diagnose failure if no moving object detected. */
		diagnose_failed_hdl(DIAGNOSE_NG_MS);
	} else { /* restart diagnose mdet. */
		diagnose_start_hook();
		set_diagnose_state(SYS_DIAGNOSE_STATE_RUNNING);
		start_timer(m_diag_dev.heartbeat_tm, GPIO_TOGGLE_MS);
		start_timer(m_diag_dev.diag_tm, DIAGNOSE_ON_MS);
	}
}

/**
 * Heartbeat timer(to toggle LED) expired handler.
 */
static void tm_heartbeat_expired_hdl(u32 arg)
{
	(void)arg;
	diagnose_heartbeat_hook();
}

/**
 * Check if the diagnosis is running, return ture if it is running,
 * else return false.
 */
bool is_diagnosing(void)
{
	return (m_diag_dev.state == SYS_DIAGNOSE_STATE_RUNNING);
}

/**
 *  The diagnose trigger handler for a moving object detected.
 */
void diagnose_trigger_hdl(void)
{
	stop_timer(m_diag_dev.diag_tm);
	stop_timer(m_diag_dev.heartbeat_tm);
	set_diagnose_state(SYS_DIAGNOSE_STATE_OK);
	diagnose_stop_hook();

	/* start normal motion detection timer. */
	start_timer(m_diag_dev.diag_tm, DIAGNOSE_OFF_MS);
}

void diagnose_entry(void)
{
	if (m_diag_dev.state == SYS_DIAGNOSE_STATE_STOPPED) {
		m_diag_dev.heartbeat_tm = create_timer("tm-heartbeat", (u8)TIMER_OPT_PERIODIC, tm_heartbeat_expired_hdl, 0);
		m_diag_dev.diag_tm = create_timer("tm-diagnose", (u8)TIMER_OPT_ONE_SHOT, tm_diagnose_expired_hdl, 0);
		m_diag_dev.save_delta = mdet_time_get_delta();

		diagnose_init_hook();
		start_timer(m_diag_dev.heartbeat_tm, GPIO_TOGGLE_MS);
		start_timer(m_diag_dev.diag_tm, DIAGNOSE_ON_MS);
		diagnose_start_hook();
		set_diagnose_state(SYS_DIAGNOSE_STATE_RUNNING);
	}
}

void diagnose_exit(void)
{
	if (m_diag_dev.state != SYS_DIAGNOSE_STATE_STOPPED) {
		stop_timer(m_diag_dev.diag_tm);
		stop_timer(m_diag_dev.heartbeat_tm);
		del_timer(m_diag_dev.diag_tm);
		del_timer(m_diag_dev.heartbeat_tm);
		m_diag_dev.diag_tm = m_diag_dev.heartbeat_tm = NULL;
		diagnose_stop_hook();
		set_diagnose_state(SYS_DIAGNOSE_STATE_STOPPED);
	}
}

#endif
