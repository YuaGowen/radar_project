/**
 *******************************************************************************
 * @file    ci_eol.h
 * @author  Airtouch Software Team
 * @brief   CI interface for EOL
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

#ifndef __CI_EOL_H__
#define __CI_EOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_types.h"
#include "osal.h"

#ifdef CFG_EOL_TESTMOD
#define EOL_MSG_ID_LOCALE_FFT_GET    (0x0)
#define EOL_MSG_ID_LOCALE_SET        (0x1)
#define EOL_MSG_ID_RES_SET           (0x2)

#define ANTCAL_TGTCOUNT_MAX          (0x5)
#define EOL_LOCALEFFT_RSP_MAX_BYTES  (0xa0)

#define CI_EOL_STATUS_IDLE                  0
#define CI_EOL_STATUS_ANGLEREF_FFT_REQ      1
#define CI_EOL_STATUS_LOCALE_INFO_REQ       3
#define CI_EOL_STATUS_LOCALE_FFT_REQ        5	//DUMP ALL RB FFT
#define CI_EOL_STATUS_SNR_REQ               7
#define CI_EOL_STATUS_INIT                  0xF0U
#define CI_EOL_STATUS_EXIT                  0xFFU

typedef struct _eol_msg_t {
  s16 msg_id;
  s16 rsvd;
  u8 data[EOL_LOCALEFFT_RSP_MAX_BYTES];
}eol_msg_t;


typedef struct {
	u8 obj_num;
	u8 rsvd[3];
	u16 range[ANTCAL_TGTCOUNT_MAX];
	s16 angle0[ANTCAL_TGTCOUNT_MAX];
	s16 angle1[ANTCAL_TGTCOUNT_MAX];
	s16 angle2[ANTCAL_TGTCOUNT_MAX];
} antcal_locale_struct;

typedef struct {
	u8 trx_num;		//((tx_num<<4)&0xf0)|(rx_num&0x0f)
	u8 obj_num;
	u16 frame_req;
	u32 rx0_data[ANTCAL_TGTCOUNT_MAX];
	u32 rx1_data[ANTCAL_TGTCOUNT_MAX];
	u32 rsvd[1];		//for future...
} antcal_tobecal_data_struct;

/*
	CALRES_mRX_GRP_COUNT:
	Cmn=m!/ [n!* (m-n)!]
	2rx-->C22 == 2!/[2*1*(1)] == 1
	3rx-->C32 == 3!/(2!*(3-2)!) == 3
	...
*/
//#define CALRES_2RX_GRP_COUNT	1
// #define CALRES_3RX_GRP_COUNT	3
typedef struct {
	// u16 count;
	s16 phase_delta;
	s16 k;
}antcal_res_struct;

typedef struct {
	osal_timer_t eolcal_rsp_timer;
	u8 status;
	u8 init_flag;
	u8 param;
	u8 rsvd[1];
} antcal_ci_rsp_t;

#endif
u8 ci_eol_test_mode_entry(const u8* p, u8 len);
u8 ci_eol_test_snr_get(const u8 *p, u8 len);
u8 ci_eol_test_mode_gpio_test(const u8* p, u8 len);
u8 ci_eol_test_mode_locale_fft_get(const u8 *p, u8 len);
u8 ci_eol_test_mode_antphase_calib(const u8* p, u8 len);
u8 ci_eol_test_mode_calres_read(const u8* p, u8 len);
u8 ci_eol_test_mode_calres_write(const u8* p, u8 len);
u8 ci_eol_test_mode_calres_apply_tgtget(const u8 *p, u8 len);
u8 ci_eol_test_mode_io_out_set(const u8* p, u8 len);
u8 ci_eol_test_mode_pwrlvl_set(const u8* p, u8 len);
u8 ci_eol_test_mode_slfchk(const u8* p, u8 len);
u8 ci_eol_test_mode_rftx_onoff(const u8* p, u8 len);
u8 ci_eol_test_mode_exit(const u8* p, u8 len);

u8 ci_eol_exit_confirm(void);
void ci_eol_timeout(void);

#ifdef __cplusplus
}
#endif

#endif /* !__CI_EOL_H__ */
