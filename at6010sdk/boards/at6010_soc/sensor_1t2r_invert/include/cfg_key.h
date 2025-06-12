/**
 *******************************************************************************
 * @file    cfg_key.h
 * @author  Airtouch Software Team
 * @brief   Key configuration of the current project
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

#ifndef __CFG_KEY_H__
#define __CFG_KEY_H__

/**
 * @addtogroup AT6010_APP_LAMPS_OS
 * @{
 */

/**
 * @defgroup TGT_CFG_KEY Key
 * @brief Config 6 keys on board
 * @{
 */

#define ADC_KEY_DEBOUNCE_TIME_MS (40U)  /**< Debounce time in ms              */
//#define DBG_ADC_KEY_CAL_EN            /**< Enable key adc value calibration */
#define ADC_KEY_IDLE_ADC        (2200U) /**< ADC value boundary for no press  */
#define ADC_KEY_MARGIN          (50U)   /**< Error range of each key adc      */

/**
 * @defgroup TGT_CFG_KEY_ID_ADC_VALUE Key ID And ADC Value
 * @brief Key ID and ADC Value
 * @details Key id 0 means no keys are pressed (ADC key idle)
 * @{
 */

/**
 * @defgroup TGT_CFG_KEY_ID Key ID
 * @brief Key ID
 * @{
 */
#define ADC_KEY_1_ID            (1U)
#define ADC_KEY_2_ID            (2U)
#define ADC_KEY_3_ID            (3U)
#define ADC_KEY_4_ID            (4U)
#define ADC_KEY_5_ID            (5U)
#define ADC_KEY_6_ID            (6U)
/**
 * @}
 */

/**
 * @defgroup TGT_CFG_KEY_ADC_VALUE Key ADC Value
 * @brief Key ADC Value
 * @{
 */
#define ADC_KEY_1_ADC           (0U)
#define ADC_KEY_2_ADC           (160U)
#define ADC_KEY_3_ADC           (350U)
#define ADC_KEY_4_ADC           (500U)
#define ADC_KEY_5_ADC           (700U)
#define ADC_KEY_6_ADC           (920U)
/**
 * @}
 */

/**
 * @brief Mapping a key and its adc value sampled by SW from voltage
 */
#define PARA_ADC_KEY_ADC_MAP         \
{                                    \
	/*  KEY ID   , Key ADC value */  \
	{ADC_KEY_1_ID, ADC_KEY_1_ADC},   \
	{ADC_KEY_2_ID, ADC_KEY_2_ADC},   \
	{ADC_KEY_3_ID, ADC_KEY_3_ADC},   \
	{ADC_KEY_4_ID, ADC_KEY_4_ADC},   \
	{ADC_KEY_5_ID, ADC_KEY_5_ADC},   \
	{ADC_KEY_6_ID, ADC_KEY_6_ADC},   \
}

// TGT_CFG_KEY_ID_ADC_VALUE
/**
 * @}
 */


/**
 * @brief Mapping a key and its functions for all events
 */
#define PARA_ADC_KEY_FUNC_MAP                           \
{                                                       \
	/* Key id,      Press func,        Release func */  \
	{ADC_KEY_1_ID, {KEY_FUNC_DIST_INC, KEY_FUNC_NULL}}, \
	{ADC_KEY_2_ID, {KEY_FUNC_DIST_DEC, KEY_FUNC_NULL}}, \
	{ADC_KEY_3_ID, {KEY_FUNC_LOT_INC,  KEY_FUNC_NULL}}, \
	{ADC_KEY_4_ID, {KEY_FUNC_LOT_DEC,  KEY_FUNC_NULL}}, \
	{ADC_KEY_5_ID, {KEY_FUNC_LUX_INC,  KEY_FUNC_NULL}}, \
	{ADC_KEY_6_ID, {KEY_FUNC_LUX_DEC,  KEY_FUNC_NULL}}, \
}

/**
 * @brief Light on time value in second, level 0 ~ 7.
 */
#define PARA_ADC_KEY_LOT_LV                     \
{                                               \
	2, 30, 60, 5*60, 10*60, 15*60, 20*60, 30*60 \
}

/**
 * @brief Threshold of Light sensor, level 0 ~ 7.
 */
#define PARA_ADC_KEY_LUX_LV                     \
{                                               \
	0, 400, 800, 1200, 1600, 2000, 2400, 3000   \
}

// TGT_CFG_KEY
/**
 * @}
 */

// AT6010_APP_LAMPS_OS
/**
 * @}
 */

#endif /* !__CFG_KEY_H__ */
