/**
 *******************************************************************************
 * @file    dualtimer.h
 * @author  Airtouch Software Team
 * @brief   Dualtimer Low-Level Driver (comes with the CM0P core)
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

#ifndef __DUAL_TIMER_H__
#define __DUAL_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_LL_DRIVER
 * @{
 */

/**
 * @defgroup LL_DUALTIMER DualTimer
 * @brief DualTimer LL Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"

/* Data Types ----------------------------------------------------------------*/

/**
 * @brief Dual Timer callback function prototype
 */
typedef void (*dualtimer_cb_t)(void);

/**
 * @brief Dual Timer ID
 */
typedef enum {
	DUALTIMER_TIM_0,/* By default,occupied by delay_ms,if reconfigured,hal_dualtimer_delay_ms will be invalid */
	DUALTIMER_TIM_1,/* By default,occupied by delay_us,if reconfigured,hal_dualtimer_delay_us will be invalid */
	DUALTIMER_TIM_QTY
} dualtimer_timer;

/**
 * @brief Dual Timer count mode
 */
typedef enum {
	DUALTIMER_ONE_SHOT_MODE = 0,
	DUALTIMER_FREE_RUNNING_MODE,
	DUALTIMER_PERIODIC_MODE,
} dualtimer_counter_mode;

/**
 * @brief Dual Timer count size
 */
typedef enum {
	DUALTIMER_COUNTER_SIZE_16BIT = 0,
	DUALTIMER_COUNTER_SIZE_32BIT,
} dualtimer_counter_size;

/**
 * @brief Dual Timer clock prescaler
 */
typedef enum {
	DUALTIMER_CLOCK_PRESCALER_DIV1 = 0,
	DUALTIMER_CLOCK_PRESCALER_DIV16,
	DUALTIMER_CLOCK_PRESCALER_DIV256,
} dualtimer_clock_prescaler;

/**
 * @brief Dual Timer load mode
 *
 */
typedef  enum  {
	DUALTIMER_SET_CUR_LOAD = 0,/* modified timer counter value immediately */
	DUALTIMER_SET_BG_LOAD,/* Take effect in next period */
} dualtimer_load_type;

typedef struct {
	dualtimer_counter_mode counter_mode;
	dualtimer_counter_size counter_size;
	dualtimer_clock_prescaler clock_prescaler;
	bool irq_enable;
	u32  load_value;
} dualtimer_config;

/* Function Declarations -----------------------------------------------------*/

void dualtimer_init(void);
void dualtimer_deinit(void);

void dualtimer_get_default_config(dualtimer_config *cfg);
void dualtimer_set_config(dualtimer_config *cfg, dualtimer_timer id);
void dualtimer_set_load(dualtimer_timer id, dualtimer_load_type cur_bg, u32 value);

void dualtimer_start(dualtimer_timer id);
void dualtimer_stop(dualtimer_timer id);

void dualtimer_irq_clear(dualtimer_timer id);
u32  dualtimer_get_value(dualtimer_timer id);
void dualtimer_set_cb(dualtimer_timer id, dualtimer_cb_t func);
bool dualtimer_is_enabled(dualtimer_timer id);
void dualtimer_hdl_proc(void);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__DUAL_TIMER_H__ */
