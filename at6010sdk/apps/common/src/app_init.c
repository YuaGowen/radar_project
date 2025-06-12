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
#include "osal_common.h"
#include "app_init.h"
#include "ir_process.h"
#include "task_schedule.h"
#include "osal_task.h"
#include "if_data_output.h"
#include "dip_mgr.h"
#include "aux_adc_key.h"
#include "sys_data.h"
#ifdef CFG_APP_LIGHT
#include "app_light.h"
#endif
#ifdef CFG_APP_HTM
#include "app_htm.h"
#endif
#ifdef CFG_APP_POD
#include "app_pod.h"
#endif
#ifdef CFG_APP_BRHR
#include "app_bhr.h"
#endif
#ifdef CFG_APP_RGN
#include "app_rgn.h"
#endif
#ifdef CFG_CI
#include "ci_main.h"
#include "ci_cmd.h"
#endif
#ifdef CFG_WDT_SUPPORT
#include "wtd.h"
#endif

#ifdef CFG_SIGMESH
extern osal_status_t app_bt_init(void);
#endif

#ifdef CFG_ATMESH
extern osal_status_t app_atmesh_init(void);
#endif

extern void radar_mgr_worker_run(void);


void app_idle_task_hook(void *pdata)
{
	UNUSED(pdata);

#ifdef CFG_WDT_SUPPORT
	wtd_feed(WDT_TIME_MS);
#endif
}

void app_initialize(void)
{
	osal_task_set_idle_hook(app_idle_task_hook);
	sys_data_init(0U);

#if defined (CFG_APP_LIGHT)
	app_light_init();
#elif defined(CFG_APP_HTM)
	app_htm_init();
#elif defined(CFG_APP_POD)
	app_pod_init();
#elif defined(CFG_APP_BRHR)
	app_bhr_init();
#elif defined(CFG_APP_RGN)
	app_rgn_init();
#endif

#ifdef CFG_IF_PRINT_TM
	if_data_out_init();
#endif

#ifdef CFG_CI
	ci_main_init();
#endif

#ifdef CFG_IR
	app_ir_init();
#endif

#ifdef CFG_DIP_SWITCH
	dip_mgr_init();
	dip_mgr_update();
#endif

#ifdef CFG_AUX_ADC_KEY
	app_aux_adc_key_init();
#endif

#if defined(CFG_CI) && defined(CFG_USER_SETTING)
	ci_setting_load();
#endif

}
