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

/**
 * @brief Common configuration
 */

/* The number of ADC channels to be processed in each radar frame, which is typically equal to RX number.
   E.g. If the RX number is equal to @ref RADAR_RX_NUM_2, then FFT_ADC_CH_NUM = 2. */
#define FFT_ADC_CH_NUM           (RADAR_PRF_RFE_RX_NUM+1)


/**
 * @brief Range-FFT configuration
 */
#define RANGE_FFT_SIZE           (256)  /* FFT point number, which is equal to the ADC chirp size in most cases. */
#define RANGE_FFT_OUT_RB         (64)   /* Range-FFT output valid Range-Bin */
#define RANGE_FFT_REMOVE_DC      (0)    /* remove dc */
#define RANGE_FFT_WIN_EN         (1)

/* fft input data point,equal to chirp size in most cases.
   E.g. if @ref chirp_size equal to 256, RANGE_FFT_INPUT_SIZE = 256 */
#define RANGE_FFT_INPUT_SIZE     (512)

/* The number of chirp to be processed in each radar frame,equal to chirp num in the most case
   E.g. if radar chirp_num equal to 5, RANGE_FFT_NFRAME = 5. */
#define RANGE_FFT_NFRAME         (64)


/**
 * @brief User-FFT configuration
 */
#define USER_FFT_SIZE            (RANGE_FFT_NFRAME)  /* FFT point number, which is equal to the ADC chirp size in most cases. */
#define RANGE_FFT_SHIFT_BIT      (4)                 /* Range-FFT right shift bits. */
#define USER_FFT_OUT_POINT       (64)                /* User-FFT output valid point. */
#define USER_FFT_NFRAME          (RANGE_FFT_OUT_RB)  /* User-FFT frame number. */

#define USER_FFT_OUT_POWER       (0)                 /* Power Spectrum |X(k)|^2 or complex data |X(k)| */
#define USER_FFT_WIN_EN          (0)

#endif

