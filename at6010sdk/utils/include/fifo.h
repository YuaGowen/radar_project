/**
 *******************************************************************************
 * @file    fifo.h
 * @author  Airtouch Software Team
 * @brief   FIFO (First In First Out)
 *******************************************************************************
 * @copyright Copyright (c) 2024, Airtouching Intelligence Technology.
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

#ifndef __FIFO_H__
#define __FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/

#include "at_common.h"

/* Data Types ------------------------------------------------------------------------------------*/

typedef enum {
	FIFO_WIDTH_8BITS  = 0,
	FIFO_WIDTH_16BITS = 1,
	FIFO_WIDTH_32BITS = 2,
} fifo_width_t;


typedef struct {
	u32 read;
	u32 write;
	u32 used;

	u8* item_buf;   /**< buffer to hold data */
	u32 item_cnt;
	u32 item_width; /**< see @ref fifo_width_t */
} fifo_t;

/* Function Declarations -------------------------------------------------------------------------*/

/**
 *@brief Init a FIFO with associated parameters.
 *
 * @param[in] pf FIFO object
 * @param[in] buf buffer to hold data
 * @param[in] buf_len buffer length in bytes
 * @param[in] width FIFO data width, see @ref fifo_width_t
 * @return 0 on success, otherwise -1
 */
s32 fifo_init(fifo_t* pf, u8 *buf, u32 buf_len, u32 width);

/**
 *@brief Add one value to FIFO.
 *
 * @param pf
 * @param val
 * @return 0 on success, otherwise -1
 */
s32 fifo_enqueue(fifo_t* pf, u32 val);

/**
 *@brief Add n entries to the FIFO.
 *
 * @param pf
 * @param[in] val buffer of s16 type to be enqueued
 * @param n number of entries to be enqueued
 * @return how many entries were enqueued, or -1 on error.
 */
s32 fifo_enqueue_n_s16(fifo_t* pf, s16* val, u32 n);

/**
 * @brief Fetch a data from the FIFO
 *
 * @param pf
 * @param val
 * @return 0 on success, otherwise -1
 */
s32 fifo_dequeue(fifo_t* pf, u32* val);

/**
 *@brief Get valid items in FIFO.
 *
 * @param[in] pf
 * @param[out] val
 * @return 0 on success, otherwise -1
 */
s32 fifo_get_used(fifo_t* pf, u32* val);

/**
 * @brief Only clear the data at the valid positions in the FIFO, and keep the other positions unchanged.
 *
 * @param pf
 * @return 0 on success, otherwise -1
 */
s32 fifo_clear(fifo_t* pf);

#ifdef __cplusplus
}
#endif

#endif /* !__FIFO_H__ */
