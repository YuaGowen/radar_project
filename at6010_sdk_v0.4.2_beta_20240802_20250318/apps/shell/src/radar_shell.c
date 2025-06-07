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

#include <FreeRTOS_CLI.h>
#include "at_types.h"
#include "radar_shell_impl.h"

#ifdef CFG_RADAR_SHELL

static const struct cli_subcmd radar_subcommands[] = {
	{
		.subcmd = "set_pwr",
		.help = "set radar power on/off",
		.handler = radar_shell_power_set,
	},
	{
		.subcmd = "fmcw_init",
		.help = "init fmcw radar",
		.handler = radar_shell_rf_fmcw_init,
	},
	{
		.subcmd = "set_fmcw_rf_freq",
		.help = "set fmcw start radar",
		.handler = radar_shell_rf_fmcw_freq_set,
	},
	{
		.subcmd = "set_fmcw_tx_gain",
		.help = "set fmcw tx gain",
		.handler = radar_shell_rf_fmcw_tx_gain_set,
	},
	{
		.subcmd = "det_start",
		.help = "start radar detection",
		.handler = radar_shell_detect_start,
	},
	{
		.subcmd = "det_stop",
		.help = "stop radar detection",
		.handler = radar_shell_detect_stop,
	},
	{
		.subcmd = "set_md_lev",
		.help = "set user motion det cfg lev",
		.handler = radar_shell_user_md_set,
	},
	{
		.subcmd = "get_md_lev",
		.help = "get user motion det cfg lev",
		.handler = radar_shell_user_md_get,
	},
	{
		.subcmd = "set_md_sen",
		.help = "set user motion sensitivity lev",
		.handler = radar_shell_mdet_sen_set,
	},
	{
		.subcmd = "get_md_sen",
		.help = "get user motion sensitivity lev",
		.handler = radar_shell_mdet_sen_get,
	},
	{
		.subcmd = "range_log_en",
		.help = "enable dist log out",
		.handler = radar_shell_md_log_en,
	},
	{
		.subcmd = "range_log_disable",
		.help = "disable dist log out",
		.handler = radar_shell_md_log_disable,
	},
#ifdef CFG_PRESENCE_DET
	{
		.subcmd = "get_bhr_lev",
		.help = "get user bhr cfg lev",
		.handler = radar_shell_user_bhr_get,
	},
	{
		.subcmd = "set_bhr_lev",
		.help = "set user bhr cfg lev",
		.handler = radar_shell_user_bhr_set,
	},
	{
		.subcmd = "get_bhr_sen",
		.help = "get bhr sensitivity lev",
		.handler = radar_shell_bhr_sen_get,
	},
	{
		.subcmd = "set_bhr_sen",
		.help = "set bhr sensitivity lev",
		.handler = radar_shell_bhr_sen_set,
	},
	{
		.subcmd = "stop_bhr_det",
		.help = "stop breathe det",
		.handler = radar_shell_bhr_det_stop,
	},
	{
		.subcmd = "start_bhr_det",
		.help = "start breathe det",
		.handler = radar_shell_bhr_det_start,
	},
#endif
#ifdef CFG_APP_POD
	{
		.subcmd = "pod_param_set",
		.help = "set pod param",
		.handler = radar_shell_pod_param_set,
	},
	{
		.subcmd = "log_det_ret",
		.help = "detail det result",
		.handler = radar_shell_pod_det_log_en,
	},
#endif
	{
		NULL,
	}
};

static const CLI_Command_Definition_t radar_command = {
	"radar",
	&radar_subcommands[0],
	"radar <subcmd>",
	NULL, /* The function to run. */
	-1 /* The user can enter any number of commands. */
};

void radar_shell_init(void)
{
	FreeRTOS_CLIRegisterCommand(&radar_command);
}
#endif /* !CFG_RADAR_SHELL */

