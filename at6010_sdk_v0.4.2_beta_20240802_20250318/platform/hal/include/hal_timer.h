/**
 *******************************************************************************
 * @file    hal_timer.h
 * @author  Airtouch Software Team
 * @brief   Timer HAL Driver
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

#ifndef __HAL_TIMER_H__
#define __HAL_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_TIMER TIMER
 * @brief TIMER HAL Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"
#include "timer.h"

/* Data Types ----------------------------------------------------------------*/

typedef enum {
	TIMER_VALUE_DEC,
	TIMER_VALUE_INC
} timer_value_t;

typedef struct {
	timer_source_t source;    /**< timer clock source                         */
	timer_value_t value_mode; /**< timer value change method                  */
	u8 start;                 /**< 0: start later, 1: start immediately       */
	u8 irq_en;                /**< 0: disable interrupt, 1: enable interrupt  */
	timer_cb_t cb;            /**< interrupt callback function                */
	u32 reload;               /**< timer reload in us                         */
} timer_cfg_t;

/* Function Declarations -----------------------------------------------------*/

/**
 * @brief timer initialization
 *
 * @param id timer ID
 * @param cfg timer configuration for initialization
 */
void hal_timer_init(timer_id_t id, const timer_cfg_t *cfg);

void hal_timer_deinit(timer_id_t id);

/**
 * @brief get available timer id
 *
 * @param id address to save timer id
 * @return u8 AT error number
 */
u8 hal_timer_get_free(timer_id_t *id);

/**
 * @brief set timer period and callback function
 *
 * @param id timer ID
 * @param us period time
 * @param func callback function
 */
void hal_timer_set(timer_id_t id, u32 us, timer_cb_t func);

void hal_timer_start(timer_id_t id);
void hal_timer_stop(timer_id_t id);
u32  hal_timer_value_get(timer_id_t id);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_TIMER_H__ */
