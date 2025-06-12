/**
 *******************************************************************************
 * @file    bt_shell.c
 * @author  Airtouch Software Team
 * @brief   Firmware Software version
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
#include "atlog.h"
#include "app_bt.h"
#include "app_msg.h"
#include "osal_mem.h"

/***************************************************************************************/
/*        Extern Functions                                                             */
/***************************************************************************************/


/***************************************************************************************/
/*        Local Functions Definition                                                   */
/***************************************************************************************/
#ifdef CFG_BT_SHELL

static s32 at_shell_bt_on(size_t argc, char **argv)
{
	u8 cmd[] = "ble_on\r\n";

	logi("bt on!\n\r");

	if (argc != 1) {
		logi("Params Num Error!\r\n");
		return 0;
	}

	at_bt_string_set(cmd);

	return 0;
}

static s32 at_shell_bt_off(size_t argc, char **argv)
{
	u8 cmd[] = "ble_off\r\n";

	logi("bt off!\n\r");

	if (argc != 1) {
		logi("Params Num Error!\r\n");
		return 0;
	}

	at_bt_string_set(cmd);

	return 0;
}

static s32 at_shell_bt_adv_on(size_t argc, char **argv)
{
	u8 cmd[] = "adv_on\r\n";


	logi("adv on!\n\r");

	if (argc != 1) {
		logi("Params Num Error!\r\n");
		return 0;
	}

	at_bt_string_set(cmd);

	return 0;
}

static s32 at_shell_bt_adv_off(size_t argc, char **argv)
{
	u8 cmd[] = "adv_off\r\n";

	logi("adv off!\n\r");

	if (argc != 1) {
		logi("Params Num Error!\r\n");
		return 0;
	}

	at_bt_string_set(cmd);

	return 0;
}

static s32 at_shell_bt_scan_on(size_t argc, char **argv)
{
	u8 cmd[] = "scan_on\r\n";

	logi("scan on!\n\r");

	if (argc != 1) {
		logi("Params Num Error!\r\n");
		return 0;
	}

	at_bt_string_set(cmd);

	return 0;
}

static s32 at_shell_bt_scan_off(size_t argc, char **argv)
{
	u8 cmd[] = "scan_off\r\n";

	logi("scan off!\n\r");

	if (argc != 1) {
		logi("Params Num Error!\r\n");
		return 0;
	}

	at_bt_string_set(cmd);

	return 0;
}

static s32 at_shell_bt_disconnect(size_t argc, char **argv)
{
	u8 cmd[] = "discon\r\n";

	logi("bt disconnect!\n\r");

	if (argc != 1) {
		logi("Params Num Error!\r\n");
		return 0;
	}

	at_bt_string_set(cmd);

	return 0;
}

static s32 at_shell_bt_string(size_t argc, char **argv)
{
	u8 str[BT_STRING_MAX_LEN] = {0,};
	u8 str_len = 0, data_type = 0;

	logi("at_shell_bt_string!\n\r");

	if (argc != 3) {
		logi("Params Num Error!\r\n");
		return 0;
	}

	data_type = str2num(argv[1]);
	str_len = strlen((char *)argv[2]);

	logi("len=%d\r\n", str_len);

#if 0
	memcpy(str, (u8 *)argv[1], str_len);
	str[str_len] = '\r';
	str[str_len + 1] = '\n';
	at_bt_string_set((u8 *)str);
#else
	{
		app_bt_data_info_t *para_info = (app_bt_data_info_t *)osal_malloc(sizeof(app_bt_data_info_t));
		osal_msg_t msg = {MSG_APP_BT_DATA_REQ, (u32)para_info};

		para_info->data_type = data_type;
		para_info->data_len = str_len;
		memcpy(para_info->data, (u8 *)argv[2], str_len);

		osal_task_send_msg(MOD_WCN, &msg, OSAL_WAIT_FOREVER);
	}
#endif

	return 0;
}


static const struct cli_subcmd atmesh_subcmds[] = {
	{
		.subcmd = "on",
		.help = "bt_on\r",
		.handler = at_shell_bt_on,
	},
	{
		.subcmd = "off",
		.help = "bt_off\r",
		.handler = at_shell_bt_off,
	},
	{
		.subcmd = "adv_on",
		.help = "adv_on\r",
		.handler = at_shell_bt_adv_on,
	},
	{
		.subcmd = "adv_off",
		.help = "adv_off\r",
		.handler = at_shell_bt_adv_off,
	},
	{
		.subcmd = "scan_on",
		.help = "scan_on\r",
		.handler = at_shell_bt_scan_on,
	},
	{
		.subcmd = "scan off",
		.help = "scan_off\r",
		.handler = at_shell_bt_scan_off,
	},
	{
		.subcmd = "discon",
		.help = "disconnect\r",
		.handler = at_shell_bt_disconnect,
	},
	{
		.subcmd = "str",
		.help = "send string\r",
		.handler = at_shell_bt_string,
	},
	{
		NULL,
	}
};

static const CLI_Command_Definition_t atmesh_cmd = {
	"bt",
	&atmesh_subcmds[0],
	"bt <subcmd>\r",
	NULL, /* The function to run. */
	-1 /* The user can enter any number of commands. */
};

void bt_shell_init(void)
{
	FreeRTOS_CLIRegisterCommand(&atmesh_cmd);
}
#endif
