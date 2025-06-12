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
	CFG_MDET_FMCW_BOARD_NF_HIGH_PROF_PARAM, \
}

/**
 * @brief Noise Filter Base Profile Parameters
 */
#define CFG_MDET_FMCW_BOARD_NF_BASE_PROF_PARAM       \
{  \
	/* 0-4	 */  54.0f,  53.0f,  53.0f,  55.1f,  56.7f,  \
	/* 5-9	 */  56.1f,  55.1f,  53.8f,  52.6f,  51.8f,  \
	/* 10-14 */  50.7f,  49.8f,  49.2f,  48.8f,  48.3f,  \
	/* 15-19 */  48.2f,  47.7f,  47.3f,  46.4f,  46.2f,  \
	/* 20-24 */  46.2f,  46.2f,  45.8f,  46.3f,  46.3f,  \
	/* 25-29 */  45.8f,  45.8f,  45.8f,  45.2f,  44.7f,  \
	/* 30-34 */  44.3f,  44.0f,  44.0f,  44.0f,  43.5f,  \
	/* 35-39 */  43.1f,  43.0f,  43.0f,  42.7f,  42.5f,  \
	/* 40-44 */  42.5f,  42.5f,  42.0f,  42.0f,  42.0f,  \
	/* 45-49 */  42.0f,  42.0f,  42.0f,  42.0f,  42.0f,  \
	/* 50-54 */  42.0f,  42.0f,  42.0f,  42.0f,  42.0f,  \
	/* 55-59 */  42.0f,  42.0f,  42.0f,  42.0f,  42.0f,  \
	/* 60-64 */  42.0f,  42.0f,  42.0f,  42.0f,  42.0f,  \
	/* 65-49 */  42.0f,  42.0f,  42.0f,  42.0f,  42.0f,  \
	/* 70-74 */  42.0f,  42.0f,  42.0f,  42.0f,  42.0f,  \
	/* 75-79 */  42.0f,  42.0f,  42.0f,  42.0f,  42.0f,  \
	/* 80-84 */  42.0f,  42.0f,  42.0f,  42.0f,  42.0f,  \
	/* 85-89 */  42.0f,  42.0f,  42.0f,  42.0f,  42.0f,  \
	/* 90-94 */  42.0f,  42.0f,  42.0f,  42.0f,  42.0f,  \
	/* 95-99 */  42.0f,  42.0f,  42.0f,  42.0f,  42.0f,  \
}

/**
 * @brief Noise Filter High Profile Parameters
 */

#define CFG_MDET_FMCW_BOARD_NF_HIGH_PROF_PARAM    \
{												  \
	 /* 0-4   */ 46.0f, 56.0f, 55.0f, 54.0f, 53.5f, \
	 /* 5-9   */ 53.5f, 52.0f, 51.0f, 50.3f, 50.3f, \
	 /* 10-14 */ 50.3f, 50.3f, 50.3f, 50.0f, 49.0f, \
	 /* 15-20 */ 48.0f, 47.0f, 47.0f, 47.0f, 47.0f, \
	 /* 21-24 */ 47.0f, 47.0f, 47.0f, 47.0f, 47.0f, \
	 /* 25-29 */ 47.0f, 47.0f, 47.0f, 47.0f, 47.0f, \
	 /* 30-34 */ 47.0f, 47.0f, 47.0f, 47.0f, 47.0f, \
	 /* 35-39 */ 47.0f, 47.0f, 47.0f, 47.0f, 47.0f, \
	 /* 40-44 */ 47.0f, 47.0f, 47.0f, 47.0f, 47.0f, \
	 /* 45-49 */ 47.0f, 47.0f, 47.0f, 47.0f, 47.0f, \
}

// TGT_CFG_CALIB
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

#endif /* !__CFG_CALIB__ */
