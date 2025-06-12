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
#ifndef __APP_MSG_H__
#define __APP_MSG_H__

#define BT_STRING_MAX_LEN 250


/*
 * message group definition, bit[15..11]
 */
typedef enum {
	MSG_APP_GRP_LIGHT = (0x1u << 11), /* app light message group id. */
	MSG_APP_GRP_COMMON = (0x1u << 12), /* app common message group id. */
	MSG_APP_GRP_BT = (0x1u << 13),     /* app bt message group id. */
} app_msg_grp_t;


/*
 * radar alg message definition, bit[10..0], support the number of message up to 2048.
 */
typedef enum {
	/* app light message id definition. */
	MSG_APP_LIGHT_ON = MSG_APP_GRP_LIGHT,
	MSG_APP_LIGHT_OFF,
	MSG_APP_LIGHT_CTRL,

	/* app common message id definition. */
	MSG_APP_COMMON_START = MSG_APP_GRP_COMMON,
	MSG_APP_COMMON_MDET_RET, /* motion deteciton result. */
	MSG_APP_COMMON_FDET_RET, /* fine motion deteciton result. */
	MSG_APP_COMMON_BDET_RET, /* breath motion deteciton result. */
	MSG_APP_COMMON_DETECTED,

	/* app bt message id definition. */
	MSG_APP_BT_ON_REQ = MSG_APP_GRP_BT,
	MSG_APP_BT_OFF_REQ,
	MSG_APP_BT_ADV_ON_REQ,
	MSG_APP_BT_ADV_OFF_REQ,
	MSG_APP_BT_DISCONNECT_REQ,
	MSG_APP_BT_DATA_REQ,

} app_msg_id_t;

/*
 * bt data type definition
 */
typedef enum {
	BT_DATA_TYPE_UART = 0,   /* uart data */
	BT_DATA_TYPE_HEIGHT,     /* height data */
	BT_DATA_TYPE_HEART,      /* heart data */
} app_bt_data_type_t;


typedef struct app_bt_data_info {
	u8 data_type;  /* see @app_bt_data_type_t */
	u8 data_len;
	u8 data[BT_STRING_MAX_LEN];
} app_bt_data_info_t;


#endif /* __APP_MSG_H__ */

