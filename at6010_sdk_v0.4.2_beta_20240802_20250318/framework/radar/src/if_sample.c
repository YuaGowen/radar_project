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

#include "if_sample.h"
#include "cpu.h"
#include "radar_mgr.h"
#include "radar_msg.h"
#include "fifo.h"
#include "osal.h"
#include "hal_radar_fmcw.h"
#include "radar_frame.h"

#define IF_SAMPLE_DMA_EN

//#define IF_SAMPLE_USE_ADDR  /* for debug. */

#define IF_SAMPLE_POINTS              32 /* sample points. */
#define IF_SAMPLE_HALF_SIZE           (IF_SAMPLE_POINTS >> 1) /* half of sample points. */
#define IF_DMA_MEM_SIZE               (IF_SAMPLE_POINTS << 1) /* total memory in bytes. */

/* fifo length, set more fifo size to avoid fifo overflow. */
#define IF_FIFO_LENGTH                (IF_SAMPLE_POINTS+IF_SAMPLE_HALF_SIZE)

#ifdef IF_SAMPLE_USE_ADDR
#define IF_DMA_MEM_ADDR                 0x20000000
#else
static s16 if_sample_dma_buf[IF_SAMPLE_POINTS];
#endif

static int que_buf[IF_FIFO_LENGTH] = { 0 };
static fifo_t if_que_fifo = { 0 };

static void get_if_data_hdl(void *buf, u16 size)
{
	s16 *buf_addr = (s16 *)buf;

#if 1
	fifo_enqueue_n_s16(&if_que_fifo, buf_addr, size);
#else
	for (u16 i = 0; i < size; i++) {
		fifo_enqueue(&if_que_fifo, (int)(*buf_addr));
		buf_addr++;
	}
#endif
}

int if_fifo_get_data(int *val)
{
	if (fifo_dequeue(&if_que_fifo, val) < 0)
		return -1;

#if 0
	/* convert to unsigned and move dc position. */
	*val = (*val & 0x8000) ? (*val&0x7fff) : (*val|0x8000);
	*val -= 20000;
#endif

	return 0;
}

u16 if_fifo_get_used_len(void)
{
	int val = 0;

	fifo_get_used(&if_que_fifo, &val);

	return (u16)val;
}

void if_fifo_clear_data(void)
{
	fifo_clear(&if_que_fifo);
}

static void adc_buf_half_hdl(void)
{
#ifdef IF_SAMPLE_USE_ADDR
	void *copysrc = (void *)IF_DMA_MEM_ADDR;
#else
	void *copysrc = (void *)if_sample_dma_buf;
#endif
	osal_msg_t msg = {
		.id = MSG_RAD_DAT_RDY,
		.para = IF_SAMPLE_HALF_SIZE
	};

	get_if_data_hdl(copysrc, IF_SAMPLE_HALF_SIZE);
	osal_task_send_msg(MOD_RAD, &msg, OSAL_WAIT_FOREVER);
}

static void adc_buf_end_hdl(void)
{
#ifdef IF_SAMPLE_USE_ADDR
	void *copysrc = (void *)(IF_DMA_MEM_ADDR + IF_SAMPLE_HALF_SIZE);
#else
	void *copysrc = (void *)(if_sample_dma_buf + IF_SAMPLE_HALF_SIZE);
#endif
	osal_msg_t msg = {
		.id = MSG_RAD_DAT_RDY,
		.para = IF_SAMPLE_HALF_SIZE
	};

	get_if_data_hdl(copysrc, IF_SAMPLE_HALF_SIZE);
	osal_task_send_msg(MOD_RAD, &msg, OSAL_WAIT_FOREVER);
}

void if_sample_pause(void)
{
	//hal_fmcw_chirp_signal_halt();
}

void if_sample_resume(void)
{
	//radar_frame_trigger_mode_set(TRIGGER_MODE_INTERNAL);
	//radar_frame_start();
}

void if_sample_stop(void)
{
	// adc_stop(ADC_CH_MAIN);
}

void if_sample_start(void)
{
	// adc_start(ADC_CH_MAIN);
}

void if_sample_init(void)
{
	int ret;

	fifo_init(&if_que_fifo, (u8 *)que_buf, IF_FIFO_LENGTH * sizeof(u32), FIFO_WIDTH_32BITS);

#if 0
	adc_if_conf adc = {
		.ch = ADC_CH_MAIN,
		.sample_rate = ADC_SAMPLE_RATE_635HZ,
#ifdef IF_SAMPLE_USE_ADDR
		.d_buf = IF_DMA_MEM_ADDR,
#else
		.d_buf = (uint32)if_sample_dma_buf,
#endif
		.d_len = IF_DMA_MEM_SIZE,
		.half_callback = adc_buf_half_hdl,
		.end_callback = adc_buf_end_hdl,
#ifdef IF_SAMPLE_DMA_EN
		.buf_head = NULL,
		.dma_enable = 1,
#else
		.buf_head = (u32*)if_sample_dma_buf,
		.dma_enable = 0,
#endif
		.adc_mode = ADC_AUTO_MODE,
	};

	// ret = adc_open(&adc, adc.ch);
	if (ret == 0) {
		// ret = adc_start(adc.ch);
	}
#endif

}
