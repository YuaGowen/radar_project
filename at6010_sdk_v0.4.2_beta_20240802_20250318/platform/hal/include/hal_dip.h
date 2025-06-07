/**
 *******************************************************************************
 * @file    hal_dip.h
 * @author  Airtouch Software Team
 * @brief   DIP HAL Driver
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

#ifndef __HAL_DIP_H__
#define __HAL_DIP_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_DIP DIP
 * @brief DIP HAL Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"

/**
 * @defgroup HAL_DIP_EC DIP Exported Constants
 * @brief DIP Exported Constants
 * @{
 */

#define DIP_TYPE_POS                    (4u)
#define DIP_TYPE_MASK                   (0xFu << DIP_TYPE_POS)
#define DIP_PIN_MASK                    (0xFu)

#define DIP_TYPE_DIST                   (0x1u)
#define PARA_DIP_DIST_PIN0              (0x10u)
#define PARA_DIP_DIST_PIN1              (0x11u)
#define PARA_DIP_DIST_PIN2              (0x12u)

#define DIP_TYPE_LOT1                   (0x2u)
#define PARA_DIP_LOT1_PIN0              (0x20u)
#define PARA_DIP_LOT1_PIN1              (0x21u)
#define PARA_DIP_LOT1_PIN2              (0x22u)

#define DIP_TYPE_LOT2                   (0x4u)
#define PARA_DIP_LOT2_PIN0              (0x40u)
#define PARA_DIP_LOT2_PIN1              (0x41u)
#define PARA_DIP_LOT2_PIN2              (0x42u)

#define DIP_TYPE_LUX                    (0x8u)
#define PARA_DIP_LUX_PIN0               (0x80u)
#define PARA_DIP_LUX_PIN1               (0x81u)
#define PARA_DIP_LUX_PIN2               (0x82u)

#define DIP_IMME_EFFECT_EN              \
	(PARA_DIP_DIST_IMME_EFFECT | (PARA_DIP_LOT1_IMME_EFFECT << 1) | (PARA_DIP_LUX_IMME_EFFECT << 3))

#define DIP_IMME_EFFECT_EN_MASK         (DIP_IMME_EFFECT_EN << DIP_TYPE_POS)

/**
 * @}
 */

typedef enum {
	DIP_SETTING_SEN_DIST = 1,       /**< sensing distance                     */
	DIP_SETTING_LIGHT_TIME1,        /**< light on keep time, level 1          */
	DIP_SETTING_LIGHT_TIME2,        /**< light on keep time, level 2          */
	DIP_SETTING_LUX,                /**< light sensing value                  */

	DIP_SETTING_INVALID,
} dip_setting_item_t;

typedef void (*dip_hdl_t)(u8 dip_type);

void hal_dip_init(void);
u8   hal_dip_get_data(dip_setting_item_t itm);
void hal_dip_set_hdl(dip_hdl_t hdl);

void hal_dip_test(void);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_DIP_H__ */
