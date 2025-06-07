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

#include "at_types.h"
#include "clock.h"
#include "hal_clock.h"

//#define HAL_CLK_LOG_EN

#ifdef HAL_CLK_LOG_EN
#include "atlog.h"
#define HAL_CLK_LOG(...) loge(__VA_ARGS__)
#else
#define HAL_CLK_LOG(...)
#endif

typedef struct {
	clock_config cfg;
	u8 pll_clock_enable;
	u8 xtal_type;
	u8 padding[2];
} clk_inst_cfg_t;

static clk_inst_cfg_t m_clk_inst = {
    /* default clk setting */
    {
        .clk_system = SYS_CLK_80MHZ,
        .clk_spi_flash = SPI_FLASH_CLK_80MHZ,
    },
    .pll_clock_enable = 0,
    .xtal_type = 0,
};

void hal_clk_init(clk_setup_t clk_id)
{
	clock_init();
	m_clk_inst.pll_clock_enable = 1; /* default use pll */
	m_clk_inst.xtal_type = clock_xtal_type_get();
	pll_ana_clk_config(m_clk_inst.xtal_type);
	(void)hal_setup_clk(clk_id);
	return;
}

static void pll_clock_set(void)
{
	m_clk_inst.pll_clock_enable = 1;
	set_pll_clk_divider(0); /* 160MHz -> clk_mcu -> fclk/sclk */
}

static u8 is_pll_clk_enable(clock_config clk_cfg)
{
	u8 enable = 0;

	if (clk_cfg.clk_spi_flash >= SPI_FLASH_CLK_40MHZ) {
		enable = 1;
	} else if (clk_cfg.clk_system >= SYS_CLK_40MHZ) {
		enable = 1;
	} else if (clk_cfg.clk_system == SYS_CLK_80MHZ && m_clk_inst.xtal_type == XTAL_TYPE_40MHZ) {
		enable = 1;
	} else {
		enable = 0;
	}
	return enable;
}

static s8 sys_clk_set(u32 freq)
{
	s8 sys_clk_div = -1, ret = 0;

	switch (freq) {
	case SYS_CLK_160MHZ: {
		sys_clk_div = -1;
		break;
	}
	case SYS_CLK_80MHZ: {
		sys_clk_div = 0;
		break;
	}
	case SYS_CLK_40MHZ: {
		if (m_clk_inst.pll_clock_enable) {
			sys_clk_div = 1;
		} else {
			if (m_clk_inst.xtal_type == XTAL_TYPE_32MHZ) {
			} else if (m_clk_inst.xtal_type == XTAL_TYPE_40MHZ) {
			}
		}
		break;
	}
	default: ret = -1; break;
	}

	set_sys_clk_divider(sys_clk_div);

	return ret;
}

static s8 spi_flash_clk_set(u32 freq)
{
	s8 flash_clk_div = -1, ret = 0;

	switch (freq) {
	case SPI_FLASH_CLK_160MHZ: {
		flash_clk_div = -1;
		break;
	}
	case SPI_FLASH_CLK_80MHZ: {
		flash_clk_div = 0;
		break;
	}
	case SPI_FLASH_CLK_40MHZ: {
		if (m_clk_inst.pll_clock_enable) {
			flash_clk_div = 1;
		} else {
			if (m_clk_inst.xtal_type == XTAL_TYPE_32MHZ) {
			} else if (m_clk_inst.xtal_type == XTAL_TYPE_40MHZ) {
			}
		}
		break;
	}
	case SPI_FLASH_CLK_20MHZ: {
		if (m_clk_inst.pll_clock_enable) {
			flash_clk_div = 2;
		} else {
			if (m_clk_inst.xtal_type == XTAL_TYPE_32MHZ) {
			} else {
			}
		}
		break;
	}
	default:
		HAL_CLK_LOG("unsupport flash clock\n\r");
		ret = -1;
		break;
	}
	set_spi_flash_clk_divider(flash_clk_div);

	return ret;
}

u32 hal_get_spi_flash_clk(void)
{
	return m_clk_inst.cfg.clk_spi_flash;
}

u32 hal_get_system_clk(void)
{
	return m_clk_inst.cfg.clk_system;
}

u8 hal_get_xtal_type(void)
{
	return m_clk_inst.xtal_type;
}

void hal_setup_adc_clk(void)
{
	sys_clk_set_adc_clk(m_clk_inst.xtal_type);

	return;
}

/**
 *
 * @param clk_cfg System clock and flash clock
 * @return none
 *
 * @brief  Sets system clock and flash clock
 * pls configure the clock according to the following table
 *
 * sys clock    flash clock    note
 * 160M         160M/80M        high performance mode
 *  80M          80M/40M        normal mode
 *  40M          40M/20M        normal mode
 */

static void sys_flash_clk_config(clock_config clk_cfg)
{
	s8 ret = 0;

	if (is_pll_clk_enable(clk_cfg)) {
		pll_clock_set();
	} else {
		m_clk_inst.pll_clock_enable = 0;
		disable_pll_clk();
	}

	/* div_pll */
	clk_mcu_src_set(CLK_SRC_PLL); /* default use src pll */

	ret = sys_clk_set(clk_cfg.clk_system);
	if (ret) {
		HAL_CLK_LOG("invalid system clock\n\r ");
	} else {
		m_clk_inst.cfg.clk_system = clk_cfg.clk_system;
	}

	ret = spi_flash_clk_set(clk_cfg.clk_spi_flash);

	if (ret) {
		HAL_CLK_LOG("invalid flash clock\n\r ");
	} else {
		m_clk_inst.cfg.clk_spi_flash = clk_cfg.clk_spi_flash;
	}
	return;
}

s32 hal_setup_clk(clk_setup_t clk_id)
{
	s32 ret = 0;
	clock_config clk;

	switch (clk_id) {
	case CLK_SYS_160M_FLASH_160M_HZ: {
		clk.clk_system = SYS_CLK_160MHZ;
		clk.clk_spi_flash = SPI_FLASH_CLK_160MHZ;
		break;
	}
	case CLK_SYS_160M_FLASH_80M_HZ: {
		clk.clk_system = SYS_CLK_160MHZ;
		clk.clk_spi_flash = SPI_FLASH_CLK_80MHZ;
		break;
	}
	case CLK_SYS_80M_FLASH_80M_HZ: {
		clk.clk_system = SYS_CLK_80MHZ;
		clk.clk_spi_flash = SPI_FLASH_CLK_80MHZ;
		break;
	}
	case CLK_SYS_80M_FLASH_40M_HZ: {
		clk.clk_system = SYS_CLK_80MHZ;
		clk.clk_spi_flash = SPI_FLASH_CLK_40MHZ;
		break;
	}
	case CLK_SYS_40M_FLASH_40M_HZ: {
		clk.clk_system = SYS_CLK_40MHZ;
		clk.clk_spi_flash = SPI_FLASH_CLK_40MHZ;
		break;
	}
	case CLK_SYS_40M_FLASH_20M_HZ: {
		clk.clk_system = SYS_CLK_40MHZ;
		clk.clk_spi_flash = SPI_FLASH_CLK_20MHZ;
		break;
	}
	default: {
		HAL_CLK_LOG("unsupport frequency\n\r");
		ret = -1;
		break;
	}
	}
	if (ret == 0)
		sys_flash_clk_config(clk);

	return ret;
}

u32 hal_ms_to_ticks(u32 ms)
{
	return ms * (m_clk_inst.cfg.clk_system / 1000U);
}
