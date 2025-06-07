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
#include "at_common.h"
#include "atlog.h"
#include "cpu.h"
#include "gpio.h"

/* sys timer state definition */
typedef enum {
    SYS_TIMER_STATE_STOPPED = 0,
    SYS_TIMER_STATE_EXPIRED = 1,
    SYS_TIMER_STATE_RUNNING = 2
} sys_timer_state_t;

/* sys timer dev structure definition */
typedef struct {
	sys_timer_t all_timers[MAX_NUM_TIMERS];

	struct list_head free_tim_list; /* free timers list head */
	struct list_head running_tim_list; /* running timers list head */
	struct list_head stopped_tim_list; /* stopped timers list head */
} sys_timer_dev_t;

static sys_timer_dev_t m_sys_tim_dev = {
	.all_timers = { 0 },
	.free_tim_list = {0, 0},
	.running_tim_list = {0, 0},
	.stopped_tim_list = {0, 0}
};


/**
 *
 * @brief    process timer expired.
 *
 * @note     This function handle expired timer.
 * Sys timers can get from m_sys_tim_dev.free_tim_list, when alloc
 * a new soft-timer, it will be added to m_sys_tim_dev.running_tim_list
 * or m_sys_tim_dev.stopped_tim_list. All soft-timers are triggered
 * by a real hardware timer(sys tick, period 1ms)
 *
 */
static void process_timer_expired(sys_timer_t *timer)
{
	/* Only handle timer which state is SYS_TIMER_STATE_EXPIRED */
	if (timer->state == SYS_TIMER_STATE_EXPIRED) {
		/* Stop timer if timer option is TIMER_OPT_ONE_SHOT */
		if (timer->opt == TIMER_OPT_ONE_SHOT) {
			/* Move to stop list */
			critical_section_start();
			timer->state = SYS_TIMER_STATE_STOPPED;
			list_move_tail(&timer->entry, &m_sys_tim_dev.stopped_tim_list);
			critical_section_end();
		}
		/* Restart timer if timer option is TIMER_OPT_PERIODIC */
		else if(timer->opt == TIMER_OPT_PERIODIC) {
			timer->delay = timer->period;
			timer->state = SYS_TIMER_STATE_RUNNING;
		}

		/* Execute callback function */
		if (timer->callback != NULL) {
			(*timer->callback)(timer->arg);
		}
	}
}

#ifdef SUPPORT_SYS_TIMER_MSG
static void process_timer_expired_msg(msg_t *msg)
{
	sys_timer_t *timer = (sys_timer_t *)msg->para;

	/* Only handle timer which state is SYS_TIMER_STATE_EXPIRED */
	if (timer->state == SYS_TIMER_STATE_EXPIRED) {
		/* Stop timer if timer option is TIMER_OPT_ONE_SHOT */
		if (timer->opt == TIMER_OPT_ONE_SHOT) {
			/* Move to stop list */
			critical_section_start();
			timer->state = SYS_TIMER_STATE_STOPPED;
			list_move_tail(&timer->entry, &m_sys_tim_dev.stopped_tim_list);
			critical_section_end();
		}
		/* Restart timer if timer option is TIMER_OPT_PERIODIC */
		else if(timer->opt == TIMER_OPT_PERIODIC) {
			timer->delay = timer->period;
			timer->state = SYS_TIMER_STATE_RUNNING;
		}

		/* Execute callback function */
		if (timer->callback != NULL) {
			(*timer->callback)(timer->arg);
		}
	}
}
#endif


void sys_timer_period(void)
{
	sys_timer_t *timer;

	critical_section_start();

	if (list_empty(&m_sys_tim_dev.running_tim_list)) {
		critical_section_end();
		return;
	}

	list_for_each_entry(timer, &m_sys_tim_dev.running_tim_list, sys_timer_t, entry) {
		/* Only count down timer which state is SYS_TIMER_STATE_RUNNING */
		if ((NULL != timer) && (timer->state == SYS_TIMER_STATE_RUNNING)) {
			if (--timer->delay == 0) {
				/* Change state to SYS_TIMER_STATE_EXPIRED when count down to 0 */
				timer->state = SYS_TIMER_STATE_EXPIRED;
				k_msg_post(MSG_ID_TIMER_EXPIRED, &timer->msg);
			}
		}
	}

	critical_section_end();
}

void sys_timer_init(void)
{
	u32 i = 0;

	/* Initialized free, running and stop list */
	INIT_LIST_HEAD(&m_sys_tim_dev.free_tim_list);
	for (i = 0; i < ARRAY_SIZE(m_sys_tim_dev.all_timers); i++) {
		list_add(&m_sys_tim_dev.all_timers[i].entry, &m_sys_tim_dev.free_tim_list);
	}

	INIT_LIST_HEAD(&m_sys_tim_dev.stopped_tim_list);
	INIT_LIST_HEAD(&m_sys_tim_dev.running_tim_list);

#ifdef SUPPORT_SYS_TIMER_MSG
	k_msg_reg_hdl(MSG_ID_TIMER_EXPIRED, process_timer_expired_msg);
#endif
}

sys_timer_t * create_timer(const char *name, u8 opt, void (*callback)(u32 arg), u32 arg)
{
	sys_timer_t *timer = NULL;

	/* Check free list */
	critical_section_start();

	timer = list_first_entry(&m_sys_tim_dev.free_tim_list, sys_timer_t, entry);
	if (timer == NULL) {
		loge("Create '%s', timer exhaust!\r\n", name);

		critical_section_end();

		return timer;
	}

	/* Initialize sys_timer_t structure */
	timer->callback = callback;
	timer->arg = arg;
	timer->delay = 0;
	timer->period = 0;
	timer->opt = opt;
	timer->state = SYS_TIMER_STATE_STOPPED;

	/* Initialize message structure */
	timer->msg.para = (void *)timer;

	/* Delete from free list and add to stop list */
	list_move_tail(&timer->entry, &m_sys_tim_dev.stopped_tim_list);

	critical_section_end();

	return timer;
}

s32 reconfig_timer(sys_timer_t *timer, u8 opt, void (*callback)(u32 arg), u32 arg)
{
	/* Check parameter */
	if (timer == NULL) {
		loge("%s | NULL == timer\r\n", __FUNCTION__);
		return -1;
	}

	/* Initialize sys_timer_t structure */
	critical_section_start();
	timer->callback = callback;
	timer->arg = arg;
	timer->delay = 0;
	timer->period = 0;
	timer->opt = opt;
	timer->state = SYS_TIMER_STATE_STOPPED;
	critical_section_end();

	return 0;
}

s32 start_timer(sys_timer_t *timer, u32 delay)
{
	/* Check parameter */
	if (timer == NULL || delay == 0) {
		loge("%s | NULL == timer or 0 == delay\r\n", __FUNCTION__);
		return -1;
	}

	/* Now msecs_to_jiffies(delay) is useless cause current HZ=1000*/

#if (HZ == 1000)
		timer->delay = delay;
		timer->period = delay;
#else
		timer->delay = msecs_to_jiffies(delay);
		timer->period = msecs_to_jiffies(delay);
#endif

	/* Add to running list */
	critical_section_start();
	if (timer->state == SYS_TIMER_STATE_STOPPED) {
		list_move_tail(&timer->entry, &m_sys_tim_dev.running_tim_list);
	}
	timer->state = SYS_TIMER_STATE_RUNNING;
	critical_section_end();

	return 0;
}

s32 stop_timer(sys_timer_t *timer)
{
	/* Check parameter */
	if (timer == NULL) {
		loge("Stop NULL\r\n");
		return -1;
	}

	/* Move to stop list */
	critical_section_start();
	if (timer->state != SYS_TIMER_STATE_STOPPED) {
		list_move_tail(&timer->entry, &m_sys_tim_dev.stopped_tim_list);
	}
	timer->state = SYS_TIMER_STATE_STOPPED;
	critical_section_end();

	return 0;
}

s32 del_timer(sys_timer_t *timer)
{
	/* Check parameter */
	if (timer == NULL) {
		loge("Delete NULL\r\n");
		return -1;
	}

	critical_section_start();
	/* Move from stop or running list and return to free list */
	list_move_tail(&timer->entry, &m_sys_tim_dev.free_tim_list);
	critical_section_end();

	return 0;
}

void sys_timer_workque(void)
{
	sys_timer_t *timer;

	if (list_empty(&m_sys_tim_dev.running_tim_list)) {
		return;
	}

	list_for_each_entry(timer, &m_sys_tim_dev.running_tim_list, sys_timer_t, entry) {
		/* Only count down timer which state is SYS_TIMER_STATE_RUNNING */
		if (timer->state == SYS_TIMER_STATE_EXPIRED) {
			process_timer_expired(timer);
		}
	}
}
