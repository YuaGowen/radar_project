/**
 *******************************************************************************
 * @file    ymodem.h
 * @author  Airtouch Software Team
 * @brief   Ymodem protocol
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

#ifndef __YMODEM_H__
#define __YMODEM_H__

#ifdef __cplusplus
extern "C" {
#endif


/* Includes --------------------------------------------------------------------------------------*/

#include "at_types.h"

/* Exported types --------------------------------------------------------------------------------*/

typedef enum
{
	YMODEM_STATUS_OK             = 0x00,
	YMODEM_STATUS_ERROR          = 0x01,
	YMODEM_STATUS_ABORT          = 0x02,
	YMODEM_STATUS_TIMEOUT        = 0x03,
	YMODEM_STATUS_DATA           = 0x04,
	YMODEM_STATUS_LIMIT          = 0x05
} Ymodem_StatusTypeDef;

/* Exported constants ----------------------------------------------------------------------------*/

//                                Packet Format
// |     0    |   1   |    2   |       3       |  4   | ... | n + 4 | n + 5 | n + 6  |
// |---------------------------------------------------------------------------------|
// | not used | start | PktIdx | 0xFF - PktIdx | D[0] | ... | D1[n] | crc0  |  crc1  |
// |                                           | <---Packet Size -->|

#define PACKET_SIZE              (128)
#define PACKET_1K_SIZE           (1024)

#define PACKET_START_INDEX       (1)
#define PACKET_NUMBER_INDEX      (2)
#define PACKET_CNUMBER_INDEX     (3)

#define PACKET_HEADER_SIZE       (3)
#define PACKET_DATA_INDEX        (4)
#define PACKET_TRAILER_SIZE      (2)

#define PACKET_OVERHEAD_SIZE     (PACKET_HEADER_SIZE + PACKET_TRAILER_SIZE - 1)

#define FILE_NAME_LENGTH         (64) /* 64 bytes at most */
#define FILE_SIZE_LENGTH         (16) /* 16 bytes at most */

#define SOH                      (0x01) /* Start, YModem    */
#define STX                      (0x02) /* Start, YModem-1K */
#define EOT                      (0x04) /* End of transmission */
#define ACK                      (0x06) /* ACK */
#define NAK                      (0x15) /* NAK */
#define CAN                      (0x18)
#define CRC16                    (0x43) /* 'C' */
#define NEGATIVE_BYTE            (0xFF)

#define ABORT1                   (0x41) /* 'A' */
#define ABORT2                   (0x61) /* 'a' */

#define NAK_TIMEOUT              (1000)
#define DOWNLOAD_TIMEOUT         (1000)
#define MAX_ERRORS               (10)
#define RETRY_TIMES_MAX          (20)

/* Exported functions ----------------------------------------------------------------------------*/

/**
 * @brief Receive file and save to flash.
 *
 * @param[out] recv_filesize File size actually received.
 * @param[in]  part_offset   Flash partition offset.
 * @param[in]  part_capacity Flash partition capacity in bytes.
 * @return Ymodem_StatusTypeDef
 */
Ymodem_StatusTypeDef Ymodem_Receive(u32* recv_filesize, const u32 part_offset, const u32 part_capacity);

/**
 * @brief Send file.
 *
 * @param[in] file_buffer File buffer.
 * @param[in] name_buffer File name.
 * @param[in] file_size   File size in bytes.
 * @return Ymodem_StatusTypeDef
 */
Ymodem_StatusTypeDef Ymodem_Transmit(u8* file_buffer, const u8* name_buffer, u32 file_size);

#ifdef __cplusplus
}
#endif

#endif  /* !__YMODEM_H__ */
