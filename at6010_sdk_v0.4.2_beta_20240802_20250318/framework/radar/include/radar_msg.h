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

#ifndef __RADAR_MSG_H__
#define __RADAR_MSG_H__

/*
 * message group definition, bit[15..11]
 */
typedef enum {
	MSG_RAD_GRP_DAT = (0x1u << 11), /* radar data process message group id. */
} rad_msg_grp_t;

/*
 * radar alg message definition, bit[10..0], support the number of message up to 2048.
 */
typedef enum {
	/* radar message id definition. */
	MSG_RAD_START = MSG_RAD_GRP_DAT,
	MSG_RAD_STOP,    /* stop radar algo param*/
	MSG_RAD_DAT_RDY,  /* radar data ready */
	MSG_RAD_MDET_PARA_SET, /* set radar mpd(motion presence detection)algo param */
	MSG_RAD_POD_PARA_SET,  /* set radar pod(presence occupancy detection) algo param */
	MSG_RAD_RF_SET, /* set radar rf  */
} rad_msg_id_t;

#endif /* __RADAR_MSG_H__ */

