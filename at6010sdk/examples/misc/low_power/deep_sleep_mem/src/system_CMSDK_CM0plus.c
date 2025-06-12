/**************************************************************************//**
 * @file     system_CMSDK_CM0plus.c
 * @brief    CMSIS Cortex-M0+ Device Peripheral Access Layer Source File for
 *           Device <Device>
 * @version  V3.01
 * @date     06. March 2012
 *
 * @note
 * Copyright (C) 2010-2012 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#include <stdint.h>
#include "CMSDK_CM0plus.h"
#include "cache.h"
#include "common.h"

/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
extern uint32_t  __Vectors[];
#endif
extern int $Super$$main(void);

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/

#define XTAL    (100000000UL)            /* Oscillator frequency               */


/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemFrequency = XTAL;    /*!< System Clock Frequency (Core Clock)  */
uint32_t SystemCoreClock = XTAL;    /*!< Processor Clock Frequency            */


/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
	SystemCoreClock = XTAL;
}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit (void)
{
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
	SCB->VTOR = (uint32_t) &(__Vectors);
#endif

	NVIC_DisableIRQ(UART0_IRQn);
	__enable_irq();

	SystemCoreClock = XTAL;
}

/**
 * @brief  re-define main function.
 * @param  none
 * @return int
 */
#ifdef ULP_PROG_1
ATTR_RAM_SECTION int $Sub$$main(void)
{
	hwp_cache->config = CONFIG_CACHE_EN(0U);
	hwp_cache->config = CONFIG_CACHE_EN(1U) | CONFIG_CACHE_SIZE(0U) | CONFIG_MISS_CNT_EN(1U) |
	                    CONFIG_HIT_CNT_EN(1U) | CONFIG_CACHE_LINE_NOFLUSH_N(1U);
	hwp_cache->ctrl = CTRL_CACHE_LINE_FLUSH(1U);
	for (u32 i = 0U; i < 1000U; i++) {
		__NOP();
	}

	$Super$$main();
	return 0;
}
#endif
