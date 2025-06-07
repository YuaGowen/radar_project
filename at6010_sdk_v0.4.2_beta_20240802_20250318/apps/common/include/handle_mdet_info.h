/**
 *******************************************************************************
 * @file    handle_mdet_info.h
 * @author  Airtouch Software Team
 * @brief   User process the motion detection information on their own.
 *******************************************************************************
 * @copyright Copyright (c) 2023, Airtouching Intelligence Technology.
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

#ifndef __HANDLE_MDET_INFO_H__
#define __HANDLE_MDET_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mdet_fmcw.h"

/* Constans --------------------------------------------------------------------------------------*/

#define _CI_HEAD_RESP                        0x59
#define _CI_CMD_MDET_INFO                    0x30
#define _CI_CMD_MDET_PC_DAT                  0x3d

#define AUTO_REPORT_HEAD                     0x5A

#define AUTO_REPORT_INTERVAL_MS              (0U)

/* Data Types ------------------------------------------------------------------------------------*/

typedef struct {
	u8  head;
	u32 cmd: 5;
	u32 cmd_grp: 3;
	u8  para_len;
	u8 *para;
	u16 check_code;
} _ci_ctrl_frm_t;


typedef struct {
	s32 x;
	s32 y;
} coords_point_t;

/* Global Function -------------------------------------------------------------------------------*/

void auto_report_resp_para(u8 type, u8 *para, u8 len);

void handle_mdet_info_stop(void);

void handle_mdet_info_all(void *info);
void handle_mdet_info_htm(void *info);
void handle_mdet_info_pod(void *info);
void handle_mdet_info_mot(fmcw_det_info_t *info);
void handle_mdet_info_rgn(rgn_det_info_t *info);

void handle_mdet_pc_data(void *pc, u8 size);
void handle_coords_cal(s32 angle, s32 length, coords_point_t *pos);

void handle_mdet_info_init(void);

void det_info_upload_cust(void *md_hdl);

#ifdef __cplusplus
}
#endif

#endif /* !__HANDLE_MDET_INFO_H__ */
