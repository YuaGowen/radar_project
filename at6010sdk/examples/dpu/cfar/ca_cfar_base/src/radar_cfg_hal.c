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
#include "hal_radar.h"
#include "cfg_radar_profile.h"

void radar_fmcw_cfg(void)
{
	radar_profile_t profile = {
			.rfe = {
				.tx_gain = RADAR_PRF_TX_GAIN,
				.rx_cfg = {
					.rx_num = RADAR_PRF_RFE_RX_NUM,
					.hpf1 = RADAR_PRF_RFE_HPF1,
					.hpf2 = RADAR_PRF_RFE_HPF2,
					.lpf = RADAR_PRF_RFE_LPF
				}
			},
			.chirp = {
				RADAR_PRF_CHIRP_0,
				RADAR_PRF_CHIRP_1,
				RADAR_PRF_CHIRP_2,
				RADAR_PRF_CHIRP_3,
				RADAR_PRF_CHIRP_4,
				RADAR_PRF_CHIRP_5,
				RADAR_PRF_CHIRP_6,
				RADAR_PRF_CHIRP_7,
			},
			.subfrm = {
				RADAR_PRF_SUBFRM_0,
				RADAR_PRF_SUBFRM_1,
				RADAR_PRF_SUBFRM_2,
				RADAR_PRF_SUBFRM_3,
				RADAR_PRF_SUBFRM_4,
				RADAR_PRF_SUBFRM_5,
				RADAR_PRF_SUBFRM_6,
				RADAR_PRF_SUBFRM_7,
			},
			.frame = RADAR_PRF_FRAME,
			.filter = RADAR_PRF_FILTER
	};

	if (E_OK != hal_radar_init(&profile)) {
		printf("ERROR: radar init fail!\r\n");
	}
}
