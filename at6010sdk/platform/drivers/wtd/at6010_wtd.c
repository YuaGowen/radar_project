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
#include "CMSDK_CM0plus.h"
#include "clock.h"
#include "sys_ctrl.h"
#include "wtd.h"

static u32 wtd_mode = 0;

inline static void wtd_unlock(void)
{
	CMSDK_WATCHDOG->LOCK = 0x1ACCE551;
}

inline static void wtd_lock(void)
{
	CMSDK_WATCHDOG->LOCK = 0;
}

static void wtd_irq_clear(void)
{
	wtd_unlock();
	CMSDK_WATCHDOG->INTCLR = CMSDK_Watchdog_INTCLR_Msk;
	wtd_lock();
}

void wtd_isr(void)
{
	if (wtd_mode == (u32)WTD_MODE_RESET) {
		wtd_feed(0U);
		for (;;) {}
	}
	wtd_irq_clear();
}

void wtd_feed(u32 reload_ms)
{
	wtd_unlock();
	CMSDK_WATCHDOG->LOAD = reload_ms * (clk_mcu_get() / 1000U);
	wtd_lock();
}

void wtd_enable(void)
{
	wtd_unlock();
	CMSDK_WATCHDOG->CTRL = wtd_mode;
	wtd_lock();
}

void wtd_disable(void)
{
	wtd_unlock();
	CMSDK_WATCHDOG->CTRL = 0;
	wtd_lock();
}

void wtd_setup(wtd_mode_t mode, u32 reload_ms)
{
	switch (mode) {
	case WTD_MODE_RESET:
		wtd_mode = CMSDK_Watchdog_CTRL_RESEN_Msk | CMSDK_Watchdog_CTRL_INTEN_Msk;
		break;
	case WTD_MODE_IRQ:
		wtd_mode = CMSDK_Watchdog_CTRL_INTEN_Msk;
		break;
	default :
		wtd_mode = 0;
	}
	wtd_feed(reload_ms);
}

void wtd_init(u32 reload_ms)
{
	hwp_sys_ctrl->clk_gate_apb |= CLK_GATE_APB_PCLK_WDG_EN_MSK;
	hwp_sys_ctrl->soft_reset_apb |= SOFT_RESET_APB_RSTN_SW_WDG_MSK;
	wtd_setup(WTD_MODE_RESET, reload_ms);
}
