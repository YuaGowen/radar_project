/**
 *******************************************************************************
 * @file    cfar.h
 * @author  Airtouch Software Team
 * @brief   FFT Low-Level Driver
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

#ifndef __CFAR_H__
#define __CFAR_H__

typedef void (*cfar_irq_callback)(void);

typedef struct cfar_vel_para_cfg {
    u8 is_cyclic;
    u8 guard_cells;
    u8 ref_cells;
    u8 threshold;
}cfar_vel_para_cfg_t;

typedef struct cfar_range_para_cfg {
    u8 is_cyclic:1;
    u8 guard_cells:7;
    u8 ref_cells;
    u8 threshold;
    u8 threshold_vb0;
}cfar_range_para_cfg_t;

typedef struct cfar_det_bmap_addr_cfg {
    u32 cfar_range_bmap_addr;
    u32 cfar_vel_bmap_addr;
    u32 cfar_mix_bmap_addr;
}cfar_det_bmap_addr_cfg_t;

void cfar_enable(bool en);/* only support ca-cfar */
void cfar_irq_enable(bool en);
void cfar_vel_dim_cfg(cfar_vel_para_cfg_t vcfg);
void cfar_range_dim_cfg(cfar_range_para_cfg_t rcfg);
void cfar_abs_threshold_cfg(u32 abs_thr);
void cfar_det_bmap_addr_cfg(cfar_det_bmap_addr_cfg_t map_addr);
void cfar_irq_callback_set(cfar_irq_callback cfar_cb);

#endif
