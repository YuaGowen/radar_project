/**
 * Copyright (c) 2022, Airtouching Intelligence Technology.
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
 */

#ifndef __AT_COMMON_H__
#define __AT_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_types.h"
#include "at_assert.h"
#include "at_error.h"
#include "at_sizes.h"

/*******************************************************************************
**                        Misc Operations                                     **
*******************************************************************************/

#define ARRAY_SIZE(x)                   (sizeof(x) / sizeof((x)[0]))
#define ARRAY_ROW(a)                    (ARRAY_SIZE(a))
#define ARRAY_COL(a)                    (ARRAY_SIZE(a[0]))

#ifndef MIN
#define MIN(a, b)                       (((a) < (b)) ? (a) : (b))
#endif

#ifndef	MAX
#define MAX(a, b)                       (((a) > (b)) ? (a) : (b))
#endif

#ifndef	ALIGN
#define ALIGN(x, a)                     (((x) + ((a) - 1)) & ( ~((a) - 1)))
#endif

#ifndef BYTE_ALIGN
#define BYTE_ALIGN(x, a)                ((((x) + ((a) - 1)) / (a)) * (a))
#endif

#ifndef	ROUND
#define ROUND(x)                        (s32)((x) + 0.5)
#endif

#ifndef DIV_ROUND
#define DIV_ROUND(x, divisor)           (((x) + (divisor)-1) / (divisor))
#endif


#ifndef UNUSED
#define UNUSED(x)                       ((void)(x))
#endif

#ifndef WAIT_FOREVER
#define WAIT_FOREVER                    (0xffffffffU)
#endif

#ifndef NO_WAIT
#define NO_WAIT                         (0x0)
#endif

#define TO_CHAR(n)                      ((char)((n) + '0'))
#define STR(x)                          #x
#define XSTR(x)                         STR(x)

/*******************************************************************************
**                    Register/Memory Operations                              **
*******************************************************************************/

#define BIT(n)                          (1ULL << (n))
#define SET_BIT(reg, bit)               ((reg) |= (bit))
#define CLEAR_BIT(reg, bit)             ((reg) &= ~(bit))
#define READ_BIT(reg, bit)              ((reg) & (bit))
#define CLEAR_REG(reg)                  ((reg) = (0x0))
#define WRITE_REG(reg, value)           ((reg) = (value))
#define READ_REG(reg)                   ((reg))
#define MODIFY_REG(reg, clear_mask, set_mask) \
	WRITE_REG((reg), (((READ_REG(reg)) & (~(clear_mask))) | (set_mask)))

#define MEM8_READ(addr)                 (*(UINT8 *)(addr))
#define MEM8_WRITE(addr, value)         ((*(UINT8 *)(addr)) = (UINT8)(value))
#define MEM16_READ(addr)                (*(UINT16 *)(addr))
#define MEM16_WRITE(addr, value)        ((*(UINT16 *)(addr)) = (UINT16)(value))
#define MEM_READ(addr)                  (*(UINT32 *)(addr))
#define MEM_WRITE(addr, value)          ((*(UINT32 *)(addr)) = (UINT32)(value))

#ifdef __cplusplus
}
#endif

#endif /* __AT_COMMON_H__ */
