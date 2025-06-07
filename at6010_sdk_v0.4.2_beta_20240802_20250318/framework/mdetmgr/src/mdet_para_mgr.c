/**
 * Copyright (c) 2023, Airtouching Intelligence Technology.
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
#include "mdet_para_mgr.h"
#include "cfg_calib.h"
#include "at_types.h"

typedef struct {
	float para_board_nf_base_prof[100];
	s32   para_board_nf_ext_prof[100];
	float para_board_nf_high_prof[70];
} mdet_fmcw_thold_cfg_t;


typedef struct {
	mdet_fmcw_thold_cfg_t mdet_fmcw_para;
}mdet_para_cfg_t;

static const mdet_para_cfg_t mdet_para = {
	.mdet_fmcw_para = CFG_CALIB_MDET_FMCW_PARAM,
};

const float* mdet_fmcw_board_nf_base_param(void)
{
	return &mdet_para.mdet_fmcw_para.para_board_nf_base_prof[0];
}

const s32* mdet_fmcw_board_nf_ext_param(void)
{
	return &mdet_para.mdet_fmcw_para.para_board_nf_ext_prof[0];
}

const float* mdet_fmcw_board_nf_high_param(void)
{
	return &mdet_para.mdet_fmcw_para.para_board_nf_high_prof[1];
}

