/**
 * Copyright (c) 2018 Airtouching Intelligence Technology.
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hal_aux_key.h"
#include "aux_adc.h"
#include "gpio.h"
#include "at_common.h"

#ifdef CFG_AUX_ADC_KEY

#define MID(x, y, z)      (MAX((x), (y)) > (z) ? MAX(MIN((x), (y)), (z)) : MIN(MAX((x), (y)), (z)))

static const s16 m_key_map_val[] = PARA_AUX_ADC_KEY_VOL_MAP;

static const u8 key_num = sizeof(m_key_map_val) / sizeof(m_key_map_val[0]);

static u8 check_key_range(s16 adc_vol, u8 cur_key, u8 prev_key)
{
	s16 adc_half_step = AUX_ADC_KEY_STEP/2;

	/* simple debounce processing. */
	if (cur_key < prev_key) { /* falling */
		if (adc_vol > (m_key_map_val[prev_key] - adc_half_step*6/5))
			cur_key = prev_key;
	} else if (cur_key > prev_key) { /* rising */
		if (adc_vol < (m_key_map_val[prev_key] + adc_half_step*6/5))
			cur_key = prev_key;
	}

	/* check range and save prev_idx. */
	if (cur_key >= key_num) {
		cur_key = key_num - 1;
	}

	return cur_key;
}

static u8 get_key(u8 ch)
{
#ifdef AUX_ADC_KEY1_EN
	static u8 prev_idx1 = 0;
#endif

#ifdef AUX_ADC_KEY2_EN
	static u8 prev_idx2 = 0;
#endif

#ifdef AUX_ADC_KEY3_EN
	static u8 prev_idx3 = 0;
#endif

	s16 adc_half_step = AUX_ADC_KEY_STEP/2;
	s16 adc_vol = 0;
	s16 adc_buf[3] = { 0 };
	u8 i = 0;

	for (i = 0; i < 3; i++) {
		adc_buf[i] = (s16)adc_get_aux_voltage(ch);
	}

	adc_vol = MID(adc_buf[0], adc_buf[1], adc_buf[2]);

	for (i = 0; i < key_num; i++) {
		if ((adc_vol > (m_key_map_val[i] - adc_half_step))
			&& (adc_vol < (m_key_map_val[i] + adc_half_step))) {
			break;
		}
	}

	switch (ch) {
#ifdef AUX_ADC_KEY1_EN
	case AUX_ADC_KEY1_CH:
		i = check_key_range(adc_vol, i, prev_idx1);
		prev_idx1 = i;
		break;
#endif

#ifdef AUX_ADC_KEY2_EN
	case AUX_ADC_KEY2_CH:
		i = check_key_range(adc_vol, i, prev_idx2);
		prev_idx2 = i;
		break;
#endif

#ifdef AUX_ADC_KEY3_EN
	case AUX_ADC_KEY3_CH:
		i = check_key_range(adc_vol, i, prev_idx3);
		prev_idx3 = i;
		break;
#endif

	default:
		break;
	}

#if 0
	logi("ch %d: adc_vol=%d, lev=%d\n\r", ch, adc_vol, i);
#endif

	return i;
}

void hal_aux_key_init(void)
{
	gpio_cfg_t gpio_cfg;

	gpio_cfg.mode = GPIO_MODE_INPUT;
	gpio_cfg.pull = GPIO_PULL_NONE;

#ifdef AUX_ADC_KEY1_EN
	(void)gpio_init(AUX_ADC_KEY1_CH, &gpio_cfg);
	gpio_set_analog_en((u32)AUX_ADC_KEY1_CH, 0u);
#endif

#ifdef AUX_ADC_KEY2_EN
	(void)gpio_init(AUX_ADC_KEY2_CH, &gpio_cfg);
	gpio_set_analog_en((u32)AUX_ADC_KEY2_CH, 0u);
#endif

#ifdef AUX_ADC_KEY3_EN
	(void)gpio_init(AUX_ADC_KEY3_CH, &gpio_cfg);
	gpio_set_analog_en((u32)AUX_ADC_KEY3_CH, 0u);
#endif
	adc_aux_init(ADC_AUX_DIR_EXTERNAL);
}

u8 hal_aux_key_get(u8 ch)
{
	u8 val = 0;

	val = get_key(ch);

	return val;
}

#endif

