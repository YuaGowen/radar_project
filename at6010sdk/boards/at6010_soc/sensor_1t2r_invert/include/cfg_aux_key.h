/**
 *******************************************************************************
 * @file    cfg_aux_key.h
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

#ifndef __CFG_AUX_KEY_H__
#define __CFG_AUX_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_APP_LAMPS_OS
 * @{
 */

/**
 * @defgroup TGT_CFG_AUX_KEY Aux ADC Key
 * @brief Config Aux ADC Key function, channel, voltage, distance, light on time and light sensor parameters.
 * @{
 */

/**
 * @defgroup TGT_CFG_KEY_FUNC_SELECT Key Function
 * @details Comment the macro to disable the key function.             <br>
 * @ref AUX_ADC_KEY1_EN -- The key1 is used for distance setting.      <br>
 * @ref AUX_ADC_KEY2_EN -- The key2 is used for light on time setting. <br>
 * @ref AUX_ADC_KEY3_EN -- The key3 is used for light sensor settting. <br>
 * @{
 */

#define AUX_ADC_KEY1_EN
// #define AUX_ADC_KEY2_EN
// #define AUX_ADC_KEY3_EN

// TGT_CFG_KEY_FUNC_SELECT
/**
 * @}
 */

/**
 * @defgroup TGT_CFG_KEY_CHAN_SELECT Key Channel
 * @details Select which GPIO pin is used for this Aux ADC Key. <br>
 * @ref AUX_ADC_KEY1_CH -- Select a channel for the key1. <br>
 * @ref AUX_ADC_KEY2_CH -- Select a channel for the key2. <br>
 * @ref AUX_ADC_KEY3_CH -- Select a channel for the key3. <br>
 * @{
 */

#define AUX_ADC_KEY1_CH                 GPIO_MCU_2

#define AUX_ADC_KEY2_CH                 GPIO_MCU_4

#define AUX_ADC_KEY3_CH                 GPIO_MCU_3


// TGT_CFG_KEY_CHAN_SELECT
/**
 * @}
 */

/**
 * @defgroup TGT_CFG_KEY_VOLTAGE Key Voltage
 * @details Includes a voltage initial map, which has 16 voltage values.
 * @{
 */


/**
 * @brief The step between two consecutive voltage values.
 */
#define AUX_ADC_KEY_STEP                175

/**
 * @brief The total number of predefined voltage values
 */
#define AUX_ADC_LEVEL_MAX               16


#define AUX_ADC_KEY_0_VOLTAGE           85
#define AUX_ADC_KEY_1_VOLTAGE           260  /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_0_VOLTAGE */
#define AUX_ADC_KEY_2_VOLTAGE           435  /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_1_VOLTAGE */
#define AUX_ADC_KEY_3_VOLTAGE           610  /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_2_VOLTAGE */
#define AUX_ADC_KEY_4_VOLTAGE           785  /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_3_VOLTAGE */
#define AUX_ADC_KEY_5_VOLTAGE           960  /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_4_VOLTAGE */
#define AUX_ADC_KEY_6_VOLTAGE           1135 /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_5_VOLTAGE */
#define AUX_ADC_KEY_7_VOLTAGE           1310 /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_6_VOLTAGE */
#define AUX_ADC_KEY_8_VOLTAGE           1485 /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_7_VOLTAGE */
#define AUX_ADC_KEY_9_VOLTAGE           1660 /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_8_VOLTAGE */
#define AUX_ADC_KEY_10_VOLTAGE          1835 /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_9_VOLTAGE */
#define AUX_ADC_KEY_11_VOLTAGE          2010 /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_10_VOLTAGE */
#define AUX_ADC_KEY_12_VOLTAGE          2185 /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_11_VOLTAGE */
#define AUX_ADC_KEY_13_VOLTAGE          2360 /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_12_VOLTAGE */
#define AUX_ADC_KEY_14_VOLTAGE          2535 /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_13_VOLTAGE */
#define AUX_ADC_KEY_15_VOLTAGE          2710 /* AUX_ADC_KEY_STEP + AUX_ADC_KEY_14_VOLTAGE */


#define PARA_AUX_ADC_KEY_VOL_MAP        \
{                                       \
	AUX_ADC_KEY_0_VOLTAGE,              \
	AUX_ADC_KEY_1_VOLTAGE,              \
	AUX_ADC_KEY_2_VOLTAGE,              \
	AUX_ADC_KEY_3_VOLTAGE,              \
	AUX_ADC_KEY_4_VOLTAGE,              \
	AUX_ADC_KEY_5_VOLTAGE,              \
	AUX_ADC_KEY_6_VOLTAGE,              \
	AUX_ADC_KEY_7_VOLTAGE,              \
	AUX_ADC_KEY_8_VOLTAGE,              \
	AUX_ADC_KEY_9_VOLTAGE,              \
	AUX_ADC_KEY_10_VOLTAGE,             \
	AUX_ADC_KEY_11_VOLTAGE,             \
	AUX_ADC_KEY_12_VOLTAGE,             \
	AUX_ADC_KEY_13_VOLTAGE,             \
	AUX_ADC_KEY_14_VOLTAGE,             \
	AUX_ADC_KEY_15_VOLTAGE              \
}

// TGT_CFG_KEY_VOLTAGE_CFG
/**
 * @}
 */


/**
 * @defgroup TGT_CFG_AUX_KEY_DISTANCE Distance
 * @details ADC key distance level setting, level 0 ~ 15.
 * @{
 */

#define AUX_ADC_KEY_0_DIST              0
#define AUX_ADC_KEY_1_DIST              2
#define AUX_ADC_KEY_2_DIST              4
#define AUX_ADC_KEY_3_DIST              6
#define AUX_ADC_KEY_4_DIST              8
#define AUX_ADC_KEY_5_DIST              10
#define AUX_ADC_KEY_6_DIST              12
#define AUX_ADC_KEY_7_DIST              14
#define AUX_ADC_KEY_8_DIST              16
#define AUX_ADC_KEY_9_DIST              18
#define AUX_ADC_KEY_10_DIST             20
#define AUX_ADC_KEY_11_DIST             22
#define AUX_ADC_KEY_12_DIST             24
#define AUX_ADC_KEY_13_DIST             26
#define AUX_ADC_KEY_14_DIST             28
#define AUX_ADC_KEY_15_DIST             30


#define PARA_AUX_ADC_KEY_DIST_MAP       \
{                                       \
	AUX_ADC_KEY_0_DIST,                 \
	AUX_ADC_KEY_1_DIST,                 \
	AUX_ADC_KEY_2_DIST,                 \
	AUX_ADC_KEY_3_DIST,                 \
	AUX_ADC_KEY_4_DIST,                 \
	AUX_ADC_KEY_5_DIST,                 \
	AUX_ADC_KEY_6_DIST,                 \
	AUX_ADC_KEY_7_DIST,                 \
	AUX_ADC_KEY_8_DIST,                 \
	AUX_ADC_KEY_9_DIST,                 \
	AUX_ADC_KEY_10_DIST,                \
	AUX_ADC_KEY_11_DIST,                \
	AUX_ADC_KEY_12_DIST,                \
	AUX_ADC_KEY_13_DIST,                \
	AUX_ADC_KEY_14_DIST,                \
	AUX_ADC_KEY_15_DIST                 \
}

// TGT_CFG_AUX_KEY_DISTANCE
/**
 * @}
 */

/**
 * @defgroup TGT_CFG_AUX_KEY_LOT Light On Time
 * @details ADC Key light on time settings, level 0~15.
 * @{
 */

#define AUX_ADC_KEY_0_LOT               2
#define AUX_ADC_KEY_1_LOT               30
#define AUX_ADC_KEY_2_LOT               60
#define AUX_ADC_KEY_3_LOT               (60*5)
#define AUX_ADC_KEY_4_LOT               (60*10)
#define AUX_ADC_KEY_5_LOT               (60*15)
#define AUX_ADC_KEY_6_LOT               (60*20)
#define AUX_ADC_KEY_7_LOT               (60*25)
#define AUX_ADC_KEY_8_LOT               (60*30)
#define AUX_ADC_KEY_9_LOT               (60*35)
#define AUX_ADC_KEY_10_LOT              (60*40)
#define AUX_ADC_KEY_11_LOT              (60*45)
#define AUX_ADC_KEY_12_LOT              (60*50)
#define AUX_ADC_KEY_13_LOT              (60*55)
#define AUX_ADC_KEY_14_LOT              (60*60)
#define AUX_ADC_KEY_15_LOT              (60*65)

#define PARA_AUX_ADC_KEY_LOT_MAP        \
{                                       \
	AUX_ADC_KEY_0_LOT,                  \
	AUX_ADC_KEY_1_LOT,                  \
	AUX_ADC_KEY_2_LOT,                  \
	AUX_ADC_KEY_3_LOT,                  \
	AUX_ADC_KEY_4_LOT,                  \
	AUX_ADC_KEY_5_LOT,                  \
	AUX_ADC_KEY_6_LOT,                  \
	AUX_ADC_KEY_7_LOT,                  \
	AUX_ADC_KEY_8_LOT,                  \
	AUX_ADC_KEY_9_LOT,                  \
	AUX_ADC_KEY_10_LOT,                 \
	AUX_ADC_KEY_11_LOT,                 \
	AUX_ADC_KEY_12_LOT,                 \
	AUX_ADC_KEY_13_LOT,                 \
	AUX_ADC_KEY_14_LOT,                 \
	AUX_ADC_KEY_15_LOT                  \
}

// TGT_CFG_AUX_KEY_LOT
/**
 * @}
 */


/**
 * @defgroup TGT_CFG_AUX_KEY_LS Light Sensor
 * @details ADC Key light sensor settings, level 0~15.
 * @{
 */

#define AUX_ADC_KEY_0_LUX               0
#define AUX_ADC_KEY_1_LUX               200
#define AUX_ADC_KEY_2_LUX               400
#define AUX_ADC_KEY_3_LUX               600
#define AUX_ADC_KEY_4_LUX               800
#define AUX_ADC_KEY_5_LUX               1000
#define AUX_ADC_KEY_6_LUX               1200
#define AUX_ADC_KEY_7_LUX               1400
#define AUX_ADC_KEY_8_LUX               1600
#define AUX_ADC_KEY_9_LUX               1800
#define AUX_ADC_KEY_10_LUX              2000
#define AUX_ADC_KEY_11_LUX              2200
#define AUX_ADC_KEY_12_LUX              2400
#define AUX_ADC_KEY_13_LUX              2600
#define AUX_ADC_KEY_14_LUX              2800
#define AUX_ADC_KEY_15_LUX              3000

#define PARA_AUX_ADC_KEY_LUX_MAP        \
{                                       \
	AUX_ADC_KEY_0_LUX,                  \
	AUX_ADC_KEY_1_LUX,                  \
	AUX_ADC_KEY_2_LUX,                  \
	AUX_ADC_KEY_3_LUX,                  \
	AUX_ADC_KEY_4_LUX,                  \
	AUX_ADC_KEY_5_LUX,                  \
	AUX_ADC_KEY_6_LUX,                  \
	AUX_ADC_KEY_7_LUX,                  \
	AUX_ADC_KEY_8_LUX,                  \
	AUX_ADC_KEY_9_LUX,                  \
	AUX_ADC_KEY_10_LUX,                 \
	AUX_ADC_KEY_11_LUX,                 \
	AUX_ADC_KEY_12_LUX,                 \
	AUX_ADC_KEY_13_LUX,                 \
	AUX_ADC_KEY_14_LUX,                 \
	AUX_ADC_KEY_15_LUX                  \
}

// TGT_CFG_AUX_KEY_LS
/**
 * @}
 */

// TGT_CFG_AUX_KEY
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

#endif /* !__CFG_AUX_KEY_H__ */
