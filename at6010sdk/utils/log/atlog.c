/**
 * Copyright (c) 2024, Airtouching Intelligence Technology.
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

#include <string.h>
#include "atlog.h"
#include "uart.h"

#ifndef UART_LOG_PORT
#define UART_LOG_PORT                 UART_ID_0
#endif

#ifdef CFG_LOG

static log_prt_lev_t m_log_print_level = LOG_PRT_LEV_INFO;

void set_log_level(log_prt_lev_t p_level)
{
	m_log_print_level = p_level;
}

log_prt_lev_t get_log_level(void)
{
	return m_log_print_level;
}

void log_printf(log_prt_lev_t p_level, const char *fmt, ...)
{
	char buf[259] = {'[', 'd', ']'}; /* max length: 256 + 3(tag_len) */
	char tag_ch[] = LOG_TAG_CH;

	if (p_level > LOG_PRT_LEV_VERBOSE) {
		return;
	}

	buf[1] = tag_ch[p_level];

	if (m_log_print_level >= p_level) {
		va_list args;
		int len;

		va_start(args, fmt);
		len = vsnprintf(buf + 3, sizeof(buf) - 3, fmt, args);
		va_end(args);
		if ((len > 0) && (len < LOG_MAX_SIZE)) {
			uart_send_str(UART_LOG_PORT, (unsigned char *)buf);
		}
	}
}

#ifdef AIRWATCHER_RAW_SUPPORT
void logr(u32 val, bool tag_flag)
{
	u16 raw_val = (u16)val;
	u8 snd_rawdata[5], snd_size = 0;
	u32 i = 0;

	//if (m_log_print_level < LOG_PRT_LEV_RAW)
	//return;
	memset(snd_rawdata, 0, sizeof(snd_rawdata));
	if (tag_flag) {
		snd_rawdata[i++] = '[';
		snd_rawdata[i++] = 'r';
		snd_rawdata[i++] = ']';
		memcpy(&snd_rawdata[i], (void *)&raw_val, sizeof(u16));
		snd_size = (u8)sizeof(snd_rawdata);
	} else {
		memcpy(snd_rawdata, (void *)&raw_val, sizeof(u16));
		snd_size = (u8)sizeof(u16);
	}
	uart_send_bs(UART_CI_PORT, (u8 *)snd_rawdata, snd_size);
}
#endif


/**
 * printf - Ouput a string to console according to format specificated.
 * \param    fmt: The format string to use
 * \param    ...: Arguments for the format string
 *
 * The function returns the number of characters output to console.
 *
 */
int printf(const char *fmt, ...)
{
	va_list args;
	int len;
	char buf[256];

	va_start(args, fmt);
	len = vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	if (len) {
		uart_send_str(UART_LOG_PORT, (unsigned char *)buf);
	}

	return len;
}

void at_printf(const char *fmt, ...)
{
	va_list args;
	int len;
	char buf[256];

	va_start(args, fmt);
	len = vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	if (len) {
		uart_send_str(UART_LOG_PORT, (unsigned char *)buf);
	}
}

#else /* Empty Implementation */

void set_log_level(log_prt_lev_t p_level)
{

}

log_prt_lev_t get_log_level(void)
{
	return LOG_PRT_LEV_INFO;
}

void log_printf(log_prt_lev_t p_level, const char *fmt, ...)
{

}

#ifdef AIRWATCHER_RAW_SUPPORT
void logr(u32 val, bool tag_flag)
{

}
#endif

int printf(const char *fmt, ...)
{
	return 0;
}

void at_printf(const char *fmt, ...)
{

}

#endif /* !CFG_LOG */
