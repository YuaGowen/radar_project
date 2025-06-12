/**
 * Copyright (c) 2024, Airtouching Intelligence Technology.
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

#ifndef __CI_PORT_H__
#define __CI_PORT_H__

#include "at_types.h"

#include "osal_timer.h"

#ifndef CFG_OS
#include "k_msg.h"
#endif

#include "ci_dld.h"

#define CI_RX_BUF_MAX                  (64)

#define CI_RECV_TIMEOUT_EN
#define CI_RECV_TIMEOUT_MS             (20)


typedef enum {
	CI_RX_MOD_CMD,
	CI_RX_MOD_DATA
} ci_rx_mod_t;

typedef enum {
	CI_STATE_HEAD,
	CI_STATE_CMD,
	CI_STATE_PARA_LEN,
	CI_STATE_PARA,
	CI_STATE_CHECK_CODE,
} ci_rx_state_t;


typedef struct {
	u8 *buf;
#ifndef CFG_OS
	msg_t msg;
#endif
} ci_port_buf_t;

typedef struct {
	u8 rx_buf[CI_RX_BUF_MAX];
	u16 rx_idx;
	u8 para_len;
	ci_rx_state_t state;
	u8 is_ready;
	ci_rx_mod_t rx_mod;
	u8 *rx_d_buf;
	u16 rx_d_size;
#ifdef CI_RECV_TIMEOUT_EN
	osal_timer_t tm_timeout;
#endif

#ifdef CFG_OTA
	dld_ctxt_t m_dld_ctxt;
#endif
} ci_port_ctx_t;

void ci_port_init(void);
void ci_port_reset(void);
u8   ci_port_data_is_ready(void);
u8*  ci_port_get_buf(void);
void ci_port_set_rx_mod(u8 mod);
void ci_port_prepare_rx_data(u16 size);
void ci_send_bytes(u8 *buf, u16 size);

#endif
