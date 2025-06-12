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

#include "hal_timer.h"
#include "clock.h"
#include "hal_clock.h"
#include "at_error.h"

typedef enum {
	TIMER_STATUS_IDLE,
	TIMER_STATUS_BUSY
} timer_status_t;

typedef struct {
	timer_source_t source;
	timer_value_t value_mode;
	timer_status_t status;
} timer_cfg_info_t;

static timer_cfg_info_t m_timer_cfg[TIMER_ID_QTY];

static void sys_clk_enable_timer(timer_id_t id)
{
	switch (id) {
	case TIMER_ID_0:
		sys_clk_enable_timer0();
		break;
	case TIMER_ID_1:
		sys_clk_enable_timer1();
		break;
	case TIMER_ID_2:
		sys_clk_enable_timer2();
		break;
	case TIMER_ID_3:
		sys_clk_enable_timer3();
		break;
	default: /* Invalid */
		break;
	}
}

static void sys_clk_disable_timer(timer_id_t id)
{
	switch (id) {
	case TIMER_ID_0:
		sys_clk_disable_timer0();
		break;
	case TIMER_ID_1:
		sys_clk_disable_timer1();
		break;
	case TIMER_ID_2:
		sys_clk_disable_timer2();
		break;
	case TIMER_ID_3:
		sys_clk_disable_timer3();
		break;
	default: /* Invalid */
		break;
	}
}

static u32 hal_timer_us_to_ticks(timer_id_t id, u32 us)
{
	u32 ticks;
	u32 sys_us_ticks;
	u32 sys_clk = hal_get_system_clk();
	timer_source_t source = m_timer_cfg[id].source;

	sys_us_ticks = sys_clk / 1000000U;

	switch (source) {
	case TIMER_SOURCE_SYSTEM:
		ticks = us * sys_us_ticks;
		break;
	case TIMER_SOURCE_SYSTEM_DIV_16:
		ticks = (us * sys_us_ticks) >> 4;
		break;
	case TIMER_SOURCE_SYSTEM_DIV_32:
		ticks = (us * sys_us_ticks) >> 5;
		break;
	case TIMER_SOURCE_SYSTEM_DIV_64:
		ticks = (us * sys_us_ticks) >> 6;
		break;
	case TIMER_SOURCE_EXTERNAL_32K:
		ticks = (us << 5) / 1000U;
		break;
	default:
		ticks = TIMER_VALUE_MAX;
		break;
	}

	return ticks;
}

void hal_timer_init(timer_id_t id, const timer_cfg_t *cfg)
{
	u32 reload;

	sys_clk_enable_timer(id);

	timer_init(id, cfg->source);
	m_timer_cfg[id].source = cfg->source;
	m_timer_cfg[id].value_mode = cfg->value_mode;
	m_timer_cfg[id].status = TIMER_STATUS_BUSY;

	if (0U != cfg->irq_en) {
		timer_cb_set(id, cfg->cb);
		timer_irq_enable(id);
	} else {
		timer_irq_disable(id);
	}

	reload = (cfg->reload == TIMER_VALUE_MAX) ? TIMER_VALUE_MAX :
	                                            hal_timer_us_to_ticks(id, cfg->reload);
	timer_reload_set(id, reload);
	timer_value_set(id, reload);
	if (0U != cfg->start) {
		timer_start(id);
	}
}

void hal_timer_deinit(timer_id_t id)
{
	sys_clk_disable_timer(id);
	m_timer_cfg[id].status = TIMER_STATUS_IDLE;
}

u8 hal_timer_get_free(timer_id_t *id)
{
	u8 err;
	u8 i;

	for (i = 0U; i < (u8)TIMER_ID_QTY; i++) {
		if (m_timer_cfg[i].status == TIMER_STATUS_IDLE) {
			break;
		}
	}
	if (i < (u8)TIMER_ID_QTY) {
		*id = (timer_id_t)i;
		err = E_OK;
	} else {
		*id = TIMER_ID_INVALID;
		err = E_BUSY;
	}
	return err;
}

void hal_timer_set(timer_id_t id, u32 us, timer_cb_t func)
{
	u32 reload;

	timer_stop(id);
	timer_irq_clear(id);

	reload = hal_timer_us_to_ticks(id, us);
	timer_reload_set(id, reload);
	timer_value_set(id, 0);

	timer_cb_set(id, func);
}

void hal_timer_start(timer_id_t id)
{
	timer_start(id);
}

void hal_timer_stop(timer_id_t id)
{
	timer_stop(id);
}

u32 hal_timer_value_get(timer_id_t id)
{
	u32 value = timer_value_get(id);
	return (m_timer_cfg[id].value_mode == TIMER_VALUE_DEC) ? value : (TIMER_VALUE_MAX - value);
}
