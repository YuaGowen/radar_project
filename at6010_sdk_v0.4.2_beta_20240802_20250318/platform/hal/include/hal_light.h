/**
 *******************************************************************************
 * @file    hal_light.h
 * @author  Airtouch Software Team
 * @brief   Light HAL driver
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
 ******************************************************************************/

#ifndef __HAL_LIGHT_H__
#define __HAL_LIGHT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_types.h"

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_LIGHT Light
 * @brief Light HAL Driver
 * @{
 */

/**
 * @defgroup HAL_LIGHT_SET_STATUS HAL Light Status
 * @{
 */
#define HAL_LIGHT_SET_OFF                   (0u)
#define HAL_LIGHT_SET_ON                    (1u)
#define HAL_LIGHT_SET_DIM                   (2u)
/**
 * @}
 */

// TODO: 分组，引用
#define LED_LUMACYCLE_LIGHT_NONE            (0x0u)
#define LED_LUMACYCLE_LIGHT_ON              (0x1u)
#define LED_LUMACYCLE_LIGHT_OFF             (0x2u)
#define LED_LUMACYCLE_LIGHT_ONOFF           (0x3u)

#if (PARA_PWM_LUMACYCLE != LED_LUMACYCLE_LIGHT_NONE)
#define LED_LUMACYCLE_STEP                  (10u)   /* Duty changed in one step during lumacycle */
#define LED_LUMACYCLE_STEP_TIME_MS          (20u)   /* Step interval in ms during lumacycle */
#endif

#ifdef CFG_AUX_LIGHT_EN
#define LIGHT_NUM                           (2u)
#else
#define LIGHT_NUM                           (1u)
#endif

#define LIGHT_PRIMARY                       (0u)
#define LIGHT_AUXILIARY                     (1u)

typedef enum {
	LIGHT_TYPE_IO = 0,
	LIGHT_TYPE_PWM = 1,
} light_type_t;

/**
 * @brief Light initialization
 *
 * @param id   light id
 * @param gpio see @ref gpio_pin_t
 * @param type see @ref light_type_t
 * @return None
 */
void hal_light_init(u8 id, u32 gpio, light_type_t type);

/**
 * @brief Get light status.
 *
 * @param id light id
 * @return light status, see @ref HAL_LIGHT_SET_STATUS
 */
u8   hal_light_get_status(u8 id);

/**
 * @brief Change light status.
 *
 * @param id light id
 * @param onoff This parameter can be one of the following values:
 *        @arg @ref HAL_LIGHT_SET_OFF
 *        @arg @ref HAL_LIGHT_SET_ON
 *        @arg @ref HAL_LIGHT_SET_DIM
 */
void hal_light_set(u8 id, u8 onoff);

#ifdef CFG_LED_PWM

void hal_pwm_set(u8 id, u16 duty);
void hal_pwm_duty_set(u8 id, u16 duty);
u16  hal_pwm_duty_get(u8 id);
void hal_pwm_standby_duty_set(u8 id, u16 duty);

#if (PARA_PWM_LUMACYCLE != LED_LUMACYCLE_LIGHT_NONE)
u8   hal_led_lumacycle_get_en(u8 id);
void hal_led_lumacycle_proc(void);
#endif /* PARA_PWM_LUMACYCLE != LED_LUMACYCLE_LIGHT_NONE */

#endif /* CFG_LED_PWM */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_LIGHT_H__ */
