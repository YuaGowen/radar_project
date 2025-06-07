/**
 *******************************************************************************
 * @file    at_types.h
 * @author  Airtouch Software Team
 * @brief   Airtouch type definitions
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

#ifndef __AT_TYPES_H__
#define __AT_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef HAVE_TARGET_CONFIG_H
#include "target_config.h"
#endif

/* Global Types --------------------------------------------------------------*/

typedef unsigned char                   u8;
typedef unsigned short                  u16;
typedef unsigned int                    u32;
typedef unsigned long long              u64;

typedef signed char                     s8;
typedef signed short                    s16;
typedef signed int                      s32;
typedef signed long long                s64;

typedef unsigned char                   uint8;
typedef unsigned short int              uint16;
typedef unsigned int                    uint32;
typedef unsigned long long              uint64;
typedef signed char                     int8;
typedef signed short int                int16;
typedef signed int                      int32;
typedef signed long long                int64;

#ifndef size_t
typedef unsigned int                    size_t;
#endif /* size_t */

#ifndef NULL
#define NULL                            ((void *)0)
#endif

#ifndef bool
typedef unsigned char                   bool;
#endif

#ifndef TRUE
#define TRUE                            (1U)
#endif

#ifndef FALSE
#define FALSE                           (0U)
#endif

#if !defined(false) && !defined(true)
enum {
	false = 0,
	true = 1
};
#endif

/* Global Constants ----------------------------------------------------------*/
#ifndef SHRT_MAX
#define SHRT_MAX                        ((s16)(USHRT_MAX >> 1))
#endif /* SHRT_MAX */

#ifndef SHRT_MIN
#define SHRT_MIN                        ((s16)(-SHRT_MAX - 1))
#endif /* SHRT_MIN */

#ifndef USHRT_MAX
#define USHRT_MAX                       ((u16)(~0U))
#endif /* USHRT_MAX */

#ifndef INT_MAX
#define INT_MAX                         ((int)(~0U >> 1))
#endif /* INT_MAX */

#ifndef INT_MIN
#define INT_MIN                         (-INT_MAX - 1)
#endif /* INT_MIN */

#ifndef UINT_MAX
#define UINT_MAX                        (~0U)
#endif /* UINT_MAX */

#ifndef LONG_MAX
#define LONG_MAX                        ((long)(~0UL >> 1))
#endif /* LONG_MAX */

#ifndef LONG_MIN
#define LONG_MIN                        (-LONG_MAX - 1)
#endif /* LONG_MIN */

#ifndef ULONG_MAX
#define ULONG_MAX                       (~0UL)
#endif /* ULONG_MAX */

#ifndef LLONG_MAX
#define LLONG_MAX                       (9223372036854775807LL)
#endif /* LLONG_MAX */

#ifndef LLONG_MIN
#define LLONG_MIN                       (-LLONG_MAX - 1)
#endif /*  */

#ifndef ULLONG_MAX
#define ULLONG_MAX                      (~0ULL)
#endif /* ULLONG_MAX */

#ifdef __cplusplus
}
#endif

#endif /* __AT_TYPES_H__ */
