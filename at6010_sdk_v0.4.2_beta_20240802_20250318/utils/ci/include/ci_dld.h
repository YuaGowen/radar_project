/**
 *******************************************************************************
 * @file    ci_dld.h
 * @author  Airtouch Software Team
 * @brief   CI commands for downloading firmware.
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

#ifndef __CI_DLD_H__
#define __CI_DLD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/

#include "at_common.h"
#include "fifo.h"

/* Constants -------------------------------------------------------------------------------------*/

#define FLASH_DLD_PART_OFFSET           0x1F000       /* Download Partition Offeset in Flash   */
#define FLASH_DLD_PART_SIZE             (1024 * 116)  /* 116KB  */


#define CI_DLD_BUF_SIZE                 1024

/* Data Types ------------------------------------------------------------------------------------*/

typedef struct
{
	u32        m_ymodem_run; /**< Ymodem run flag                       */
	u32        m_ymodem_ret; /**< Ymodem return value                   */

	u8         m_dld_buf[CI_DLD_BUF_SIZE]; /**< UART FIFO for YModem            */
	fifo_t     m_dld_fifo;
} dld_ctxt_t;

/* Function Declarations -------------------------------------------------------------------------*/

u8 ci_dld_start(const u8 *p, u8 len);


#ifdef __cplusplus
}
#endif

#endif /* !__CI_DLD_H__ */
