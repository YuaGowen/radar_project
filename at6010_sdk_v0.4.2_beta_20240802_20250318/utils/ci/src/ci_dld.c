/**
 *******************************************************************************
 * @file    ci_dld.c
 * @author  Airtouch Software Team
 * @brief   CI commands for downloading firmware.
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
#include "ci_port.h"
#include "ci_cmd.h"
#include "ci_dld.h"
#include "spi_flash_drv.h"
#include "atlog.h"

#if defined(CFG_OTA)
#include "ymodem.h"
#endif

#if defined(CFG_OS)
#include "FreeRTOS.h"
#include "task.h"
#endif

#ifdef CFG_DET_INFO_OUT
#include "handle_mdet_info.h"
#endif

#if defined(CFG_OTA)

/* Constants -------------------------------------------------------------------------------------*/

/* Macros ----------------------------------------------------------------------------------------*/

// Convert a U8 byte stream to U32 data in little-endian format.
#define U8_BYTE_STREM_TO_U32(p)  (u32)(((p)[3] << 24) | ((p)[2] << 16) | ((p)[1] << 8 ) | (p)[0])

/* Global Variables ------------------------------------------------------------------------------*/

extern ci_port_ctx_t m_ci_ctxt;

/* Private Variables -----------------------------------------------------------------------------*/
#if defined(CFG_OS)
TaskHandle_t m_dld_task_hdl = NULL;
#endif

/* Private Functions -----------------------------------------------------------------------------*/

#if defined(CFG_OS)

/**
 * @brief dld task
 *
 * @param arg
 */
static void ymodem_recv_entry(void *arg)
{
	dld_ctxt_t *ctxt = (dld_ctxt_t *)arg;
	u32 recv_size = 0;

	for (;;) {
		if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) { // wait for notify
			ctxt->m_ymodem_ret = Ymodem_Receive(&recv_size, FLASH_DLD_PART_OFFSET, FLASH_DLD_PART_SIZE);
			ctxt->m_ymodem_run = 0;
		}
	}
}

#endif /* !CFG_OS */


/* Function implementations ----------------------------------------------------------------------*/

/**
 * @brief no arguments.
 *
 * @param p
 * @param len
 * @return u8
 */
u8 ci_dld_start(const u8 *p, u8 len)
{
	u8 ret = CI_ERR_NO;

	cmd_resp_para(CI_DLD_START, (u8 *)&ret, sizeof(ret));

#ifdef CFG_DET_INFO_OUT
	handle_mdet_info_stop();
#endif

	fifo_init(&m_ci_ctxt.m_dld_ctxt.m_dld_fifo, (u8 *)m_ci_ctxt.m_dld_ctxt.m_dld_buf, CI_DLD_BUF_SIZE * sizeof(u8), FIFO_WIDTH_8BITS);

	if (m_ci_ctxt.m_dld_ctxt.m_ymodem_run == 0) {
		m_ci_ctxt.m_dld_ctxt.m_ymodem_run = 1;
#if defined(CFG_OS)
		if (NULL == m_dld_task_hdl) {
			if (pdPASS == xTaskCreate(ymodem_recv_entry, "dld_task", 1024, &m_ci_ctxt.m_dld_ctxt, 3, &m_dld_task_hdl)) {
			} else {
				logw("Error: failed to create dld task !!! \r\n");
			}
		}

		if (NULL != m_dld_task_hdl) { /* Notify the dld task */
			xTaskNotifyGive(m_dld_task_hdl);
		}
#else
		/* Bare-metal, just start here */
		u32 recv_size = 0;
		m_ci_ctxt.m_dld_ctxt.m_ymodem_ret = Ymodem_Receive(&recv_size, FLASH_DLD_PART_OFFSET, FLASH_DLD_PART_SIZE);
		m_ci_ctxt.m_dld_ctxt.m_ymodem_run = 0;
#endif
	}

	return ret;
}

#endif /* !CFG_OTA */
