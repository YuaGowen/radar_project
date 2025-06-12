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
 
#include "at_types.h"
#include "frame_header.h"

#define C_NUM_RXANT     2
#define C_NUM_SUBFRAME  1
#define C_NUM_CHIRP     5
#define C_NUM_SAMPLE_PER_CHIRP   256

static u32 convertFreqToFreqSmart(u32 freqInMHz)
{
    return (freqInMHz < 20000u) ? (freqInMHz) : ((freqInMHz - 20000u) / 2 + 20000u);
}

#if 0
static u32 convertFreqSmartToFreqMHz(u32 freqSmart)
{
    return (freqSmart < 20000u) ? (freqSmart) : (freqSmart - 20000u) * 2 + 20000u;
}
#endif

void write_data_frame_header(u8 *buf)
{
	u8 prf_id = 0;
	ATCommV2_RadarCommonDef* pHdr= (ATCommV2_RadarCommonDef*)buf;
	ATCommV2_RadarSystem* pSys = (ATCommV2_RadarSystem*)(pHdr+1);
	ATCommV2_SubframeConfig* pSF = (ATCommV2_SubframeConfig*)(pSys+1);

	pHdr->syncBound = 0x7FFF;
	pHdr->version = 2;
	pHdr->flag = (1<<5);
	pHdr->hwId = 60;
	pHdr->swId = 2118;
	pHdr->rxSubFrameId = 0U;

	pSys->numTxAnt = 1;
	pSys->numRxAnt = C_NUM_RXANT;
	pSys->numVTRX = C_NUM_RXANT;
	pSys->numSubframe = C_NUM_SUBFRAME;
	pSys->recommend_ts_exclusiveThreshold = 255;
	pSys->recommend_algo = 0;
	pSys->frameInterval = 25;
	pSys->errorCode = 0;

	u32* stepByte = (u32*)pSys->stepRxH;
	for (int32_t cnt = 0; cnt < C_MAX_NUMVTRX * 2/4; cnt++)
	{
		stepByte[cnt] = 0;
	}
	pSys->stepRxH[1] = 1;
	pSys->fovHmin = -60;
	pSys->fovHmax = 60;
	pSys->fovVmin = -60;
	pSys->fovVmax = 60;

	pSF[prf_id].numChirpPerSubFrame = C_NUM_CHIRP;
	pSF[prf_id].numSamplePerChirpForOneAnt = C_NUM_SAMPLE_PER_CHIRP;
	pSF[prf_id].recommend1DFFTSize = 0;
	pSF[prf_id].recommend2DFFTSize = 0;
	pSF[prf_id].sweepFreq100kHz = 10000;
	pSF[prf_id].samplingRate = 10000000;
	pSF[prf_id].startFreqSmart = convertFreqToFreqSmart(61000);
	pSF[prf_id].timeUnit = 2;
	pSF[prf_id].upTime = 3000;
	pSF[prf_id].dnTime = 1000;
	pSF[prf_id].idTime = 1000;
	pSF[prf_id].sfIntervalTime = 100;
	pSF[prf_id].styleInfo = 0;
}
