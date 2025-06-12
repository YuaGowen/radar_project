/**
 *******************************************************************************
 * @file    hal_fft.h
 * @author  Airtouch Software Team
 * @brief   FFT HAL Driver
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

#ifndef __HAL_FFT_H__
#define __HAL_FFT_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_FFT FFT
 * @brief FFT HAL Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "fft.h"
#include "at_types.h"
#include "cfar.h"
/* Data Types ----------------------------------------------------------------*/

/**
 * @brief FFT ADC Sample Rate
 */
typedef enum fft_adc_sr {
	FFT_ADC_SAMPLE_RATE_50000HZ,
	FFT_ADC_SAMPLE_RATE_100000HZ,
	FFT_ADC_SAMPLE_RATE_500000HZ,
	FFT_ADC_SAMPLE_RATE_1000000HZ,
} fft_adc_sr;

/**
 * @brief FFT work mode
 */
typedef enum fft_work_mode {
	FFT_ADC_RDAT_MODE,        /**< FMCW ADC Raw Data Mode                     */
	FFT_ADC_RANGE_FFT_MODE,   /**< 1D FFT Mode                                */
	FFT_ADC_DOPPLER_FFT_MODE, /**< 2D FFT Mode                                */
	FFT_USER_FFT_MODE,        /**< User FFT Mode                              */
} fft_work_mode;

/**
 * @brief FFT Data type, complex or real.
 */
typedef enum {
	COMPLEX_DATA,
	REAL_DATA,
} data_type_t;

/**
 * @brief
 *
 */
typedef enum {
	FFT_MONO_ADC_CH,
	FFT_DUAL_ADC_CH,
} fft_adc_ch_t;

/**
 * @brief
 *
 */
typedef enum {
	FFT_IDX_CONFIG_ADC_RDAT,        /**< ADC Raw data                         */
	FFT_IDX_CONFIG_USER_WIN,        /**< only support 1d fft win              */
	FFT_IDX_CONFIG_USER_FFT,        /**< only support 1d fft                  */
	FFT_IDX_CONFIG_ADC_WIN,         /**< 1d/2d adc fft window coeff           */
	FFT_IDX_CONFIG_ADC_RANGE_FFT,   /**< range adc fft configure              */
	FFT_IDX_CONFIG_ADC_DOPPLER_FFT, /**< doppler adc fft configure            */
	FFT_IDX_CONFIG_DOPPLER_CFAR,    /**< doppler fft cfar configure           */
} fft_param_idx_t;

/**
 * @brief
 *
 */
typedef struct fft_win_para {
	u8  enable;
	u8  fft_mode; /**< @ref enum_fft_type_t                                   */
	u32 waddr;    /**< address of window data in RAM                          */
} fft_win_para_t;

typedef struct adc_rdat_para{
	u32 enable:1;
	u32 num_adc_ch:1;
	u32 half_int_en:1;
	u32 frame_size:13;
	u32 nframe:16;
	u32 out_buf;
	void (*half_callback)(void);
	void (*end_callback)(void);
}adc_rdat_para_t;

typedef struct adc_range_fft_para{
	u32 enable:1;
	u32 dc_cut:1;
	u32 num_adc_ch:1;
	u32 half_int_en:1;
	u32 fft_point:12;
	u32 shift_value:4;
	u32 in_frame_size:12;
	u32 nframe:16;
	u32 rfft_upper_rb:16;
	u32 out_buf;
	void (*half_callback)(void);
	void (*end_callback)(void);
}adc_range_fft_para_t;


typedef struct ca_cfar_para{
	cfar_range_para_cfg_t rcfar_cfg;
	cfar_vel_para_cfg_t   vcfar_cfg;
	u32 abs_thresh;
	cfar_det_bmap_addr_cfg_t det_ret;
}ca_cfar_para_t;


/* NXM FFT 2D FFT*/
typedef struct adc_doppler_fft_para{
	u32 enable:1;
	u32 dc_cut:1;
	u32 is_power_out:1;
	u32 num_adc_ch:1;
	u32 cfar_en:1;
	u32 half_int_en:3;
	u32 rshift_value_N:12;
	u32 vshift_value_M:12;
	u32 rfft_point_N:16;
	u32 vfft_point_M:16;
	u16 in_frame_size;
	u16 rfft_upper_rb;
	u16 vfft_upper_vb;
	u32 out_buf;
	void (*half_callback)(void);
	void (*end_callback)(void);
}adc_doppler_fft_para_t;

typedef struct user_fft_para{
	u32 data_type:1;
	u32 is_power_out:1;
	u32 shift_value:14;
	u32 fft_point:16;
	u32 nframe:16;
	u16 in_frame_size;
	u16 out_frame_size;
	u32 in_buf;
	u32 out_buf;
	void (*end_callback)(void);
} user_fft_para_t;

/* Function Declarations -----------------------------------------------------*/

void hal_fft_init(void);
void hal_fft_deinit(void);

void hal_fft_set_config(fft_param_idx_t param_idx, void *param);
void fft_process_trigger(fft_work_mode mode);

void fft_range_fft_suspend(void);
void fft_range_fft_resume(void);
void fft_doppler_fft_suspend(void);
void fft_doppler_fft_resume(void);
void hal_fft_ulp_init(void);
void hal_fft_notify_set(u8 id,void *half_func,void *end_func);


/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_FFT_H__ */
