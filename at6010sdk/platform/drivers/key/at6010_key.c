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

#include "key.h"
#include "ana_intf.h"

static key_hdl_t m_key_hdl = NULL;

void key_init(const key_cfg_t *cfg)
{
	if (NULL == cfg) {
		return;
	}

	hwp_ana_intf->ana_irq_clr |= ANA_IRQ_CLR_KEY_MSK;
	hwp_ana_intf->key_ctrl = (u8)(KEY_CTRL_EN(cfg->en) | KEY_CTRL_INV(cfg->invert) |
	                         KEY_CTRL_SMOOTH_EN(cfg->smooth_en));
	hwp_ana_intf->ana_irq_clr &= ~ANA_IRQ_CLR_KEY_MSK;
}

/**
 * key_ctrl en
 */

void key_set_en(u8 en)
{
	if (en == 0U) {
		hwp_ana_intf->key_ctrl &= ~KEY_CTRL_EN_MSK;
	} else {
		hwp_ana_intf->key_ctrl |= KEY_CTRL_EN_MSK;
	}
}

void key_enable(void)
{
	hwp_ana_intf->key_ctrl |= KEY_CTRL_EN_MSK;
	hwp_ana_intf->ana_irq_clr &= ~ANA_IRQ_CLR_KEY_MSK;
}

void key_disable(void)
{
	hwp_ana_intf->ana_irq_clr |= ANA_IRQ_CLR_KEY_MSK;
	hwp_ana_intf->key_ctrl &= ~KEY_CTRL_EN_MSK;
}

/**
 * key_ctrl inv
 */
void key_set_inv_inverse(void)
{
	hwp_ana_intf->key_ctrl ^= KEY_CTRL_INV_MSK;
}

u8 key_get_inv(void)
{
	return (u8)((hwp_ana_intf->key_ctrl & KEY_CTRL_INV_MSK) >> KEY_CTRL_INV_POS);
}

/**
 * key_ctrl smooth
 */
void key_set_smooth(u8 en, u8 smooth_high, u8 smooth_low)
{
	if (en > 0U) {
		hwp_ana_intf->key_ctrl |= KEY_CTRL_SMOOTH_EN_MSK;
		hwp_ana_intf->key_smooth_high = smooth_high;
		hwp_ana_intf->key_smooth_low = smooth_low;
	} else {
		hwp_ana_intf->key_ctrl &= ~KEY_CTRL_SMOOTH_EN_MSK;
	}
}

/**
 * key_ctrl irq_clr
 */
void key_set_irq_clr(u8 en)
{
	if (en > 0U) {
		hwp_ana_intf->ana_irq_clr |= ANA_IRQ_CLR_KEY_MSK;
	} else {
		hwp_ana_intf->ana_irq_clr &= ~ANA_IRQ_CLR_KEY_MSK;
	}
}

void key_irq_enable(void)
{
	hwp_ana_intf->ana_irq_msk &= ~ANA_IRQ_MSK_KEY_MSK; // 0 -- enable
}

void key_irq_disable(void)
{
	hwp_ana_intf->ana_irq_msk |= ANA_IRQ_MSK_KEY_MSK; // 1 -- disable
}

void key_irq_set_hdl(key_hdl_t hdl)
{
	m_key_hdl = hdl;
}

/**
 * KEY ISR
 */
void key_isr(void)
{
	if (m_key_hdl != NULL) {
		m_key_hdl();
	}
}
