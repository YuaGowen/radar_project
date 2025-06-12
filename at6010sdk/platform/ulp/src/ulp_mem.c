/**
 *******************************************************************************
 * @file    ulp_mem.c
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

/* Includes --------------------------------------------------------------------------------------*/
#include <string.h>
#include "at_malloc_defs.h"
#include "at_malloc.h"
#include "atlog.h"

/* Private Variables -----------------------------------------------------------------------------*/
#ifndef _WIN32
#ifdef ULP_PROG_1
static u32 ret_ram_addr_tbl[ULP_MEM_ID_MAX] __attribute__((section(".sec_ret_ram_addr_tbl")));
#endif
#ifdef ULP_PROG_2
typedef u32 tbl_t[ULP_MEM_ID_MAX];
typedef tbl_t* tbl_ptr_t;
tbl_ptr_t ret_ram_addr_tbl_ptr = (tbl_ptr_t)0x20004000;
#endif
#else
static u32 mem_addr[128];
static u32 ret_ram_addr_tbl[ULP_MEM_ID_MAX];
typedef u32 tbl_t[ULP_MEM_ID_MAX];
typedef tbl_t* tbl_ptr_t;
tbl_ptr_t ret_ram_addr_tbl_ptr = (tbl_ptr_t)mem_addr;
#endif

/**
 * @brief
 * @return int
 * @note This function can only be called in flash-run.
 */
int ulp_mem_init(void)
{
	/* Pool 0*/
	int ret = at_malloc_init_pool(AT_MEM_POOL_0_ID, (void *)AT_MEM_POOL_0_BASE, (uintptr_t)AT_MEM_POOL_0_BASE, (int)AT_MEM_POOL_0_SIZE, 1);
	if (0 != ret) {
		loge("at_malloc_init_pool 0 failed!!! \r\n");
		return ret;
	}

#if (AT_MEM_POOL_CNT > 1)
	/* Pool 1*/
	ret = at_malloc_init_pool(AT_MEM_POOL_1_ID, (void *)AT_MEM_POOL_1_BASE, (uintptr_t)AT_MEM_POOL_1_BASE, (int)AT_MEM_POOL_1_SIZE, 1);
	if (0 != ret) {
		loge("at_malloc_init_pool 1 failed!!! \r\n");
		return ret;
	}
#endif

#ifdef ULP_PROG_1
	memset((void *)ret_ram_addr_tbl, 0, sizeof(ret_ram_addr_tbl));
#endif

	return ret;
}

void *ulp_mem_get_addr(ulp_mem_id_t id)
{
	void *addr = NULL;
#ifdef ULP_PROG_1
	addr = (void *)ret_ram_addr_tbl[id];
#endif
#ifdef ULP_PROG_2
	addr = (void *)(*ret_ram_addr_tbl_ptr)[id];
#endif
	return addr;
}

void ulp_mem_set_addr(ulp_mem_id_t id, u32 addr)
{
#if 0
	if (ret_ram_addr_tbl[id] != 0) {
		loge("Error: cannot set twice for module %u \r\n", (u32)id);
		return;
	}
#endif

#ifdef ULP_PROG_1
	ret_ram_addr_tbl[id] = addr;
#endif
#ifdef ULP_PROG_2
	(*ret_ram_addr_tbl_ptr)[id] = addr;
#endif
}
