/**
 * Copyright (c) 2023, Airtouching Intelligence Technology.
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
#include "osal_common.h"
#include "at_types.h"
#include "osal_mem.h"
#include "string.h"

s32 osal_memory_init(void)
{
	/* Heap is initialized in pvPortMalloc() of heap_4.c */
	return 0;
}

void *osal_malloc(u32 size)
{
	void *mem_ptr = pvPortMalloc(size);

	if (mem_ptr == NULL)
		OSAL_ERR("malloc failed !\n\r");

	return mem_ptr;
}

void *osal_calloc(u32 n, u32 wanted_size)
{
	void *result = NULL;
	size_t size = wanted_size * n;

	/* guard vs integer overflow, but allow n
	 * to fall through and call malloc(0) */
	if ((n > 0U) && (wanted_size != (size / n))) {
		return result;
	}

	result = pvPortMalloc(size);
	if (result != NULL) {
		memset(result, 0, size);
	} else {
		OSAL_ERR("malloc failed !\n\r");
	}

	return result;
}

void osal_free(void *ptr)
{
	vPortFree(ptr);
	return;
}

u32 osal_get_free_size(void)
{
	return xPortGetFreeHeapSize();
}

u32 osal_get_min_free_size(void)
{
	return xPortGetMinimumEverFreeHeapSize();
}
