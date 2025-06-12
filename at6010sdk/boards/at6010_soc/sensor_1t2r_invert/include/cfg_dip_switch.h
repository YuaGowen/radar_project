/**
 *******************************************************************************
 * @file    cfg_dip_switch.h
 * @author  Airtouch Software Team
 * @brief   DIP switch configuration of the current project
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

#ifndef __CFG_DIP_SWITCH_H__
#define __CFG_DIP_SWITCH_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_APP_LAMPS_OS
 * @{
 */

/**
 * @defgroup TGT_CFG_DIP_SWITCH DIP Switch
 * @brief DIP Switch configurations
 * @{
 */

/**
 * @brief DIP Pin Configuration
 */
#define PARA_DIP_MAP                    \
{   /*    DIP PIN        GPIO     */    \
	{PARA_DIP_DIST_PIN0, GPIO_MCU_0},   \
	{PARA_DIP_DIST_PIN1, GPIO_MCU_1},   \
	{PARA_DIP_DIST_PIN2, GPIO_MCU_2},   \
	{PARA_DIP_LOT1_PIN0, GPIO_MCU_3},   \
	{PARA_DIP_LOT1_PIN1, GPIO_MCU_4},   \
	{PARA_DIP_LOT1_PIN2, GPIO_MCU_5},   \
	{PARA_DIP_LUX_PIN0,  GPIO_AON_0},   \
	{PARA_DIP_LUX_PIN1,  GPIO_AON_1},   \
	{PARA_DIP_LUX_PIN2,  GPIO_AON_4},   \
}

/**
 * @brief DIP distance level: 0 ~ 15.
 */
#define PARA_DIP_DIST_LV                                      \
{                                                             \
	0, 2, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31 \
}

/**
 * @brief Enable the para takes effect immediately.
 */
#define PARA_DIP_DIST_IMME_EFFECT               1u

/**
 * @brief Light on time value in second, level 0 ~ 7.
 */
#define PARA_DIP_LOT1_LV                        \
{                                               \
	2, 30, 60, 5*60, 10*60, 15*60, 20*60, 30*60 \
}

/**
 * @brief Enable the para takes effect immediately.
 */
#define PARA_DIP_LOT1_IMME_EFFECT               1u


/**
 * @brief Threshold of Light sensor, value range: 0 ~ 2800, level 0 ~ 7.
 */
#define PARA_DIP_LUX_LV                         \
{                                               \
	0, 550, 700, 2000, 2000, 2000, 2000, 2000   \
}

/**
 * @brief Enable the para takes effect immediately.
 */
#define PARA_DIP_LUX_IMME_EFFECT                1u

// TGT_CFG_DIP_SWITCH
/**
 * @}
 */

// AT6010_APP_LAMPS_OS
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__CFG_DIP_SWITCH_H__ */
