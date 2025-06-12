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

#include "FreeRTOS_CLI.h"
#include "light_ctrl.h"
#include "shell.h"

/***************************************************************************************/
/*        Extern Functions                                                             */
/***************************************************************************************/


/***************************************************************************************/
/*        Local Functions Definition                                                   */
/***************************************************************************************/

static s32 light_shell_on(size_t argc, char **argv)
{
	u32 duration;
	if (argc > 1U) {
		duration = str2num(argv[1]);
		logi("light on %u ms\n\r", duration);
		light_ctrl_switch(1U, duration);
	}
	return 0;
}

static s32 light_shell_off(size_t argc, char **argv)
{
	logi("light off\n\r");
	light_ctrl_switch(0U, 0U);
	return 0;
}

static const struct cli_subcmd light_subcmds[] = {
	{
		.subcmd = "on",
		.help = "light on <ms>\r",
		.handler = light_shell_on,
	},
	{
		.subcmd = "off",
		.help = "light off\r",
		.handler = light_shell_off,
	},
	{
		NULL,
	}
};

static const CLI_Command_Definition_t light_cmd = {
	"light",
	&light_subcmds[0],
	"light <subcmd> <param>\r",
	NULL, /* The function to run. */
	-1 /* The user can enter any number of commands. */
};

void light_shell_init(void)
{
	(void)FreeRTOS_CLIRegisterCommand(&light_cmd);
}

