/**
* Copyright (c) 2020, Airtouching Intelligence Technology.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form, except as embedded into a Airtouching
* Intelligence Technology integrated circuit in a product or a software update for
* such product, must reproduce the above copyright notice, this list of
* conditions and the following disclaimer in the documentation and/or other
* materials provided with the distribution.
*
* 3. Any software provided in binary form under this license must not be reverse
* engineered, decompiled, modified and/or disassembled.
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

// #include <stdlib.h>
#include <string.h>
#include "at_error.h"
#include "osal_mem.h"
#include "osal_common.h"
#include "mem_heap.h"

#define OSAL_MALLOC_SIZE_MAX            CFG_HEAP_SIZE

static volatile bool heap_initialized = false;

/**
 * @defgroup 内存管理
 */

/**
 * Memory init.
 *
 * @return  0: 成功, 其他: 失败.
 */
s32 osal_memory_init(void)
{
	if (heap_initialized) {
		return -1;
	}

	k_mem_init();
	heap_initialized = true;

	return 0;
}

/**
 * Memory deinit.
 *
 */
void osal_memory_deinit(void)
{
	heap_initialized = false;
}


/**
 * Realloc memory.
 *
 * @param[in]  mem   current memory address point.
 * @param[in]  size  new size of the mem to remalloc.
 *
 * @return  NULL: error.
 */
void *osal_realloc(void *mem, size_t size)
{
	void *new_mem;

	if (mem == NULL) {
		if (size > 0) {
			return osal_malloc(size);
		} else {
			return NULL;
		}
	} else if (size == 0) {
		osal_free(mem);
		return NULL;
	}

	if (size >= OSAL_MALLOC_SIZE_MAX) {
		return mem;
	}

	new_mem = osal_malloc(size);
	if (!new_mem) {
		OSAL_ERR("%s, %d malloc fail.\r\n", __func__, __LINE__);
		return NULL;
	}
	memcpy(new_mem, mem, size);
	osal_free(mem);

	return new_mem;
}

/**
 * Alloc memory.
 *
 * @param[in]  size  size of the mem to malloc.
 *
 * @return  NULL: error.
 */
void *osal_malloc(size_t size)
{
	if (!heap_initialized || size <= 0 || size >= OSAL_MALLOC_SIZE_MAX) {
		return NULL;
	}

	return k_malloc(size);
}

/**
 * Alloc memory and clear to zero.
 *
 * @param[in]  nitems  number of items to malloc.
 * @param[in]  size    size of one item to malloc.
 *
 * @return  NULL: error.
 */
void *osal_calloc(size_t nitems, size_t size)
{
	return k_calloc(nitems, size);
}

/**
 * Alloc memory and clear to zero.
 *
 * @param[in]  size  size of the mem to malloc.
 *
 * @return  NULL: error.
 */
void *osal_zalloc(size_t size)
{
	return osal_calloc(1, size);
}

/**
 * Free memory.
 *
 * @param[in]  ptr  address point of the mem.
 *
 * @return  none.
 */
void osal_free(void *mem)
{
	if (!heap_initialized || !mem) {
		return;
	}

	k_free(mem);
}
