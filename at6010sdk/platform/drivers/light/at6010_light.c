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
 * GOODS OR SERVICES{} LOSS OF USE, DATA, OR PROFITS{} OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "light.h"
#include "ana.h"
#include "ana_intf.h"

void light_init(void)
{
	hwp_ana_intf->light_ctrl = LIGHT_CTRL_EN(0U) | LIGHT_CTRL_INV(0U) | LIGHT_CTRL_DR(0U) |
	                           LIGHT_CTRL_LIGHT_REG(0U) | LIGHT_CTRL_SMOOTH_LEN(4U);
}

void light_enable(void)
{
	hwp_ana_intf->light_ctrl |= LIGHT_CTRL_EN_MSK;
}

void light_disable(void)
{
	hwp_ana_intf->light_ctrl &= ~LIGHT_CTRL_EN_MSK;
}

void light_set_smooth(u8 length)
{
	u8 data;
	if (length < 0x10U) {
		data = hwp_ana_intf->light_ctrl;
		data &= ~LIGHT_CTRL_SMOOTH_LEN_MSK;
		data |= LIGHT_CTRL_SMOOTH_LEN(length);
		hwp_ana_intf->light_ctrl = data;
	}
}

/**
 * @brief
 *
 * @retval 1 -- daylight
 * @retval 0 -- night
 */
u8 light_get_status(void)
{
	u8 data = hwp_ana_intf->light_status;
	return (data & LIGHT_STATUS_LIGHT_SMOOTH_MSK) >> LIGHT_STATUS_LIGHT_SMOOTH_POS;
}

void light_set_th(u16 low, u16 high)
{
	u8 low_l;
	u8 high_l;
	u8 th_h;

	low_l = low & 0xFFU;
	high_l = high & 0xFFU;
	th_h = (u8)(((high >> 8) & 0xFU) << 4) + ((low >> 8) & 0xFU);

	hwp_ana_intf->light_th_low_l = low_l;
	hwp_ana_intf->light_th_high_l = high_l;
	hwp_ana_intf->light_th_h = th_h;
}

void light_set_sleep_cnt(u32 cnt)
{
	hwp_ana_intf->time_light_sleep_3 = (cnt >> 24) & 0xFFU;
	hwp_ana_intf->time_light_sleep_2 = (cnt >> 16) & 0xFFU;
	hwp_ana_intf->time_light_sleep_1 = (cnt >> 8) & 0xFFU;
	hwp_ana_intf->time_light_sleep_0 = cnt & 0xFFU;

}
