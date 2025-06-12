/**
 *******************************************************************************
 * @file    at_malloc_intern.h
 * @author  Airtouch Software Team
 * @brief   memory pool private defines
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

#ifndef __AT_MALLOC_INTERN_H__
#define __AT_MALLOC_INTERN_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX_MEM_POOLS
#define MAX_MEM_POOLS         8
#endif

#define ALIGNNUM              (4)
#define ALIGNMASK             (ALIGNNUM-1)
#define ALIGN(s)              (( ((s)+ALIGNMASK) / ALIGNNUM) * ALIGNNUM)

#define FLAG_EMPTY            0xf0f0U
#define FLAG_NONEMPTY         0xf1f1U

#define IS_LAST(c)            ( (c)->next_offset == 0U )
#define OFFSET(c)             ((uintptr_t)(c) - (uintptr_t)HEAD)

typedef struct cell {
	int       size;
	unsigned  flags;
	uintptr_t next_offset;
} cell_t;

typedef struct pool {
	uintptr_t base_phys;
	uintptr_t base_virt;
	uintptr_t end_virt;
	cell_t*   head_of_empty_list;
	int       size;
} pool_t;

static const int cell_t_size = (ALIGN((int)sizeof(cell_t)));

#ifdef __cplusplus
}
#endif

#endif /* !__AT_MALLOC_INTERN_H__ */
