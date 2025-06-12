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

#include "if_data_output.h"
#include "atlog.h"
#include "fifo.h"
#include "hal_common.h"
#include "hal_timer.h"
#include "mdet_mgr.h"


#ifdef CFG_IF_PRINT_TM

#define IF_PRINT_TIM           TIMER_ID_3
#ifdef ALG_DDC_DIV2_EN
#define IF_PRINT_TIM_PERIOD    3151U /* us, sample_rate/time: 635/1574, 317/3151 */
#else
#define IF_PRINT_TIM_PERIOD    1574U
#endif
#define IF_PRINT_BUF_SIZE      128

static int out_buf[IF_PRINT_BUF_SIZE] = { 0 };
static fifo_t if_out_fifo = {
	0, 0, 0, 0, out_buf
};

static void tm_if_out_expired_hdl(void *arg)
{
	int ret;
	int val;

	ret = fifo_dequeue(&if_out_fifo, &val);
	if (ret == 0) {
#ifdef AIRWATCHER_RAW_SUPPORT
		logr((u32)val, 1);
#else
		at_printf("%04x\r\n", (u16)val);
#endif
	}
}

static void if_data_cb_func(int *buf, u16 size)
{
	u16 i = 0;
	int val = 0;

	if ((buf == NULL) || (size == 0)) {
		return;
	}

	do {
		val = *(buf+i);
		fifo_enqueue(&if_out_fifo, val);
	} while(++i < size);
}

void if_data_out_init(void)
{
	timer_cfg_t timer_cfg = {
		.source     = TIMER_SOURCE_SYSTEM,
		.value_mode = TIMER_VALUE_DEC,
		.start      = 0,
		.irq_en     = 1,
		.cb         = tm_if_out_expired_hdl,
	};

	fifo_init(&if_out_fifo, IF_PRINT_BUF_SIZE);
	mdet_set_data_cb(if_data_cb_func);

	hal_timer_init(IF_PRINT_TIM, &timer_cfg);
	hal_timer_set(IF_PRINT_TIM, IF_PRINT_TIM_PERIOD, tm_if_out_expired_hdl);
	hal_timer_start(IF_PRINT_TIM);
}

#endif

