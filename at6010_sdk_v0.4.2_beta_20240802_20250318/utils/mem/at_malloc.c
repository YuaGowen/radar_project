/**
 *******************************************************************************
 * @file    at_malloc.c
 * @author  Airtouch Software Team
 * @brief   memory pool implementations
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

#include "at_malloc_defs.h"
#include "at_malloc.h"
#include "at_malloc_intern.h"


#ifdef _WIN32
u8 ulp_mem_pool_sys[ULP_MEM_POOL_SYS_SIZE];
u8 ulp_mem_pool_dsp[ULP_MEM_POOL_DSP_SIZE];
#endif

uintptr_t at_virt2phys(void* addr);

static pool_t pools[MAX_MEM_POOLS] = { {0} };
#define HEAD (pools[pool].head_of_empty_list)

#define IF_POOL_IS_INVALID  if (pool < 0 || pool >= MAX_MEM_POOLS || HEAD == NULL)

static inline int pool_from_virt_addr(void* addr) {
	uintptr_t addr_u = (uintptr_t)addr;
	for (int pool = 0; pool < MAX_MEM_POOLS; ++pool) {
		if (HEAD != NULL) {
			if (addr_u >= pools[pool].base_virt && addr_u < pools[pool].end_virt) {
				return pool;
			}
		}
	}

	return -1;
}

// pointer to next cell
static inline cell_t* next_cell(int pool, cell_t* cur_cell) {
	uintptr_t cur_cell_u =
		(uintptr_t)pools[pool].head_of_empty_list + cur_cell->next_offset;
	return (cell_t*)cur_cell_u;
}

// cell to pointer
static inline uintptr_t c2p(const cell_t* const c) {
	return (uintptr_t)(c)+(uintptr_t)cell_t_size;
}

// pointer to cell
static inline cell_t* p2c(void* p) {
	uintptr_t ptr = (uintptr_t)(p)-(uintptr_t)cell_t_size;
	return (cell_t*)ptr;
}

// c1 is adjascent to c2
static inline bool isadj(const cell_t* c1, const cell_t* c2) {
	uintptr_t c1_p_ptr = c2p(c1);
	uintptr_t c1_next_c_ptr = c1_p_ptr + (uintptr_t)(int)(c1)->size;
	uintptr_t c2_c_ptr = (uintptr_t)c2;
	return c1_next_c_ptr == c2_c_ptr;
}

int at_malloc_init_pool(int pool, void* base_virt, uintptr_t base_phys,
	int size, int reset) {
	if (pool >= MAX_MEM_POOLS || pool < 0) {
		return -1;
	}

	HEAD = (cell_t*)base_virt;
	pools[pool].end_virt = (uintptr_t)base_virt + (uintptr_t)(int)size;

	pools[pool].base_phys = base_phys;
	pools[pool].base_virt = (uintptr_t)base_virt;
	pools[pool].size = size;

	if (reset != 0) {
		HEAD->size = 0;
		HEAD->next_offset = cell_t_size;
		HEAD->flags = FLAG_EMPTY;

		cell_t* next = next_cell(pool, HEAD);
		next->size = size - 2 * cell_t_size;
		next->next_offset = 0;
		next->flags = FLAG_EMPTY;
	}

	return 0;
}

void* at_malloc_pool(int pool, int size) {
	IF_POOL_IS_INVALID{ return NULL; }

	cell_t* c1, * c_prev;

	if (size < cell_t_size) {
		size = cell_t_size;
	}

	size = ALIGN(size);

	c_prev = HEAD;
	c1 = HEAD;

	// find a big enough cell (c1->next)
	while (c1->size < size) {
		if (IS_LAST(c1) /*|| next_cell(pool, c1)->flags == FLAG_NONEMPTY*/) {
		  // returned to head_of_empty_list without finding space
			return NULL;
		}

		// try next cell
		c_prev = c1;
		c1 = next_cell(pool, c1);
	}

	// c1 is a big enough empty cell
	if (c1->size > (cell_t_size + size)) {
	  // c1 is bigger than needed
	  // split it
		uintptr_t next_ptr = c2p(c1) + (uintptr_t)(int)size;
		cell_t* c_next = (cell_t*)(next_ptr);
		c_next->size = c1->size - (size + cell_t_size);
		c_next->flags = FLAG_EMPTY;
		c_next->next_offset = c1->next_offset;

		c_prev->next_offset = OFFSET(c_next);

		c1->size = size;
	} else {
		c_prev->next_offset = c1->next_offset;
	}

	c1->flags = FLAG_NONEMPTY;

	uintptr_t ptr = c2p(c1);
	return (void*)(char*)ptr;
}

void at_free(void* ptr) {
	if (ptr == NULL) {
		return;
	}

	int pool = pool_from_virt_addr(ptr);

	IF_POOL_IS_INVALID{ return; }

	cell_t* c1, * c2, * c3;
	bool j1, j2;

	// find cell from pointer
	char* _ptr = ptr; // misra stuff...
	c2 = p2c(_ptr);
	if (c2->flags != FLAG_NONEMPTY) {
		return;
	}

	c1 = HEAD;

	c2->flags = FLAG_EMPTY;

	uintptr_t next_ptr = (uintptr_t)next_cell(pool, c1);
	uintptr_t c2_ptr = (uintptr_t)c2;

	while (next_ptr < c2_ptr && !IS_LAST(c1)) { /* find insertion point */
		c1 = next_cell(pool, c1);
		next_ptr = (uintptr_t)next_cell(pool, c1);
	}

	c3 = next_cell(pool, c1);

	if (c1 == HEAD) {
		j1 = false;
	} else {
		j1 = isadj(c1, c2); /* c1 and c2 need to be joined */
	}

	j2 = isadj(c2, c3); /* c2 and c3 need to be joined */

	// make c1 point to c2
	c1->next_offset = OFFSET(c2);
	// make c2 point to c3
	c2->next_offset = OFFSET(c3);

	if (j1) {
	  // join c1 with c2
		c1->size += cell_t_size + c2->size;
		c1->next_offset = OFFSET(c3);
		c2 = c1;
	}

	if (j2) {
	  // join c2 with c3
		c2->size += cell_t_size + c3->size;
		;
		c2->next_offset = c3->next_offset;
	}
}

uintptr_t at_virt2phys(void* addr) {
#ifdef USE_GLOBAL_MMU
	return (uintptr_t)addr;
#else
	int pool = pool_from_virt_addr(addr);
	IF_POOL_IS_INVALID{ return 0U; }

	char* _addr = addr; // misra stuff...

	uintptr_t offset = OFFSET(_addr);
	uintptr_t phys_addr = offset + pools[pool].base_phys;
	return phys_addr;
#endif
}
