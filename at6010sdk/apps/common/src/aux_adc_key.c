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

#include "aux_adc_key.h"
#include "radar_mgr.h"
#include "hal_aux_key.h"
#include "hal_ls.h"
#include "gpio.h"

#ifdef CFG_OS
#include "osal.h"
#include "sys_task.h"
#endif

#ifdef CFG_APP_LIGHT
#include "app_light.h"
#endif

#ifdef CFG_AUX_ADC_KEY

#ifdef AUX_ADC_KEY1_EN
	static u8 prev_key1 = 0xffU;
#endif

#ifdef AUX_ADC_KEY2_EN
	static u8 prev_key2 = 0xffU;
#endif

#ifdef AUX_ADC_KEY3_EN
	static u8 prev_key3 = 0xffU;
#endif

#ifdef CFG_OS
	static osal_timer_t aux_key_tm;
#endif

#ifdef AUX_ADC_KEY1_EN
static void adc_key_set_dist(u8 val)
{
	u8 dist_map_tbl[] = PARA_AUX_ADC_KEY_DIST_MAP;

	logi("%s, dist=%d\r\n", __func__, val);

	if (val >= AUX_ADC_LEVEL_MAX) {
		val = AUX_ADC_LEVEL_MAX - 1;
	}

	val = dist_map_tbl[val];

	radar_mgr_mdet_user_cfg_set(val);
}
#endif

#ifdef AUX_ADC_KEY2_EN
static void adc_key_set_lot(u8 val)
{
	u32 lot_map_tbl[] = PARA_AUX_ADC_KEY_LOT_MAP;
	u32 time_ms = 0;

	logi("%s, lot=%d\r\n", __func__, val);

	if (val >= AUX_ADC_LEVEL_MAX) {
		val = AUX_ADC_LEVEL_MAX - 1;
	}

	time_ms = lot_map_tbl[val];
#ifdef CFG_APP_LIGHT
	app_light_set_lot(time_ms*1000);
#endif
}
#endif

#ifdef AUX_ADC_KEY3_EN
static void adc_key_set_lux(u8 val)
{
	u32 lux_map_tbl[] = PARA_AUX_ADC_KEY_LUX_MAP;
	u32 lux_val = 0;

	logi("%s, lux=%d\r\n", __func__, val);

	if (val >= AUX_ADC_LEVEL_MAX) {
		val = AUX_ADC_LEVEL_MAX - 1;
	}

	lux_val = lux_map_tbl[val];

	hal_ls_set_threshold(lux_val);
}
#endif

#ifdef CFG_OS
static void aux_adc_key_msg_post(sys_msg_id_t id, u8 val)
{
	osal_msg_t msg;
	msg.id = (u32)id;
	msg.para = val;
	OSAL_ASSERT_ERR(osal_task_send_msg(MOD_SYS, &msg, OSAL_WAIT_FOREVER),
	                "send aux adc key msg failed!\r\n");
}

static void aux_adc_key_timer_cb(void *arg)
{
	UNUSED(arg);

	aux_adc_key_proc();
}

static bool aux_adc_key_msg_handler(const osal_msg_t *msg)
{
	bool ret;

	switch (msg->id) {
#ifdef AUX_ADC_KEY1_EN
	case (u32)MSG_SYS_AUX_ADC_KEY1_GET:
		adc_key_set_dist((u8)msg->para);
		ret = TRUE;
		break;
#endif

#ifdef AUX_ADC_KEY2_EN
	case (u32)MSG_SYS_AUX_ADC_KEY2_GET:
		adc_key_set_lot((u8)msg->para);
		ret = TRUE;
		break;
#endif

#ifdef AUX_ADC_KEY3_EN
	case (u32)MSG_SYS_AUX_ADC_KEY3_GET:
		adc_key_set_lux((u8)msg->para);
		ret = TRUE;
		break;
#endif
	default:
		ret = FALSE;
		break;
	}

	return ret;
}

void app_aux_adc_key_init(void)
{
	sys_task_reg_msg(MSG_SYS_GRP_AUX_ADC_KEY, aux_adc_key_msg_handler);
	OSAL_ASSERT_ERR(osal_timer_create(&aux_key_tm, OSAL_TIMER_MODE_PERIODIC,
	                                  aux_adc_key_timer_cb, NULL, 2000U),
	                "create aux key timer failed!\r\n");
	OSAL_ASSERT_ERR(osal_timer_start(&aux_key_tm), "start aux key timer failed!\r\n");
}

#endif /* CFG_OS */

void aux_adc_key_proc(void)
{
	u8 val = 0;

#ifdef AUX_ADC_KEY1_EN
	/* adc key1 process. */
	val = hal_aux_key_get(AUX_ADC_KEY1_CH);
	if ((val != 0xff) && (val != prev_key1)) {
#ifdef CFG_OS
		aux_adc_key_msg_post(MSG_SYS_AUX_ADC_KEY1_GET, val);
#else
		adc_key_set_dist(val);
#endif /* CFG_OS */
		prev_key1 = val;
	}
#endif

#ifdef AUX_ADC_KEY2_EN
	/* adc key2 process. */
	val = hal_aux_key_get(AUX_ADC_KEY2_CH);
	if ((val != 0xff) && (val != prev_key2)) {
#ifdef CFG_OS
		aux_adc_key_msg_post(MSG_SYS_AUX_ADC_KEY2_GET, val);
#else
		adc_key_set_lot(val);
#endif /* CFG_OS */
		prev_key2 = val;
	}
#endif

#ifdef AUX_ADC_KEY3_EN
	/* adc key3 process. */
	val = hal_aux_key_get(AUX_ADC_KEY3_CH);
	if ((val != 0xffU) && (val != prev_key3)) {
#ifdef CFG_OS
		aux_adc_key_msg_post(MSG_SYS_AUX_ADC_KEY3_GET, val);
#else
		adc_key_set_lux(val);
#endif /* CFG_OS */
		prev_key3 = val;
	}
#endif
}

#endif /* CFG_AUX_ADC_KEY */
