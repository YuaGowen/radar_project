/**
 *******************************************************************************
 * @file    app_eol.c
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
#include "app_eol.h"
#include "at_error.h"
#include "atlog.h"
#include "ci_main.h"
#include "eol_radar_cfg.h"
#include "output_config.h"
#include "sys_data.h"
#include "mdet_eolcal.h"

#if defined(CFG_APP_HTM_ULP)
#include "radar_rf_cfg.h"
#endif

//#define DBG_EOL_OUTPUT_RANGEFFT_DATA

#if defined(CFG_EOL_TESTMOD)
static bool mdet_eol_md_flag(void)
{
	return radar_eol_entry_flag();
}

void eol_app_init(void)
{
#ifdef DBG_EOL_OUTPUT_RANGEFFT_DATA
	output_dma_config();
#endif
	eol_radar_rf_cfg();
	eol_radar_dev_init();
	sys_data_init(0U);
}

void eol_app_process(void)
{
	u32 d_update = 0;
	u16 dlen = 0;
	bool eol_proc_flag = mdet_eol_md_flag();

	//logi("eol_proc_flag=%d\n",eol_proc_flag);
	if(eol_proc_flag) {
		d_update = eol_radar_rangefft_get(&dlen);
		if(d_update != 0) {
			//logi("len=%u\n",dlen);
			radar_fmcw_ca_dispatch((const int16_t *)d_update);
#ifdef DBG_EOL_OUTPUT_RANGEFFT_DATA
			output_port_send_data(d_update,(u32)dlen);
#endif
		}
	}
}

void eol_app_deinit(void)
{
	// FIXME: 对于其他APP来说，EOL 退出是否干净？
#ifdef CFG_APP_HTM_ULP
	radar_rf_eol_to_mdet_cfg();
#endif
}

#endif /* !CFG_EOL_TESTMOD */
