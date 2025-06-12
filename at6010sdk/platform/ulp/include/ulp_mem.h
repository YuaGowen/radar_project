/**
 *******************************************************************************
 * @file    ulp_mem.h
 * @author  Airtouch Software Team
 * @brief   Retention memory management
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

#ifndef __ULP_MEM_H__
#define __ULP_MEM_H__

/* Includes --------------------------------------------------------------------------------------*/

#include "at_types.h"


/* Data Types ------------------------------------------------------------------------------------*/

typedef enum {
	/* AT reserved */
	ULP_MEM_AT_ID_1        = 0,
	ULP_MEM_AT_ID_2        = 1, /* fft module  */
	ULP_MEM_AT_ID_3        = 2, /* algo module */
	ULP_MEM_AT_ID_4        = 3, /* ulp context */
	ULP_MEM_AT_ID_5        = 4,
	ULP_MEM_AT_ID_6        = 5,
	ULP_MEM_AT_ID_7        = 6,
	ULP_MEM_AT_ID_8        = 7,
	ULP_MEM_AT_ID_9        = 8,
	ULP_MEM_AT_ID_10       = 9,
	ULP_MEM_AT_ID_11       = 10,
	ULP_MEM_AT_ID_12       = 11,
	ULP_MEM_AT_ID_13       = 12,
	ULP_MEM_AT_ID_14       = 13,
	ULP_MEM_AT_ID_15       = 14,
	ULP_MEM_AT_ID_16       = 15,
	/* User can use these IDs */
	ULP_MEM_USER_ID_1      = 16,
	ULP_MEM_USER_ID_2      = 17,
	ULP_MEM_USER_ID_3      = 18,
	ULP_MEM_USER_ID_4      = 19,
	ULP_MEM_USER_ID_5      = 20,
	ULP_MEM_USER_ID_6      = 21,
	ULP_MEM_USER_ID_7      = 22,
	ULP_MEM_USER_ID_8      = 23,
	ULP_MEM_USER_ID_9      = 24,
	ULP_MEM_USER_ID_10     = 25,
	ULP_MEM_USER_ID_11     = 26,
	ULP_MEM_USER_ID_12     = 27,
	ULP_MEM_USER_ID_13     = 28,
	ULP_MEM_USER_ID_14     = 29,
	ULP_MEM_USER_ID_15     = 30,
	ULP_MEM_USER_ID_16     = 31,

	ULP_MEM_ID_MAX         = 32
} ulp_mem_id_t;

/* Constants -------------------------------------------------------------------------------------*/

/**
 * @defgroup MEM_POOL_CFG Memory Pool Configuration
 * @{
 */


/**
 * @defgroup MEM_POOL_SYS_CFG Memory Pool -- Sys Pool Configuration
 * @{
 */
#define ULP_MEM_POOL_SYS_ID               0
#define ULP_MEM_POOL_SYS_BASE             (0x20004000 + 128)
#define ULP_MEM_POOL_SYS_SIZE             (0x800 - 128)
/**
 * @}
 */

/**
 * @defgroup MEM_POOL_DSP_CFG Memory Pool -- DSP Pool Configuration
 * @{
 */
#define ULP_MEM_POOL_DSP_ID               1
#define ULP_MEM_POOL_DSP_BASE             (0x20004800)
#define ULP_MEM_POOL_DSP_SIZE             (0x7800)
/**
 * @}
 */

/**
 * @}
 */


/* Function Declarations -------------------------------------------------------------------------*/

s32   ulp_mem_init(void);
void* ulp_mem_get_addr(ulp_mem_id_t id);
void  ulp_mem_set_addr(ulp_mem_id_t id, u32 addr);


#endif /* !__ULP_MEM_H__ */
