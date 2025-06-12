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

#ifndef __SYS_TIMER_H__
#define __SYS_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_compiler.h"
#include "at_types.h"
#include "k_msg.h"


#define SUPPORT_SYS_TIMER_MSG

#define HZ                              (1000)
#define MAX_NUM_TIMERS                  (10)

/* Timer options define */
typedef enum {
    TIMER_OPT_ONE_SHOT = 0,    /* One shot, move to stop list when expired   */
    TIMER_OPT_PERIODIC = 1     /* Periodic, restart when expired             */
} timer_opt_t;

/* Soft timer structure */
typedef struct {
	struct list_head entry;

	void (*callback)(u32 arg);
	u32 arg;
	u32 delay;              /* Ticks remain to expired               */
	u32 period;             /* TIMER_OPT_PERIODIC timer interval     */
	u8 opt;                /* Timer option, TIMER_OPT_xxx           */
	u8 state;              /* Timer state, TIMER_STATE_xxx          */

	msg_t msg;
} sys_timer_t;

/**
 * @brief    Convert micro second to jiffies.
 */
__STATIC_INLINE u32 msecs_to_jiffies(const u32 ms)
{
	return (ms * HZ / 1000);
}

/**
 * @brief    Convert jiffies to milliseconds and back.
 *
 * @note     Avoid unnecessary multiplications/divisions in the
 * two most common HZ cases:
 */
__STATIC_INLINE u32 jiffies_to_msecs(const u32 j)
{
	return (j * 1000 / HZ);
}

/**
 *
 * @brief    initialize the sys timer, call once when system startup.
 *
 *
 * @note     Soft-timers can get from m_sys_tim_dev.free_tim_list, when alloc a new soft-timer,
 * it will be added to m_sys_tim_dev.running_tim_list or m_sys_tim_dev.stopped_tim_list.
 * All soft-timers are triggered by a real hardware timer(systick timer, period 1ms)
 * Note: This function must be called once during system startup after msg init;
 *
 */
void sys_timer_init(void);

/**
 *
 * @brief    create a soft timer.
 *
 * @param    name soft-timer name
 * @param    opt soft-timer work mode
 * @param    callback Pointer to callback function
 * @param    arg Callback function arg
 *
 * @note     This function move free soft-timer into m_sys_tim_dev.stopped_tim_list.
 * Soft-timer state change to SYS_TIMER_STATE_STOPPED.
 *
 */
sys_timer_t* create_timer(const char *name, u8 opt, void (*callback)(u32 arg), u32 arg);

/**
 *
 * @brief    timer parameters reconfiguration.
 *
 * @param    timer soft-timer
 * @param    opt soft-timer work mode
 * @param    callback Pointer to callback function
 * @param    arg Callback function arg
 *
 * @note     This function reconfigure paramters of the specified timer.
 *
 */
s32 reconfig_timer(sys_timer_t *timer, u8 opt, void (*callback)(u32 arg), u32 arg);

/**
 *
 * @brief    start soft timer.
 *
 * @param    timer soft-timer
 * @param    delay msec count
 *
 * @retval   0 on sucess
 * @retval   1 on Start NULL or delay = 0
 *
 * @note     This function move stop soft-timer into m_sys_tim_dev.running_tim_list.
 * Soft-timer state change to SYS_TIMER_STATE_RUNNING.
 *
 */
s32 start_timer(sys_timer_t* timer, u32 delay);

/**
 *
 * @brief    Stops the specified timer.
 *
 * @param    timer soft-timer
 *
 * @retval   0 on sucess
 * @retval   1 on Stop NULL
 *
 * @note     This function move soft-timer into m_sys_tim_dev.stopped_tim_list.
 * Soft-timer state change to SYS_TIMER_STATE_STOPPED.
 *
 */
s32 stop_timer(sys_timer_t* timer);

/**
 *
 * @brief    Removes the specified timer.
 *
 * @param    timer soft-timer
 *
 * @retval   0 on sucess
 * @retval   1 on Delete NULL
 *
 * @note     This function delete soft-timer and move it into m_sys_tim_dev.free_tim_list.
 *
 */
s32 del_timer(sys_timer_t* timer);

/**
 *
 * @brief    the function is called with a period of 1ms.
 *
 * @note     This function handle expired timer.
 * Sys timers can get from m_sys_tim_dev.free_tim_list, when alloc
 * a new soft-timer, it will be added to m_sys_tim_dev.running_tim_list
 * or m_sys_tim_dev.stopped_tim_list. All soft-timers are triggered
 * by a real hardware timer(systick timer, period 1ms)
 *
 */
void sys_timer_period(void);

/**
 *
 * @brief    loop call function.
 *
 * @note     This function is called in main loop, it will process expired timer
 * in the running timer lists.
 *
 */
void sys_timer_workque(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_TIMER_H__ */
