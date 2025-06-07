/**
 *******************************************************************************
 * @file    cfg_log.h
 * @author  Airtouch Software Team
 * @brief   log configuration for the current project
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

#ifndef __CFG_LOG__
#define __CFG_LOG__

/**
 * @addtogroup AT6010_APP_LAMPS_OS
 * @{
 */

/**
 * @defgroup TGT_LOG_CFG Log
 * @details Log configuration
 * @{
 */

/**
 * @defgroup APP_LOG_CFG Application Log Configuration
 * @details Application Log configuration
 * @{
 */

//#define APP_LIGHT_LOG_EN

// APP_LOG_CFG
/**
 * @}
 */


/**
 * @defgroup HAL_LOG_CFG HAL Log Configuration
 * @details HAL Log configuration
 * @{
 */

#define HAL_CLK_LOG_EN        /**< HAL Clock Log Enable                       */

// HAL_LOG_CFG
/**
 * @}
 */

/**
 * @defgroup DRIVER_LOG_ENABLE Driver Log Configuration
 * @details Driver Log Configuration
 * @{
 */

// #define DRV_DMA_LOG_EN        /**< DMA Driver Log Enable                      */
// #define DRV_SPI_LOG_EN        /**< SPI Driver Log Enable                      */

// DRIVER_LOG_ENABLE
/**
 * @}
 */

// TGT_LOG_CFG
/**
 * @}
 */

// AT6010_APP_LAMPS_OS
/**
 * @}
 */

#endif /* !__CFG_LOG__ */
