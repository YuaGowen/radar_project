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

#include "CMSDK_CM0plus.h"
#include "mpu.h"
#include "atlog.h"


void mpu_enable(void)
{
	__DMB();
	MPU->CTRL = 1;
	__DSB();
	__ISB();
}

void mpu_disable(void)
{
	__DMB();
	MPU->CTRL = 0;
	__DSB();
	__ISB();
}
static void mpu_region_config(mpu_region_conf conf)
{
	if (conf.region_enable) {
		MPU->RBAR = ((conf.base_addr & MPU_RBAR_ADDR_Msk) |
			     (conf.region_nb << MPU_RBAR_REGION_Pos) |
			     (0x1 << MPU_RBAR_VALID_Pos));

		MPU->RASR = ((conf.exeable ? 0 : 1)       << MPU_RASR_XN_Pos)
			    | (conf.ap                    << MPU_RASR_AP_Pos)
			    | (conf.tex                   << MPU_RASR_TEX_Pos)
			    | ((conf.shareable   ? 1 : 0) << MPU_RASR_S_Pos)
			    | ((conf.cacheable   ? 1 : 0) << MPU_RASR_C_Pos)
			    | ((conf.bufferable  ? 1 : 0) << MPU_RASR_B_Pos)
			    | (conf.srd                   << MPU_RASR_SRD_Pos)
			    | (conf.region_size           << MPU_RASR_SIZE_Pos)
			    | (1                          << MPU_RASR_ENABLE_Pos);
		__DSB();
		__ISB();
	}
}
void mpu_region_setup(u32 base_addr, mpu_region_size region_size,
		      mpu_region_nb region_nb, mpu_access_attr mode)
{

	mpu_region_conf conf = {
		.region_enable = true,
		.region_nb = region_nb,
		.base_addr = base_addr,
		.region_size = region_size,
		.ap = mode,
		.exeable = true,
		.shareable = true,
		.cacheable  = true,
		.bufferable  = true,
		.srd = 0,
		.tex = 0,
	};
	mpu_region_config(conf);
	return;

}
void mpu_region_init(void)
{
	mpu_region_conf conf[] = {MPU_FLASH_CONF_DEFAULT,         MPU_SRAM_CONF_DEFAULT_P0,
	                          MPU_SRAM_CONF_DEFAULT_P1,       MPU_PERIPHERAL_CONF_DEFAULT_P0,
	                          MPU_PERIPHERAL_CONF_DEFAULT_P1, MPU_PERIPHERAL_CONF_DEFAULT_P2,
	                          MPU_CACHE_CONF_DEFAULT};
	u32 len = sizeof(conf) / sizeof(mpu_region_conf);

	for (u8 i = 0; i < len; i++) {
		mpu_region_config(conf[i]);
	}
	mpu_enable();
	return;
}
