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

#include "at6010.h"
#include "gpio.h"
#include "sys_ctrl.h"
#include "ana_intf.h"

#define PIN_OUTPUT_ONLY         GPIO_AON_2

#define hwp_gpio                ((CMSDK_GPIO_TypeDef *)CMSDK_GPIO0_BASE)

#define GPIO_O_POS              0U
#define GPIO_O_MSK              (1U << GPIO_O_POS)
#define GPIO_O(x)               (((x)&1U) << GPIO_O_POS)

#define GPIO_OE_N_POS           1U
#define GPIO_OE_N_MSK           (1U << GPIO_OE_N_POS)
#define GPIO_OE_N(x)            (((x)&1U) << GPIO_OE_N_POS)

#define GPIO_TIE_UP_POS         2U
#define GPIO_TIE_UP_MSK         (1U << GPIO_TIE_UP_POS)
#define GPIO_TIE_UP(x)          (((x)&1U) << GPIO_TIE_UP_POS)

#define GPIO_TIE_DN_POS         3U
#define GPIO_TIE_DN_MSK         (0x1 << GPIO_TIE_DN_POS)
#define GPIO_TIE_DN(x)          (((x)&1U) << GPIO_TIE_DN_POS)

#define GPIO_DRV_SEL_POS        4U
#define GPIO_DRV_SEL_MSK        (3U << GPIO_DRV_SEL_POS)
#define GPIO_DRV_SEL(x)         (((x)&3U) << GPIO_DRV_SEL_POS)

#define GPIO_ANA_EN_POS         6U
#define GPIO_ANA_EN_MSK         (1U << GPIO_ANA_EN_POS)
#define GPIO_ANA_EN(x)          (((x)&1U) << GPIO_ANA_EN_POS)

static gpio_hdl_t m_gpio_hdl[16]; /* NOTE: only 16 pins has interrupt */

/*******************************************************************************
*                       GPIO Register Functions                                *
*******************************************************************************/

__STATIC_INLINE u8 get_iomux_sel_pos(gpio_pin_t gpio)
{
	u8 idx = (gpio >= GPIO_AON_3) ? ((u8)gpio - 1U) : (u8)gpio;
	return (u8)(idx << 1);
}

__STATIC_INLINE u32 get_gpio_mcu_data_addr(u32 gpio)
{
	return (u32)REG_SYS_CTRL_BASE + (u32)GPIO_MCU_0_3_OFFSET + (u32)(((u8)gpio >> 2) << 2);
}

__STATIC_INLINE u8 get_gpio_mcu_data_pos(u32 gpio)
{
	return (u8)(((u8)gpio & 0x3U) << 3U);
}

__STATIC_INLINE void set_gpio_mcu_data(u32 gpio, u8 field_mask, u8 field_data)
{
	u32 addr  = get_gpio_mcu_data_addr(gpio);
	u8  pos   = get_gpio_mcu_data_pos(gpio);
	u32 value = *(REG32 *)addr;

	value &= ~((u32)field_mask << pos);
	value |= (u32)field_data << pos;
	*(REG32 *)addr = value;
}

__STATIC_INLINE u32 get_gpio_aon_data_addr(u32 gpio)
{
	return (u32)REG_ANA_INTF_BASE + (u32)GPIO_AON_0_OFFSET + (u32)gpio - (u32)GPIO_AON_0;
}

__STATIC_INLINE void set_gpio_aon_data(u32 gpio, u8 field_mask, u8 field_data)
{
	u32 addr = get_gpio_aon_data_addr(gpio);
	u8 value = *(REG8 *)addr;

	value &= ~field_mask;
	value |= field_data;
	*(REG8 *)addr = value;
}

__STATIC_INLINE void set_gpio_mcu_sel(u32 gpio, u32 sel)
{
	u8 bit_pos = (u8)gpio - (u8)GPIO_MCU_0;

	if (sel == 1U) {
		hwp_sys_ctrl->gpio_mcu_sel |= (1U << bit_pos);
	} else {
		hwp_sys_ctrl->gpio_mcu_sel &= ~(1U << bit_pos);
	}
}

__STATIC_INLINE void set_gpio_aon_sel(u32 gpio, u32 sel)
{
	u8 bit_pos = (u8)gpio - (u8)GPIO_AON_0;

	if (sel == 1U) {
		hwp_ana_intf->gpio_aon_sel |= (u8)(1U << bit_pos);
	} else {
		hwp_ana_intf->gpio_aon_sel &= ~(u8)(1U << bit_pos);
	}
}

__STATIC_INLINE u8 get_gpio_data(u32 gpio)
{
	u32 addr, data;
	u8 pos;
	if (gpio <= (u32)GPIO_MCU_9) {
		addr = get_gpio_mcu_data_addr(gpio);
		data = *(REG32 *)addr;
		pos = get_gpio_mcu_data_pos(gpio);
		data = (data >> pos) & 0xFU;
	} else if ((gpio >= (u32)GPIO_AON_0) && (gpio <= (u32)GPIO_AON_6)) {
		addr = get_gpio_aon_data_addr(gpio);
		data = *(REG8 *)addr;
	} else {
		/* Invalid pin */
		data = 0U;
	}
	return (u8)data;
}

u8 gpio_get_direction(u32 gpio)
{
	u8 data = get_gpio_data(gpio);
	return ((data & GPIO_OE_N_MSK) == 0U) ? 1U : 0U;
}

void gpio_set_direction(u32 gpio, u8 direction)
{
	if (gpio <= (u32)GPIO_MCU_9) {
		set_gpio_mcu_data(gpio, GPIO_OE_N_MSK, (u8)GPIO_OE_N(direction ^ 1U));
	} else if ((gpio >= (u32)GPIO_AON_0) && (gpio <= (u32)GPIO_AON_6)) {
		set_gpio_aon_data(gpio, GPIO_OE_N_MSK, (u8)GPIO_OE_N(direction ^ 1U));
	} else {
		/* Invalid pin */
	}
}

u8 gpio_get_value(u32 gpio)
{
	u32 value = 0;
    u32 addr = 0;

    if (gpio == (u32)GPIO_AON_2) {
        addr = get_gpio_aon_data_addr(gpio);
	    value = *(REG8 *)addr;
        value &= GPIO_O_MSK;
	} else if (gpio < (u32)GPIO_AON_2) {
        value = (hwp_gpio->DATA >> gpio) & GPIO_O_MSK;
	} else if ((gpio > (u32)GPIO_AON_2) && (gpio <= (u32)GPIO_AON_6)) {
        value = (hwp_gpio->DATA >> (gpio - 1)) & GPIO_O_MSK;
	}

	return value;
}

void gpio_set_value(u32 gpio, u8 val)
{
	if (gpio <= (u32)GPIO_MCU_9) {
		set_gpio_mcu_data(gpio, GPIO_O_MSK, (u8)GPIO_O(val));
	} else if ((gpio >= (u32)GPIO_AON_0) && (gpio <= (u32)GPIO_AON_6)) {
		set_gpio_aon_data(gpio, GPIO_O_MSK, (u8)GPIO_O(val));
	} else {
		/* Invalid pin */
	}
}

s32 gpio_direction_input(u32 gpio)
{
	gpio_set_direction(gpio, 0);

	return 0;
}

s32 gpio_direction_output(u32 gpio, u32 val)
{
	gpio_set_value(gpio, (u8)val);
	gpio_set_direction(gpio, 1);

	return 0;
}

void gpio_set_pinmux(u32 gpio, u32 mod)
{
	if (gpio <= (u32)GPIO_MCU_9) {
		set_gpio_mcu_sel(gpio, mod);
	} else if ((gpio >= (u32)GPIO_AON_0) && (gpio <= (u32)GPIO_AON_6)) {
		set_gpio_aon_sel(gpio, mod);
	} else {
		/* Invalid pin */
	}
}

void gpio_set_alternate_function(gpio_pin_t gpio, gpio_af_t af)
{
	u8 pos;
	u32 data;

	if (((u32)gpio < NUM_OF_GPIO) &&(gpio != GPIO_AON_2)) {
		pos = get_iomux_sel_pos(gpio);
		data = hwp_sys_ctrl->iomux_sel;
		data &= ~((u32)0x3U << pos);
		data |= (u32)af << pos;
		hwp_sys_ctrl->iomux_sel = data;
	}
}

void gpio_set_analog_en(u32 gpio, u32 en)
{
	if (gpio <= (u32)GPIO_MCU_9) {
		set_gpio_mcu_data(gpio, GPIO_ANA_EN_MSK, (u8)GPIO_ANA_EN(en));
	} else if ((gpio >= (u32)GPIO_AON_0) && (gpio <= (u32)GPIO_AON_6)) {
		set_gpio_aon_data(gpio, GPIO_ANA_EN_MSK, (u8)GPIO_ANA_EN(en));
	} else {
		/* Invalid pin */
	}
}

void gpio_set_rd_en(u32 gpio, u32 en)
{
	u32 data = hwp_gpio->IN_ANA_ENABLE;

	if (en) {
		hwp_gpio->IN_ANA_ENABLE = data | (u32)((0x1 << gpio) << CMSDK_GPIO_INENSET_Pos);
	} else {
		hwp_gpio->IN_ANA_ENABLE = data & (u32)~((0x1 << gpio) << CMSDK_GPIO_INENSET_Pos);
	}
}

void gpio_set_pull_up(u32 gpio, u32 en)
{
	if (gpio <= (u32)GPIO_MCU_9) {
		set_gpio_mcu_data(gpio, GPIO_TIE_UP_MSK, (u8)GPIO_TIE_UP(en));
	} else if ((gpio >= (u32)GPIO_AON_0) && (gpio <= (u32)GPIO_AON_6)) {
		set_gpio_aon_data(gpio, GPIO_TIE_UP_MSK, (u8)GPIO_TIE_UP(en));
	} else {
		/* Invalid pin */
	}
}

void gpio_set_pull_down(u32 gpio, u32 en)
{
	if (gpio <= (u32)GPIO_MCU_9) {
		set_gpio_mcu_data(gpio, GPIO_TIE_DN_MSK, (u8)GPIO_TIE_DN(en));
	} else if ((gpio >= (u32)GPIO_AON_0) && (gpio <= (u32)GPIO_AON_6)) {
		set_gpio_aon_data(gpio, GPIO_TIE_DN_MSK, (u8)GPIO_TIE_DN(en));
	} else {
		/* Invalid pin */
	}
}

/*******************************************************************************
**                      HAL GPIO Functions                                    **
*******************************************************************************/

__STATIC_INLINE u32 gpio_pin_to_num(gpio_pin_t pin)
{
	/* NOTE: gpio0-11 match gpio_mcu0-9, gpio_aon0-1, gpio12-15 match gpio_aon_3-6 */
	return (pin < GPIO_AON_2) ? pin : (pin - 1);
}

gpio_status_t gpio_init(gpio_pin_t gpio, const gpio_cfg_t *cfg)
{
	u8 gpio_sel;
	u8 field_data;
	u8 oe_n, ana_en, dn, up;

	/* Get gpio_sel data */
	gpio_sel = (GPIO_MODE_AF != cfg->mode) ? 1U : 0U;

	/* Get gpio cfg data */
	oe_n = (GPIO_MODE_OUTPUT == cfg->mode) ? 0U : 1U;
	ana_en = (GPIO_MODE_ANALOG == cfg->mode) ? 1U : 0U;
	dn = (GPIO_PULL_DOWN == cfg->pull) ? 1U : 0U;
	up = (GPIO_PULL_UP == cfg->pull) ? 1U : 0U;
	field_data = (u8)(GPIO_AON_0_GPIO_AON0_OE_N(oe_n) | GPIO_AON_0_GPIO_AON0_ANA_EN(ana_en) |
	             GPIO_AON_0_GPIO_AON0_TIE_UP(up) | GPIO_AON_0_GPIO_AON0_TIE_DN(dn) |
	             GPIO_AON_0_GPIO_AON0_DRV_SEL(0x3U));

	/* Set gpio_sel & gpio data */
	if (gpio <= GPIO_MCU_9) {
		set_gpio_mcu_sel(gpio, gpio_sel);
		set_gpio_mcu_data((u32)gpio, 0xFFU, field_data);
	} else if ((gpio >= GPIO_AON_0) && (gpio <= GPIO_AON_6)) {
		set_gpio_aon_sel(gpio, gpio_sel);
		set_gpio_aon_data(gpio, 0xFFU, field_data);
	} else {
		return GPIO_ERROR;
	}

	/* Set iomux_sel reg */
	if (GPIO_MODE_AF == cfg->mode) {
		gpio_set_alternate_function(gpio, cfg->af);
	}

	return GPIO_OK;
}

/*******************************************************************************
**                      GPIO IRQ                                              **
*******************************************************************************/
void gpio_set_irq_type(gpio_pin_t gpio, irq_type_t type)
{
	u32 num = gpio_pin_to_num(gpio);
	switch (type) {
	case IRQ_TYPE_EDGE_RISING:
		hwp_gpio->INTTYPESET = (1U << num);
		hwp_gpio->INTPOLSET = (1U << num);
		hwp_gpio->INTPOLCLR = (u32)((1U << num) << 16U);
		break;

	case IRQ_TYPE_EDGE_FALLING:
		hwp_gpio->INTTYPESET = (1U << num);
		hwp_gpio->INTPOLCLR = (1U << num);
		hwp_gpio->INTPOLCLR = (u32)((1U << num) << 16U);
		break;

	case IRQ_TYPE_EDGE_BOTH:
		hwp_gpio->INTTYPESET = (1U << num);
		hwp_gpio->INTPOLSET = (u32)((1U << num) << 16U);
		break;

	case IRQ_TYPE_LEVEL_HIGH:
		hwp_gpio->INTTYPECLR = (1U << num);
		hwp_gpio->INTPOLSET = (1U << num);
		hwp_gpio->INTPOLCLR = (u32)((1U << num) << 16U);
		break;

	case IRQ_TYPE_LEVEL_LOW:
		hwp_gpio->INTTYPECLR = (1U << num);
		hwp_gpio->INTPOLCLR = (1U << num);
		hwp_gpio->INTPOLCLR = (u32)((1U << num) << 16U);
		break;
	default:
		/* Invalid irq type */
		break;
	}
}

void gpio_irq_hdl(u32 gpio)
{
	if (m_gpio_hdl[gpio] != NULL) {
		m_gpio_hdl[gpio]();
	}
}

void gpio_isr(void)
{
	u16 int_status = hwp_gpio->INTSTATUS & 0xFFFFU;

	for (u8 num = 0U; num < 16U; num++) {
		if ((int_status & 1U) > 0U) {
			gpio_irq_hdl(num);
		}
		int_status >>= 1;
	}
}

void gpio_irq_set_hdl(gpio_pin_t gpio, gpio_hdl_t hdl)
{
	u32 num = gpio_pin_to_num(gpio);
	m_gpio_hdl[num] = hdl;
}

void gpio_irq_enable(gpio_pin_t gpio)
{
	u32 num = gpio_pin_to_num(gpio);
	hwp_gpio->INTENSET = (1 << num);
}

void gpio_irq_disable(gpio_pin_t gpio)
{
	u32 num = gpio_pin_to_num(gpio);
	hwp_gpio->INTENCLR = (1 << num);
}

void gpio_irq_clr_flag(gpio_pin_t gpio)
{
	u32 num = gpio_pin_to_num(gpio);
	hwp_gpio->INTCLEAR = (1 << num);
}

u32 gpio_irq_get_status(gpio_pin_t gpio)
{
	u32 dat = hwp_gpio->INTSTATUS;
	u32 num = gpio_pin_to_num(gpio);
	dat = (dat >> num) & 0x00000001;
	return dat;
}

u32 gpio_irq_rise_get_status(gpio_pin_t gpio)
{
	u32 dat = hwp_gpio->INTSTATUS;
	u32 num = gpio_pin_to_num(gpio);
	dat = (dat >> num >> 16) & 0x00000001;
	return dat;
}

void gpio_toggle_init(u32 gpio)
{
	if (gpio >= NUM_OF_GPIO) {
		return;
	}

	gpio_set_pinmux(gpio, 1);
	gpio_set_direction(gpio, 1);
}

void gpio_toggle(u32 gpio)
{
	u8 value;

	if (gpio < NUM_OF_GPIO) {
		value = gpio_get_value(gpio);
		gpio_set_value(gpio, value ^ 1U);
	}
}
