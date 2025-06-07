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

#define FFT_RSIZE                (1<<9) /* fft point,equal to adc chirp size in the  most case. */
#define FFT_OUT_VALID_RB         64
#define FFT_NUM_ADC_CH           2  /* The number of adc channels to be processed in each radar frame, equal to rx num in the  most case
                                     E.g.if rx num equal to @ref RADAR_RX_NUM_2 ,FFT_NUM_ADC_CH =2. */

//#define FFT_DC_CUT_ENABLE               /* remove dc */

#define FFT_INPUT_SIZE           (1<<9) /* fft input data point,equal to chirp size in the  most case.
                                           E.g.	if @ref chirp_size equal to 256, FFT_INPUT_SIZE = 256 */

#define FFT_NFRAME               16  /* The number of chirp to be processed in each radar frame,equal to chirp num in the most case
                                     E.g. if radar chirp_num equal to 5, FFT_NFRAME = 5. */

#define WIN_ENABLE

#endif

