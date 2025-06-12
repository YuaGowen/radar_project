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
#include "hal_fft.h"
#include "fft.h"
#include "radar.h"
#include "hal_dualtimer.h"
#include "CMSDK_CM0plus.h"
#ifdef OSAL_SELECTION
#include "osal_mem.h"
#endif
#if defined(ULP_PROG_1) || defined(ULP_PROG_2)
#include "ulp_mem.h"
#include "at_malloc.h"
#include "at_malloc_defs.h"
#endif

#ifdef DBG_LOG_DRV_FFT
#include "atlog.h"
#endif
#ifdef DBG_LOG_DRV_FFT
#define dbg_FFT_log        loge
#else
#define dbg_FFT_log(...)
#endif

/* Data Types ------------------------------------------------------------------------------------*/

typedef struct fft_inst {
	u32 buf_start;
	u32 buf_end;
	u32 buf_head;
	u32 data_len;  /**< data length in bytes for one frame */

	void (*half_fill_callback)(void);
	void (*end_fill_callback)(void);
	void (*fill_this_buffer)(u32 buf, u8 mode);
} fft_instance;

struct fft_handle {
	fft_instance fft_inst[4];
};

/* Private Function Declarations -----------------------------------------------------------------*/

static s32 fft_rank_id_get(u16 fft_size);

static void fft_fill_this_buffer(u32 buf, u8 fft_mode);

static void fft_adc_radat_cfg(adc_rdat_para_t *param);

static void fft_adc_range_fft_cfg(adc_range_fft_para_t *param);

static void fft_adc_doppler_fft_cfg(adc_doppler_fft_para_t *param);

static void invoke_fft_callback(u8 irq_type);

static void invoke_cfar_callback(void);

#ifdef OSAL_SELECTION
static struct fft_handle *fft_handle;
#else
static struct fft_handle _fft_handle = {0};
static struct fft_handle *fft_handle = &_fft_handle;
#endif

static s32 fft_rank_id_get(u16 fft_size)
{
	int rank_id = -1;

	switch (fft_size) {
	case 1024:
		rank_id = 10;
		break;

	case 512:
		rank_id = 9;
		break;

	case 256:
		rank_id = 8;
		break;

	case 128:
		rank_id = 7;
		break;

	case 64:
		rank_id = 6;
        break;
    
	case 32:        
        rank_id = 5;
		break;
    
	case 16:
        rank_id = 4;
		break;
    
    case 8:
        rank_id = 3;
		break;
    
    case 4:
        rank_id = 2;
		break;
    
    case 2:
        rank_id = 1;
		break;

	default:
		rank_id = -1;
		break;
	}

	return rank_id;
}

static void fft_fill_this_buffer(u32 buf, u8 fft_mode)
{
	u32 buf1 = 0;

	if (fft_mode == FFT_ADC_RDAT_MODE) {
		fft_adc_rdat_addr_set(buf);
	} else if (fft_mode == FFT_ADC_RANGE_FFT_MODE || fft_mode == FFT_ADC_DOPPLER_FFT_MODE) {
		buf1 = buf + (fft_handle->fft_inst[fft_mode].data_len >> 1);

		fft_adc_fft_addr_set(buf,buf1);
	}
}

static void fft_adc_radat_cfg(adc_rdat_para_t *param)
{
	adc_rdat_para_t   *adc_raw_cfg_para = param;
	adc_rdat_fft_cfg_t adc_rdat_fft_drv_cfg;

	fft_instance *adc_rdat_inst = &fft_handle->fft_inst[FFT_ADC_RDAT_MODE];

	if (adc_raw_cfg_para->enable) {
		adc_rdat_fft_drv_cfg.num_adc_ch = adc_raw_cfg_para->num_adc_ch;
		adc_rdat_fft_drv_cfg.nframe     =  adc_raw_cfg_para->nframe;
		adc_rdat_fft_drv_cfg.in_frame_size  = adc_raw_cfg_para->frame_size;
		adc_rdat_fft_drv_cfg.out_buf  = adc_raw_cfg_para->out_buf;

		adc_rdat_inst->data_len = (u32)(adc_raw_cfg_para->frame_size * adc_raw_cfg_para->nframe << 1);
		adc_rdat_inst->data_len = adc_rdat_inst->data_len + adc_rdat_inst->data_len*adc_raw_cfg_para->num_adc_ch;

		adc_rdat_inst->fill_this_buffer = fft_fill_this_buffer;
		adc_rdat_inst->buf_start = adc_rdat_inst->buf_head = adc_raw_cfg_para->out_buf;

		if (adc_raw_cfg_para->half_int_en) {
			adc_rdat_inst->half_fill_callback = adc_raw_cfg_para->half_callback;
			adc_rdat_inst->buf_end = adc_raw_cfg_para->out_buf + (adc_rdat_inst->data_len << 1);
		} else {
			adc_rdat_inst->buf_end =
				adc_raw_cfg_para->out_buf + adc_rdat_inst->data_len;
		}

		adc_rdat_inst->end_fill_callback = adc_raw_cfg_para->end_callback;

		fft_adc_rdat_cfg(adc_rdat_fft_drv_cfg);
		fft_irq_enable(1,ADC_RDAT_FFT_TYPE);
	}else {
		fft_irq_enable(0,ADC_RDAT_FFT_TYPE);
	}

	fft_adc_rdat_enable(adc_raw_cfg_para->enable);
}

static void fft_adc_range_fft_cfg(adc_range_fft_para_t *param)
{
	adc_range_fft_para_t *adc_range_fft_cfg_para = param;
	adc_1d_fft_cfg_t adc_1d_fft_drv_cfg;
	fft_instance *adc_rfft_inst = &fft_handle->fft_inst[FFT_ADC_RANGE_FFT_MODE];

	if (adc_range_fft_cfg_para->enable) {
		adc_1d_fft_drv_cfg.fft_order    =   (u16)fft_rank_id_get(adc_range_fft_cfg_para->fft_point);
		adc_1d_fft_drv_cfg.sta_rb       =   0;
		adc_1d_fft_drv_cfg.nframe       =    adc_range_fft_cfg_para->nframe;
		adc_1d_fft_drv_cfg.shift_value  =    adc_range_fft_cfg_para->shift_value;
		adc_1d_fft_drv_cfg.in_frame_size  =  adc_range_fft_cfg_para->in_frame_size;
		adc_1d_fft_drv_cfg.rfft_upper_rb  =  adc_range_fft_cfg_para->rfft_upper_rb;
		adc_1d_fft_drv_cfg.num_adc_ch     =  adc_range_fft_cfg_para->num_adc_ch;
		adc_1d_fft_drv_cfg.out_buf_ch0    =  adc_range_fft_cfg_para->out_buf;
		adc_1d_fft_drv_cfg.out_buf_ch1    =  adc_range_fft_cfg_para->out_buf +
						(u32)(adc_range_fft_cfg_para->rfft_upper_rb *adc_range_fft_cfg_para->nframe <<2);

		adc_rfft_inst->data_len = (u32)(adc_range_fft_cfg_para->rfft_upper_rb * adc_range_fft_cfg_para->nframe << 2);
		adc_rfft_inst->data_len = adc_rfft_inst->data_len + adc_rfft_inst->data_len*adc_range_fft_cfg_para->num_adc_ch;

		adc_rfft_inst->fill_this_buffer = fft_fill_this_buffer;
		adc_rfft_inst->buf_start = adc_rfft_inst->buf_head = adc_range_fft_cfg_para->out_buf;

		if (adc_range_fft_cfg_para->half_int_en) {
			adc_rfft_inst->half_fill_callback = adc_range_fft_cfg_para->half_callback;
			adc_rfft_inst->buf_end = adc_range_fft_cfg_para->out_buf + (adc_rfft_inst->data_len << 1);
		} else {
			adc_rfft_inst->buf_end = adc_range_fft_cfg_para->out_buf + adc_rfft_inst->data_len;
		}

		adc_rfft_inst->end_fill_callback = adc_range_fft_cfg_para->end_callback;

		fft_dc_cut_enable(adc_range_fft_cfg_para->dc_cut);
		fft_adc_1d_fft_cfg(adc_1d_fft_drv_cfg);
		fft_irq_enable(1,ADC_1D_FFT_TYPE);
	}else {
		fft_irq_enable(0,ADC_1D_FFT_TYPE);
	}

	fft_adc_1d_fft_enable(adc_range_fft_cfg_para->enable);
}

static u16 cfar_thr_hw_adapt(u8 ref_cells, u8 guard_cells, u16 th)
{
	u32 effective = 0;
	u16 thr_adj = 0;

	u32 msk = ref_cells - guard_cells;

	if (msk == 1) {
		effective = 1;
	} else if (msk == 2) {
		effective = 2;
	} else if (msk <= 5) {
		effective = 4;
	} else if (msk <= 8) {
		effective = 8;
	}

	thr_adj = (u32)((float)th * effective / msk + 0.5);

	return thr_adj;
}

static void fft_ca_cfar_cfg(ca_cfar_para_t *cfar_cfg)
{
	cfar_cfg->rcfar_cfg.threshold = (u8)cfar_thr_hw_adapt(cfar_cfg->rcfar_cfg.ref_cells,
					cfar_cfg->rcfar_cfg.guard_cells,cfar_cfg->rcfar_cfg.threshold);
	cfar_cfg->rcfar_cfg.threshold_vb0 = (u8)cfar_thr_hw_adapt(cfar_cfg->rcfar_cfg.ref_cells,
					cfar_cfg->rcfar_cfg.guard_cells,cfar_cfg->rcfar_cfg.threshold_vb0);
	cfar_cfg->vcfar_cfg.threshold = (u8)cfar_thr_hw_adapt(cfar_cfg->vcfar_cfg.ref_cells,
					cfar_cfg->vcfar_cfg.guard_cells,cfar_cfg->vcfar_cfg.threshold);

	cfar_vel_dim_cfg(cfar_cfg->vcfar_cfg);
	cfar_range_dim_cfg(cfar_cfg->rcfar_cfg);
	cfar_abs_threshold_cfg(cfar_cfg->abs_thresh);

	cfar_det_bmap_addr_cfg(cfar_cfg->det_ret);
}

static void fft_adc_doppler_fft_cfg(adc_doppler_fft_para_t *param)
{
	adc_doppler_fft_para_t * adc_doppler_fft_cfg_para = param;
	adc_2d_fft_cfg_t adc_2d_fft_drv_cfg;
	fft_instance *adc_vfft_inst = &fft_handle->fft_inst[FFT_ADC_DOPPLER_FFT_MODE];

	if (adc_doppler_fft_cfg_para->enable) {
		adc_2d_fft_drv_cfg.rfft_order_N    = (u32)fft_rank_id_get(adc_doppler_fft_cfg_para->rfft_point_N);
		adc_2d_fft_drv_cfg.vfft_order_M    = (u32)fft_rank_id_get(adc_doppler_fft_cfg_para->vfft_point_M);
		adc_2d_fft_drv_cfg.is_power_out    = adc_doppler_fft_cfg_para->is_power_out;
		adc_2d_fft_drv_cfg.rshift_value_N  = adc_doppler_fft_cfg_para->rshift_value_N;
		adc_2d_fft_drv_cfg.vshift_value_M  = adc_doppler_fft_cfg_para->vshift_value_M;
		adc_2d_fft_drv_cfg.in_frame_size  =  adc_doppler_fft_cfg_para->in_frame_size;
		adc_2d_fft_drv_cfg.num_adc_ch     =  adc_doppler_fft_cfg_para->num_adc_ch;

		adc_2d_fft_drv_cfg.rfft_upper_rb    = adc_doppler_fft_cfg_para->rfft_upper_rb;
		adc_2d_fft_drv_cfg.vfft_upper_rb    = adc_doppler_fft_cfg_para->vfft_upper_vb;

		adc_2d_fft_drv_cfg.out_buf_ch0    =  adc_doppler_fft_cfg_para->out_buf;
		adc_2d_fft_drv_cfg.out_buf_ch1    =  adc_doppler_fft_cfg_para->out_buf +
						(u32)(adc_doppler_fft_cfg_para->rfft_upper_rb *adc_doppler_fft_cfg_para->vfft_upper_vb <<2);

		adc_vfft_inst->data_len = (u32)(adc_doppler_fft_cfg_para->rfft_upper_rb * adc_doppler_fft_cfg_para->vfft_upper_vb << 2);
		adc_vfft_inst->data_len = adc_vfft_inst->data_len + adc_vfft_inst->data_len*adc_doppler_fft_cfg_para->num_adc_ch;

		adc_vfft_inst->fill_this_buffer = fft_fill_this_buffer;
		adc_vfft_inst->buf_start = adc_vfft_inst->buf_head = adc_doppler_fft_cfg_para->out_buf;

		if (adc_doppler_fft_cfg_para->half_int_en) {
			adc_vfft_inst->half_fill_callback = adc_doppler_fft_cfg_para->half_callback;
			adc_vfft_inst->buf_end = adc_doppler_fft_cfg_para->out_buf + (adc_vfft_inst->data_len << 1);
		} else {
			adc_vfft_inst->buf_end = adc_doppler_fft_cfg_para->out_buf + adc_vfft_inst->data_len;
		}
		adc_vfft_inst->end_fill_callback = adc_doppler_fft_cfg_para->end_callback;

		fft_dc_cut_enable(adc_doppler_fft_cfg_para->dc_cut);
		fft_adc_2d_fft_cfg(adc_2d_fft_drv_cfg);

		if (adc_doppler_fft_cfg_para->cfar_en == 0) {
			cfar_enable(0);
			cfar_irq_enable(0);
			fft_irq_enable(1,ADC_2D_FFT_TYPE);
		} else {
			fft_irq_enable(0,ADC_2D_FFT_TYPE);
			cfar_irq_callback_set(invoke_cfar_callback);
			cfar_enable(1);
			cfar_irq_enable(1);
		}
	}

	fft_adc_2d_fft_enable(adc_doppler_fft_cfg_para->enable);
}

/* Global Functions ------------------------------------------------------------------------------*/

void hal_fft_init()
{
#if defined(ULP_PROG_1)
	fft_handle = (struct fft_handle*)at_malloc_pool(AT_MEM_POOL_0_ID, sizeof(struct fft_handle));
	ulp_mem_set_addr(ULP_MEM_AT_ID_2,(u32)fft_handle);
#elif defined(OSAL_SELECTION)
	fft_handle = (struct fft_handle*)osal_malloc(sizeof(struct fft_handle));
#else
#endif

	fft_dev_init();
	fft_irq_callback_set(invoke_fft_callback);
}

#if defined(ULP_PROG_2)
void hal_fft_ulp_init()
{
	fft_handle = (struct fft_handle*) ulp_mem_get_addr(ULP_MEM_AT_ID_2);

	fft_irq_callback_set(invoke_fft_callback);
	NVIC_EnableIRQ(RADAR_IRQn);
}
#endif

void hal_fft_notify_set(u8 id,void *half_func,void *end_func)
{
	fft_handle->fft_inst[id].fill_this_buffer = fft_fill_this_buffer;

	fft_handle->fft_inst[id].end_fill_callback = (void (*)(void))end_func;
	fft_handle->fft_inst[id].half_fill_callback = (void (*)(void))half_func;
}

void hal_fft_deinit()
{
	fft_dev_deinit();
}

void hal_fft_set_config(fft_param_idx_t param_idx, void *param)
{
	fft_win_para_t *win_cfg = NULL;
	user_fft_para_t *usr_fft_cfg_param = NULL;
	user_fft_cfg_t user_fft_drv_cfg;

	switch (param_idx) {
	case FFT_IDX_CONFIG_ADC_RDAT:
		fft_adc_radat_cfg((adc_rdat_para_t*)param);
		break;

	case FFT_IDX_CONFIG_USER_WIN:
		win_cfg = (fft_win_para_t *)param;

		fft_user_win_enable(win_cfg->enable,win_cfg->fft_mode);
		if (win_cfg->enable){
			fft_user_win_cfg(win_cfg->waddr,win_cfg->fft_mode);
		}
		break;

	case FFT_IDX_CONFIG_USER_FFT:
		usr_fft_cfg_param = (user_fft_para_t *)param;

		user_fft_drv_cfg.fft_order = (u16)fft_rank_id_get(usr_fft_cfg_param->fft_point);
		user_fft_drv_cfg.is_power_out = usr_fft_cfg_param->is_power_out;
		user_fft_drv_cfg.nframe = usr_fft_cfg_param->nframe;
		user_fft_drv_cfg.shift_value = usr_fft_cfg_param->shift_value;
		user_fft_drv_cfg.in_frame_size = usr_fft_cfg_param->in_frame_size;
		user_fft_drv_cfg.out_frame_size = usr_fft_cfg_param->out_frame_size;
		user_fft_drv_cfg.in_buf         = usr_fft_cfg_param->in_buf;
		user_fft_drv_cfg.out_buf        = usr_fft_cfg_param->out_buf;

		fft_handle->fft_inst[FFT_USER_FFT_MODE].end_fill_callback = usr_fft_cfg_param->end_callback;
		fft_input_data_type_set(usr_fft_cfg_param->data_type);
		fft_user_fft_cfg(user_fft_drv_cfg);
		fft_irq_enable(1,USER_1D_FFT_TYPE);
		break;

	case FFT_IDX_CONFIG_ADC_WIN:
		win_cfg = (fft_win_para_t *)param;

		fft_adc_win_enable(win_cfg->enable,win_cfg->fft_mode);
		if (win_cfg->enable) {
			fft_adc_win_cfg(win_cfg->waddr,win_cfg->fft_mode);
		}
		break;

	case FFT_IDX_CONFIG_ADC_RANGE_FFT:
		fft_adc_range_fft_cfg((adc_range_fft_para_t *)param);
		break;

	case FFT_IDX_CONFIG_ADC_DOPPLER_FFT:
		fft_adc_doppler_fft_cfg((adc_doppler_fft_para_t *)param);
		break;

	case FFT_IDX_CONFIG_DOPPLER_CFAR:
		fft_ca_cfar_cfg((ca_cfar_para_t *)param);
		break;
	}
}

void fft_range_fft_suspend(void)
{
	fft_adc_1d_fft_enable(0);
}

void fft_range_fft_resume(void)
{
	fft_adc_1d_fft_enable(1);
}

void fft_doppler_fft_suspend(void)
{
	fft_adc_2d_fft_enable(0);
}

void fft_doppler_fft_resume(void)
{
	fft_adc_2d_fft_enable(1);
}

void fft_process_trigger(fft_work_mode mode)
{
	if (mode == FFT_USER_FFT_MODE) {
		fft_user_fft_start();
	} else {
		/* other mode will auto start by fmcw control.*/
	}
}

static void invoke_fft_callback(u8 irq_type)
{
	if (irq_type > FFT_ADC_DOPPLER_FFT_MODE) {
		fft_handle->fft_inst[irq_type].end_fill_callback();
	} else {
		fft_handle->fft_inst[irq_type].buf_head += fft_handle->fft_inst[irq_type].data_len;

		if (fft_handle->fft_inst[irq_type].buf_head == fft_handle->fft_inst[irq_type].buf_end) {
			fft_handle->fft_inst[irq_type].end_fill_callback();
			fft_handle->fft_inst[irq_type].buf_head = fft_handle->fft_inst[irq_type].buf_start;
		} else {
			fft_handle->fft_inst[irq_type].half_fill_callback();
		}

		fft_handle->fft_inst[irq_type].fill_this_buffer(fft_handle->fft_inst[irq_type].buf_head,irq_type);
	}
}

static void invoke_cfar_callback(void)
{
	u8 inst_id = FFT_ADC_DOPPLER_FFT_MODE;

	fft_handle->fft_inst[inst_id].buf_head += fft_handle->fft_inst[inst_id].data_len;

	if (fft_handle->fft_inst[inst_id].buf_head == fft_handle->fft_inst[inst_id].buf_end) {
		fft_handle->fft_inst[inst_id].end_fill_callback();
		fft_handle->fft_inst[inst_id].buf_head = fft_handle->fft_inst[inst_id].buf_start;
	} else {
		fft_handle->fft_inst[inst_id].half_fill_callback();
	}

	fft_handle->fft_inst[inst_id].fill_this_buffer(fft_handle->fft_inst[inst_id].buf_head,inst_id);
}

