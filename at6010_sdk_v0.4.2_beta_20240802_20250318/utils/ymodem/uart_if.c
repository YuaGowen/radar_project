/**
 *******************************************************************************
 * @file    uart_if.c
 * @author  Airtouch Software Team
 * @brief   UART interface
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

#include "uart_if.h"
#include "at6010.h"
#include "fifo.h"
#include "ci_port.h"

#if defined(CFG_OTA)

extern ci_port_ctx_t m_ci_ctxt;

port_uart_type_t port_uart = {
	.id = UART_CI_PORT
};

status_code_t uart_if_send_timeout(port_uart_type_t* port, u8* buffer, u16 length)
{
	if (port == NULL || buffer == NULL || length == 0) {
		return STATUS_ERROR;
	}

	uart_send_bs(port->id, buffer, length);

	return STATUS_OK;
}

ATTR_RAM_SECTION status_code_t uart_if_recv_timeout(port_uart_type_t* port, u8* buffer, u16 length, u32 timeout)
{
	u16 recv_len = 0;
	u32  recv_ch = 0;
	u32 delay_cnt = 0;

#if defined(CFG_OS)
	timeout /= 2;
#endif

	if (NULL == port) {
		return STATUS_ERROR;
	}

	while (recv_len < length) {
		if (0 == fifo_dequeue(&m_ci_ctxt.m_dld_ctxt.m_dld_fifo, &recv_ch)) {
			*(buffer + recv_len) = (u8)recv_ch;
			recv_len++;
		} else {
			if (delay_cnt >= timeout) {
				return STATUS_TIMEOUT;
			}
			ytool_delay(1);
			delay_cnt++;
		}
	}

	return STATUS_OK;
}


void uart_if_flush_recvd(port_uart_type_t* port)
{
	u32 ch;

	if (NULL == port) {
		return;
	}

	while(0 == fifo_dequeue(&m_ci_ctxt.m_dld_ctxt.m_dld_fifo, &ch));
}

/**
  * @brief  Transmit a byte to the HyperTerminal
  * @param  data The byte to be sent
  * @retval status_code_t STATUS_OK if OK
  */
status_code_t uart_if_send_byte(u8 data)
{
	uart_send_byte((uart_id_t)port_uart.id, data);
	return STATUS_OK;
}

#endif /* !CFG_OTA */
