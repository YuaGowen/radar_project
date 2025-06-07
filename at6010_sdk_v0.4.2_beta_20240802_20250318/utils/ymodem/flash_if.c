/**
 *******************************************************************************
 * @file    flash_if.c
 * @author  Airtouch Software Team
 * @brief   Flash interface
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

#include "flash_if.h"
#include "spi_flash_drv.h"
#include "atlog.h"

#if defined(CFG_OTA)

u32 flash_if_erase(u32 start, u32 end)
{
	spifls_err_id_t err = 0;

	err = spi_flash_erase(start, end);
	if (err != SPIFLS_ERR_NO) {
		loge("%s | Failed to erase flash at 0x%0X, errno = %u\r\n", __FUNCTION__, start, (u16)err);
		return FLASHIF_ERASE_ERROR;
	}

	return FLASHIF_OK;
}

u32 flash_if_program(u32 offset, uint8_t* buffer, u32 length)
{
	u32 written_bytes = 0;
	spifls_err_id_t err = SPIFLS_ERR_NO;

	// logi("%s | addr: 0x%0X, len = %u, 0x%0x 0x%0x 0x%0x 0x%0x ...\r\n", __FUNCTION__,
	// 	offset, length, *(buffer), *(buffer+1), *(buffer+2), *(buffer+3));

	err = spi_flash_write(offset, length, &written_bytes, (const u8 *)buffer);

	if ((err != SPIFLS_ERR_NO) || (written_bytes != length)) {
		loge("%s | err: %d\r\n", __FUNCTION__, err);
		return FLASHIF_WRITING_ERROR;
	}

	return FLASHIF_OK;
}

#endif /* !CFG_OTA */
