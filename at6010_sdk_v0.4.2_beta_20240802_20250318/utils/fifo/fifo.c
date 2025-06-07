/**
 *******************************************************************************
 * @file    fifo.c
 * @author  Airtouch Software Team
 * @brief   FIFO (First In First Out)
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

#include "fifo.h"
#include "common.h"
#include "atlog.h"

#define IS_VALID_FIFO_WIDTH(w)        ((FIFO_WIDTH_8BITS == (w)) || (FIFO_WIDTH_16BITS == (w)) || (FIFO_WIDTH_32BITS == (w)))

s32 fifo_init(fifo_t* pf, u8 *buf, u32 buf_len, u32 width)
{
	if ((NULL == pf) || (NULL == buf) || (0 == buf_len) || (!IS_VALID_FIFO_WIDTH(width))) {
		return -1;
	}

	pf->read = pf->write = pf->used = 0;

	pf->item_buf = (u8*)buf;
	pf->item_width = width;

	switch (width) {
	case FIFO_WIDTH_8BITS:
		pf->item_cnt = buf_len;
		break;

	case FIFO_WIDTH_16BITS:
		pf->item_cnt = buf_len >> 1;
	break;

	case FIFO_WIDTH_32BITS:
		pf->item_cnt = buf_len >> 2;
	break;

	default:
		break;
	}

	return 0;
}

s32 fifo_enqueue(fifo_t* pf, u32 val)
{
	s32 ret = 0;
	u32 idx = 0;
	u8* p8 = NULL;
	u16* p16 = NULL;
	u32* p32 = NULL;

	if (!pf) {
		return -1;
	}

	critical_section_start();
	if (pf->used == pf->item_cnt) { /* fifo full. */
		loge("fifo is full\n");
		ret = -1;
	} else {
		/* index where to get the next data to add to the fifo. */
		idx = pf->write++ % pf->item_cnt;

		switch (pf->item_width) {
		case FIFO_WIDTH_8BITS:
			p8 = (u8 *)(pf->item_buf + idx);
			*p8 = (u8)val;
			break;

		case FIFO_WIDTH_16BITS:
			p16 = (u16 *)(pf->item_buf + (idx << 1));
			*p16 = (u16)val;
			break;

		case FIFO_WIDTH_32BITS:
			p32 = (u32 *)(pf->item_buf + (idx << 2));
			*p32 = (u32)val;
			break;

		default:
			break;
		}

		pf->used++;
	}
	critical_section_end();

	return ret;
}

s32 fifo_enqueue_n_s16(fifo_t* pf, s16* val, u32 n)
{
	u32 idx, i, bond;
	s16* p16 = NULL;

	if (!pf) {
		return -1;
	}

	if (n > (pf->item_cnt - pf->used)) {
		n = pf->item_cnt - pf->used; /* filled available data. */
		/*printf("fifo overflow\n");*/
	}

	critical_section_start();

	bond = pf->item_cnt - 1;

	for (i = 0; i < n; i++) {
		/* index where to get the next data to add to the fifo. */
		idx = pf->write++ % pf->item_cnt;

		p16 = (s16*)(pf->item_buf + (idx << 1));
		*p16 = val[i];
	}
	pf->used += n;
	critical_section_end();

	return n;
}

s32 fifo_dequeue(fifo_t* pf, u32* val)
{
	u32 idx = 0;
	s32 ret = 0;

	if (!pf) {
		return -1;
	}

	critical_section_start();
	if (!pf->used) { /* fifo empty. */
		ret = -1;
	} else {
		idx = pf->read++ % pf->item_cnt;
		switch (pf->item_width) {
		case FIFO_WIDTH_8BITS:
			*val = *(u8 *)(pf->item_buf + idx);
			break;

		case FIFO_WIDTH_16BITS:
			*val = *(u16 *)(pf->item_buf + (idx << 1));
			break;

		case FIFO_WIDTH_32BITS:
			*val = *(u32 *)(pf->item_buf + (idx << 2));
			break;

		default:
			break;
		}
		pf->used--;
	}
	critical_section_end();

	return ret;
}

s32 fifo_get_used(fifo_t *pf, u32 *val)
{
	if (!pf) {
		return -1;
	}

	critical_section_start();
	*val = pf->used;
	critical_section_end();

	return 0;
}

s32 fifo_clear(fifo_t *pf)
{
	u32 idx = 0;
	u32 val0 = 0;

	u8* p8 = NULL;
	u16* p16 = NULL;
	u32* p32 = NULL;

	if (!pf) {
		return -1;
	}

	critical_section_start();
	while (pf->used) { /* fifo not empty. */
		idx = pf->read++ % pf->item_cnt;

		switch (pf->item_width)
		{
		case FIFO_WIDTH_8BITS:
			p8 = (u8 *)(pf->item_buf + idx);
			*p8 = (u8)val0;
			break;

		case FIFO_WIDTH_16BITS:
			p16 = (u16 *)(pf->item_buf + (idx << 1));
			*p16 = (u16)val0;
			break;

		case FIFO_WIDTH_32BITS:
			p32 = (u32*)(pf->item_buf + (idx << 2));
			*p32 = (u32)val0;
			break;

		default:
			break;
		}
		pf->used--;
	}
	critical_section_end();

	return 0;
}
