/**
 *******************************************************************************
 * @file    hal_common.c
 * @author  Airtouch Software Team
 * @brief   HAL common Initialization
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

#include "clock.h"
#include "dma.h"
#include "gpio.h"
#include "wtd.h"

#ifndef CFG_OS
#include "jiffies.h"
#include "sys_timer.h"
#endif

#include "hal_common.h"
#include "hal_light.h"
#include "hal_key.h"
#include "hal_aux_key.h"
#include "hal_dualtimer.h"
#include "hal_ls.h"
#include "hal_ir.h"
#include "hal_clock.h"
#include "hal_uart.h"
#include "hal_relay.h"
#include "hal_random.h"

static void platform_clk_init(void)
{
	sys_clk_enable_gpio();
#ifdef CFG_LED_PWM
	sys_clk_enable_pwm();
#endif
	sys_clk_enable_dma();
}

static void platform_hal_driver_init(void)
{
	uart_cfg_t uart_cfg;

	hal_clk_init(SYS_FLASH_CLK_SEL);

	dma_enable_global(DMA1);

#ifdef CFG_WDT_SUPPORT
	wtd_init(WDT_TIME_MS);
	wtd_enable();
#endif

#ifndef CFG_OS
	sys_tick_init();
#endif

#ifdef CFG_GPIO_TOGGLE
	gpio_toggle_init(GPIO_TOGGLE_PIN);
#endif

	hal_dualtimer_init();

	uart_cfg.baud_rate = UART_LOG_BAUDRATE;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.irq_enable = 0;
	uart_cfg.fifo_cfg.fifo_en = 1U;
	(void)hal_uart_init(UART_LOG_PORT, &uart_cfg);

	(void)hal_light_init(LIGHT_PRIMARY, RADAR_OUT_PIN, PRIMARY_LIGHT_TYPE);

#ifdef CFG_AUX_LIGHT_EN
	hal_light_init(LIGHT_AUXILIARY, LIGHT_AUX_IO_PIN, AUX_LIGHT_TYPE);
#endif

#ifdef CFG_LS
	hal_ls_init();
#endif

#ifdef CFG_ADC_KEY
	hal_key_init();
#endif

#ifdef CFG_AUX_ADC_KEY
	hal_aux_key_init();
#endif

#ifdef CFG_RELAY_ZERO_PROTECTION
	hal_relay_init();
#endif
	hal_random_seed_init();
}

#ifndef CFG_OS
static void platform_system_init(void)
{
	sys_timer_init();
}
#endif

void hal_common_init(void)
{
	platform_clk_init();

	platform_hal_driver_init();
#ifndef CFG_OS
	platform_system_init();
#endif
}

/**
 * hal_intr_en() - Enable interrupt.
 *
 * @return None
 */
void hal_intr_en(void)
{
}

/**
 * hal_intr_dis() - Disable interrupt.
 *
 * @return None
 */
void hal_intr_dis(void)
{
}
