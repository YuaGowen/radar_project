/**
 *******************************************************************************
 * @file    hal_dualtimer.h
 * @author  Airtouch Software Team
 * @brief   Dualtimer HAL Driver (comes with the CM0P core)
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

#ifndef __HAL_DUALTIMER_H__
#define __HAL_DUALTIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_DUALTIMER DualTimer
 * @brief DualTimer HAL Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"
#include "dualtimer.h"

/* Function Declarations -----------------------------------------------------*/

void hal_dualtimer_init(void);
void hal_dualtimer_deinit(void);

void hal_dualtimer_config(dualtimer_timer idx, dualtimer_counter_mode mode,uint32 time_us, dualtimer_cb_t func);
void hal_dualtimer_set_time(dualtimer_timer idx,dualtimer_load_type mode,uint32 time_us);

void hal_dualtimer_start(dualtimer_timer idx);
void hal_dualtimer_stop(dualtimer_timer idx);

void hal_dualtimer_delay_us(u32 us);
void hal_dualtimer_delay_ms(u32 ms);

u32  hal_get_start_cnt_value(void);
u32  hal_get_elapsed_time_us(u32 start_cnt_value);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_DUALTIMER_H__ */
