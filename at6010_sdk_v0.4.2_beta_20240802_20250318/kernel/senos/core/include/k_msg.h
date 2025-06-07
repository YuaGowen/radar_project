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

#ifndef __K_MSG_H__
#define __K_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "l_list.h"

/** Message Identifier */
typedef enum {
	MSG_ID_TIMER_EXPIRED = 10,

#ifdef CFG_CI
	MSG_ID_CI,
#endif

#ifdef CFG_DIP_SWITCH
	MSG_ID_DIP,
#endif

#ifdef CFG_ADC_KEY
	MSG_ID_ADC_KEY,
#endif

	MSG_ID_MAX = 21
} msg_id_t;

/**
 * Message defination as below:
 * Note: message data may have different meaning,
 * timer_expired_msg_data or hrxc_msg_data
 */
typedef struct
{
	struct list_head entry;
	void *para;
} msg_t;

void k_msg_init(void);
int  k_msg_reg_hdl(msg_id_t msg_id, void (*hdl)(msg_t *));
int  k_msg_post(msg_id_t msg_id, msg_t *msg);
void k_msg_loop(void);

#ifdef __cplusplus
}
#endif

#endif /* __K_MSG_H__ */
