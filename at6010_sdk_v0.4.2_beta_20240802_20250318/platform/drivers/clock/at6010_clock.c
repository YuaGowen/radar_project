/**
 * Copyright (c) 2022, Airtouching Intelligence Technology.
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
#include "clock.h"
#include "sys_ctrl.h"

#define CLK_SRC_RC_32K_FREQ (32000U)
#define CLK_SRC_XTAL_FREQ   (40000000U)
#define CLK_SRC_PLL_FREQ    (320000000U)
#define CLK_DIV_INVALID     (0xFFFFU)

static xtal_type_t m_xtal_type;
static const u32 m_xtal_freq[] = {40000000U, 32000000U};
static u32 m_clk_mcu;

void clock_init(void)
{
	m_xtal_type = XTAL_TYPE_40MHZ;
	m_clk_mcu = CLK_SRC_XTAL_FREQ;
	// TODO read xtal bit
}

xtal_type_t clock_xtal_type_get(void)
{
	return m_xtal_type;
}

void pll_ana_clk_config(u8 xtal_type)
{
#if 0
	if (xtal_type == 1) {
		pmu_reg_field_write(SYSPLL3_OFFSET, SYSPLL3_SYSPLL_DIN_MSK, SYSPLL3_SYSPLL_DIN(26)); /*16M XTAL*/
	} else {
		pmu_reg_field_write(SYSPLL3_OFFSET, SYSPLL3_SYSPLL_DIN_MSK, SYSPLL3_SYSPLL_DIN(16)); /*26M XTAL*/
	}
	return;
#else
	(void)xtal_type;
#endif
}

void set_pll_clk_divider(s8 div)
{
	/* enable pll */
	SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_EN_PLL_MSK);
	if (div >= 0) {
		MODIFY_REG(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_PLL_MSK,
			           CLK_DIV_CTRL_0_DIV_PLL((u8)div));
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_LOAD_PLL_MSK);
		CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_LOAD_PLL_MSK);
		CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_BYPASS_PLL_MSK);
	} else {
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_BYPASS_PLL_MSK);
	}
	return;
}

void disable_pll_clk(void)
{
	MODIFY_REG(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_EN_PLL_MSK, CLK_DIV_CTRL_0_DIV_EN_PLL(0));
}


void clk_mcu_src_set(u8 clk_src)
{
	switch (clk_src) {
	case CLK_SRC_RC_32K:
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_CLK_XTAL_SEL(0));
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_CLK_PLL_SEL(0));
		break;
	case CLK_SRC_XTAL:
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_CLK_XTAL_SEL(1));
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_CLK_PLL_SEL(0));
		break;
	case CLK_SRC_PLL: SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_CLK_PLL_SEL(1)); break;
	default: /* Invalid src */ break;
	}
}

void set_sys_clk_divider(s8 div)
{
	if (div >= 0) {
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_1, CLK_DIV_CTRL_1_DIV_EN_FCLK_MSK);

		MODIFY_REG(hwp_sys_ctrl->clk_div_ctrl_1, CLK_DIV_CTRL_1_DIV_FCLK_MSK, CLK_DIV_CTRL_1_DIV_FCLK((u8)div));
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_1, CLK_DIV_CTRL_1_DIV_LOAD_FCLK_MSK);

		CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_1, CLK_DIV_CTRL_1_DIV_LOAD_FCLK_MSK);
		CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_1, CLK_DIV_CTRL_1_DIV_BYPASS_FCLK_MSK);

		m_clk_mcu = (CLK_SRC_PLL_FREQ >> 2) / ((u8)div + 1U);
	}
	else {
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_1, CLK_DIV_CTRL_1_DIV_BYPASS_FCLK_MSK);
		m_clk_mcu = (CLK_SRC_PLL_FREQ >> 1);
	}

	return;
}

void set_spi_flash_clk_divider(s8 div)
{
	if (div >= 0) {
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_1, CLK_DIV_CTRL_1_DIV_EN_SCLK_MSK);
		MODIFY_REG(hwp_sys_ctrl->clk_div_ctrl_1, CLK_DIV_CTRL_1_DIV_SCLK_MSK, CLK_DIV_CTRL_1_DIV_SCLK((u8)div));
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_1, CLK_DIV_CTRL_1_DIV_LOAD_SCLK_MSK);

		CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_1, CLK_DIV_CTRL_1_DIV_LOAD_SCLK_MSK);
		CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_1, CLK_DIV_CTRL_1_DIV_BYPASS_SCLK_MSK);

	} else {
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_1, CLK_DIV_CTRL_1_DIV_BYPASS_SCLK_MSK);
	}
	return;
}

void sys_clk_set_adc_clk(u8 xtal_type)
{
#if 0
	if(xtal_type == 1) {
		pmu_reg_field_write(CRYSTAL3_OFFSET, CRYSTAL3_CRYSTAL_CLK26M_ADC_SW_MSK,CRYSTAL3_CRYSTAL_CLK26M_ADC_SW(1));
	}
#else
	(void)xtal_type;
#endif
}

/*******************************************************************************
**                      Clock Frequency Configuration                         **
*******************************************************************************/

__STATIC_INLINE u16 get_clk_div(u32 dest_freq, u32 src_freq)
{
	u16 multiple = (u16)((src_freq >> 1U) / dest_freq);
	return (multiple > 0U) ? (multiple - 1U) : CLK_DIV_INVALID;
}

__STATIC_INLINE u32 get_dest_freq(u32 src_freq, u16 div)
{
	return (src_freq >> 1) / ((u32)div + 1U);
}

u32 xtal_freq_get(void)
{
	return m_xtal_freq[m_xtal_type];
}

void clk_mcu_set(clk_src_t src, u32 freq)
{
	u16 div;

	switch (src) {
	case CLK_SRC_RC_32K:
		CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_CLK_XTAL_SEL_MSK);
		CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_CLK_PLL_SEL_MSK);
		m_clk_mcu = CLK_SRC_RC_32K_FREQ;
		break;
	case CLK_SRC_XTAL:
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_EN_XTAL_MSK);
		div = get_clk_div(freq, CLK_SRC_XTAL_FREQ);
		if (div == CLK_DIV_INVALID) {
			SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_BYPASS_XTAL_MSK);
			m_clk_mcu = CLK_SRC_XTAL_FREQ;

		} else {
			MODIFY_REG(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_XTAL_MSK,
			           CLK_DIV_CTRL_0_DIV_XTAL(div));
			SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_LOAD_XTAL_MSK);
			CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_LOAD_XTAL_MSK);
			CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_BYPASS_XTAL_MSK);
			m_clk_mcu = get_dest_freq(CLK_SRC_XTAL_FREQ, div);
		}
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_CLK_XTAL_SEL_MSK);
		CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_CLK_PLL_SEL_MSK);
		break;
	case CLK_SRC_PLL:
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_EN_PLL_MSK);
		div = get_clk_div(freq, CLK_SRC_PLL_FREQ);
		if (div == CLK_DIV_INVALID) {
			SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_BYPASS_PLL_MSK);
			m_clk_mcu = CLK_SRC_PLL_FREQ;
		} else {
			MODIFY_REG(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_PLL_MSK,
			           CLK_DIV_CTRL_0_DIV_PLL(div));
			SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_LOAD_PLL_MSK);
			CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_LOAD_PLL_MSK);
			CLEAR_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_DIV_BYPASS_PLL_MSK);
			m_clk_mcu = get_dest_freq(CLK_SRC_PLL_FREQ, div);
		}
		SET_BIT(hwp_sys_ctrl->clk_div_ctrl_0, CLK_DIV_CTRL_0_CLK_PLL_SEL_MSK);
		break;
	default:
		/* Invalid src */
		break;
	}
}

u32 clk_mcu_get(void)
{
	return m_clk_mcu;
}

/*******************************************************************************
**                      Module Clock Configuration                            **
*******************************************************************************/

/**
 * @brief Set the ADC/sdm sampling edge.
 *
 * @param inv 1 -- rising edge, 0 -- falling edge
 */
void sys_clk_adc_sdm_inv_set(u8 adc_inv,u8 sdm_inv)
{
	MODIFY_REG(hwp_sys_ctrl->pmu_ctrl, PMU_CTRL_CLK_ADC_INV_MSK | PMU_CTRL_CLK_SDM_INV_MSK,
	           PMU_CTRL_CLK_ADC_INV(adc_inv) | PMU_CTRL_CLK_SDM_INV(sdm_inv));

}

/*******************************************************************************
**                      Module enable/disable Functions                       **
*******************************************************************************/

__STATIC_INLINE void sys_clk_ahb_enable(u32 clk_msk, u32 rst_msk)
{
	hwp_sys_ctrl->clk_gate_ahb |= clk_msk;
	hwp_sys_ctrl->soft_reset_ahb |= rst_msk;
}

__STATIC_INLINE void sys_clk_ahb_disable(u32 clk_msk, u32 rst_msk)
{
	hwp_sys_ctrl->clk_gate_ahb &= ~clk_msk;
	hwp_sys_ctrl->soft_reset_ahb &= ~rst_msk;
}

__STATIC_INLINE void sys_clk_apb_enable(u32 clk_msk, u32 rst_msk)
{
	hwp_sys_ctrl->clk_gate_apb |= clk_msk;
	hwp_sys_ctrl->soft_reset_apb |= rst_msk;
}

__STATIC_INLINE void sys_clk_apb_disable(u32 clk_msk, u32 rst_msk)
{
	hwp_sys_ctrl->clk_gate_apb &= ~clk_msk;
	hwp_sys_ctrl->soft_reset_apb &= ~rst_msk;
}

void sys_clk_enable_radar(void)
{
	sys_clk_ahb_enable(CLK_GATE_AHB_HCLK_RADAR_EN_MSK, SOFT_RESET_AHB_RSTN_SW_RADAR_MSK);
}

void sys_clk_disable_radar(void)
{
	sys_clk_ahb_disable(CLK_GATE_AHB_HCLK_RADAR_EN_MSK, SOFT_RESET_AHB_RSTN_SW_RADAR_MSK);
}

void sys_clk_enable_dualtimer(void)
{
	sys_clk_apb_enable(CLK_GATE_APB_PCLK_DUALTIMER_EN_MSK, SOFT_RESET_APB_RSTN_SW_DUALTIMER_MSK);
}

void sys_clk_disable_dualtimer(void)
{
	sys_clk_apb_disable(CLK_GATE_APB_PCLK_DUALTIMER_EN_MSK, SOFT_RESET_APB_RSTN_SW_DUALTIMER_MSK);
}

void sys_clk_enable_gpio(void)
{
	sys_clk_ahb_enable(CLK_GATE_AHB_HCLK_GPIO_EN_MSK, SOFT_RESET_AHB_RSTN_SW_GPIO_MSK);
}

void sys_clk_disable_gpio(void)
{
	sys_clk_ahb_disable(CLK_GATE_AHB_HCLK_GPIO_EN_MSK, SOFT_RESET_AHB_RSTN_SW_GPIO_MSK);
}

void sys_clk_enable_spi0(void)
{
	sys_clk_ahb_enable(CLK_GATE_AHB_HCLK_SPI_0_EN_MSK, SOFT_RESET_AHB_RSTN_SW_SPI_0_MSK);
}

void sys_clk_enable_spi1(void)
{
	sys_clk_ahb_enable(CLK_GATE_AHB_HCLK_SPI_1_EN_MSK, SOFT_RESET_AHB_RSTN_SW_SPI_1_MSK);
}

void sys_clk_disable_spi0(void)
{
	sys_clk_ahb_disable(CLK_GATE_AHB_HCLK_SPI_0_EN_MSK, SOFT_RESET_AHB_RSTN_SW_SPI_0_MSK);
}

void sys_clk_disable_spi1(void)
{
	sys_clk_ahb_disable(CLK_GATE_AHB_HCLK_SPI_1_EN_MSK, SOFT_RESET_AHB_RSTN_SW_SPI_1_MSK);
}

void sys_clk_enable_dma(void)
{
	sys_clk_ahb_enable(CLK_GATE_AHB_HCLK_DMA_EN_MSK, SOFT_RESET_AHB_RSTN_SW_DMA_MSK);
}

void sys_clk_disable_dma(void)
{
	sys_clk_ahb_disable(CLK_GATE_AHB_HCLK_DMA_EN_MSK, SOFT_RESET_AHB_RSTN_SW_DMA_MSK);
}

void sys_clk_enable_pwm(void)
{
	sys_clk_apb_enable(CLK_GATE_APB_PCLK_PWM_EN_MSK, SOFT_RESET_APB_RSTN_SW_PWM_MSK);
}

void sys_clk_disable_pwm(void)
{
	sys_clk_apb_disable(CLK_GATE_APB_PCLK_PWM_EN_MSK, SOFT_RESET_APB_RSTN_SW_PWM_MSK);
}

void sys_clk_enable_i2c(void)
{
	sys_clk_apb_enable(CLK_GATE_APB_PCLK_I2C_EN_MSK, SOFT_RESET_APB_RSTN_SW_I2C_MSK);
}

void sys_clk_disable_i2c(void)
{
	sys_clk_apb_disable(CLK_GATE_APB_PCLK_I2C_EN_MSK, SOFT_RESET_APB_RSTN_SW_I2C_MSK);
}

void sys_clk_enable_uart0(void)
{
	sys_clk_apb_enable(CLK_GATE_APB_PCLK_UART_0_EN_MSK, SOFT_RESET_APB_RSTN_SW_UART_0_MSK);
}

void sys_clk_disable_uart0(void)
{
	sys_clk_apb_disable(CLK_GATE_APB_PCLK_UART_0_EN_MSK, SOFT_RESET_APB_RSTN_SW_UART_0_MSK);
}

void sys_clk_enable_uart1(void)
{
	sys_clk_apb_enable(CLK_GATE_APB_PCLK_UART_1_EN_MSK, SOFT_RESET_APB_RSTN_SW_UART_1_MSK);
}

void sys_clk_disable_uart1(void)
{
	sys_clk_apb_disable(CLK_GATE_APB_PCLK_UART_1_EN_MSK, SOFT_RESET_APB_RSTN_SW_UART_1_MSK);
}

void sys_clk_enable_uart2(void)
{
	sys_clk_apb_enable(CLK_GATE_APB_PCLK_UART_2_EN_MSK, SOFT_RESET_APB_RSTN_SW_UART_2_MSK);
}

void sys_clk_disable_uart2(void)
{
	sys_clk_apb_disable(CLK_GATE_APB_PCLK_UART_2_EN_MSK, SOFT_RESET_APB_RSTN_SW_UART_2_MSK);
}

void sys_clk_enable_timer0(void)
{
	sys_clk_apb_enable(CLK_GATE_APB_PCLK_TIMER_0_EN_MSK, SOFT_RESET_APB_RSTN_SW_TIMER_0_MSK);
}

void sys_clk_enable_timer1(void)
{
	sys_clk_apb_enable(CLK_GATE_APB_PCLK_TIMER_1_EN_MSK, SOFT_RESET_APB_RSTN_SW_TIMER_1_MSK);
}

void sys_clk_enable_timer2(void)
{
	sys_clk_apb_enable(CLK_GATE_APB_PCLK_TIMER_2_EN_MSK, SOFT_RESET_APB_RSTN_SW_TIMER_2_MSK);
}

void sys_clk_enable_timer3(void)
{
	sys_clk_apb_enable(CLK_GATE_APB_PCLK_TIMER_3_EN_MSK, SOFT_RESET_APB_RSTN_SW_TIMER_3_MSK);
}

void sys_clk_disable_timer0(void)
{
	sys_clk_apb_disable(CLK_GATE_APB_PCLK_TIMER_0_EN_MSK, SOFT_RESET_APB_RSTN_SW_TIMER_0_MSK);
}

void sys_clk_disable_timer1(void)
{
	sys_clk_apb_disable(CLK_GATE_APB_PCLK_TIMER_1_EN_MSK, SOFT_RESET_APB_RSTN_SW_TIMER_1_MSK);
}

void sys_clk_disable_timer2(void)
{
	sys_clk_apb_disable(CLK_GATE_APB_PCLK_TIMER_2_EN_MSK, SOFT_RESET_APB_RSTN_SW_TIMER_2_MSK);
}

void sys_clk_disable_timer3(void)
{
	sys_clk_apb_disable(CLK_GATE_APB_PCLK_TIMER_3_EN_MSK, SOFT_RESET_APB_RSTN_SW_TIMER_3_MSK);
}
