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

#ifndef __SYS_MSG_H__
#define __SYS_MSG_H__

#define MSG_SYS_GRP_POS 11

/**
 * message group definition, bit[15..11]
 */
typedef enum {
	MSG_SYS_GRP_CI = 1UL << MSG_SYS_GRP_POS,
	MSG_SYS_GRP_DIP_SWITCH = 2UL << MSG_SYS_GRP_POS,
	MSG_SYS_GRP_ADC_KEY = 3UL << MSG_SYS_GRP_POS,
	MSG_SYS_GRP_AUX_ADC_KEY = 4UL << MSG_SYS_GRP_POS,
	MSG_SYS_GRP_IR = 5UL << MSG_SYS_GRP_POS,
	MSG_SYS_GRP_COMMON = 0x1FUL << MSG_SYS_GRP_POS
} sys_msg_grp_t;

/**
 * sys task message definition, bit[10..0], support the number of message up to 2048.
 */
typedef enum {
	/* sys ci message id definition. */
	MSG_SYS_CI_FRAME_RECEIVED = MSG_SYS_GRP_CI,

	/* sys dip switch message id definition. */
	MSG_SYS_DIP_SWITCH_CHANGED = MSG_SYS_GRP_DIP_SWITCH,

	/* sys adc key message id definition. */
	MSG_SYS_ADC_KEY_IDLE = MSG_SYS_GRP_ADC_KEY,
	MSG_SYS_ADC_KEY_KEY1,
	MSG_SYS_ADC_KEY_KEY2,
	MSG_SYS_ADC_KEY_KEY3,
	MSG_SYS_ADC_KEY_KEY4,
	MSG_SYS_ADC_KEY_KEY5,
	MSG_SYS_ADC_KEY_KEY6,

	/* sys aux adc message id definition. */
	MSG_SYS_AUX_ADC_KEY1_GET = MSG_SYS_GRP_AUX_ADC_KEY,
	MSG_SYS_AUX_ADC_KEY2_GET,
	MSG_SYS_AUX_ADC_KEY3_GET,

	/* sys ir message id definition. */
	MSG_SYS_IR_PRESSED = MSG_SYS_GRP_IR,

	/* sys common message id definition. */
	MSG_SYS_COMMON_START = MSG_SYS_GRP_COMMON
} sys_msg_id_t;

#endif /* __SYS_MSG_H__ */
