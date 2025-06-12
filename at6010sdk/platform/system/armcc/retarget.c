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


/******************************************************************************/
/* Retarget functions for ARM DS-5 Professional / Keil MDK (AC5)              */
/******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <rt_misc.h>

#ifdef CORTEX_M0PLUS
#include "CMSDK_CM0plus.h"
#endif

#include "jiffies.h"
#include "uart.h"
#include "common.h"

#pragma import(__use_no_semihosting_swi)

/* Private Constants ---------------------------------------------------------*/

#define DEFAULT_UART_CH                 UART_ID_0

#define TIM_DELAY_EN
#define DELAY_LOOP_CYCLES               5U

/* Private Types -------------------------------------------------------------*/

struct __FILE {
	int handle; /* Add whatever you need here */
};

/* Private Variables ---------------------------------------------------------*/

FILE __stdout;
FILE __stdin;


/* Low-level I/O Implementations ---------------------------------------------*/

int fputc(int ch, FILE *f)
{
	(void)f;
	return (int)(uart_send_byte(DEFAULT_UART_CH, ch));
}

int fgetc(FILE *f)
{
	(void)f;
	return (uart_send_byte(DEFAULT_UART_CH,
		uart_recv_byte(DEFAULT_UART_CH)));
}

int ferror(FILE *f)
{
	(void)f;
	/* Your implementation of ferror */
	return EOF;
}

/* System I/O Implementations ------------------------------------------------*/

void _ttywrch(int ch)
{
	uart_send_byte(DEFAULT_UART_CH, (u8)ch);
}

void _sys_exit(int return_code)
{
	(void)return_code;
	label:  goto label;  /* endless loop */
}

/* Delay us/ms Implementations -----------------------------------------------*/

#ifdef TIM_DELAY_EN
void delay_us(u32 us)
{
	u32 tmp;

	CMSDK_DUALTIMER1->TimerControl = 0;
	CMSDK_DUALTIMER1->TimerIntClr = 0;
	CMSDK_DUALTIMER1->TimerLoad = 26 * us;

	tmp = (0x0) << CMSDK_DUALTIMER_CTRL_PRESCALE_Pos | /* prescale, 0: 1, 1: 16, 2: 256. */
		(0x1) << CMSDK_DUALTIMER_CTRL_INTEN_Pos | /* interrupt, 0: disable, 1: enable. */
		CMSDK_DUALTIMER_CTRL_EN_Msk |
		CMSDK_DUALTIMER_CTRL_SIZE_Msk; /* 32-bits */

	CMSDK_DUALTIMER1->TimerControl = tmp;

	tmp = CMSDK_DUALTIMER1->TimerMIS;
	while (!tmp) {
		tmp = CMSDK_DUALTIMER1->TimerMIS;
	}

	CMSDK_DUALTIMER1->TimerControl = 0x00;
}
#else
void delay_us(u32 us)
{
	u32 tmp = us;

	tmp *= (SYS_CLK_FREQ/1000000U/DELAY_LOOP_CYCLES);

	while (tmp--) {
		__asm volatile ("nop");
	}
}
#endif /* !TIM_DELAY_EN */

#ifdef TIM_DELAY_EN
void delay_ms(u32 ms)
{
	u32 tmp;

	CMSDK_DUALTIMER1->TimerControl = 0;
	CMSDK_DUALTIMER1->TimerIntClr = 0;
	CMSDK_DUALTIMER1->TimerLoad = 1625 * ms;

	tmp = (0x1) << CMSDK_DUALTIMER_CTRL_PRESCALE_Pos | /* prescale, 0: 1, 1: 16, 2: 256. */
		(0x1) << CMSDK_DUALTIMER_CTRL_INTEN_Pos | /* interrupt, 0: disable, 1: enable. */
		CMSDK_DUALTIMER_CTRL_EN_Msk |
		CMSDK_DUALTIMER_CTRL_SIZE_Msk; /* 32-bits */

	CMSDK_DUALTIMER1->TimerControl = tmp;

	tmp = CMSDK_DUALTIMER1->TimerMIS;
	while (!tmp) {
		tmp = CMSDK_DUALTIMER1->TimerMIS;
	}

	CMSDK_DUALTIMER1->TimerControl = 0x00;
}
#else
void delay_ms(u32 ms)
{
	u32 tmp = ms;

	tmp *= (SYS_CLK_FREQ/1000U/DELAY_LOOP_CYCLES);

	while (tmp--) {
		__asm volatile ("nop");
	}
}
#endif /* !TIM_DELAY_EN */
