/**
 *******************************************************************************
 * @file    uart_if.h
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

#ifndef __UART_IF_H__
#define __UART_IF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/

#include "at_types.h"
#include "uart.h"
#include "ytool.h"

/* Data Types ------------------------------------------------------------------------------------*/

typedef struct {
	uart_id_t id;
} port_uart_type_t;

/* Global Variables ------------------------------------------------------------------------------*/

extern port_uart_type_t port_uart;

/* Function Declarations -------------------------------------------------------------------------*/

status_code_t uart_if_send_timeout(port_uart_type_t* port, u8* buffer, u16 length);

status_code_t uart_if_recv_timeout(port_uart_type_t* port, u8* buffer, u16 length, u32 timeout);

status_code_t uart_if_send_byte(u8 data);

void uart_if_flush_recvd(port_uart_type_t* port);

#ifdef __cplusplus
}
#endif

#endif /* !__UART_IF_H__ */
