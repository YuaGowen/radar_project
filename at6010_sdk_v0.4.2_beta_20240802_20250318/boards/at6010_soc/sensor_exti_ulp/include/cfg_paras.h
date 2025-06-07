/**
 *******************************************************************************
 * @file    cfg_paras.h
 * @author  Airtouch Software Team
 * @brief   Rader misc parameters of the current project
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

#ifndef __CFG_PARAS__
#define __CFG_PARAS__

/**
 * @addtogroup AT6010_APP_LAMPS_OS
 * @{
 */

/**
 * @defgroup TGT_RADAR_PARAS Radar Parameters
 * @brief Radar Parameters
 * @{
 */

/**
 * @brief Distance parameters for radar Motion Detection
 */
#define RAD_MOT_DIST_PARAS                              \
{                                                       \
	/* level, delta sta, sensitivity, delta sto. */     \
	{/* 0 */  0x00,      0x04,        RAD_DET_MOT_MAX}, \
	{/* 1 */  0x00,      0x04,        950},             \
	{/* 2 */  0x00,      0x04,        900},             \
	{/* 3 */  0x00,      0x04,        850},             \
	{/* 4 */  0x00,      0x04,        800},             \
	{/* 5 */  0x00,      0x04,        750},             \
	{/* 6 */  0x00,      0x04,        700},             \
	{/* 7 */  0x00,      0x04,        650},             \
	{/* 8 */  0x00,      0x04,        600},             \
	{/* 9 */  0x00,      0x04,        550},             \
	{/* 10 */ 0x00,      0x04,        500},             \
	{/* 11 */ 0x00,      0x04,        450},             \
	{/* 12 */ 0x00,      0x04,        400},             \
	{/* 13 */ 0x00,      0x04,        350},             \
	{/* 14 */ 0x00,      0x04,        300},             \
	{/* 15 */ 0x00,      0x04,        250},             \
	{/* 16 */ 0x00,      0x04,        200},             \
	{/* 17 */ 0x00,      0x04,        150},             \
	{/* 18 */ 0x00,      0x04,        100},             \
	{/* 19 */ 0x00,      0x04,        50 },             \
	{/* 20 */ 0x00,      0x04,        50 },             \
	{/* 21 */ 0x00,      0x04,        50 },             \
	{/* 22 */ 0x00,      0x04,        50 },             \
	{/* 23 */ 0x00,      0x04,        50 },             \
	{/* 24 */ 0x00,      0x04,        50 },             \
	{/* 25 */ 0x00,      0x04,        50 },             \
	{/* 26 */ 0x00,      0x04,        50 },             \
	{/* 27 */ 0x00,      0x04,        50 },             \
	{/* 28 */ 0x00,      0x04,        50 },             \
	{/* 29 */ 0x00,      0x04,        50 },             \
	{/* 30 */ 0x00,      0x04,        50 },             \
	{/* 31 */ 0x00,      0x04,        50 },             \
}

/**
 * @brief bhr-detection distance level: 0 ~ 31, value range: 6 ~ 600.
 */
#define RAD_BHR_DIST_PARAS                              \
{                                                       \
	/* level, delta sta, sensitivity, delta sto. */     \
	{/* 0 */  0x00,      0x03,        RAD_DET_BHR_MAX}, \
	{/* 1 */  0x00,      0x03,        600},             \
	{/* 2 */  0x00,      0x03,        550},             \
	{/* 3 */  0x00,      0x03,        500},             \
	{/* 4 */  0x00,      0x03,        450},             \
	{/* 5 */  0x00,      0x03,        400},             \
	{/* 6 */  0x00,      0x03,        350},             \
	{/* 7 */  0x00,      0x03,        300},             \
	{/* 8 */  0x00,      0x03,        250},             \
	{/* 9 */  0x00,      0x03,        200},             \
	{/* 10 */ 0x00,      0x03,        150},             \
	{/* 11 */ 0x00,      0x03,        100},             \
	{/* 12 */ 0x00,      0x03,        50 },             \
}

/**
 * micro-detection distance level: 0 ~ 31, value range: 10 ~ 16000.
 */
#define RAD_MICRO_DIST_PARAS                             \
{                                                        \
	/* level, reserve, sensitivity , delta. */           \
	{/* 0 */  0x00,    0x02,         RAD_DET_MICRO_MAX}, \
	{/* 1 */  0x00,    0x02,         600},               \
	{/* 2 */  0x00,    0x02,         550},               \
	{/* 3 */  0x00,    0x02,         500},               \
	{/* 4 */  0x00,    0x02,         450},               \
	{/* 5 */  0x00,    0x02,         400},               \
	{/* 6 */  0x00,    0x02,         350},               \
	{/* 7 */  0x00,    0x02,         300},               \
	{/* 8 */  0x00,    0x02,         250},               \
	{/* 9 */  0x00,    0x02,         200},               \
	{/* 10 */ 0x00,    0x02,         150},               \
	{/* 11 */ 0x00,    0x02,         100},               \
	{/* 12 */ 0x00,    0x02,         50},                \
}

/**
 * @brief The distance between radar device and the start detection point
 */
#define RAD_DET_DIST_START   0

/**
 * @brief The max distance of motion detection
 */
#define RAD_DET_MOT_MAX     800

/**
 * @brief The max distance of micro-motion detection
 */
#define RAD_DET_MICRO_MAX   650

/**
 * @brief The max distance of breath and heart-rate detection
 */
#define RAD_DET_BHR_MAX     650

// TGT_RADAR_PARAS
/**
 * @}
 */

// AT6010_APP_LAMPS_OS
/**
 * @}
 */

#endif /* !__CFG_PARAS__ */
