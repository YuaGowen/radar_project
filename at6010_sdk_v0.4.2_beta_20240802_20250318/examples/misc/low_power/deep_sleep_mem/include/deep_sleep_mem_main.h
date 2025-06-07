/**
 *******************************************************************************
 * @file    deep_sleep_mem_main.h
 * @author  Airtouch Software Team
 * @brief   Firmware Software version
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

#ifndef __DEEP_SLEEP_MEM_MAIN_H__
#define __DEEP_SLEEP_MEM_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cfg_paras.h"
#include "cfg_radar_hwa.h"
#include "frame_header.h"

/**
 * @defgroup DEEP_SLEEP_FRAMES_SETTINGS DEEP SLEEP FRAMES SETTINGS
 * @{
 */
#define FAKE_DS_RADAR_WORK_CNT      8 /**<  4 or 8: number of radar init/de-init in 1 second */
/**
 * @}
 */

/**
 * @defgroup DEMO_TYPE_DEFINE DEMO TYPE DEFINE And Selection
 * @{
 */
#define DEMO_TYPE_RAWDATA           1
#define DEMO_TYPE_RANGEFFT          2
#define DEMO_TYPE_USING             DEMO_TYPE_RAWDATA /**<  DEMO_TYPE_RAWDATA or DEMO_TYPE_RANGEFFT */
/**
 * @}
 */

#define NUM_ANT                     FFT_NUM_ADC_CH /* TODO: Just used in RawData */

#define FFT_BUF_ATTRIBUTES          __attribute__ ((section(".radar_mem"),  __aligned__(32)))
#define FFT_WIN_ATTRIBUTES          __attribute__ ((section(".fft_win_mem"),__aligned__(32)))

                                       
typedef enum {
	DEV_FIFO_WP_NA   = 0,
	DEV_FIFO_WP_HALF = 1,
	DEV_FIFO_WP_END  = 2,
} enum_fifo_wp_status;


#ifdef __cplusplus
}
#endif
#endif /* !__DEEP_SLEEP_MEM_MAIN_H__ */
