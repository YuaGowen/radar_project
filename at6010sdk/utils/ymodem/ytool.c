/**
 *******************************************************************************
 * @file    ytool.c
 * @author  Airtouch Software Team
 * @brief   Utils for Ymodem
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

#include "ytool.h"
#if defined(CFG_OS)
#include "FreeRTOS.h"
#include "task.h"
#else
#include "hal_dualtimer.h"
#endif

#if defined(CFG_OTA)

static char my_toupper(char ch)
{
	if ((ch >= 'a') && (ch <= 'z')) {
		ch -= 'a' - 'A';
	}

	return ch;
}

void ytool_int2str(u32 intnum, u8* intstr)
{
	int i = 0;
	do {
		intstr[i++] = intnum % 10 + '0';
		intnum /= 10;
	} while (intnum != 0);

	int start = 0;
	int end = i - 1;
	while (start < end) {
		char temp = intstr[start];
		intstr[start] = intstr[end];
		intstr[end] = temp;
		start++;
		end--;
	}

	intstr[i] = '\0';
}

u32 ytool_str2int(u8* intStr, u32* intNum)
{
	u32 i = 0, res = 0;
	u32 val = 0;

	if ((intStr[0] == '0') && ((intStr[1] == 'x') || (intStr[1] == 'X'))) { // hexadecimal: 0xABCD
		i = 2;
		while ((i < 11) && (intStr[i] != '\0')) {
			if (ISVALIDHEX(my_toupper(intStr[i]))) {
				val = (val << 4) + CONVERTHEX(my_toupper(intStr[i]));
			} else {
				res = 0; // wrong
				break;
			}
			i++;
		}

		if (intStr[i] == '\0') {
			*intNum = val;
			res = 1; // correct
		}
	} else { // decimal: 1234
		while ((i < 11) && (res != 1)) {
			if (intStr[i] == '\0') {
				*intNum = val;
				res = 1; // correct
			} else if ((intStr[i] == 'k' || intStr[i] == 'K') && (i > 0)) {
				val = val << 10;
				*intNum = val;
				res = 1; // correct
			} else if ((intStr[i] == 'm' || intStr[i] == 'M') && (i > 0)) {
				val = val << 20;
				*intNum = val;
				res = 1; // correct
			} else if (ISVALIDDEC(intStr[i])) {
				if (val <= (UINT32_MAX - CONVERTDEC(intStr[i])) / 10) {
					val = val * 10 + CONVERTDEC(intStr[i]);
				} else {
					res = 0; // wrong, overflow
					break;
				}
			} else {
				res = 0; // wrong
				break;
			}
			i++;
		}
	}

	return res;
}

void ytool_delay(u32 delay)
{
#if defined(CFG_OS)
	u32 end_tick = xTaskGetTickCount() + delay;
	while (xTaskGetTickCount() <= end_tick);
#else
	hal_dualtimer_delay_ms(delay);
#endif
}

#endif /* !CFG_OTA */
