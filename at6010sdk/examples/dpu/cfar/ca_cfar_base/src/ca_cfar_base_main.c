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

#include <stdlib.h>

#include "math.h"
#include "stdio.h"
#include "pmu.h"
#include "string.h"

#include "CMSDK_CM0plus.h"
#include "output_config.h"
#include "sys_ctrl.h"

#include "at_types.h"
#include "bytestream.h"
#include "frame_header.h"

#include "radar.h"
#include "radar_frame.h"
#include "radar_adc.h"
#include "hal_fft.h"

#include "clock.h"
#include "hal_uart.h"
#include "hal_dualtimer.h"
#include "clk_cfg.h"
#include "cpu.h"
#include "cfg_radar_hwa.h"
#include "common.h"

//#define FRAME_HEADER_LEN 2
#define FRAME_HEADER_LEN ((sizeof(ATCommV2_RadarCommonDef)+sizeof(ATCommV2_RadarSystem)+sizeof(ATCommV2_SubframeConfig)*8) >> 2)


#define OUT_FRAME_HEADER_SIZE   (FRAME_HEADER_LEN*4)
#define OUT_DAT_SIZE            (FFT_OUT_VALID_RB*FFT_OUT_VALID_VB *4*FFT_NUM_ADC_CH)


#define RADAR_BUF_ATTRIBUTES  __attribute__ ((section(".radar_mem"),__aligned__(32)))
#define FFT_WIN_ATTRIBUTES  __attribute__ ((section(".fft_win_mem"),__aligned__(32)))

enum enum_fifo_wp_status {
	DEV_FIFO_WP_NA = 0,
	DEV_FIFO_WP_HALF = 1,
	DEV_FIFO_WP_END = 2,
};

#ifdef WIN_ENABLE
#include "fft_win.h"
static u16 rangefft_win[] FFT_WIN_ATTRIBUTES = {
    #if (FFT_RSIZE == 8)
    u16_blackman8(),
    #elif (FFT_RSIZE == 16)
    u16_blackman16(),
    #elif (FFT_RSIZE == 32)
    u16_blackman32(),
    #elif (FFT_RSIZE == 64)
    u16_blackman64(),
    #elif (FFT_RSIZE == 128)
    u16_blackman128(),
    #elif (FFT_RSIZE == 256)
    u16_blackman256(),
    #elif (FFT_RSIZE == 512)
    u16_blackman512(),
    #elif (FFT_RSIZE == 1024)
    u16_blackman1024(),
    #else
    #error "窗序列长度错误，请注意检查！"
    #endif
};

static u16 dopplerfft_win[] FFT_WIN_ATTRIBUTES = {
    #if (FFT_VSIZE == 8)
    u16_blackman8(),
    #elif (FFT_VSIZE == 16)
    u16_blackman16(),
    #elif (FFT_VSIZE == 32)
    u16_blackman32(),
    #elif (FFT_VSIZE == 64)
    u16_blackman64(),
    #elif (FFT_VSIZE == 128)
    u16_blackman128(),
    #elif (FFT_VSIZE == 256)
    u16_blackman256(),
    #elif (FFT_VSIZE == 512)
    u16_blackman512(),
    #elif (FFT_VSIZE == 1024)
    u16_blackman1024(),
    #else
    #error "窗序列长度错误，请注意检查！"
    #endif
};
#endif

static u32 vfft_out_buf[FRAME_HEADER_LEN + OUT_DAT_SIZE/4*2]  RADAR_BUF_ATTRIBUTES = {0};

static u32 cfar_bitmap_r[CFAR_RANGE_BITMAP_SIZE] RADAR_BUF_ATTRIBUTES = {0};
static u32 cfar_bitmap_v[CFAR_VEL_BITMAP_SIZE] RADAR_BUF_ATTRIBUTES = {0};
static u32 cfar_bitmap_mix[CFAR_MIX_BITMAP_SIZE] RADAR_BUF_ATTRIBUTES = {0};

static volatile u32 fifo_state = 0;
static radar_frame_hdr_t *frame_header =  (radar_frame_hdr_t*)vfft_out_buf;
static u32 *doppler_fft_buffer;

extern void radar_fmcw_cfg(void);

static void adc_vfft_end_isr(void)
{
	fifo_state = DEV_FIFO_WP_END;
	frame_header->radar_common_cfg.rxSubFrameId++;
}

static void adc_vfft_half_isr(void)
{
	fifo_state = DEV_FIFO_WP_HALF;
	frame_header->radar_common_cfg.rxSubFrameId++;
}

static u32 get_current_data_addr(void)
{
	u32 d_update = 0;

	critical_section_start();

	if (fifo_state == DEV_FIFO_WP_HALF) {
		d_update = (u32)doppler_fft_buffer;
	} else if (fifo_state == DEV_FIFO_WP_END) {
		d_update = (u32)doppler_fft_buffer + OUT_DAT_SIZE;
	}
	fifo_state = DEV_FIFO_WP_NA;

	critical_section_end();

	return d_update;
}

static void radar_clock_init(void)
{
	sys_clk_disable_radar();
	sys_clk_enable_radar();
	sys_clk_enable_dma();
}
#ifdef WIN_ENABLE

static void adc_fft_win_cfg(void)
{
	fft_win_para_t win_cfg = {
		.enable = 1,
		.waddr    = (u32)rangefft_win,
		.fft_mode = FFT_ADC_RANGE_FFT_MODE,
	};
	hal_fft_set_config(FFT_IDX_CONFIG_ADC_WIN,(void*)&win_cfg); 	/* range fft win cfg*/

	win_cfg.waddr = (u32)dopplerfft_win;
	win_cfg.fft_mode = FFT_ADC_DOPPLER_FFT_MODE;

	hal_fft_set_config(FFT_IDX_CONFIG_ADC_WIN,(void*)&win_cfg); 	/* dopppler fft win cfg*/

}
#endif

int main(void)
{
	uart_cfg_t uart_cfg;

	adc_doppler_fft_para_t doppler_fft_cfg = {0};
	ca_cfar_para_t         ca_cfar_cfg = {0};

	u32 fft_dat = 0;
	radar_clock_init();

	doppler_fft_buffer = &vfft_out_buf[FRAME_HEADER_LEN];
#if SYSTEM_CLK_80M
	clk_mcu_set(CLK_SRC_PLL, 80000000U);
#else
	clk_mcu_set(CLK_SRC_XTAL, 40000000U);
#endif
	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	hal_dualtimer_init();

	radar_clock_init();
	write_data_frame_header((u8*)vfft_out_buf);

	hal_fft_init();
	radar_fmcw_cfg();
#ifdef WIN_ENABLE
	adc_fft_win_cfg();
#endif
	/*doppler fft config */
#ifdef FFT_DC_CUT_ENABLE
	doppler_fft_cfg.dc_cut = 1;
#endif
	doppler_fft_cfg.enable = 1;
	doppler_fft_cfg.is_power_out = FFT_OUT_IS_POWER;
#ifdef CA_CFAR_ENABLE
	doppler_fft_cfg.cfar_en = 1;
#endif

#if FFT_NUM_ADC_CH == 2
	doppler_fft_cfg.num_adc_ch = FFT_DUAL_ADC_CH;
#else
	doppler_fft_cfg.num_adc_ch = FFT_MONO_ADC_CH;
#endif
	doppler_fft_cfg.rshift_value_N = 2;
	doppler_fft_cfg.vshift_value_M = 3;

	doppler_fft_cfg.rfft_point_N = FFT_RSIZE;
	doppler_fft_cfg.vfft_point_M = FFT_VSIZE;

	doppler_fft_cfg.in_frame_size = FFT_INPUT_SIZE;
	doppler_fft_cfg.rfft_upper_rb = FFT_OUT_VALID_RB;
	doppler_fft_cfg.vfft_upper_vb = FFT_OUT_VALID_VB;

	doppler_fft_cfg.out_buf =  (u32)doppler_fft_buffer;

	doppler_fft_cfg.half_int_en = 1;
	doppler_fft_cfg.half_callback = adc_vfft_half_isr;
	doppler_fft_cfg.end_callback = adc_vfft_end_isr;


	hal_fft_set_config(FFT_IDX_CONFIG_ADC_DOPPLER_FFT,(void*)&doppler_fft_cfg);

	/* ca_cfar config */
	if(doppler_fft_cfg.cfar_en) {
		ca_cfar_cfg.det_ret.cfar_mix_bmap_addr = (u32)cfar_bitmap_mix;
		ca_cfar_cfg.det_ret.cfar_vel_bmap_addr  = (u32)cfar_bitmap_v;
		ca_cfar_cfg.det_ret.cfar_range_bmap_addr  = (u32)cfar_bitmap_r;

		ca_cfar_cfg.abs_thresh = CFAR_THR_ABS;
		ca_cfar_cfg.rcfar_cfg.is_cyclic   = CFAR_RGE_BOUNDARY_PADDING;
		ca_cfar_cfg.rcfar_cfg.guard_cells = CFAR_RGE_GUARD_CELL;
		ca_cfar_cfg.rcfar_cfg.ref_cells   = CFAR_RGE_REF_CELL;
		ca_cfar_cfg.rcfar_cfg.threshold   = CFAR_RGE_THR_FACTOR;
		ca_cfar_cfg.rcfar_cfg.threshold_vb0 = CFAR_RGE_THR_VB0;

		ca_cfar_cfg.vcfar_cfg.is_cyclic   = CFAR_VEL_BOUNDARY_PADDING;
		ca_cfar_cfg.vcfar_cfg.guard_cells = CFAR_VEL_GUARD_CELL;
		ca_cfar_cfg.vcfar_cfg.ref_cells   = CFAR_VEL_REF_CELL;
		ca_cfar_cfg.vcfar_cfg.threshold   = CFAR_VEL_THR_FACTOR;

		hal_fft_set_config(FFT_IDX_CONFIG_DOPPLER_CFAR,(void*)&ca_cfar_cfg);
	}

	/* output config */
	output_dma_config();

	radar_frame_trigger_mode_set(TRIGGER_MODE_INTERNAL);

	radar_frame_start();

	while(1) {
		while(fifo_state == DEV_FIFO_WP_NA) {
			__NOP();
		}

		fft_dat = get_current_data_addr();
		if (fft_dat != 0) {
			output_port_send_data((u32)frame_header,OUT_FRAME_HEADER_SIZE);
			output_port_send_data((u32)fft_dat,OUT_DAT_SIZE);
            #ifdef CA_CFAR_ENABLE
            output_port_send_data((u32)cfar_bitmap_mix,CFAR_MIX_BITMAP_SIZE*4);
            #endif
            
            radar_frame_start();
		}
	}
}

