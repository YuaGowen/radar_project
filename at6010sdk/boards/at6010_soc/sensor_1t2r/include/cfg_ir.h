/**
 *******************************************************************************
 * @file    cfg_ir.h
 * @author  Airtouch Software Team
 * @brief   Infrared Radiation receive configuration of the current project
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

#ifndef __CFG_IR__
#define __CFG_IR__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_APP_LAMPS_OS
 * @{
 */

/**
 * @defgroup TGT_CFG_IR IR
 * @brief Infrared Radiation
 * @{
 */

/**
 * @defgroup TGT_CFG_IR_DISTANCE IR Distance
 * @brief IR distance key and parameters
 * @{
 */

/**
 * @defgroup TGT_CFG_IR_DIST_KEY Distance Key
 * @brief Distance key
 * @{
 */
#define IR_CODE_DIST_L1                 (0x08)
#define IR_CODE_DIST_L2                 (0x09)
#define IR_CODE_DIST_L3                 (0x0a)
#define IR_CODE_DIST_L4                 (0x0b)
/**
 * @}
 */

/**
 * @defgroup TGT_CFG_IR_DIST_PARAMS Distance Parameters
 * @brief Distance parameters in range of [0, 31], 0 is the largest distance, 31 is the smallest.
 * @{
 */
#define PARA_IR_DIST_L1                 (20)
#define PARA_IR_DIST_L2                 (8)
#define PARA_IR_DIST_L3                 (3)
#define PARA_IR_DIST_L4                 (0)
/**
 * @}
 */

// TGT_CFG_IR_DISTANCE
/**
 * @}
 */


/**
 * @defgroup TGT_CFG_IR_STDBY_LIGHT Standby Light
 * @brief Standby light level key and parameters
 * @{
 */

/**
 * @defgroup TGT_CFG_IR_STDBY_LIGHT_KEY Standby Light Key
 * @brief Standby light level key
 * @{
 */
#define IR_CODE_STD_L1                  (0x0c)
#define IR_CODE_STD_L2                  (0x0d)
#define IR_CODE_STD_L3                  (0x0e)
#define IR_CODE_STD_L4                  (0x0f)
/**
 * @}
 */

/**
 * @defgroup TGT_CFG_IR_STDBY_LIGHT_PARAMS Standby Light Parameters
 * @brief Standby light parameters (uint: 1%)
 * @{
 */
#define PARA_STD_L1                     (0)
#define PARA_STD_L2                     (10)
#define PARA_STD_L3                     (30)
#define PARA_STD_L4                     (50)
/**
 * @}
 */

// TGT_CFG_IR_STDBY_LIGHT
/**
 * @}
 */


/**
 * @defgroup TGT_CFG_IR_LAT Light Active Time
 * @brief Light active time key & paramters
 * @{
 */

/**
 * @defgroup TGT_CFG_IR_LAT_KEY Light Active Time Key
 * @brief Light active time key
 * @{
 */
#define IR_CODE_KEEP_TIME1_L1           (0x10)
#define IR_CODE_KEEP_TIME1_L2           (0x11)
#define IR_CODE_KEEP_TIME1_L3           (0x12)
#define IR_CODE_KEEP_TIME1_L4           (0x13)
/**
 * @}
 */

/* keep time value(in seconds) to set */
/**
 * @defgroup TGT_CFG_IR_LAT_PARAMS Light Active Time Parameters
 * @brief Light active time parameters (unit: seconds)
 * @{
 */
#define PARA_KEEP_TIME1_L1              (30)
#define PARA_KEEP_TIME1_L2              (60)
#define PARA_KEEP_TIME1_L3              (10*60)
#define PARA_KEEP_TIME1_L4              (30*60)
/**
 * @}
 */

// TGT_CFG_IR_LAT
/**
 * @}
 */


/**
 * @defgroup TGT_CFG_IR_LUX Lux Level
 * @brief Lux level key and parameters
 * @{
 */

/**
 * @defgroup TGT_CFG_IR_LUX_KEY Lux Level Key
 * @brief Lux level key
 * @{
 */
#define IR_CODE_LUX_L1                  (0x14)
#define IR_CODE_LUX_L2                  (0x15)
#define IR_CODE_LUX_L3                  (0x16)
#define IR_CODE_LUX_L4                  (0x17)
/**
 * @}
 */

/**
 * @defgroup TGT_CFG_IR_LUX_PARAMS Lux Level Parameters
 * @brief Lux level parameters (ADC value)
 * @{
 */
#define PARA_IR_LUX_L1                  (2000u)
#define PARA_IR_LUX_L2                  (700u)
#define PARA_IR_LUX_L3                  (550u)
#define PARA_IR_LUX_L4                  (0u)
/**
 * @}
 */

// TGT_CFG_IR_LUX
/**
 * @}
 */

/**
 * @defgroup TGT_CFG_IR_SC_MODE Scene Mode
 * @brief Scene mode key
 * @{
 */
#define IR_CODE_SC1                     (0x00)
#define IR_CODE_SC2                     (0x01)
#define IR_CODE_SC3                     (0x04)
#define IR_CODE_SC4                     (0x05)
/**
 * @}
 */

/*
 * ------------------------------------------------
 * misc key definition
 * ------------------------------------------------
 */

/**
 * @defgroup TGT_CFG_IR_MISC_KEY Misc Key
 * @brief Misc key definition
 * @{
 */

/**
 * @brief Keep the light on always
 */
#define IR_CODE_ALWAYS_ON               (0x1c)

/**
 * @brief Keep the light off always
 */
#define IR_CODE_ALWAYS_OFF              (0x1d)

/**
 * @brief Enter the test mode
 */
#define IR_CODE_TEST_MODE               (0x1e)

/**
 * @brief Enter the auto mode
 */
#define IR_CODE_AUTO_MODE               (0x1f)

/**
 * @brief Set the light intensity to high
 */
#define IR_CODE_PWR_HIGHT               (0x02)

/**
 * @brief Set the light intensity to low
 */
#define IR_CODE_PWR_LOW                 (0x06)

/**
 * @brief Increase the light intensity
 */
#define IR_CODE_DIM_ADD                 (0x03)

/**
 * @brief Decrease the light intensity
 */
#define IR_CODE_DIM_SUB                 (0x07)

// TGT_CFG_IR_MISC_KEY
/**
 * @}
 */

// TGT_CFG_IR
/**
 * @}
 *
 */

// AT6010_APP_LAMPS_OS
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__CFG_IR__ */
