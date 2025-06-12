/**
 *******************************************************************************
 * @file    version.c
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

/* Includes ------------------------------------------------------------------*/

#include "version.h"
#include "atlog.h"

/* Private Constants ---------------------------------------------------------*/

#define PRT_BUILD_DATE

/* Functions Implementations -------------------------------------------------*/

void show_version(void)
{
	logi("------------------------------------------------------\r\n");
	logi("Base Version: %d.%d.%d \r\n", (s32)SW_VER_MAJOR, (s32)SW_VER_MINOR, (s32)SW_VER_REVISON);
#ifdef PRT_BUILD_DATE
	logi("Build   Date: %s %s \r\n", __DATE__, __TIME__);
#endif
	logi("------------------------------------------------------\r\n");
}

char *get_version(char *str)
{
	return SW_VER_STR;
}
