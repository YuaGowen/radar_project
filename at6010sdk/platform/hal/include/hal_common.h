/**
 *******************************************************************************
 * @file    hal_common.h
 * @author  Airtouch Software Team
 * @brief   HAL common Initialization
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

#ifndef __HAL_COMMON_H__
#define __HAL_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @mainpage AT6010 HAL Driver
 * @section AT_INTRO_DESC HAL Drivers User Manual
 *
 * This document contains the User Manual (UM) of the AT6010 HAL (Hardware Abstraction Layer) driver.
 *
 * @image html airtouch_logo_rect_300x200.jpg ""
 *
 * @subsection AT_DOC_DESC Document tabsheets description:
 * @li "Modules", List the different modules.
 * @li "Files", List all the files and globals.
 * @li "Directories", Firmware Directory hierarchy.
 *
 */

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_COMMON COMMON
 * @brief COMMON HAL Driver
 * @{
 */

/* Function Declarations -----------------------------------------------------*/

void hal_common_init(void);
void hal_intr_en(void);
void hal_intr_dis(void);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_COMMON_H__ */
