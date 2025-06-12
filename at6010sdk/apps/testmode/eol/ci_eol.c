/**
 *******************************************************************************
 * @file    ci_eol.c
 * @author  Airtouch Software Team
 * @brief   CI interface for EOL
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

#include "gpio.h"
#include "hal_radar.h"
#include "sys_data.h"
#include "mdet_fmcw.h"
#include "radar_mgr.h"
#include "ci_port.h"
#include "ci_cmd.h"
#include "ci_eol.h"
#include "osal_time.h"
#include "string.h"
#include "cfg_radar_profile_eol.h"
#include "chip_defs.h"
#include "spi_flash_drv.h"
#include "app_eol.h"
#include "mdet_eolcal.h"
#include "gpio.h"
#include "common.h"

#ifdef CFG_EOL_TESTMOD

/* enable eol debug mode, disable it in the release version */
//#define DBG_EOL

#ifdef DBG_EOL
#define dbg_eol_log    logi
#else
#define dbg_eol_log(...)
#endif

#define CI_EOL_TEST_MODE_GPIO_TEST_SEL  0

static antcal_ci_rsp_t eol_ci_rsp = {
	.eolcal_rsp_timer = { 0 },
	.status = CI_EOL_STATUS_INIT,
	.init_flag = false,
	.param = 0,
};

extern void radar_profile_init(void);

static void eol_profile_init(void)
{
	radar_profile_t eol_profile_sweep_1g_if_62g = {
		.rfe = {
			.tx_gain = RADAR_PRF_TX_GAIN,
			.rx_cfg = {
				.rx_num = RADAR_PRF_RFE_RX_NUM,
				.hpf1 = RADAR_PRF_RFE_HPF1,
				.hpf2 = RADAR_PRF_RFE_HPF2,
				.lpf = RADAR_PRF_RFE_LPF
			}
		},
		.chirp = {
			RADAR_PRF_CHIRP_0,
			RADAR_PRF_CHIRP_1,
			RADAR_PRF_CHIRP_2,
			RADAR_PRF_CHIRP_3,
			RADAR_PRF_CHIRP_4,
			RADAR_PRF_CHIRP_5,
			RADAR_PRF_CHIRP_6,
			RADAR_PRF_CHIRP_7,
		},
		.subfrm = {
			RADAR_PRF_SUBFRM_0,
			RADAR_PRF_SUBFRM_1,
			RADAR_PRF_SUBFRM_2,
			RADAR_PRF_SUBFRM_3,
			RADAR_PRF_SUBFRM_4,
			RADAR_PRF_SUBFRM_5,
			RADAR_PRF_SUBFRM_6,
			RADAR_PRF_SUBFRM_7,
		},
		.frame = RADAR_PRF_FRAME,
		.filter = RADAR_PRF_FILTER
	};
	if (E_OK != hal_radar_init(&eol_profile_sweep_1g_if_62g)) {
		loge("eol init fail!\r\n");
	}
}

static void eolcal_response_timer_cb(void)
{
	s32 ret = 0;
	u16 chksum = 0;
	fmcw_det_ca_info_t rsp_fft_data[5];

	logi("%s,status=%d\n",__FUNCTION__,eol_ci_rsp.status);
	osal_timer_stop(&(eol_ci_rsp.eolcal_rsp_timer));
	hal_radar_stop();
	switch(eol_ci_rsp.status) {
		case CI_EOL_STATUS_ANGLEREF_FFT_REQ:
			logi("responese from anglereflect request.\n");
			ret = radar_fmcw_ca_info_get(rsp_fft_data);
			if(0 == ret) {
				for(s32 i=0;i<5;i++)
					cmd_resp_para(CI_EOL_PHASECAL_LOCALE_SET,(u8 *)&rsp_fft_data[i],sizeof(fmcw_det_ca_info_t));
			}
		break;
		case CI_EOL_STATUS_LOCALE_FFT_REQ:	//DUMP ALL RB FFT
			logi("responese from dump all rb fft request.\n");
			if(1) {
				u8 rsp_data[3+EOL_LOCALEFFT_RSP_MAX_BYTES+2];
				ret = radar_fmcw_ca_locale_fft_get((u8 *)&(rsp_data[3]));
				if(0 != ret) {
					s8 rsp_ret = (s8)ret;
					cmd_resp_para(CI_EOL_LOCALE_FFT_DUMP, (u8 *)&rsp_ret, 1u);
					eol_ci_rsp.status = CI_EOL_STATUS_IDLE;
					return;
				}
				rsp_data[0] = 0x59;
				rsp_data[1] = CI_EOL_LOCALE_FFT_DUMP;
				rsp_data[2] = EOL_LOCALEFFT_RSP_MAX_BYTES;
				for(s32 i=0;i<EOL_LOCALEFFT_RSP_MAX_BYTES;i++) {
					chksum += rsp_data[3+i];
					chksum &= 0xffff;
				}
				rsp_data[3+EOL_LOCALEFFT_RSP_MAX_BYTES] = (chksum&0xff);
				rsp_data[3+EOL_LOCALEFFT_RSP_MAX_BYTES+1] = ((chksum>>8)&0xff);
				ci_send_bytes(rsp_data, sizeof(rsp_data));
			}
		break;
		case CI_EOL_STATUS_LOCALE_INFO_REQ:
			logi("after cal res apply,responese from locale info request.\n");
			if(1) {
				phase_ca_loc_info_t tgtinfo;
				ret = radar_fmcw_ca_tgt_info_get((phase_ca_loc_info_t *)&tgtinfo);
				if(0 != ret) {
					s8 rsp_ret = (s8)ret;
					cmd_resp_para(CI_EOL_PHASECAL_APPLY_TGTGET, (u8 *)&rsp_ret, 1u);
					eol_ci_rsp.status = CI_EOL_STATUS_IDLE;
					return;
				}
				cmd_resp_para(CI_EOL_PHASECAL_APPLY_TGTGET, (u8 *)&tgtinfo, sizeof(phase_ca_loc_info_t));
			}
		break;
		case CI_EOL_STATUS_SNR_REQ:
			logi("response from snr request.\n");
			if(1) {
				s8 snr_res[2];
				ret = radar_fmcw_ca_snr_get((void *)&snr_res);
				if(0 != ret) {
					s8 rsp_ret = (s8)ret;
					cmd_resp_para(CI_EOL_SNR_GET, (u8 *)&rsp_ret, 1u);
					eol_ci_rsp.status = CI_EOL_STATUS_IDLE;
					return;
				}
				if((0x3 == eol_ci_rsp.param) || (0x1 == eol_ci_rsp.param)) {	//rx0 and rx1
					u8 rsp_data[3];
					rsp_data[0] = eol_ci_rsp.param;
					rsp_data[2] = rsp_data[1] = 0;
					if(1 == eol_ci_rsp.param)
						rsp_data[1] = snr_res[0];
					if(3 == eol_ci_rsp.param) {
						rsp_data[1] = snr_res[0];
						rsp_data[2] = snr_res[1];
					}
					cmd_resp_para(CI_EOL_SNR_GET, (u8 *)&rsp_data, 3);
					eol_ci_rsp.param = 0;
				}
				else {
					u8 rsp_data = 0x00;
					cmd_resp_para(CI_EOL_SNR_GET, (u8 *)&rsp_data, sizeof(rsp_data));
				}

			}
		break;

		default:
		break;
	}
	eol_ci_rsp.status = CI_EOL_STATUS_IDLE;

}


u8 ci_eol_test_mode_entry(const u8 *p, u8 len)
{
	u8 ret = CI_ERR_NO;

	dbg_eol_log("eol entry.\r\n");

	if ((p == NULL) || (len != 4u)) {
		ret = (u8)CI_ERR_PARA_LEN_INVLD;
	}
	else {
		u32 magic_val = (p[3] << 24 |p[2] << 16 | p[1] << 8 |p[0]);
		if (0xA012345F != magic_val) {
			ret = (u8)CI_ERR_PARA_ERR;
			return ret;
		}
		if(!eol_ci_rsp.init_flag) {
			osal_status_t ret = osal_timer_create(&(eol_ci_rsp.eolcal_rsp_timer),
	                                        OSAL_TIMER_MODE_SINGLE,
	                                        (osal_timer_cb_t)eolcal_response_timer_cb,
	                                        NULL,
	                                        300);
			if(0 != ret) {
				ret = (u8)CI_ERR_PARA_ERR;
				return ret;
			}
			eol_ci_rsp.status = CI_EOL_STATUS_IDLE;
			eol_ci_rsp.init_flag = true;
		}

		eol_app_init();
		hal_radar_stop();
		hal_radar_ulp_stop();
#if 0
		radar_mgr_profile_set(eol_profile_init);
#endif
		eolcal_init();
	}

	cmd_resp_para(CI_EOL_ENTRY_TESTMOD, &ret, 1u);

	return ret;
}

u8 ci_eol_test_snr_get(const u8 *p, u8 len)
{
	u8 ret = CI_ERR_NO;
	u8 rx_mask = 0;

	if ((p == NULL) || (len != 1u)) {
		ret = (u8)CI_ERR_PARA_LEN_INVLD;
		cmd_resp_para(CI_EOL_SNR_GET, &ret, 1u);
		return ret;
	}
	rx_mask = p[0];
	hal_radar_start();
	radar_fmcw_ca_snr_req();
	eol_ci_rsp.param = rx_mask;
	eol_ci_rsp.status = CI_EOL_STATUS_SNR_REQ;
	osal_timer_start((osal_timer_t *)&eol_ci_rsp.eolcal_rsp_timer);

	return ret;
}

#if (CI_EOL_TEST_MODE_GPIO_TEST_SEL == 1)
static u8 eol_gpio_irq_occur(u32 pin_sel, u8 val, u32 interval, u32 eol_gpio_test_port)
{
	u8 ret = 1u;
	u32 data, gpio_irq, gpio_irq_rise, rise_exp;

	rise_exp = (0u == val) ? 0u : eol_gpio_test_port;

	for (u8 i = 0; i < 17; i++) {
		if (((pin_sel >> i) & 1u) == 1u) {
			gpio_set_value((gpio_pin_t)i, val);
		}
	}
	delay_ms(interval);

	data = CMSDK_GPIO0->INTSTATUS;

	//logi("data 0x%08x  ", data);

	gpio_irq = data & eol_gpio_test_port;
	gpio_irq_rise = (data >> 16u) & eol_gpio_test_port;

	ret = ((gpio_irq == 0u) || (gpio_irq_rise != rise_exp)) ? 1u : 0u;
	//logi("gpio_irq %02d, gpio_irq_rise 0x%08x rise_exp 0x%08x\r\n", gpio_irq, gpio_irq_rise, rise_exp);
	CMSDK_GPIO0->INTCLEAR = eol_gpio_test_port;

	return ret;
}

static u8 gpio_test_irq(const u8 *p)
{
	u8 ret = 0u, tmp = 0u;
	u8 item, toggle_cnt;
	u16 interval;
	u32 pin_sel;
	u32 eol_gpio_test_port = 0u;

	toggle_cnt = p[1u];
	interval = (p[3] << 8u) | p[2u];                       /* unit: ms */
	pin_sel = (p[7] << 24) | (p[6] << 16) | (p[5] << 8) | p[4];

	/* irq port, except GPIO_AON_2 */
	eol_gpio_test_port = (((pin_sel >> 13u) & 0xfu) << 12u) | (((pin_sel >> 10u) & 0x3u) << 10u) | (pin_sel & 0x3ffu);

	/* eol_gpio_test_init(pin_sel); */
	gpio_cfg_t gpio_cfg = {
		.mode = GPIO_MODE_OUTPUT,
		.pull = GPIO_PULL_NONE
	};

	for (u8 i = 0u; i < 17u; i++) {
		if (((pin_sel >> i) & 1u) == 1u) {
			gpio_init((gpio_pin_t)i, &gpio_cfg);
		}
	}
	for (u8 i = 0u; i < 17u; i++) {
		if (((pin_sel >> i) & 1u) == 1u) {
			gpio_set_value((gpio_pin_t)i, 0u);
		}
	}

	CMSDK_GPIO0->INTTYPESET = eol_gpio_test_port;          /* 0: level; 1: edge */
	CMSDK_GPIO0->INTENSET = eol_gpio_test_port;
	CMSDK_GPIO0->INTPOLSET = (eol_gpio_test_port << 16u);  /* edge both */

	tmp = 1u;
	for (u16 i = 0u; i < 2u * toggle_cnt; i++) {
		ret += eol_gpio_irq_occur(pin_sel, tmp, interval, eol_gpio_test_port);
		tmp = 1u - tmp;
	}

	logi("eol_gpio_test %s\r\n", (ret == 0u) ? "PASS" : "FAIL");
	return ret;
}
#endif

u8 ci_eol_test_mode_gpio_test(const u8* p, u8 len)
{
	u8 ret = 0u, item, pin_val;
	u32 pin_sel;
	gpio_cfg_t gpio_cfg = {
		.mode = GPIO_MODE_OUTPUT,
		.pull = GPIO_PULL_NONE
	};

	if ((p == NULL)) {
		ret = (u8)CI_ERR_PARA_LEN_INVLD;
	}

	item = p[0u];
	if (item == 0u) {
		pin_val = p[1u];
		pin_sel = (p[5] << 24) | (p[4] << 16) | (p[3] << 8) | p[2];
		logi("pin_val 0x%x, pin_sel 0x%x\r\n", pin_val, pin_sel);

		for (u8 i = 0u; i < 17u; i++) {
			if (((pin_sel >> i) & 1u) == 1u) {
				gpio_init((gpio_pin_t)i, &gpio_cfg);
				gpio_set_value((gpio_pin_t)i, pin_val);
			}
		}
	}
	else {
		ret = (u8)CI_ERR_PARA_ERR;
#if (CI_EOL_TEST_MODE_GPIO_TEST_SEL == 1)
		ret = gpio_test_irq(p);
#endif
	}

	cmd_resp_para(CI_EOL_GPIO_TEST, (u8 *)&ret, sizeof(u8));

	return 0u;
}

//DUMP ALL RB FFT
u8 ci_eol_test_mode_locale_fft_get(const u8 *p, u8 len)
{
	if(0 != len) {
		return CI_ERR_PARA_LEN_INVLD;
	}

	hal_radar_start();
	///logi("ln %d",__LINE__);
	radar_fmcw_ca_loc_fft_dump_req();
	eol_ci_rsp.status = CI_EOL_STATUS_LOCALE_FFT_REQ;
	osal_timer_start((osal_timer_t *)&eol_ci_rsp.eolcal_rsp_timer);

	return CI_ERR_NO;
}

u8 ci_eol_test_mode_antphase_calib(const u8 *p, u8 len)
{
	fmcw_det_ca_loc_t angle_reflect_info;

	if (sizeof(fmcw_det_ca_loc_t) != len)
		return CI_ERR_PARA_LEN_INVLD;
	memcpy(&angle_reflect_info, (void *)p, len);
	hal_radar_start();	//radar stop @ eolcal_rsp_timer time out cb.
	radar_fmcw_ca_loc_set((void *)&angle_reflect_info);
	eol_ci_rsp.status = CI_EOL_STATUS_ANGLEREF_FFT_REQ;
	osal_timer_start((osal_timer_t *)&eol_ci_rsp.eolcal_rsp_timer);

	return CI_ERR_NO;
}

u8 ci_eol_test_mode_calres_read(const u8 *p, u8 len)
{
	u8 ret = CI_ERR_NO;
	u32 read_val = 0;
	antcal_res_struct cal_result;
	u32 read_byte_cnt, offset;

	if(0 != len)
		return CI_ERR_PARA_LEN_INVLD;

	ant_phase_item_t *ant_phase = (ant_phase_item_t *)sys_data_item_get(INDEX_ANT_PHASE);
	if (ant_phase != NULL) {
		offset = (u32)(ant_phase) - (CACHE_FLASH_ADDR);
		spi_flash_ll_read(offset, sizeof(read_val), &read_byte_cnt, (u8 *)&read_val);
	}

	memcpy(&cal_result, &read_val, sizeof(u32));
	cmd_resp_para(CI_EOL_PHASECAL_RES_R, (u8 *)&cal_result, sizeof(antcal_res_struct));
	logi("phase_delta=%04x,k=%04x\r\n",cal_result.phase_delta,cal_result.k);
	return ret;
}

u8 ci_eol_test_mode_calres_write(const u8 *p, u8 len)
{
	u8 ret = CI_ERR_NO, count = 0;
	antcal_res_struct cal_result;
	u32 write_val = 0;

	if ((NULL == p) || (0 == len))
		return CI_ERR_PARA_LEN_INVLD;
	count = p[0];
	if (count != 1)
		return CI_ERR_PARA_ERR;
	cal_result.phase_delta = (p[2]<<8 | p[1]);
	cal_result.k = (p[4]<<8 | p[3]);
	memcpy(&write_val, &cal_result, sizeof(u32));

	ant_phase_item_t tmp_ant_phase;
	tmp_ant_phase.calib = write_val;
	sys_data_item_set(INDEX_ANT_PHASE, &tmp_ant_phase);

	cmd_resp_para(CI_EOL_PHASECAL_RES_W, (u8 *)&ret, sizeof(u8));
	logi("write:phase_data=0x%04x,k=0x%04x\r\n",cal_result.phase_delta,cal_result.k);

	return ret;
}

u8 ci_eol_test_mode_calres_apply_tgtget(const u8 *p, u8 len)
{
	u8 ret = CI_ERR_NO;
	u32 offset,read_val,read_byte_cnt;
	antcal_res_struct cfg_res;
	ant_phase_item_t *ant_phase = NULL;

	if (0 != len)
		return CI_ERR_PARA_LEN_INVLD;
	ant_phase = (ant_phase_item_t *)sys_data_item_get(INDEX_ANT_PHASE);
	if (NULL == ant_phase) {
		ret = 0xff;
		cmd_resp_para(CI_EOL_PHASECAL_APPLY_TGTGET, (u8 *)&ret, sizeof(u8));
		return CI_ERR_NO;
	}
	offset = (u32)(ant_phase) - (CACHE_FLASH_ADDR);
	spi_flash_ll_read(offset, sizeof(read_val), &read_byte_cnt, (u8 *)&read_val);
	hal_radar_start();	//radar stop @ eolcal_rsp_timer time out cb.
	memcpy((void *)&cfg_res, &read_val, sizeof(u32));
	logi("delta=0x%04x,k=0x%04x\r\n",cfg_res.phase_delta,cfg_res.k);
	radar_fmcw_ca_res_set((void *)&cfg_res);
	eol_ci_rsp.status = CI_EOL_STATUS_LOCALE_INFO_REQ;
	osal_timer_start((osal_timer_t *)&eol_ci_rsp.eolcal_rsp_timer);
	return CI_ERR_NO;
}

u8 ci_eol_test_mode_io_out_set(const u8 *p, u8 len)
{
	u8 ret = CI_ERR_NO, value = 0;
	gpio_cfg_t gpio_cfg;
	gpio_pin_t gpio_pin = RADAR_OUT_PIN;

#if defined(CFG_CUST_ANKER)
	gpio_pin = DET_INFO_NOTIFY_PIN;
#endif

	if ((NULL == p) || (1 != len))
		return CI_ERR_PARA_LEN_INVLD;
	value = p[0];
	if ((0 != value) && (1 != value))
		return CI_ERR_PARA_ERR;
	gpio_cfg.mode = GPIO_MODE_OUTPUT;
	gpio_cfg.pull = GPIO_PULL_NONE;
	(void)gpio_init(gpio_pin, &gpio_cfg);
	gpio_set_value((u32)gpio_pin, value);
	cmd_resp_para(CI_EOL_IO_OUT_SET, (u8 *)&ret, sizeof(u8));

	dbg_eol_log("io out: io=%d, val=%d\r\n", gpio_pin, value);

	return ret;
}


u8 ci_eol_test_mode_pwrlvl_set(const u8 *p, u8 len)
{
	u8 ret = CI_ERR_NO, pwr_mode=0;

	if ((NULL == p) || (1 != len))
		return CI_ERR_PARA_LEN_INVLD;
	pwr_mode = p[0];
	logi("power mode == %u\r\n", pwr_mode);
	cmd_resp_para(CI_EOL_PWRLVL_SET, (u8 *)&ret, sizeof(u8));
	return ret;
}


u8 ci_eol_test_mode_slfchk(const u8 *p, u8 len)
{
	u8 ret = CI_ERR_NO;

	if ((NULL == p) || (0 != len))
		return CI_ERR_PARA_LEN_INVLD;
	cmd_resp_para(CI_EOL_SELF_CHECK, (u8 *)&ret, sizeof(u8));
	return ret;
}


u8 ci_eol_test_mode_rftx_onoff(const u8 *p, u8 len)
{
	u8 ret = CI_ERR_NO, _flag = 0;

	if ((NULL == p) || (1 != len))
		return CI_ERR_PARA_LEN_INVLD;
	_flag = p[0];
	if ((_flag != 0) && (_flag != 1))
		return CI_ERR_PARA_LEN_INVLD;
	if (0 == _flag)          //the 0 bit is 0, the 0 tx off.
		radar_tx_pa_off();
	else
		radar_tx_pa_on();	 //the 0 bit is 1, the 0 tx on.
	cmd_resp_para(CI_EOL_RFTX_ONOFF, (u8 *)&ret, sizeof(u8));

	return ret;
}

u8 ci_eol_exit_confirm(void)
{
	return (CI_EOL_STATUS_EXIT == eol_ci_rsp.status) ? 1U : 0U;
}

void ci_eol_timeout(void)
{
	if (CI_EOL_STATUS_INIT == eol_ci_rsp.status) {
		eol_ci_rsp.status = CI_EOL_STATUS_EXIT;
	}
}

u8 ci_eol_test_mode_exit(const u8 *p, u8 len)
{
	u8 ret = CI_ERR_NO;

	if ((p == NULL) || (len != 0u))
		return CI_ERR_PARA_LEN_INVLD;

	eol_app_deinit();
	if(eol_ci_rsp.init_flag) {
		osal_timer_delete(&(eol_ci_rsp.eolcal_rsp_timer));
		eol_ci_rsp.status = CI_EOL_STATUS_EXIT;
		eol_ci_rsp.init_flag = false;
	}

	cmd_resp_para(CI_EOL_EXIT_TESTMOD, &ret, 1u);
	return ret;
}

#endif /* !CFG_EOL_TESTMOD */
