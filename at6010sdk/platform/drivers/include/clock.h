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
#ifndef __CLOCK_H__
#define __CLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_common.h"

/* Global Data Types ---------------------------------------------------------*/

typedef enum {
	XTAL_TYPE_40MHZ = 0,
	XTAL_TYPE_32MHZ = 1
} xtal_type_t;

typedef enum {
	CLK_SRC_RC_32K,
	CLK_SRC_XTAL,
	CLK_SRC_PLL
} clk_src_t;

/* Global Function Declarations ----------------------------------------------*/

void clock_init(void);
xtal_type_t clock_xtal_type_get(void);
void pll_ana_clk_config(u8 xtal_type);
void set_pll_clk_divider(s8 div);
void disable_pll_clk(void);
void set_sys_clk_divider(s8 div);
void set_spi_flash_clk_divider(s8 div);
void sys_clk_set_adc_clk(u8 xtal_type);

void clk_mcu_src_set(u8 clk_src);

/*******************************************************************************
**                      Clock Frequency Configuration                         **
*******************************************************************************/

u32 xtal_freq_get(void);
void clk_mcu_set(clk_src_t src, u32 freq);
u32 clk_mcu_get(void);

/*******************************************************************************
**                      Module Clock Configuration                            **
*******************************************************************************/

/**
 * @brief Set the ADC/sdm sampling edge.
 *
 * @param inv 1 -- rising edge, 0 -- falling edge
 */
void sys_clk_adc_sdm_inv_set(u8 adc_inv,u8 sdm_inv);

/*******************************************************************************
**                      Module Clock Enable/Disable                           **
*******************************************************************************/

void sys_clk_enable_radar(void);
void sys_clk_disable_radar(void);

void sys_clk_enable_gpio(void);
void sys_clk_disable_gpio(void);

void sys_clk_enable_spi0(void);
void sys_clk_enable_spi1(void);

void sys_clk_disable_spi0(void);
void sys_clk_disable_spi1(void);

void sys_clk_enable_dma(void);
void sys_clk_disable_dma(void);

void sys_clk_enable_dualtimer(void);
void sys_clk_disable_dualtimer(void);

void sys_clk_enable_pwm(void);
void sys_clk_disable_pwm(void);

void sys_clk_enable_i2c(void);
void sys_clk_disable_i2c(void);

void sys_clk_enable_uart0(void);
void sys_clk_disable_uart0(void);

void sys_clk_enable_uart1(void);
void sys_clk_disable_uart1(void);

void sys_clk_enable_uart2(void);
void sys_clk_disable_uart2(void);

void sys_clk_enable_timer0(void);
void sys_clk_disable_timer0(void);

void sys_clk_enable_timer1(void);
void sys_clk_disable_timer1(void);

void sys_clk_enable_timer2(void);
void sys_clk_disable_timer2(void);

void sys_clk_enable_timer3(void);
void sys_clk_disable_timer3(void);

#ifdef __cplusplus
}
#endif

#endif /* !__CLOCK_H__ */
