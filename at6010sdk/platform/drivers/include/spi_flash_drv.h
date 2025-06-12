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

#ifndef __SPIFLS_DRV_H__
#define __SPIFLS_DRV_H__

#include "at_types.h"
#include "spi_flash_host.h"

#define SPIFLS_CMD_DP                   0xb9 /* Deep Power-down(DP) */
#define SPIFLS_CMD_RDP                  0xAB /**< Release Power-down */

#define KBYTES		1024
#define MBYTES		(1024*KBYTES)

/* flash basic info */
#define FLASH_MAX_SIZE			(KBYTES * 256)
#define FLASH_BASE_ADDR			0x0a000000
#define FLASH_PAGE_SIZE			(256)


typedef enum {
	SPIFLS_MFACT_SPANSION = 0x01,
	SPIFLS_MFACT_EON = 0x1c,
	SPIFLS_MFACT_MXIC = 0xc2,
	SPIFLS_MFACT_GIGADEVICE = 0xc8,
	SPIFLS_MFACT_BERGMICRO = 0xe0,
	SPIFLS_MFACT_WINBOND = 0xef,
	SPIFLS_MFACT_FIDELIX = 0xf8,
	SPIFLS_MFACT_ESMT = 0x8c,
	SPIFLS_MFACT_XMCKGD = 0x20,
} spifls_manufacturer_t;

typedef enum {
	SPIFLS_TYPE_SPANSION_30V = 0x02,
	SPIFLS_TYPE_EON_30V = 0x30,
	SPIFLS_TYPE_EON_18V = 0x38,
	SPIFLS_TYPE_MXIC_30V = 0x5E,
	SPIFLS_TYPE_MXIC_18V = 0x25,
	SPIFLS_TYPE_GIGADEVICE_30V = 0x40,
	SPIFLS_TYPE_GIGADEVICE_18V = 0x60,
	SPIFLS_TYPE_WINBOND_30V = 0x40,
	SPIFLS_TYPE_WINBOND_18V = 0x60,
	SPIFLS_TYPE_FIDELIX_30V = 0x32,
	SPIFLS_TYPE_FIDELIX_18V = 0x42,
	SPIFLS_TYPE_ESMT_30V = 0x41,
	SPIFLS_TYPE_ESMT_18V = 0x25,
} spifls_mem_type_t;

typedef struct {
    spifls_manufacturer_t manufacturer;
	spifls_mem_type_t mem_type;
	u32 uid[2];
} spifls_device_id_t;

typedef enum {
	SPIFLS_LINE_SERIAL,
	SPIFLS_LINE_DUAL,
	SPIFLS_LINE_QUAD
} spifls_line_mode_t;

typedef enum {
	SPIFLS_ERR_NO = 0,

	SPIFLS_ERR_ERASE = -10,
	SPIFLS_ERR_WRITE,

	/**
	 * This error occurs when a command requiring sector aligned addresses
	 * as parameters is called with unaligned addresses.
	 */
	SPIFLS_ERR_ALIGN,

	/* An erase or write operation was attempted on a protected sector */
	SPIFLS_ERR_PROTECT,

	SPIFLS_ERR_SUSPEND,
} spifls_err_id_t;

typedef struct {
	u32 bank_start_addr;

	/**
	 * Composition of the bank, which can be made of up to 3 different sector
	 * types. One line is for one sector type. The first column is the number
	 * of sectors of this type and the second column is the size of this type
	 * of sector.
	 */
	u32 sector_layout_tbl[3][2];
} spifls_bank_layout_t;

/* This structure describes a flash through its different banks. */
typedef struct {
	u8 num_of_bank;

	/* Table of pointers to the structures describing the different banks */
	const spifls_bank_layout_t* bank_layout;

	/* Size of the flash in bytes. */
	u32 full_size;

	/* User Data Field Base Address */
	u32 user_data_base;

	/* User Data Field Size */
	u32 user_data_size;

	/* User Data Sector Logical Block Size */
	u32 user_data_sect_logic_blk_size;
} spifls_flash_layout_t;

spifls_flash_layout_t* spi_flash_init(void);
spifls_err_id_t spifls_set_line_mode(spifls_line_mode_t mode);

/**
 * This function reads data from the flash. It gets its data from fls_addr,
 * copies byte_size bytes to buffer. read_byte_size is filled with the
 * actual number of read bytes (Equal to byte_size, or less in case of error)
 *
 * @fls_addr: The byte offset within the flash chip (Take care of the positions
 *              of the code in the flash)
 * @byte_size: Number of bytes to read from the flash
 * @read_byte_size: Filled with the number of bytes actually read
 * @buffer: Buffer to fill with data from flash
 *
 * @return #SPIFLS_ERR_NO
 */
spifls_err_id_t spi_flash_read(u32 fls_addr, u32  byte_size, u32 *read_byte_size, u8 *buffer);

spifls_err_id_t spi_flash_ll_read(u32 fls_addr, u32  byte_size, u32 *read_byte_size, u8 *buffer);

/**
 * This function writes data into the flash. It gets its data from buffer, copies
 *  byte_size bytes to the flash location designed by  fls_addr.
 * written_byte_size is filled with the actual number of written bytes (Equal to
 * size, or less in case of error).
 *
 * @fls_addr: The byte offset within the flash chip. (Take care not to overwrite
 *              the code present at the beginning of the flash)
 * @byte_size: Number of bytes to write in flash
 * @written_byte_size: Number of bytes actually written in flash
 * @buffer: Buffer where to get the data to write in the flash
 *
 * @return #SPIFLS_ERR_NO, #SPIFLS_ERR_WRITE or #SPIFLS_ERR_PROTECT
 *           whether the operation succeeded or failed.
 */
spifls_err_id_t spi_flash_write(u32 fls_addr, u32 byte_size, u32 *written_byte_size, const u8 *buffer);

/**
 * This function erases contiguous flash sectors.
 * Addresses are byte offset within the flash chip and must be aligned on sectors:
 * - The start_addr is the address of the first sector to erase.
 * - The end_addr is the address of the first sector NOT to erase.
 *   If end_addr is NULL, only one sector will be erased.
 *
 * @start_addr: The address of the first sector to erase
 * @end_addr: The address of the first sector NOT to erase.
 *              If  NULL, only one sector will be erased.
 * @return #SPIFLS_ERR_NO, #SPIFLS_ERR_ERASE, #SPIFLS_ERR_ALIGN or #SPIFLS_ERR_PROTECT
 *           whether the operation succeeded or failed
 *
 */
spifls_err_id_t spi_flash_erase(u32 start_addr, u32 end_addr);

void spi_flash_cache_on(void);
void spi_flash_cache_off(void);

/**
 * @brief Deep Power-Down
 *
 * @retval none
 */
void spi_flash_deep_pd(void);

/**
 * @brief Release Power-Down
 *
 * @retval none
 */
void spi_flash_release_deep_pd(void);

#endif
