/**
 *******************************************************************************
 * @file    timer.h
 * @author  Airtouch Software Team
 * @brief   Timer LL Driver
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

#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cpluscplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_LL_DRIVER
 * @{
 */

/**
 * @defgroup TIMER_LL  TIMER
 * @brief TIMER Low-Level Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"

/* Constants -----------------------------------------------------------------*/

#define TIMER_VALUE_MAX (0xFFFFFFFFU)

/* Data Types ----------------------------------------------------------------*/

typedef enum {
	TIMER_ID_0 = 0,
	TIMER_ID_1 = 1,
	TIMER_ID_2 = 2,
	TIMER_ID_3 = 3,

	TIMER_ID_QTY = 4,
	TIMER_ID_INVALID = 0xFF
} timer_id_t;

/**
 * @brief Timer Clock Source
 */
typedef enum {
	TIMER_SOURCE_SYSTEM_DIV_16 = 0,
	TIMER_SOURCE_SYSTEM_DIV_32 = 1,
	TIMER_SOURCE_SYSTEM_DIV_64 = 2,
	TIMER_SOURCE_EXTERNAL_32K = 3,
	TIMER_SOURCE_SYSTEM = 4
} timer_source_t;

/**
 * @brief Timer callback handler type
 */
typedef void (*timer_cb_t)(void *arg);

/**
 * @brief Timer initialization
 *
 * @param id Timer ID
 * @param source select timer clock source
 */
void timer_init(timer_id_t id, timer_source_t source);

/**
 * @brief  Enable the microcontroller timer interrupts.
 *
 * @param id Timer ID
 * @return none
 */
void timer_irq_enable(timer_id_t id);

/**
 * @brief  Disable the microcontroller timer interrupts.
 *
 * @param id Timer ID
 * @return none
 */
void timer_irq_disable(timer_id_t id);

/**
 * @brief  Start the Timer.
 *
 * @param id Timer ID
 * @return none
 */
void timer_start(timer_id_t id);

/**
 * @brief  Stop the Timer.
 *
 * @param id Timer ID
 * @return none
 */
void timer_stop(timer_id_t id);

/**
 * @brief  Returns the current value of the timer.
 *
 * @param id Timer ID
 * @return TimerValue
 */
u32 timer_value_get(timer_id_t id);

/**
 * @brief  Sets the timer to the specified value.
 *
 * @param id Timer ID
 * @param value the value to which the timer is to be set
 * @return TimerValue
 */
void timer_value_set(timer_id_t id, u32 value);

/**
 * @brief  Returns the reload value of the timer. The reload value is the
 *           value which the timer is set to after an underflow occurs.
 *
 * @param id Timer ID
 * @return TimerReload
 */
u32 timer_reload_get(timer_id_t id);

/**
 * @brief  Sets the reload value of the timer to the specified value.
 *           The reload value is the value which the timer is set to
 *           after an underflow occurs.
 *
 * @param id Timer ID
 * @param value Value to be loaded
 * @return none
 */
void timer_reload_set(timer_id_t id, u32 value);

/**
 * @brief  Clears the timer IRQ if set.
 *
 * @param id Timer ID
 * @return none
 */
void timer_irq_clear(timer_id_t id);

/**
 * @brief  Returns the IRQ status of the timer in question.
 *
 * @param id Timer ID
 * @return none
 */
u32 timer_irq_status(timer_id_t id);

/**
 * @brief  Sets the timer to the callback function.
 *
 * @param id Timer ID
 * @param func Callback function to be loaded
 * @return none
 */
void timer_cb_set(timer_id_t id, timer_cb_t func);

/**
 * @brief Timer interrupt service routine, which should be called in TIMERx_Handler().
 *
 * @param id
 */
void timer_isr(timer_id_t id);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cpluscplus
}
#endif

#endif /* !__TIMER_H__ */
