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

#ifndef __CFG_RADAR_HWA_H__
#define __CFG_RADAR_HWA_H__

#define FFT_RSIZE                (1<<9) /* range fft point,equal to adc chirp size in the  most case. */
#define FFT_VSIZE                (1<<6) /* doppler fft point,equal to adc chirp num in the  most case. */

#define FFT_INPUT_SIZE           (1<<9) /* fft input data point,equal to chirp size in the  most case. */

#define FFT_OUT_VALID_RB         96
#define FFT_OUT_VALID_VB         FFT_VSIZE  /* always equal to FFT_VSIZE. */

#define FFT_DC_CUT_ENABLE               /* remove dc */
#define FFT_OUT_IS_POWER         1      /* Power Spectrum |X(k)|^2 or complex data |X(k)| */

#define FFT_NUM_ADC_CH           1  /* The number of adc channels to be processed in each frame, equal to rx num in the  most case */

#define CFAR_MIRROR_PADDING        0
#define CFAR_CYCLIC_PADDING        1

#define CFAR_RANGE_BITMAP_SIZE    (((FFT_OUT_VALID_RB + 31)/32) * FFT_OUT_VALID_VB)  /* ca_cfar range bitmap size */
#define CFAR_VEL_BITMAP_SIZE      (FFT_OUT_VALID_RB * ((FFT_OUT_VALID_VB + 31 )/32)) /* ca_cfar velocity bitmap size */
#define CFAR_MIX_BITMAP_SIZE      (FFT_OUT_VALID_RB * ((FFT_OUT_VALID_VB + 31 )/32)) /* ca_cfar det result,range && velocity final bitmap */

#define CFAR_THR_ABS                     20000   /* cfar absolute threshold, cfar process only if data > CFAR_THR_ABS, */
#define CFAR_VEL_BOUNDARY_PADDING        CFAR_CYCLIC_PADDING  /* Velocity cfar padding mode, mirror or cyclic */
#define CFAR_VEL_GUARD_CELL              2 /* Velocity cfar guard cell */
#define CFAR_VEL_REF_CELL                8 /* Velocity cfar ref cell,inclue guard cell, total ref cell = left cell (ref cell - guard cell) + right cell (ref cell - guard cell) */
#define CFAR_VEL_THR_FACTOR              50 /* cfar threshold factor, cell value > (sum(total_ref_cell_value)/total_ref_cell * CFAR_VEL_THR_FACTOR/2) */

#define CFAR_RGE_BOUNDARY_PADDING        CFAR_MIRROR_PADDING /* range cfar padding mode, mirror or cyclic */
#define CFAR_RGE_GUARD_CELL              2 /* range cfar guard cell */
#define CFAR_RGE_REF_CELL                8 /* range cfar ref cell,inclue guard cell, total ref cell = left cell (ref cell - guard cell) + right cell (ref cell - guard cell) */
#define CFAR_RGE_THR_FACTOR              40  /* range cfar threshold factor, cfar alarm if cell value > (sum(total_ref_cell_value)/total_ref_cell * CFAR_VEL_THR_FACTOR/2) */
#define CFAR_RGE_THR_VB0                 100 /* doppler fft idx0 threshold factor, if cell idx = idx0 ,set CFAR_RGE_THR_VB0 to cfar threshold factor */

#define WIN_ENABLE
#define CA_CFAR_ENABLE

#endif

