/**
 *******************************************************************************
 * @file    ytool.h
 * @author  Airtouch Software Team
 * @brief   Utils for Ymodem
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

#ifndef __YTOOL_H__
#define __YTOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/

#include "at_common.h"

/* Exported macro --------------------------------------------------------------------------------*/

#define IS_CAP_LETTER(c)        (((c) >= 'A') && ((c) <= 'F'))
#define IS_LC_LETTER(c)         (((c) >= 'a') && ((c) <= 'f'))
#define IS_09(c)                (((c) >= '0') && ((c) <= '9'))

#define ISVALIDHEX(c)           (IS_CAP_LETTER(c) || IS_LC_LETTER(c) || IS_09(c))
#define ISVALIDDEC(c)           IS_09(c)

#define CONVERTDEC(c)           (c - '0')
#define CONVERTHEX_ALPHA(c)     (IS_CAP_LETTER(c) ? ((c) - 'A'+10) : ((c) - 'a'+10))
#define CONVERTHEX(c)           (IS_09(c) ? ((c) - '0') : CONVERTHEX_ALPHA(c))

/* Data Types ------------------------------------------------------------------------------------*/

typedef enum
{
	STATUS_OK                   = 0x00U,

	STATUS_ERROR                = 0x01U,

	STATUS_BUSY                 = 0x02U,

	STATUS_TIMEOUT              = 0x03U
} status_code_t;


/* Exported functions ----------------------------------------------------------------------------*/

/**
 * @brief  Convert an Integer to a string
 * @param[in]  intnum: The integer to be converted
 * @param[out] intstr: The string output pointer
 * @retval None
 */
void ytool_int2str(u32  intnum, u8* intstr);

/**
 * @brief
 *
 * @param[in]  intstr
 * @param[out] intnum
 * @return u32 return 1 on success, 0 on failure.
 */
u32  ytool_str2int(u8* intstr, u32* intnum);

void ytool_delay(u32 delay);

#ifdef __cplusplus
}
#endif

#endif /* !__YTOOL_H__ */
