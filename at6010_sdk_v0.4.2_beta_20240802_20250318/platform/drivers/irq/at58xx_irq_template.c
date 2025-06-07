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

void HardFault_Handler_c(u32 * hardfault_args, unsigned lr_value)
{
}
/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__asm void HardFault_Handler(void)
{
  MOVS   r0, #4
  MOV    r1, LR
  TST    r0, r1
  BEQ    stacking_used_MSP
  MRS    R0, PSP ; // first parameter - stacking was using PSP
  B      get_LR_and_branch
stacking_used_MSP
  MRS    R0, MSP ; // first parameter - stacking was using MSP
get_LR_and_branch
  MOV    R1, LR  ; // second parameter is LR current value
  LDR    R2,=__cpp(HardFault_Handler_c)
  BX     R2
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

void UARTRX0_Handler(void)
{
}

void UARTTX0_Handler(void)
{
}

void UARTRX1_Handler(void)
{
}

void UARTTX1_Handler(void)
{
}

void UARTOVF0_Handler(void)
{
}

void UARTOVF1_Handler(void)
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

void I2C_Handler(void)
{
}

void DMA_Handler(void)
{
}

void RADAR_Handler(void)
{
}

void PORT0_0_Handler(void)
{
}

void PORT0_1_Handler(void)
{
}

void PORT0_2_Handler(void)
{
}

void PORT0_3_Handler(void)
{
}

void PORT0_4_Handler(void)
{
}

void PORT0_5_Handler(void)
{
}

void PORT0_6_Handler(void)
{
}

void PORT0_7_Handler(void)
{
}

void PORT0_8_Handler(void)
{
}

void PORT0_9_Handler(void)
{
}

void PORT0_10_Handler(void)
{
}

void PORT0_11_Handler(void)
{
}

void RADAR_ADC_Handler(void)
{
}
