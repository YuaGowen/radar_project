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
 
#ifndef __FRAME_HEADER_H__
#define __FRAME_HEADER_H__

#define C_MAX_NUMVTRX (16)
#define C_MAX_SF  (8)

//3DWORD
typedef struct ATCommV2_RadarCommonDef_tag
{
    u16  syncBound;      //must be 0x7FFF
    u8   version;        //2
    u8   flag;           //bit0: 0-2bit/1-4bit
                              //bit1: 0:real/1-complex
                              //bit2,3: 0:raw bit 1:4bitSDM-16x 2:4bitSDM-8x
                              //bit4:   calibration
                              //bit5:   multi header mode
    u16  hwId;           //0xFxxx: Matlab Generated
    u16  swId;
    u32  rxSubFrameId;   //frameId = rxSubFrameId/numSubframe
}ATCommV2_RadarCommonDef;

//11DWORD
typedef struct ATCommV2_RadarSystem_tag
{
    u8 numTxAnt;
    u8 numRxAnt;
    u8 numVTRX;
    u8 numSubframe;   //

    u8 recommend_ts_exclusiveThreshold;  //255:Invalid
    u8 recommend_algo;    //bit0: 0-DBF  bit:1-DML
    u8 frameInterval;     //frame interval (ms)
    u8 errorCode;         //error code

    u8 stepRxH[C_MAX_NUMVTRX];
    u8 stepRxV[C_MAX_NUMVTRX];

    int8_t  fovHmin;
    int8_t  fovHmax;
    int8_t  fovVmin;
    int8_t  fovVmax;
}ATCommV2_RadarSystem;

//7DWORD
typedef struct ATCommV2_SubframeConfig_tag
{
    u16 numChirpPerSubFrame;
    u16 numSamplePerChirpForOneAnt;

    u16 recommend1DFFTSize;    //0: Invalid
    u16 recommend2DFFTSize;    //0: Invalid

    u32 samplingRate;

    u16 startFreqSmart;        //Fc<20G: Fc = startFreqSmart * 1MHz         (<20000)
                                    //Fc>20G: Fc = startFreqSmart * 2MHz + 20G;  (startFreqSmart - 20000)*2MHz + 20000MHz
    u16 sweepFreq100kHz;       //??50000

    u16 timeUnit;          // 1 = 5ns
    u16 upTime;            // ?timeUnit???????
    u16 dnTime;            // ?timeUnit???????
    u16 idTime;            // ?timeUnit???????
    u16 sfIntervalTime;    // ?timeUnit???subFrameInterval??
    u16 styleInfo;         // bit0-2: 0-no 1-intra 2-interComp 3-interTrad 4-Boschv4HMIMO
}ATCommV2_SubframeConfig;

typedef struct radar_frame_hdr_t {
	ATCommV2_RadarCommonDef radar_common_cfg;
	ATCommV2_RadarSystem    radar_system_cfg;
	ATCommV2_SubframeConfig radar_frame_cfg[C_MAX_SF];

}radar_frame_hdr_t;

void write_data_frame_header(void *buf);

#endif
