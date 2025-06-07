/**
 *******************************************************************************
 * @file    handle_mdet_info.c
 * @author  Airtouch Software Team
 * @brief   User process the motion detection information on their own.
 *******************************************************************************
 * @copyright Copyright (c) 2023, Airtouching Intelligence Technology.
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

#include "atlog.h"
#include "uart.h"
#include "hal_uart.h"
#include "handle_mdet_info.h"

#if AUTO_REPORT_INTERVAL_MS > 0U
#include "at_malloc.h"
#if defined(ULP_PROG_1) || defined(ULP_PROG_2)
#include "ulp_mem.h"
#else
#include "osal.h"
#endif
#endif /* #if AUTO_REPORT_INTERVAL_MS > 0U */

#define G_RESP_BUF_LEN          (1 + 1 + 255 + 1)

#define SIN_COS_TABLE {\
        0x0000, 0x023B, 0x0477, 0x06B2, 0x08ED, 0x0B27, 0x0D61, 0x0F99,\
        0x11D0, 0x1406, 0x163A, 0x186C, 0x1A9C, 0x1CCB, 0x1EF7, 0x2120,\
        0x2348, 0x256C, 0x278D, 0x29AC, 0x2BC7, 0x2DDF, 0x2FF3, 0x3203,\
        0x340F, 0x3618, 0x381C, 0x3A1C, 0x3C17, 0x3E0E, 0x4000, 0x41EC,\
        0x43D4, 0x45B6, 0x4793, 0x496A, 0x4B3C, 0x4D08, 0x4ECD, 0x508D,\
        0x5246, 0x53F9, 0x55A6, 0x574B, 0x58EA, 0x5A82, 0x5C13, 0x5D9C,\
        0x5F1F, 0x609A, 0x620D, 0x6379, 0x64DD, 0x6639, 0x678D, 0x68D9,\
        0x6A1D, 0x6B59, 0x6C8C, 0x6DB7, 0x6ED9, 0x6FF3, 0x7104, 0x720C,\
        0x730B, 0x7401, 0x74EF, 0x75D3, 0x76AD, 0x777F, 0x7847, 0x7906,\
        0x79BC, 0x7A68, 0x7B0A, 0x7BA3, 0x7C32, 0x7CB8, 0x7D33, 0x7DA5,\
        0x7E0E, 0x7E6C, 0x7EC1, 0x7F0B, 0x7F4C, 0x7F83, 0x7FB0, 0x7FD3,\
        0x7FEC, 0x7FFB, 0x8000\
}

static const u16 m_sin_cos_table[91] = SIN_COS_TABLE;
static u8 m_report_stop;

#if AUTO_REPORT_INTERVAL_MS > 0U
#if defined(ULP_PROG_1) || defined(ULP_PROG_2)
static u32 *m_report_tick; /* [0] last, [1] current */
#else
static u32 m_last_report_time;
#endif
#endif /* #if AUTO_REPORT_INTERVAL_MS > 0U */


/* Private Functions -----------------------------------------------------------------------------*/

static u16 _get_check_code(const _ci_ctrl_frm_t *sfram)
{
	u16 cc = 0;

	cc = sfram->head + ((sfram->cmd_grp << 5) | (sfram->cmd)) + sfram->para_len;
	for (s32 i = 0; i < sfram->para_len; i++) {
		cc += sfram->para[i];
	}

	return cc;
}

static void _uart_write_frame(const _ci_ctrl_frm_t *frame)
{
	u8 buf[128] = { 0 };

	u8 idx = 0;
	s32 i = 0;

	buf[idx++] = frame->head;
	buf[idx++] = (frame->cmd_grp << 5) | (frame->cmd);
	buf[idx++] = frame->para_len;
	for (i = 0; i < frame->para_len; i++) {
		buf[idx++] = frame->para[i];
	}

	buf[idx++] = (u8)frame->check_code & 0xFF;
	buf[idx++] = (u8)(frame->check_code >> 8) & 0xFF;

	hal_uart_send_wait(UART_CI_PORT, buf, idx, 0xFFFFFFFFU);
}

static void _cmd_resp_para(u8 cmdi, u8 *para, u8 len)
{
#ifndef CFG_DET_INFO_OUT
	/* No need to output, directly exit */
	return;
#endif
	_ci_ctrl_frm_t sfram = { 0 };

	sfram.head = _CI_HEAD_RESP;
	sfram.cmd = cmdi & 0x1F;
	sfram.cmd_grp = (cmdi >> 5) & 0x07;
	sfram.para_len = len;

	if (0 != len) {
		sfram.para = para;
	}

	sfram.check_code = _get_check_code(&sfram);

	_uart_write_frame(&sfram);
}

#if AUTO_REPORT_INTERVAL_MS > 0U
static bool report_interval_reached(void)
{
	bool ret = FALSE;
	u32 current;
	u32 last;
	u32 interval;
	u32 tick_ms;
	u32 *p_last;

#if defined(ULP_PROG_1) || defined(ULP_PROG_2)
	current = m_report_tick[0];
	last = m_report_tick[1];
	p_last = &m_report_tick[1];
	tick_ms = PARA_FRAME_INTERVAL_MS;
#else
	current = osal_time_get_ms();
	last = m_last_report_time;
	p_last = &m_last_report_time;
	tick_ms = 1U;
#endif

	interval = (current > last) ? (current - last) : (0xFFFFFFFFU - current + last);
	interval *= tick_ms;
	if (interval > AUTO_REPORT_INTERVAL_MS) {
		*p_last = current;
		ret = TRUE;
	}
	return ret;
}

static void report_time_init(void)
{
#ifdef ULP_PROG_1
	m_report_tick = (u32 *)at_malloc_pool(AT_MEM_POOL_0_ID, sizeof(u32) * 2U);
	ulp_mem_set_addr(ULP_MEM_USER_ID_16, (u32)m_report_tick);
	m_report_tick[0] = 0U;
	m_report_tick[1] = 0U;
#endif

#ifdef ULP_PROG_2
	m_report_tick = (u32 *)ulp_mem_get_addr(ULP_MEM_USER_ID_16);
	m_report_tick[1]++;
#endif
}
#endif /* #if AUTO_REPORT_INTERVAL_MS > 0U */

static void auto_report_port_init(void)
{
	if (HAL_UART_STATE_RESET == hal_uart_state_get(UART_CI_PORT)) {
		uart_cfg_t uart_cfg = {0};
		uart_cfg.baud_rate = UART_CI_BAUDRATE;
		uart_cfg.parity = UART_PARITY_DISABLE;
		uart_cfg.stop_bits = UART_STOP_BITS_1;
		uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
		uart_cfg.fifo_cfg.fifo_en = 1U;
		(void)hal_uart_init(UART_CI_PORT, &uart_cfg);
	}
}

/**
 * @brief
 *
 * @param type see @ref det_result_type_t
 * @param para
 * @param len
 */
void auto_report_resp_para(u8 type, u8 *para, u8 len)
{
#ifndef CFG_DET_INFO_OUT
	/* No need to output, directly exit */
	return;
#endif
	if (1U == m_report_stop) {
		return;
	}

#if AUTO_REPORT_INTERVAL_MS > 0U
	if (FALSE == report_interval_reached()) {
		return;
	}
#endif
	u16 idx = 0;
	u8  chk = 0;
	u8 resp_buf[G_RESP_BUF_LEN];

	// fill buffer
	resp_buf[idx++] = AUTO_REPORT_HEAD;
	resp_buf[idx++] = 1 + len;
	resp_buf[idx++] = type;

	for (u16 i = 0; i < len; i++) {
		resp_buf[idx++] = para[i];
	}

	// check sum
	for (u16 i = 0; i < idx; i++) {
		chk += resp_buf[i];
	}
	resp_buf[idx++] = chk;

	auto_report_port_init();
	hal_uart_send_wait(UART_CI_PORT, resp_buf, idx, 0xFFFFFFFFU);
}

/* Global Functions ------------------------------------------------------------------------------*/

void handle_mdet_info_stop(void)
{
	m_report_stop = 1U;
}

void handle_mdet_info_all(void *info)
{
	if (NULL == info) {
		return;
	}

	fmcw_det_info_t *fmcw = (fmcw_det_info_t *)info;

	if (fmcw->is_detected == 1)
		logi("%d cm\n\r", fmcw->range_val / 10);

	auto_report_resp_para(DET_TYPE_ALL, (u8 *)fmcw, sizeof(fmcw_det_info_t));
}

void handle_mdet_info_htm(void *info)
{
	if (NULL == info) {
		return;
	}

	htm_det_info_t *htm_hdl = (htm_det_info_t *)info;
	if(htm_hdl->status == 3) {
		logi("htm res:%dmm\n\r", htm_hdl->htm);
	} else if(htm_hdl->status == 2){
		logi("htm sta \n\r");
	} else if (htm_hdl->status == 1){
		if(htm_hdl->htm == 0) {
			logi("htm cal failed\n\r");
		} else {
			logi("htm cal done\n\r");
		}
	}

	auto_report_resp_para(DET_TYPE_HTM, (u8 *)htm_hdl, sizeof(htm_det_info_t));
}

void handle_mdet_info_pod(void *info)
{
	if (NULL == info) {
		return;
	}

	pod_det_info_t *pod = (pod_det_info_t *)info;

	auto_report_resp_para(DET_TYPE_POD, (u8 *)pod, sizeof(pod_det_info_t));
}

void handle_mdet_info_mot(fmcw_det_info_t *info)
{
	if (NULL == info) {
		return;
	}

	mot_det_info_t *mot = (mot_det_info_t *)info;

	if (mot->is_detected == 1) {
		logi("%d cm\n\r", mot->range_val / 10);
	}

	auto_report_resp_para(DET_TYPE_MOT, (u8 *)mot, sizeof(mot_det_info_t));
}

void handle_mdet_info_rgn(rgn_det_info_t *info)
{
	if (NULL == info) {
		return;
	}

	for(u8 i = 0; i < info->obj_num;i++ ) {
		logi("obj_id:%d,dist:%dcm,angle:%d\n\r", i,info->obj[i].range_val/10,info->obj[i].angle_val);
	}

	auto_report_resp_para(DET_TYPE_RGN, (u8 *)info, sizeof(rgn_det_info_t));
}

void handle_mdet_info_init(void)
{
#if AUTO_REPORT_INTERVAL_MS > 0U
	report_time_init();
#endif
}

void handle_mdet_pc_data(void *pc, u8 size)
{
	if (NULL == pc) {
		return;
	}
	_cmd_resp_para(_CI_CMD_MDET_PC_DAT, (u8 *)pc, size);
}

void handle_coords_cal(s32 angle, s32 length, coords_point_t *pos)
{
	if (angle >= 0 && angle <= 90) {
		s32 index = angle;
		s32 angsin = m_sin_cos_table[index];
		s32 angcos = m_sin_cos_table[90 - index];

		pos->x = length * angsin >> 15;
		pos->y = length * angcos >> 15;

	} else if (angle < 0 && angle >= -90) {
		s32 index = -angle;

		s32 angsin = m_sin_cos_table[index];
		s32 angcos = m_sin_cos_table[90 - index];

		pos->x = -1 * length * angsin >> 15;
		pos->y = length * angcos >> 15;
	} else {
		pos->x = 0;
		pos->y = length;
	}
}
