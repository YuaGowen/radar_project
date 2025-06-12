/**
 *******************************************************************************
 * @file    version.h
 * @author  Airtouch Software Team
 * @brief   Firmware Software version
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

#ifndef __VERSION_H__
#define __VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"

/* Constants -----------------------------------------------------------------*/

/* SDK version name */

#define FIRMWARE_NAME           "at6010_lamps_os"
#define HARDWARE_NAME           "ECO_0"

/* SDK software version */

#define SW_VER_MAJOR            0
#define SW_VER_MINOR            4
#define SW_VER_REVISON          2
#define SW_VER_STR              XSTR(SW_VER_MAJOR)"."XSTR(SW_VER_MINOR)"."XSTR(SW_VER_REVISON)

#ifdef CFG_CI
typedef struct _ci_ver_struct {
	u8 sw_ver_major;
	u8 sw_ver_minor;
	u8 sw_ver_revision;
	u8 cust_ver_major;
	u8 cust_ver_minor;
	u8 hw_ver_major;
	u8 hw_ver_minor;
	u8 reserved;
} ci_ver_struct;
#endif

/* Function Declarations -----------------------------------------------------*/

void show_version(void);
char *get_version(char *str);

#ifdef __cplusplus
}
#endif

#endif /* !__VERSION_H__ */
