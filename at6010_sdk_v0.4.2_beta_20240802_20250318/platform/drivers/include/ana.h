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

#ifndef __ANA_H__
#define __ANA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_types.h"

typedef enum {
    ANA_RESET_CTRL       = 0x00, // reset_ctrl
    ANA_DEVICE_ID        = 0x01, // device_id
    ANA_EFUSE_CTRL       = 0x02, // efuse_ctrl
    ANA_EFUSE_PGM_TIME_0 = 0x03, // efuse_pgm_time_0
    ANA_EFUSE_PGM_TIME_1 = 0x04, // efuse_pgm_time_1
    ANA_EFUSE_WDATA_0    = 0x05, // efuse_wdata_0
    ANA_EFUSE_WDATA_1    = 0x06, // efuse_wdata_1
    ANA_EFUSE_WDATA_2    = 0x07, // efuse_wdata_2
    ANA_EFUSE_WDATA_3    = 0x08, // efuse_wdata_3
    ANA_EFUSE_RDATA_0    = 0x09, // efuse_rdata_0
    ANA_EFUSE_RDATA_1    = 0x0A, // efuse_rdata_1
    ANA_EFUSE_RDATA_2    = 0x0B, // efuse_rdata_2
    ANA_EFUSE_RDATA_3    = 0x0C, // efuse_rdata_3
    ANA_EFUSE_STATUS     = 0x0D, // efuse_status
    ANA_AUX_ADC_CTRL     = 0x0E, // aux_adc_ctrl
    ANA_AUX_ADC_STATUS_0 = 0x0F, // aux_adc_status_0
    ANA_AUX_ADC_STATUS_1 = 0x10, // aux_adc_status_1
    ANA_KEY_CTRL         = 0x11, // key_ctrl
    ANA_KEY_SMOOTH_HIGH  = 0x12, // key_smooth_high
    ANA_KEY_SMOOTH_LOW   = 0x13, // key_smooth_low
    ANA_IRQ_CTRL         = 0x14, // irq_ctrl
    ANA_IRQ_STATUS       = 0x15, // irq_status
} ana_reg_t;


// FIXME: all the following functions are not implemented

void ana_domain_init(void);

u32  ana_reg_read(u32 reg);
u8   ana_reg_write(u32 reg, u32 dat);
u8   ana_reg_field_write(u32 reg, u32 field_mask, u32 field_val);

u32  get_efuse(void);

#ifdef __cplusplus
}
#endif

#endif /* !__ANA_H__ */
