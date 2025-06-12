/**
 *******************************************************************************
 * @file    ci_ulp.h
 * @author  Airtouch Software Team
 * @brief   ULP commands
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

#ifndef __CI_ULP_H__
#define __CI_ULP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Data Types ------------------------------------------------------------------------------------*/

typedef enum {
	REFRESH_MODE_RELOAD = 1,
	REFRESH_MODE_FORCE  = 2,
} refresh_mode_t;

/* Function Declarations -------------------------------------------------------------------------*/

void ulp_prog1_refresh_active_timer(u8 to, u8 mode);


/**
 * @brief Update ULP_PROG_1 active time.
 * Positive value means active time, zero means jump to ULP_PROG_2 immediately.
 *
 * @param p
 * @param len
 * @return u8
 */
u8 ci_ulp_set_active_time(const u8* p, u8 len);

/**
 * @brief ULP_PROG_1 支持 CI 命令，倒计时进入 ULP_PROG_2 程序
 */
void ulp_start_active_timer(void);

/**
 * @brief Read AT5815C register via I2C.
 * usage: reg_read reg_addr
 *
 * @param p
 * @param len
 * @return u8
 */
u8 ci_ulp_exit_reg_read(const u8* p, u8 len);

/**
 *@brief Write AT5815C register via I2C.
 * usage: reg_write reg_addr reg_value
 *
 * @param p
 * @param len
 * @return u8
 */
u8 ci_ulp_exit_reg_write(const u8* p, u8 len);


#ifdef __cplusplus
}
#endif

#endif /* !__CI_ULP_H__ */
