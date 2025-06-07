/**
 *******************************************************************************
 * @file    hal_key.c
 * @author  Airtouch Software Team
 * @brief   KEY HAL Driver
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


#include "hal_key.h"
#include "aux_adc.h"
#include "gpio.h"
#include "atlog.h"
#include "sys_timer.h"
#include "key.h"
#include "at6010.h"
#ifdef CFG_OS
#include "osal_timer.h"
#endif

#ifdef CFG_ADC_KEY

#define ADC_KEY_PIN     GPIO_AON_0
#define ADC_KEY_IRQ_NUM KEY_IRQn
#define ADC_KEY_NUM     (sizeof(key_adc_cal) / sizeof(key_adc_cal_t))

typedef struct {
	u8 num;
	u8 pressed;
	u8 event;
#if (ADC_KEY_DEBOUNCE_TIME_MS > 0u)
#ifdef CFG_OS
	osal_timer_t tm_debounce;
#else
	sys_timer_t *tm_debounce;
#endif
#endif /* #if (ADC_KEY_DEBOUNCE_TIME_MS > 0u) */

#ifdef KEY_EVENT_MSG_POST_EN
	key_cb_t msg_hdl;
#endif

#ifdef KEY_EVENT_CALLBACK_EN
	key_cb_t cb;
#endif

} key_status_t;

typedef struct {
	u8 key_id;
	u16 adc;
} key_adc_cal_t;

#ifdef KEY_EVENT_MSG_POST_EN
typedef struct {
	u8 num;
	u8 event;
} adc_key_msg_para_t;
#endif

/*******************************************************************************
**                      Private Variable Definitions                          **
*******************************************************************************/
static const key_adc_cal_t key_adc_cal[] = PARA_ADC_KEY_ADC_MAP;

static key_status_t m_key = {0u};

#ifdef KEY_EVENT_MSG_POST_EN

/* NOTE Only 1 msg support */
static adc_key_msg_para_t m_adc_key_msg_para = {
	.num = 0u,
	.event = (u8)KEY_EVENT_RELEASE
};

static msg_t m_adc_key_msg = {
	.para = &m_adc_key_msg_para
};
#endif /* KEY_EVENT_MSG_POST_EN */

/*******************************************************************************
**                      Private Function Definitions                          **
*******************************************************************************/
#ifdef KEY_EVENT_MSG_POST_EN
__STATIC_INLINE void adc_key_msg_process(void)
{
		m_adc_key_msg_para.num = m_key.num;
		m_adc_key_msg_para.event = m_key.event;
	if (0 != k_msg_post(MSG_ID_ADC_KEY, &m_adc_key_msg)) {
		logw("%s, Send adc key msg failed!\r\n", __func__);
	}
}

static void adc_key_proc_msg(msg_t *msg)
{
	adc_key_msg_para_t *para = msg->para;

		if (m_key.msg_hdl != NULL) {
			m_key.msg_hdl(para->num, para->event);
		} else {
			logi("key %d event %d.\n\r", para->num, para->event);
		}
}
#endif

static u8 get_adc_key_num(gpio_pin_t pin, const key_adc_cal_t *adc_cal, u8 total_num) {
	u8 num = 0xFFu;
	u16 adc, lower, upper;

	adc = (u16)adc_get_aux_voltage((u8)pin);
	if (adc > ADC_KEY_IDLE_ADC) {
		num = 0u;
	} else {
		for (u8 i = 0u; i < total_num; i++) {
			lower = (adc_cal[i].adc > ADC_KEY_MARGIN) ? (adc_cal[i].adc - ADC_KEY_MARGIN) : 0u;
			upper = (adc_cal[i].adc + ADC_KEY_MARGIN);
			if ((adc <= upper) && (adc >= lower)) {
				num = adc_cal[i].key_id;
				break;
			}
		}
	}
#ifdef DBG_ADC_KEY_CAL_EN
	logi("Key adc = %d, id = %d\n\r", adc, num);
#endif
	return num;
}

#if (ADC_KEY_DEBOUNCE_TIME_MS > 0u)
#ifdef CFG_OS
static void tm_adc_key_hdl(void *arg)
#else
static void tm_adc_key_hdl(u32 arg)
#endif
{
	u8 num;
	(void)arg;

	num = get_adc_key_num(ADC_KEY_PIN, key_adc_cal, (u8)ADC_KEY_NUM);
	if ((num != m_key.num) && (num != 0xFFu)) {
		if (num > 0u) {
			m_key.num = num;
			m_key.pressed = num;
			m_key.event = (u8)KEY_EVENT_PRESS;
		} else {
			m_key.event = (u8)KEY_EVENT_RELEASE;
		}
#ifdef KEY_EVENT_MSG_POST_EN
		adc_key_msg_process();
#endif

#ifdef KEY_EVENT_CALLBACK_EN
		if (m_key.cb != NULL) {
			m_key.cb(m_key.num, m_key.event);
		}
#endif
		m_key.num = num;
	}
	key_set_inv_inverse();
	key_enable();
	NVIC_EnableIRQ(ADC_KEY_IRQ_NUM);
}
#endif

static void key_intr_hdl(void)
{
	NVIC_DisableIRQ(ADC_KEY_IRQ_NUM);
	key_disable();

#if (ADC_KEY_DEBOUNCE_TIME_MS > 0u)
#ifdef CFG_OS
	if (OSAL_OK != osal_timer_start(&m_key.tm_debounce)) {
		loge("%s, start timer failed.\n\r", __func__);
	}
#else
	if (0 != start_timer(m_key.tm_debounce, ADC_KEY_DEBOUNCE_TIME_MS)) {
		loge("%s, start timer failed.\n\r", __func__);
	}
#endif /* CFG_OS */

#else
	u8 edge = 1u - key_get_inv();
	if (0u == edge) {
		m_key.num = get_adc_key_num(ADC_KEY_PIN, key_adc_cal, ADC_KEY_NUM);
		m_key.pressed = m_key.num;
		m_key.event = (u8)KEY_EVENT_PRESS;
	} else {
		/* NOTE: No need to get adc here as interrupt indicates releasing */
		m_key.event = (u8)KEY_EVENT_RELEASE;
	}

#ifdef KEY_EVENT_MSG_POST_EN
	adc_key_msg_process();
#endif

#ifdef KEY_EVENT_CALLBACK_EN
	if (m_key.cb != NULL) {
		m_key.cb(m_key.num, m_key.event);
	}
#endif
	key_set_inv_inverse();
	key_enable();
	NVIC_EnableIRQ(ADC_KEY_IRQ_NUM);
#endif /* (ADC_KEY_DEBOUNCE_TIME_MS > 0u) */
}

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
void hal_key_init(void)
{
	key_cfg_t key_cfg;
	gpio_cfg_t gpio_cfg;

	gpio_cfg.mode = GPIO_MODE_INPUT;
	gpio_cfg.pull = GPIO_PULL_NONE;
	(void)gpio_init(ADC_KEY_PIN, &gpio_cfg);
	gpio_set_analog_en((u32)ADC_KEY_PIN, 0u);
	adc_aux_init(ADC_AUX_DIR_EXTERNAL);

	key_irq_set_hdl(key_intr_hdl);

	key_cfg.en = 0;
	key_cfg.en_sel = 1;
	key_cfg.invert = 1;
	key_cfg.smooth_en = 0;
	key_init(&key_cfg);
	key_irq_enable();
	key_enable();

#if (ADC_KEY_DEBOUNCE_TIME_MS > 0u)
#ifdef CFG_OS
	if (OSAL_OK
	    != osal_timer_create(&m_key.tm_debounce, OSAL_TIMER_MODE_SINGLE, tm_adc_key_hdl, NULL,
	                         ADC_KEY_DEBOUNCE_TIME_MS)) {
		loge("Create adc key debounce timer failed!\r\n");
	}
#else
	m_key.tm_debounce = create_timer("adc_key", (u8)TIMER_OPT_ONE_SHOT, tm_adc_key_hdl, 0u);
#endif /* CFG_OS */
#endif /* #if (ADC_KEY_DEBOUNCE_TIME_MS > 0u) */

#ifdef KEY_EVENT_MSG_POST_EN
	if (0 != k_msg_reg_hdl(MSG_ID_ADC_KEY, adc_key_proc_msg)) {
		loge("%s, Create adc key msg failed!\n\r", __func__);
	}
#endif
	NVIC_EnableIRQ(ADC_KEY_IRQ_NUM);
}

u8 hal_key_get(void)
{
	u8 num;
	if (m_key.pressed > 0u) {
		num = m_key.pressed;
		m_key.pressed = 0u;
	} else {
		num = 0xFFu;
	}
	return num;
}

#ifdef KEY_EVENT_MSG_POST_EN
void adc_key_reg_msg_hdl(key_cb_t cb)
{
	m_key.msg_hdl = cb;
}
#endif

#ifdef KEY_EVENT_CALLBACK_EN
void adc_key_reg_cb_hdl(key_cb_t cb)
{
	m_key.cb = cb;
}
#endif

#endif /* CFG_ADC_KEY */
