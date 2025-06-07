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

#include "mod_test.h"
#include "atlog.h"
#include "k_msg.h"
#include "ci_main.h"
#include "hal_common.h"
#include "if_sample.h"
#include "light_ctrl.h"
#include "ir_process.h"
#include "radar_mgr.h"
#include "version.h"
#include "wtd.h"
#include "osal_mem.h"
#include "task_schedule.h"
#include "app_init.h"


/* Function Declarations -----------------------------------------------------*/

extern void app_task_process_msg(msg_t *msg);
extern void radar_task_process_msg(msg_t *msg);


/* Main function -------------------------------------------------------------*/
int main(void)
{
	k_msg_init();

	osal_memory_init();

	hal_common_init();

	show_version();

	set_log_level(LOG_DEFAULT_LEVEL);

	logi("\r\n AT6010 SOC Start...\r\n");

#ifdef CFG_MOD_TEST
	mod_test_main_proc();
#endif

	k_msg_reg_hdl((u8)MOD_APP, app_task_process_msg);
	k_msg_reg_hdl((u8)MOD_RAD, radar_task_process_msg);

	radar_mgr_init();
	app_initialize();
	app_task_schedule_init();

	logi("\r\n AT6010 SOC Entry Loop...\r\n");

	while (1) {

#ifdef CFG_WDT_SUPPORT
		wtd_feed(WDT_TIME_MS);
#endif
		k_msg_loop();

		task_schedule_proc();
	}
}
