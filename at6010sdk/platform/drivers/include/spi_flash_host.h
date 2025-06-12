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

#ifndef __SPI_FLASH_HOST_H__
#define __SPI_FLASH_HOST_H__

#include "at_types.h"

typedef enum {
	SPIFLS_READ_SERIAL_MODE = 0x0,
	SPIFLS_READ_DUAL_MODE = 0x1,
	SPIFLS_READ_QUAD_MODE = 0x2,
	SPIFLS_READ_MAX_MODE
} spifls_read_mode_t;

typedef enum {
	SPIFLS_CMD_MODE_SERIAL = 0x0,
	SPIFLS_CMD_MODE_DUAL = 0x1,
	SPIFLS_CMD_MODE_QUAD = 0x2,
	SPIFLS_CMD_MODE_MAX
} spifls_cmd_mode_t;

typedef struct {
	spifls_cmd_mode_t cmd_mode;
	spifls_read_mode_t read_mode;
	u8 clk_div;        /* spi clock divider */
	u8 sample_delay;   /* spi data input sample delay: 0~7 */
	bool din_cap_edge; /* spi data input capture edge */
	bool hold_pin;     /* set hold pin active */
	bool wp_pin;       /* set wprotect pin active */
} spifls_cfg_t;

bool spifls_host_is_busy(void);
bool spifls_host_tx_fifo_empty(void);
void spifls_host_read_disable(void);
void spifls_host_read_enable(void);
bool spifls_host_send_tx_fifo_data(u8 data_arr[], u32 data_len, bool is_quart_mod);
u32 spifls_host_get_rx_data(u8 *dest, u32 len);
bool spifls_host_clear_fifo_data(bool clr_read_flag, bool clr_write_flag);
void spifls_host_send_cmd(u8 cmd, u32 addr, u8 opmode, u16 blk_size);
void spifls_host_cmd_write(u8 cmd);
bool spifls_host_init(spifls_cfg_t *cfg);
void spifls_host_set_cont_mode(bool enable);

#endif
