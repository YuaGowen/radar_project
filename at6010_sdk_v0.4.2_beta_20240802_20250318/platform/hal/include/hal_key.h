/**
 *******************************************************************************
 * @file    hal_key.h
 * @author  Airtouch Software Team
 * @brief   KEY HAL Driver
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

#ifndef __HAL_KEY_H__
#define __HAL_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_KEY KEY
 * @brief KEY HAL Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"
#include "cfg_key.h"

/* Constants -----------------------------------------------------------------*/

// #define KEY_EVENT_MSG_POST_EN         /**< Enable send message                */
#define KEY_EVENT_CALLBACK_EN         /**< Enable callback                    */

/* Data Types ----------------------------------------------------------------*/

/**
 * @brief Message configuration
 */
typedef enum {
	KEY_EVENT_PRESS,
	KEY_EVENT_RELEASE,
	KEY_EVENT_NUM
} key_event_t;

/**
 * @brief Key callback function prototype
 */
typedef void (*key_cb_t)(u8 id, u8 event);

/* Function Declarations -----------------------------------------------------*/

void hal_key_init(void);

/**
 * @brief get pressed key number
 * @param none
 * @return 0xFF  - None pressed
 *         other - Key number
 */
u8 hal_key_get(void);

#ifdef KEY_EVENT_MSG_POST_EN
void adc_key_reg_msg_hdl(key_cb_t cb);
#endif

#ifdef KEY_EVENT_CALLBACK_EN
void adc_key_reg_cb_hdl(key_cb_t cb);
#endif

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_KEY_H__ */
