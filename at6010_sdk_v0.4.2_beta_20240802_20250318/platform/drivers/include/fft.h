/**
 *******************************************************************************
 * @file    fft.h
 * @author  Airtouch Software Team
 * @brief   FFT Low-Level Driver
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

#ifndef __FFT_H__
#define __FFT_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_LL_DRIVER
 * @{
 */

/**
 * @defgroup LL_FFT FFT
 * @brief FFT HAL Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"

/* Data Types ----------------------------------------------------------------*/

typedef enum {
	ADC_RDAT_FFT_TYPE,
	ADC_1D_FFT_TYPE,
	ADC_2D_FFT_TYPE,
	USER_1D_FFT_TYPE,/* only user fft support 1d fft */
} enum_fft_type_t;

typedef void (*irq_trg_callback)(u8);

typedef struct user_fft_cfg{
	u16 fft_order:5;
	u16 is_power_out:1;
	u16 nframe;
	u16 shift_value;
	u16 in_frame_size;
	u16 out_frame_size;
	u32 in_buf;
	u32 out_buf;
}user_fft_cfg_t;

typedef struct adc_rdat_fft_cfg{
	u16 num_adc_ch;
	u16 nframe;
	u16 in_frame_size;
	u32 out_buf;
}adc_rdat_fft_cfg_t;

typedef struct adc_1d_fft_cfg{
	u16 num_adc_ch:1;
	u16 fft_order:4;
	u16 sta_rb:10;
	u16 nframe;
	u16 shift_value;
	u16 in_frame_size;
	u16 rfft_upper_rb;
	u32 out_buf_ch0;
	u32 out_buf_ch1;
}adc_1d_fft_cfg_t;

typedef struct adc_2d_fft_cfg{
	u32 is_power_out:1;
	u32 num_adc_ch:1;
	u32 rshift_value_N:7;
	u32 vshift_value_M:7;
	u32 rfft_order_N:16;
	u32 vfft_order_M:16;
	u16 in_frame_size;
	u16 rfft_upper_rb;
	u16 vfft_upper_rb;
	u32 out_buf_ch0;
	u32 out_buf_ch1;
	void (*end_callback)(void);
}adc_2d_fft_cfg_t;

/* Function Declarations -----------------------------------------------------*/

void fft_dev_init(void);
void fft_dev_deinit(void);

void fft_user_win_enable(bool en,u32 fft_type);
void fft_user_win_cfg(u32 waddr,u32 fft_type);

void fft_adc_win_enable(bool en,u32 fft_type);
void fft_adc_win_cfg(u32 waddr,u32 fft_type);

void fft_dc_cut_enable(bool en);

void fft_input_data_type_set(u8 type);

void fft_irq_callback_set(irq_trg_callback trg_cb);

void fft_adc_2d_fft_enable(bool en);
void fft_adc_1d_fft_enable(bool en);

void fft_adc_1d_fft_cfg(adc_1d_fft_cfg_t fft_cfg);
void fft_adc_2d_fft_cfg(adc_2d_fft_cfg_t fft_cfg);

//void fft_user_fft_enable(bool en);
void fft_user_fft_cfg(user_fft_cfg_t fft_cfg);
void fft_user_fft_start(void);

void fft_irq_enable(u8 en,u8 fft_type);

void fft_adc_rdat_cfg(adc_rdat_fft_cfg_t fft_cfg);
void fft_adc_rdat_addr_set(u32 buf);
void fft_adc_rdat_enable(bool en);
void fft_adc_fft_addr_set(u32 buf0,u32 buf1);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__FFT_H__ */
