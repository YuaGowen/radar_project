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

#ifndef __PMU_H__
#define __PMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_common.h"

typedef enum {
	PMU_BOOTMODE_INIT = 0x0,
	PMU_BOOTMODE_FLASH = 0x6a,
	PMU_BOOTMODE_SRAM0 = 0x6b,
	PMU_BOOTMODE_SRAM1 = 0x6c,
	PMU_BOOTMODE_ADDR = 0x6d,
} pmu_bootmode_t;

typedef enum {
	PMU_WAKEUP_LIGHT = 0x1,
	PMU_WAKEUP_KEY   = 0x2,
	PMU_WAKEUP_TIMER = 0x4,
	PMU_WAKEUP_ALL   = 0x7
} pmu_wakeup_t;

typedef enum {
	PMU_RETENTION_NONE      = 0x00U,
	PMU_RETENTION_RF        = 0x01U,
	PMU_RETENTION_RADAR     = 0x02U,
	PMU_RETENTION_RADAR_MEM = 0x04U,
	PMU_RETENTION_SRAM_0    = 0x08U,
	PMU_RETENTION_SRAM_1    = 0x10U,
	PMU_RETENTION_ALL       = 0x1FU
} pmu_retention_t;

u8 pmu_reg_read(u8 reg);
void pmu_reg_write(u8 reg, u8 dat);
void pmu_reg_field_write(u8 reg, u8 field_mask, u8 field_val);
void pmu_por_osc_enable(void);
void pmu_por_osc_disable(void);

void pmu_rf_pwr_on(void);
void pmu_rf_pwr_off(void);

void pmu_ldo_enable(void);
void pmu_clk_dcdc_set(u32 freq);

/**
 * @brief pmu set/get software data, 32 bytes total
 * @param id byte index
 */
void pmu_set_sw_data(u8 id, u8 value);
u8   pmu_get_sw_data(u8 id);

void pmu_set_bootmode(pmu_bootmode_t mod);
pmu_bootmode_t pmu_get_bootmode(void);
void pmu_set_boot_addr(u32 addr);
void pmu_set_wakeup_time(u32 ms);

/**
 * @brief pmu set retention
 * @param val 0: disable retention, 1: enable retention
 */
void pmu_set_retention(u8 val);

void pmu_set_sleep_req(u8 val);

void pmu_enter_sleep(void);

void pmu_retention_select(pmu_retention_t region);

void pmu_enable_wakeup_source(pmu_wakeup_t source);

void pmu_disable_wakeup_source(pmu_wakeup_t source);

void pmu_enable_gpio_wakeup(u8 mask);

void pmu_disable_gpio_wakeup(u8 mask);

/**
 * @brief pmu set flash pu/pd.
 * @param val 0: disable skip flash pu/pd, 1: enable skip flash pu/pd
 */
void pmu_skip_flash_pupd(u8 val);

/**
 * @brief pmu set dvs.
 * @param val 0: disable dvs, 1: enable dvs
 */
void pmu_set_dvs(u8 val);

/**
 * @brief pmu cache power down.
 * @param none.
 */
void pmu_set_cache_pd(void);

/**
 * @brief pmu set dig vcore.
 * @param val 0 ~ 7
 */
void pmu_set_dig_vcore(u8 val);

#ifdef __cplusplus
}
#endif

#endif /* !__PMU_H__ */
