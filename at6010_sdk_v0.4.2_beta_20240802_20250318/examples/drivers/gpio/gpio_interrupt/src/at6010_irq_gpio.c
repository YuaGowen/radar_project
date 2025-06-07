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

#include "irq.h"
#include "gpio.h"

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
}

/**
 * @brief  This function handles Timer0 Handler.
 * @param  None
 * @retval None
 */
void TIMER0_Handler(void)
{
}

/**
 * @brief  This function handles Timer1 Handler.
 * @param  None
 * @retval None
 */
void TIMER1_Handler(void)
{
}

/**
 * @brief  This function handles Timer2 Handler.
 * @param  None
 * @retval None
 */
void TIMER2_Handler(void)
{
}

/**
 * @brief  This function handles Timer3 Handler.
 * @param  None
 * @retval None
 */
void TIMER3_Handler(void)
{
}

void DUALTIMER_Handler(void)
{
}

void UART0_Handler(void)
{
}

void UART1_Handler(void)
{
}

void UART2_Handler(void)
{
}

void SPI0_Handler(void)
{
}

void SPI1_Handler(void)
{
}

void I2C_Handler(void)
{
}

void GPIO_Handler(void)
{
	gpio_isr();
}

void DMA_Handler(void)
{
}

void RADAR_Handler(void)
{
}

void KEY_Handler(void)
{
}

void GPIO0_Handler(void)
{
	gpio_irq_hdl(0U);
}

void GPIO1_Handler(void)
{
	gpio_irq_hdl(1U);
}

void GPIO2_Handler(void)
{
	gpio_irq_hdl(2U);
}

void GPIO3_Handler(void)
{
	gpio_irq_hdl(3U);
}

void GPIO4_Handler(void)
{
	gpio_irq_hdl(4U);
}

void GPIO5_Handler(void)
{
	gpio_irq_hdl(5U);
}

void GPIO6_Handler(void)
{
	gpio_irq_hdl(6U);
}

void AON_WDT_Handler(void)
{
}
