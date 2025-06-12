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

#include "hal_dip.h"
#include "gpio.h"
#include "atlog.h"
#include "common.h"
#include "at6010.h"

//#define DIP_DBG_EN

#ifdef CFG_DIP_SWITCH

#define DIP_PIN_NUM                     (sizeof(m_dip_cfg)/sizeof(hal_dip_cfg_t))

typedef struct {
	u8 dip;
	gpio_pin_t gpio;
} hal_dip_cfg_t;

static dip_hdl_t m_dip_hdl;
static const hal_dip_cfg_t m_dip_cfg[] = PARA_DIP_MAP;
static const IRQn_Type irq_type[] = {GPIO0_IRQn, GPIO1_IRQn, GPIO2_IRQn, GPIO3_IRQn, GPIO4_IRQn, GPIO5_IRQn,
                                     GPIO6_IRQn, GPIO_IRQn,  GPIO_IRQn,  GPIO_IRQn,  GPIO_IRQn,  GPIO_IRQn,
                                     GPIO_IRQn,  GPIO_IRQn,  GPIO_IRQn,  GPIO_IRQn,  GPIO_IRQn};

static void hal_dip_hdl(void)
{
	u8 dip_type = 0u;
	u8 i;
	for (i = 0u; i < DIP_PIN_NUM; i++) {
		if ((m_dip_cfg[i].dip & DIP_IMME_EFFECT_EN_MASK) > 0u) {
			if (gpio_irq_get_status(m_dip_cfg[i].gpio) > 0u) {
				dip_type |= (m_dip_cfg[i].dip >> DIP_TYPE_POS);
				gpio_irq_clr_flag(m_dip_cfg[i].gpio);
	}
}
	}
	if (NULL != m_dip_hdl) {
		m_dip_hdl(dip_type);
}
	}

__STATIC_INLINE void dip_pin_init(gpio_pin_t pin)
{
	gpio_set_pinmux(pin, 1u);
	gpio_set_direction(pin, 0u);
	gpio_set_pull_up(pin, 1u);
	gpio_set_pull_down(pin, 0u);
}

__STATIC_INLINE void dip_pin_irq_init(gpio_pin_t pin)
{
	gpio_set_irq_type(pin, IRQ_TYPE_EDGE_BOTH);
	gpio_irq_set_hdl(pin, hal_dip_hdl);
	gpio_irq_enable(pin);
	NVIC_EnableIRQ(irq_type[pin]);
}

void hal_dip_init(void)
{
	for (u8 i = 0u; i < DIP_PIN_NUM; i++) {
		dip_pin_init(m_dip_cfg[i].gpio);
		if ((m_dip_cfg[i].dip & DIP_IMME_EFFECT_EN_MASK) > 0u) {
			dip_pin_irq_init(m_dip_cfg[i].gpio);
		}
	}
}

u8 hal_dip_get_data(dip_setting_item_t itm)
{
	u8 dat = 0u, i, type, tmp = 0xFFu;

	switch (itm) {
	case DIP_SETTING_SEN_DIST: type = DIP_TYPE_DIST; break;
	case DIP_SETTING_LIGHT_TIME1: type = DIP_TYPE_LOT1; break;
	case DIP_SETTING_LIGHT_TIME2: type = DIP_TYPE_LOT2; break;
	case DIP_SETTING_LUX: type = DIP_TYPE_LUX; break;
	case DIP_SETTING_INVALID:
	default: type = 0u; break;
	}

	for (i = 0u; i < DIP_PIN_NUM; i++) {
		if (((m_dip_cfg[i].dip >> DIP_TYPE_POS) & type) > 0u) {
			tmp = (u8)gpio_get_value(m_dip_cfg[i].gpio);
			dat |= (u8)(tmp << (m_dip_cfg[i].dip & DIP_PIN_MASK));
		}
	}

#ifdef DIP_DBG_EN
	logi("hal_dig_get_data, itm=%d, dat=%d\r\n", (uint16)itm, (uint16)dat);
#endif

	return (tmp == 0xFFu) ? 0xFFu : dat;
}

#ifdef CFG_MOD_TEST
void hal_dip_test(void)
{
	u8 val = 0;

	while (1) {
		logi("---------------- hal_dip_test ---------------\r\n");
		val = hal_dip_get_data(DIP_SETTING_SEN_DIST);
		logi("DIP_SETTING_SEN_DIST, dat=%d\r\n", (uint16)val);

		val = hal_dip_get_data(DIP_SETTING_LIGHT_TIME1);
		logi("DIP_SETTING_LIGHT_TIME1, dat=%d\r\n", (uint16)val);

		//val = hal_dip_get_data(DIP_SETTING_LIGHT_TIME2);
		val = hal_dip_get_data(DIP_SETTING_LUX);
		logi("DIP_SETTING_LUX, dat=%d\r\n", (uint16)val);

		delay_ms(2000);
	}
}
#endif /* CFG_MOD_TEST */

void hal_dip_set_hdl(dip_hdl_t hdl)
{
	m_dip_hdl = hdl;
}

#endif
