/**
 *******************************************************************************
 * @file    app_bt.c
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
#include "ctype.h"
#include "at_types.h"
#include "at_error.h"
#include "atlog.h"
#include "common.h"
#include "uart.h"
#include "hal_uart.h"
#include "string.h"
#include "app_msg.h"
#include "osal_mem.h"
#include "osal_common.h"
#include "osal_task.h"
#if defined(CFG_HTM)
#include "app_htm.h"
#endif

#ifdef CFG_BT
#define CMD_END_CRLF    ("\r\n")

u8 m_uart1_rx_buf[BT_STRING_MAX_LEN] = {0,};
u8 m_rx_index;

ATTR_RAM_SECTION static void uart1_rx_data_handle(u8 length, u8 *pdata)
{
	if (NULL != strstr((char *)pdata, "con_ind")) {
		logi("ble connect ind\r\n");
	} else if (NULL != strstr((char *)pdata, "disc_ind")) {
		logi("ble disconnect ind\r\n");
	} else {
		logi("[%d]:%s\r\n", length, pdata);

		if (!strncmp((char *)pdata, "HTM_Start", 9)) {
			logi("Start Height Measure!\r\n");
#ifdef CFG_APP_HTM
			app_htm_det_start();
#else
			{
				app_bt_data_info_t *para_info = (app_bt_data_info_t *)osal_malloc(sizeof(app_bt_data_info_t));
				osal_msg_t msg = {MSG_APP_BT_DATA_REQ, (u32)para_info};

				para_info->data_type = BT_DATA_TYPE_HEIGHT;
				para_info->data_len = 4;
				memcpy(para_info->data, "1758", 4);

				osal_task_send_msg(MOD_WCN, &msg, OSAL_WAIT_FOREVER);
			}
#endif
		} else if (!strncmp((char *)pdata, "HBM_Start", 9)) {
			logi("Start Heartbeat Measure!\r\n");

		} else {
			//To Do...
		}
	}
}

ATTR_RAM_SECTION static void uart1_rx_hdl(void)
{
	u8 data, ch;

	while (0U == uart_get_status(UART_CI_PORT, UART_RX_BUF_EMPTY)) {
		ch = uart_recv_byte(UART_CI_PORT);
		m_uart1_rx_buf[m_rx_index++] = ch;
	}

	uart_send_byte(UART_LOG_PORT, ch);

	if (ch == '\r' || ch == '\n') {
		if (NULL != strstr((char *)m_uart1_rx_buf, CMD_END_CRLF)) {

			logi("uart rx: length=%d\r\n", m_rx_index);

			uart1_rx_data_handle(m_rx_index, m_uart1_rx_buf);

			memset((char *)m_uart1_rx_buf, 0, sizeof(m_uart1_rx_buf));
			m_rx_index = 0;
		}
	}

	uart_irq_enable(UART_CI_PORT, UART_RX_IDLE_INTR);
}

static void uart1_init(void)
{
	uart_cfg_t cfg;

	cfg.baud_rate = UART_CI_BAUDRATE;
	cfg.parity = UART_PARITY_DISABLE;
	cfg.stop_bits = UART_STOP_BITS_1;
	cfg.data_width = UART_DATA_WIDTH_8BIT;
	cfg.irq_enable = 1;
	cfg.fifo_cfg.fifo_en = 1;
	cfg.fifo_cfg.rx_fifo_level = UART_RX_FIFO_FULL_2;
	cfg.fifo_cfg.tx_fifo_level = UART_TX_FIFO_CHAR_2;
	(void)hal_uart_init(UART_CI_PORT, &cfg);

	uart_set_hdl(UART_CI_PORT, UART_RX_IDLE_INTR, uart1_rx_hdl);
	uart_irq_enable(UART_CI_PORT, UART_RX_INTR);
	uart_irq_enable(UART_CI_PORT, UART_RX_IDLE_INTR);
}

void at_bt_string_set(u8 *pstr)
{
	uart_send_str(UART_CI_PORT, pstr);
}

void app_bt_init(void)
{
	uart1_init();
}

void app_bt_message_handler(u16 msg_id, u8 data_len, u8 *msg_data)
{
	switch (msg_id) {
	case MSG_APP_BT_ON_REQ: {
		u8 cmd[] = "Ble_On\r\n";

		at_bt_string_set(cmd);
	}
	break;

	case MSG_APP_BT_OFF_REQ: {
		u8 cmd[] = "Ble_Off\r\n";

		at_bt_string_set(cmd);
	}
	break;

	case MSG_APP_BT_ADV_ON_REQ: {
		u8 cmd[] = "Adv_On\r\n";

		at_bt_string_set(cmd);
	}
	break;

	case MSG_APP_BT_ADV_OFF_REQ: {
		u8 cmd[] = "Adv_Off\r\n";

		at_bt_string_set(cmd);
	}

	case MSG_APP_BT_DISCONNECT_REQ: {
		u8 cmd[] = "Discon\r\n";

		at_bt_string_set(cmd);
	}
	break;

	case MSG_APP_BT_DATA_REQ: {
		u8 str[BT_STRING_MAX_LEN + 2] = {0,}; /* 2 more Bytes to save \r\n */
		app_bt_data_info_t *data_info = (app_bt_data_info_t *)msg_data;
		u8 index = 0, sLen = 0, i = 0;

		logi("data type=%d, len=%d\r\n", data_info->data_type, data_info->data_len);

		if (data_info->data_type == BT_DATA_TYPE_HEIGHT) {
			const u8 tmpStr[] = "Height,";

			sLen = strlen((char *)tmpStr);
			memcpy(str, tmpStr, sLen);
			index = sLen;

#if 0 //cm

			if (data_info->data_len == 1) {
				str[index++] = '0';
			} else {
				for (u8 i = 0; i < data_info->data_len - 1; i++)
					str[index++] = data_info->data[i];
			}

			str[index++] = '.';
			str[index++] = data_info->data[data_info->data_len - 1];
#else //m

			if (data_info->data_len < 4) {
				str[index++] = '0';
				str[index++] = '.';

				if (data_info->data_len < 3)
					str[index++] = '0';

				if (data_info->data_len < 2)
					str[index++] = '0';

				for (i = 0; i < data_info->data_len; i++)
					str[index++] = data_info->data[i];

			} else {
				u8 j;

				for (i = 0; i < data_info->data_len - 3 ; i++)
					str[index++] = data_info->data[i];

				str[index++] = '.';

				for (i = 0; i < 3 ; i++)
					str[index++] = data_info->data[data_info->data_len - 3 + i];
			}

#endif
			str[index++] = '\r';
			str[index++] = '\n';
		} else if (data_info->data_type == BT_DATA_TYPE_HEART) {
			const u8 tmpStr[] = "Heart,";

			sLen = strlen((char *)tmpStr);
			memcpy(str, tmpStr, sLen);
			index = sLen;

			memcpy(&str[index], (u8 *)data_info->data, data_info->data_len);
			index += data_info->data_len;
			str[index++] = '\r';
			str[index++] = '\n';

		} else if (data_info->data_type == BT_DATA_TYPE_UART) {
			const u8 tmpStr[] = "Uart,";

			sLen = strlen((char *)tmpStr);
			memcpy(str, tmpStr, sLen);
			index = sLen;

			if ((data_info->data_len + index) > BT_STRING_MAX_LEN) {
				loge("uart data longer than Max and trim it!!!\r\n");
				sLen = BT_STRING_MAX_LEN - index;
			} else
				sLen = data_info->data_len;

			memcpy(&str[index], (u8 *)data_info->data, sLen);
			index += sLen;
			str[index++] = '\r';
			str[index++] = '\n';
		} else {
			loge("Error Data type!!!");
			osal_free(msg_data);
			break;
		}

		at_bt_string_set((u8 *)str);
		osal_free(msg_data);
	}
	break;

	default:
		logi("Error bt command\r\n");
		break;
	}
}

#endif
