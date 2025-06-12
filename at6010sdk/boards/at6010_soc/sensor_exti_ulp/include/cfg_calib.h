/**
 *******************************************************************************
 * @file    cfg_calib.h
 * @author  Airtouch Software Team
 * @brief   MDET FMCW calibration configuration of the current project
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

#ifndef __CFG_CALIB__
#define __CFG_CALIB__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_APP_LAMPS_OS
 * @{
 */

/**
 * @defgroup TGT_CFG_CALIB Noise Filter
 * @brief Calibrate the noise floor parameters on different board if needed.
 * @{
 */

/**
 * @brief Calibration parameters for MDET FMCW.
 */
#define CFG_CALIB_MDET_FMCW_PARAM           \
{                                           \
	CFG_MDET_FMCW_BOARD_NF_BASE_PROF_PARAM, \
	CFG_MDET_FMCW_BOARD_NF_EXT_PROF_PARAM,  \
	CFG_MDET_FMCW_BOARD_NF_HIGH_PROF_PARAM, \
}

/**
 * @brief Noise Filter Base Profile Parameters
 */
#define CFG_MDET_FMCW_BOARD_NF_BASE_PROF_PARAM   \
{  \
	/* 0-4	 */ 58.7f, 52.2f, 47.7f, 46.7f, 46.7f, \
	/* 5-9	 */ 47.0f, 47.1f, 47.1f, 47.1f, 47.1f, \
	/* 10-14 */ 47.1f, 47.1f, 47.1f, 47.1f, 47.1f, \
	/* 15-19 */ 47.1f, 47.1f, 46.9f, 46.6f, 46.5f, \
	/* 20-24 */ 46.4f, 46.2f, 46.1f, 46.0f, 45.9f, \
	/* 25-29 */ 45.9f, 45.9f, 45.9f, 45.9f, 45.4f, \
	/* 30-34 */ 45.0f, 44.7f, 44.7f, 44.7f, 44.7f, \
	/* 35-39 */ 44.7f, 44.7f, 44.7f, 44.7f, 44.7f, \
	/* 40-44 */ 44.7f, 44.7f, 44.7f, 44.7f, 44.7f, \
	/* 45-49 */ 44.7f, 44.7f, 44.7f, 44.7f, 44.7f, \
}

/**
 * @brief Noise Filter Extend Profile Parameters
 */
#define CFG_MDET_FMCW_BOARD_NF_EXT_PROF_PARAM    \
{  \
	/* 0-4	 */  64, 32, 32, 32, 32, \
	/* 5-9	 */  32, 28, 24, 24, 24, \
	/* 10-14 */  24, 22, 20, 18, 16, \
	/* 15-19 */  18, 16, 14, 10, 10, \
	/* 20-24 */  8,  8,  8,  8,  8,  \
	/* 25-29 */  8,  8,  8,  8,  8,  \
	/* 30-34 */  7,  7,  7,  7,  7,  \
	/* 35-39 */  6,  6,  6,  6,  6,  \
	/* 40-44 */  5,  5,  5,  5,  5,  \
	/* 45-49 */  5,  5,  5,  5,  5,  \
	/* 50-54 */  5,  5,  5,  5,  5,  \
	/* 55-59 */  5,  5,  5,  5,  5,  \
	/* 60-64 */  5,  5,  5,  5,  5,  \
	/* 65-49 */  5,  5,  5,  5,  5,  \
	/* 70-74 */  5,  5,  5,  5,  5,  \
	/* 75-79 */  5,  5,  5,  5,  5,  \
	/* 80-84 */  5,  5,  5,  5,  5,  \
	/* 85-89 */  5,  5,  5,  5,  5,  \
	/* 90-94 */  5,  5,  5,  5,  5,  \
	/* 95-99 */  5,  5,  5,  5,  5,  \
}
/**
 * @brief Noise Filter High Profile Parameters
 */

#define CFG_MDET_FMCW_BOARD_NF_HIGH_PROF_PARAM      \
{	\
	 /* 0-4   */ 	45.0f, 47.0f, 38.1f, 37.0f, 37.0f,  \
	 /* 5-9   */ 	37.0f, 37.0f, 37.5f, 37.5f, 37.5f,  \
	 /* 10-14 */ 	37.5f, 37.5f, 38.0f, 38.5f, 39.0f,  \
	 /* 15-20 */ 	39.5f, 40.0f, 40.0f, 40.0f, 40.0f,  \
	 /* 21-24 */ 	40.0f, 40.0f, 40.0f, 40.0f, 40.0f,  \
	 /* 25-29 */ 	40.0f, 40.0f, 40.0f, 40.0f, 40.0f,  \
	 /* 30-34 */ 	40.0f, 40.0f, 40.0f, 40.0f, 40.0f,  \
	 /* 35-39 */ 	40.0f, 40.0f, 40.0f, 40.0f, 40.0f,  \
	 /* 40-44 */ 	40.0f, 40.0f, 40.0f, 40.0f, 40.0f,  \
	 /* 45-49 */ 	40.0f, 40.0f, 40.0f, 40.0f, 40.0f,  \
}

// TGT_CFG_CALIB
/**
 * @}
 */

// AT6010_APP_SENSOR_ULP
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__CFG_CALIB__ */
