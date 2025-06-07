/**
 *******************************************************************************
 * @file    hal_ls.c
 * @author  Airtouch Software Team
 * @brief   Light Sensor HAL Driver
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

#include "hal_ls.h"
#include "gpio.h"
#include "aux_adc.h"
#include "atlog.h"

#ifdef CFG_LS

#define AUX_ADC_LS_CH GPIO_AON_1
//#define DBG_LS_LOG

typedef struct _ls_ctx {
	u16 threshold;
	u8 darkness : 1;     /* 1: darkness mode, 0: daylight mode. */
#ifdef LS_CHECK_DITHER
	u8 adc_dither_flag: 1;
#endif
} ls_ctx_t;

static ls_ctx_t ls_ctx = {
	.threshold = 0u,
	.darkness = 0u,
#ifdef LS_CHECK_DITHER
	.adc_dither_flag = 0u,
#endif
};

#if (PARA_LS_SET_PIN_EN == 1)
__STATIC_INLINE void ls_set_pin(u8 flag)
{
	gpio_set_value(PARA_LS_SET_PIN_NUM, (flag == 0u) ? 0u : 1u);
}
#endif

#ifdef LS_CHECK_DITHER
static void ls_dither_check(u16 val)
{
	static u16 bak_val = 0;
	u16 adc_diff = 0;

	if (bak_val == 0) {
		bak_val = val;
		return;
	}

	adc_diff = (bak_val > val) ? (bak_val - val) : (val - bak_val);
	if (adc_diff > PARA_LS_JT_TH) {
		ls_ctx.adc_dither_flag++;
	}

	bak_val = val;
}
#endif

void hal_ls_init(void)
{
	/* aux channel init for light sensor. */
	gpio_cfg_t gpio_cfg;

	gpio_cfg.mode = GPIO_MODE_ANALOG;
	gpio_cfg.pull = GPIO_PULL_NONE;

	(void)gpio_init(AUX_ADC_LS_CH, &gpio_cfg);
	adc_aux_init(ADC_AUX_DIR_EXTERNAL);

	ls_ctx.threshold = PARA_LS_THRESHOLD;

#if (PARA_LS_SET_PIN_EN == 1)
	gpio_set_pinmux(PARA_LS_SET_PIN_NUM, 1u);
	gpio_direction_output(PARA_LS_SET_PIN_NUM, 0u);
#endif
}

void hal_ls_set_threshold(u16 val)
{
	ls_ctx.threshold = val;
}

u16 hal_ls_get_threshold(void)
{
	return ls_ctx.threshold;
}

/**
 * hal_ls_get_status() - Get light sensor status.
 *
 * @return: 1 - is dark mode.
 *            0 - is daylight mode.
 *
 */
u8 hal_ls_get_status(void)
{
	return ls_ctx.darkness;
}

#ifdef LS_CHECK_DITHER
u8 hal_ls_get_dither_flag(void)
{
	return ls_ctx.adc_dither_flag;
}

void hal_ls_clr_dither_flag(void)
{
	ls_ctx.adc_dither_flag = 0;
}
#endif

__STATIC_INLINE u8 is_darkness(u16 val, u16 th)
{
	u8 dark;
	u16 th_hysteresis;

	if (th == 0u) {
		dark = 1u;
	} else {
#if (PARA_LS_VOL_CON_INVERSE == 1)
		th_hysteresis = th + PARA_LS_TH_HYSTERESIS;
		dark = (val > th_hysteresis) ? 0u : ((val < th) ? 1u : 0xFFu);
#else
		th_hysteresis = (th > PARA_LS_TH_HYSTERESIS) ? (th - PARA_LS_TH_HYSTERESIS) : 0u;
		dark = (val < th_hysteresis) ? 0u : ((val > th) ? 1u : 0xFFu);
#endif
	}
	return dark;
}

/**
 * hal_ls_detect() - Light sensor detect.
 *
 * @return: 1 - turn on radar detect
 *            0 - turn off radar detect.
 *
 */
u8 hal_ls_detect(void)
{
	u16 adc_vol;
	u8 flag_dark;

	adc_vol = (u16)adc_get_aux_voltage(AUX_ADC_LS_CH);

#ifdef DBG_LS_LOG
	logi("%s, adc_vol=%d, threshold=%d\r\n", __func__, adc_vol, ls_ctx.threshold);
#endif

#ifdef LS_CHECK_DITHER
	ls_dither_check(adc_vol);
#endif
	flag_dark = is_darkness(adc_vol, ls_ctx.threshold);

	if (0xFFu != flag_dark) {
#if (PARA_LS_SET_PIN_EN == 1)
		ls_set_pin(flag_dark);
#endif
		ls_ctx.darkness = flag_dark;
	}

	return flag_dark;
}
#endif
