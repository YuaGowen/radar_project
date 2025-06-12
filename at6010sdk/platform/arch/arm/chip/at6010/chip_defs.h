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

#ifndef __CHIP_DEFS_H__
#define __CHIP_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * chip rom(romcode) memory map.
 */
#define ROM_BASE_ADDR                0x1d000000
#define ROM_SIZE                     0x00002000

/**
 * chip sram memory map.
 */
#define SRAM_BASE_ADDR               0x20000000
#define SRAM_SIZE                    0x00030000

/**
 * flash(direct/no_cache) memory map: 0x0a000000 - 0x0bffffff
 */
#define FLASH_BASE_ADDR              0x0a000000
#define FLASH_SIZE                   0x00040000

/**
 * flash(through cache) memory map: 0x08000000 - 0x09ffffff
 */
#define CACHE_FLASH_ADDR             0x08000000
#define CACHE_FLASH_SIZE             0x00040000

/**
 * flash sys data(through cache) memory map: 0x0803d000 - 0x0803dfff
 */
#define FLASH_SYS_DATA_ADDR          0x0803d000
#define FLASH_SYS_DATA_SIZE          0x00001000

/**
 * flash user data(through cache) memory map: 0x0803e000 - 0x0803ffff
 */
#define FLASH_USER_DATA_ADDR         0x0803e000
#define FLASH_USER_DATA_SIZE         0x00002000

/**
 * cache sram memory map: 0x22000000 - 0x22001fff
 */
#define CACHE_SRAM_ADDR              0x22000000
#define CACHE_SRAM_SIZE              0x00004000

/**
 * other addr definition.
 */
#define FLASH_HEAD_ADDR              0x0a000000
#define FLASH_HEAD_SIZE              0x00000100
#define FLASH_MAGIC_CODE             0x5820acef

#ifdef __cplusplus
}
#endif

#endif /* !__CHIP_DEFS_H__ */
