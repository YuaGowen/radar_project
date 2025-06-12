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

#ifndef __ATLOG_H__
#define __ATLOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_common.h"
#include "stdarg.h"

typedef enum {
	LOG_PRT_LEV_ASSERT = 0,
	LOG_PRT_LEV_ERR = 1,
	LOG_PRT_LEV_WARN = 2,
	LOG_PRT_LEV_RAW = 3,
	LOG_PRT_LEV_INFO = 4,
	LOG_PRT_LEV_VERBOSE = 5
} log_prt_lev_t;

#define LOG_TAG_CH			{'a', 'e', 'w', 'r', 'i', 'v'}
#define LOG_MAX_SIZE		256

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define log_printf(_level, ...)     \
    printf("[%d]%s(%d): ", _level, __FILENAME__, __LINE__), \
    printf(__VA_ARGS__)
#else
int printf(const char *fmt, ...);
int vsnprintf(char *buf, u32 size, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);
void log_printf(log_prt_lev_t p_level, const char *fmt, ...);
#ifdef AIRWATCHER_RAW_SUPPORT
void logr(u32 val, bool tag_flag);
#endif
void at_printf(const char *fmt, ...);

void set_log_level(log_prt_lev_t p_level);
log_prt_lev_t get_log_level(void);
#endif

#define logv(...) log_printf(LOG_PRT_LEV_VERBOSE, __VA_ARGS__)
#define logi(...) log_printf(LOG_PRT_LEV_INFO, __VA_ARGS__)
#define logw(...) log_printf(LOG_PRT_LEV_WARN, __VA_ARGS__)
#define loge(...) log_printf(LOG_PRT_LEV_ERR, __VA_ARGS__)
#define loga(...) log_printf(LOG_PRT_LEV_ASSERT, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* !__ATLOG_H__ */
