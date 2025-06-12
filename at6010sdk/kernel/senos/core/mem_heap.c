/*
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

#include <string.h>
#include "mem_heap.h"

#ifndef CFG_OS

#define MEM_BYTE_ALIGNMENT 4

#if MEM_BYTE_ALIGNMENT == 32
#define MEM_BYTE_ALIGNMENT_MASK ( 0x001f )
#endif

#if MEM_BYTE_ALIGNMENT == 16
#define MEM_BYTE_ALIGNMENT_MASK ( 0x000f )
#endif

#if MEM_BYTE_ALIGNMENT == 8
#define MEM_BYTE_ALIGNMENT_MASK ( 0x0007 )
#endif

#if MEM_BYTE_ALIGNMENT == 4
#define MEM_BYTE_ALIGNMENT_MASK ( 0x0003 )
#endif

#if MEM_BYTE_ALIGNMENT == 2
#define MEM_BYTE_ALIGNMENT_MASK ( 0x0001 )
#endif

#if MEM_BYTE_ALIGNMENT == 1
#define MEM_BYTE_ALIGNMENT_MASK ( 0x0000 )
#endif

/* Block sizes must not get too small. */
#define HEAP_MINIMUM_BLOCK_SIZE  ( ( size_t ) ( xheap_struct_size << 1 ) )

/* Assumes 8bit bytes! */
#define HEAP_BITS_PER_BYTE       ( ( size_t ) 8 )

typedef struct k_block_link {
	struct k_block_link *pnext_free_block;   /*<< The next free block in the list. */
	size_t xblock_size;                      /*<< The size of the free block. */
} block_link_t;

static void insert_block_into_freelist(block_link_t *block_to_insert);

static const size_t xheap_struct_size = (sizeof(block_link_t) + ((size_t)(
										 MEM_BYTE_ALIGNMENT - 1))) & ~((size_t) MEM_BYTE_ALIGNMENT_MASK);

/* Create a couple of list links to mark the start and end of the list. */
static block_link_t xStart, *pxEnd = NULL;

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
static size_t xfree_bytes_remaining = 0U;
static size_t xmin_ever_free_bytes_remaining = 0U;
static size_t xblock_allocated_bit = 0;

static unsigned char k_heap[CFG_HEAP_SIZE] __attribute__ ((section(".heap")));
static heap_config k_heap_region[] = {
		{ k_heap, CFG_HEAP_SIZE },
		{ NULL,   0             }
};
void *k_malloc(size_t wanted_size)
{
	block_link_t *pblock, *prv_block, *new_block_link;
	void *header_ptr = NULL;
	if ((wanted_size & xblock_allocated_bit) == 0) {
		if (wanted_size > 0) {
			wanted_size += xheap_struct_size;
			/* Ensure that blocks are always aligned to the required number
			of bytes. */
			if ((wanted_size & MEM_BYTE_ALIGNMENT_MASK) != 0x00) {
				/* Byte alignment required. */
				wanted_size += (MEM_BYTE_ALIGNMENT - (wanted_size & MEM_BYTE_ALIGNMENT_MASK));
			}
		}
		if ((wanted_size > 0) && (wanted_size <= xfree_bytes_remaining)) {
			prv_block = &xStart;
			pblock = xStart.pnext_free_block;
			while ((pblock->xblock_size < wanted_size) && (pblock->pnext_free_block != NULL)) {
				prv_block = pblock;
				pblock = pblock->pnext_free_block;
			}
			if (pblock != pxEnd) {
				header_ptr = (void *)(((u8 *) prv_block->pnext_free_block) + xheap_struct_size);

				prv_block->pnext_free_block = pblock->pnext_free_block;

				/* If the block is larger than required it can be split into
				two. */
				if ((pblock->xblock_size - wanted_size) > HEAP_MINIMUM_BLOCK_SIZE) {
					new_block_link = (void *)(((u8 *) pblock) + wanted_size);

					new_block_link->xblock_size = pblock->xblock_size - wanted_size;
					pblock->xblock_size = wanted_size;

					/* Insert the new block into the list of free blocks. */
					insert_block_into_freelist(new_block_link);
				}
				xfree_bytes_remaining -= pblock->xblock_size;

				if (xfree_bytes_remaining < xmin_ever_free_bytes_remaining) {
					xmin_ever_free_bytes_remaining = xfree_bytes_remaining;
				}
				pblock->xblock_size |= xblock_allocated_bit;
				pblock->pnext_free_block = NULL;
			}
		}

	}
	return header_ptr;
}
/*-----------------------------------------------------------*/

void *k_calloc(size_t n, size_t wanted_size)
{
	void *result;
	size_t size = wanted_size * n;

	/* guard vs integer overflow, but allow n
	* to fall through and call malloc(0) */
	if (n && wanted_size != (size / n)) {
		return NULL;
	}
	if ((result = k_malloc(size)) != NULL) {
		memset(result, 0, size);
	}
	return result;
}

void k_free(void *ptr)
{
	u8 *puc = (u8 *) ptr;
	block_link_t *pxLink;

	if (ptr != NULL) {
		puc -= xheap_struct_size;
		pxLink = (void *) puc;
		if ((pxLink->xblock_size & xblock_allocated_bit) != 0) {
			if (pxLink->pnext_free_block == NULL) {
				pxLink->xblock_size &= ~xblock_allocated_bit;
				xfree_bytes_remaining += pxLink->xblock_size;
				insert_block_into_freelist(((block_link_t *) pxLink));
			}
		}
	}
}

size_t k_cur_free_size(void)
{
	return xfree_bytes_remaining;
}

size_t k_min_free_size(void)
{
	return xmin_ever_free_bytes_remaining;
}

static void insert_block_into_freelist(block_link_t *block_to_insert)
{
	block_link_t *pxIterator;
	u8 *puc;

	for (pxIterator = &xStart; pxIterator->pnext_free_block < block_to_insert; pxIterator = pxIterator->pnext_free_block) {
		/* Nothing to do here, just iterate to the right position. */
	}

	puc = (u8 *) pxIterator;
	if ((puc + pxIterator->xblock_size) == (u8 *) block_to_insert) {
		pxIterator->xblock_size += block_to_insert->xblock_size;
		block_to_insert = pxIterator;
	}

	puc = (u8 *) block_to_insert;
	if ((puc + block_to_insert->xblock_size) == (u8 *)pxIterator->pnext_free_block) {
		if (pxIterator->pnext_free_block != pxEnd) {
			/* Form one big block from the two blocks. */
			block_to_insert->xblock_size += pxIterator->pnext_free_block->xblock_size;
			block_to_insert->pnext_free_block = pxIterator->pnext_free_block->pnext_free_block;
		} else {
			block_to_insert->pnext_free_block = pxEnd;
		}
	} else {
		block_to_insert->pnext_free_block = pxIterator->pnext_free_block;
	}
	if (pxIterator != block_to_insert) {
		pxIterator->pnext_free_block = block_to_insert;
	}
}

void k_mem_init(void)
{
	block_link_t *pfirst_free_block_in_region = NULL, *previous_freeblock;
	size_t aligned_addr;
	size_t total_region_size, total_heap_size = 0;
	u32 region_cnt = 0;
	size_t addr;
	const heap_config *heap_region;
	heap_region = &(k_heap_region[region_cnt]);

	while (heap_region->size > 0) {
		total_region_size = heap_region->size;
		/* Ensure the heap region starts on a correctly aligned boundary. */
		addr = (size_t) heap_region->start_addr;
		if ((addr & MEM_BYTE_ALIGNMENT_MASK) != 0) {
			addr += (MEM_BYTE_ALIGNMENT - 1);
			addr &= ~MEM_BYTE_ALIGNMENT_MASK;
			total_region_size -= addr - (size_t) heap_region->start_addr;
		}
		aligned_addr = addr;
		if (region_cnt == 0) {
			xStart.pnext_free_block = (block_link_t *) aligned_addr;
			xStart.xblock_size = (size_t) 0;
		}
		previous_freeblock = pxEnd;

		/* pxEnd is used to mark the end of the list of free blocks and is
		inserted at the end of the region space. */
		addr = aligned_addr + total_region_size;
		addr -= xheap_struct_size;
		addr &= ~MEM_BYTE_ALIGNMENT_MASK;
		pxEnd = (block_link_t *) addr;
		pxEnd->xblock_size = 0;
		pxEnd->pnext_free_block = NULL;

		pfirst_free_block_in_region = (block_link_t *) aligned_addr;
		pfirst_free_block_in_region->xblock_size = addr - (size_t)pfirst_free_block_in_region;
		pfirst_free_block_in_region->pnext_free_block = pxEnd;

		if (previous_freeblock != NULL) {
			previous_freeblock->pnext_free_block = pfirst_free_block_in_region;
		}

		total_heap_size += pfirst_free_block_in_region->xblock_size;

		/* Move onto the next heap_config structure. */
		region_cnt++;
		heap_region = &(k_heap_region[ region_cnt ]);
	}

	xmin_ever_free_bytes_remaining = total_heap_size;
	xfree_bytes_remaining = total_heap_size;
	xblock_allocated_bit = ((size_t) 1) << ((sizeof(size_t) * HEAP_BITS_PER_BYTE) - 1);
}

#endif /* !CFG_OS */
