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

#include <stdlib.h>
#include "stdio.h"
#include "pmu.h"
#include "string.h"
#include "CMSDK_CM0plus.h"
#include "output_config.h"
#include "sys_ctrl.h"
#include "at_types.h"
#include "bytestream.h"
#include "frame_header.h"
#include "radar.h"
#include "radar_frame.h"
#include "radar_adc.h"
#include "hal_fft.h"
#include "clock.h"
#include "hal_uart.h"
#include "hal_dualtimer.h"
#include "cpu.h"
#include "radar_fmcw.h"
#include "cfg_paras.h"
#include "fft_win.h"
#include "hal_timer.h"
#include "cfg_radar_profile.h"
#include "cfg_paras.h"
#include <math.h>
#include <float.h>

#define FFT_BUF_ATTRIBUTES  __attribute__ ((section(".radar_mem"),__aligned__(32)))
#define FFT_WIN_ATTRIBUTES  __attribute__ ((section(".fft_win_mem"),__aligned__(32)))

/*********************************************************************************************************
* ��������
******************************************************************************************************** */
void adc_rdat_end_isr(void);
void adc_rdat_half_isr(void);
void rangeFftIsr(void);
void velocFftIsr(void);

/*********************************************************************************************************
* �궨��
******************************************************************************************************** */
#define CHIRPS                   NUM_CHIRP /* ֡��chirp���� */
#define SAMPLES                  NUM_SAMPLE_PER_CHIRP /* Chirp�ڲ�����������ͨ��Ҳ��һάFFT���� */
#define FFTS_1D                  9 /* 1D FFT��������ӦFFT����Ϊ2^FFTS_1D */
#define FFTS_2D                  4 /* 2D FFT��������ӦFFT����Ϊ2^FFTS_2D */
#define FFTN_RB                  64 /* һ��FFT���Range Bin������һ���������෶Χ�趨 */
#define FFTN_VB                  CHIRPS /* ����FFT���Velocity Bin������һ����������������趨 */

/*********************************************************************************************************
* ���º궨�������޸�
******************************************************************************************************** */
#define FFTN_1D                  (1UL << FFTS_1D) /* 1D FFT������ͨ����Chirp�ڲ�����������ͬ */
#define FFTN_2D                  (1UL << FFTS_2D) /* 2D FFT������ͨ����֡��Chirp������ͬ */
#define RX_ANTS                  (1 + (RADAR_PRF_RFE_RX_NUM == RADAR_RX_NUM_2)) /* ����ͨ���� */
#define DUMP_NONE                0 /* ������κ�֡���� */
#define DUMP_RAW                 1 /* ���RAW DATA���� */
#define DUMP_RANGE               2 /* ���RANGE BIN���� */
#define DUMP_VELOC               3 /* ���VELOC BIN���� */
#define SYS_TICK_FREQ            1000UL /* ϵͳ��ʱ��Ƶ��,Ĭ��Ϊ1KHz,�����޸� */

/*********************************************************************************************************
* �����������ѡ��
******************************************************************************************************** */
#define DUMP_TYPE                DUMP_VELOC /* DUMP_RAW=���RAW DATA����,DUMP_RANGE=���RANGE BIN����,DUMP_VELOC=���VELOC BIN���� */

/*********************************************************************************************************
* �궨��������
******************************************************************************************************** */
#if (SAMPLES != FFTN_1D) /* CHIRP����������1D FFT������ͬ����������Ԥ�ڣ��ɺ��Դ˾��棡 */
#pragma message("The value of SAMPLES is different from FFTN_1D. If it meets the expectations, this warning can be ignored!")
#endif
#if (CHIRPS != FFTN_2D) /* CHIRP������2D FFT������ͬ����������Ԥ�ڣ��ɺ��Դ˾��棡 */
#pragma message("The value of CHIRPS is different from FFTN_2D. If it meets the expectations, this warning can be ignored!")
#endif
#if (FFTN_RB > (FFTN_1D >> 1)) /* ���Range Bin����������FFT������һ�룬��������Ԥ�ڣ��ɺ��Դ˾��棡 */
#pragma message("The value of FFTN_RB is greater than (FFTN_1D >> 1). If it meets the expectations, this warning can be ignored!")
#endif

/*********************************************************************************************************
* ȫ�ֱ�����������
******************************************************************************************************** */
short GsIfAdc[1][CHIRPS][SAMPLES][RX_ANTS] FFT_BUF_ATTRIBUTES = {0}; /* ����1֡ADC���ݵĻ��� */
#if ((DUMP_TYPE == DUMP_RANGE) || (DUMP_TYPE == DUMP_VELOC))
short GsFrame[RX_ANTS][CHIRPS][SAMPLES] FFT_BUF_ATTRIBUTES = {0}; /* һ��FFT�任������(֡)���� */
short GsRange[RX_ANTS][CHIRPS][FFTN_RB][2] FFT_BUF_ATTRIBUTES = {0}; /* һ��FFT�任���������� */
short GsVecIn[RX_ANTS][FFTN_RB][CHIRPS][2] FFT_BUF_ATTRIBUTES = {0}; /* ����FFT�任ǰ�����뻺�� */
#if (DUMP_TYPE == DUMP_VELOC)
short GsVeloc[RX_ANTS][FFTN_RB][FFTN_VB][2] FFT_BUF_ATTRIBUTES = {0}; /* ����FFT�任���������� */
#endif
#endif
volatile unsigned long g_frame_id = 0; /* ֡ID */
#if (DUMP_TYPE != DUMP_NONE)
volatile radar_frame_hdr_t GtFrameHeader = {0}; /* ���� */
#endif

volatile unsigned long uwTick = 0; /* ��ʱ���� */

/* ���ڲ�������,Ĭ��Log���������Ϊ921600, ������������Log���ڸ��ã������ʽ���output_dma_config()�������� */
uart_cfg_t GtUartCfg = {
    .baud_rate = UART_BAUD_RATE_921600,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .data_width = UART_DATA_WIDTH_8BIT,
    .fifo_cfg.fifo_en = 1U,
};

/* 1D FFT�����ж���,�����г��ȱ�����FFT������ͬ */
unsigned short GusRangeWin[] FFT_WIN_ATTRIBUTES = {
    #if (FFTN_1D == 8)
    u16_hann8(),
    #elif (FFTN_1D == 16)
    u16_hann16(),
    #elif (FFTN_1D == 32)
    u16_hann32(),
    #elif (FFTN_1D == 64)
    u16_hann64(),
    #elif (FFTN_1D == 128)
    u16_hann128(),
    #elif (FFTN_1D == 256)
    u16_hann256(),
    #elif (FFTN_1D == 512)
    u16_hann512(),
    #elif (FFTN_1D == 1024)
    u16_hann1024(),
    #else
    #error "�����г��ȴ�����ע���飡"
    #endif
};

/* 2D FFT�����ж���,�����г��ȱ�����FFT������ͬ */
unsigned short GusVelocWin[] FFT_WIN_ATTRIBUTES = {
    #if (FFTN_2D == 8)
    u16_hann8(),
    #elif (FFTN_2D == 16)
    u16_hann16(),
    #elif (FFTN_2D == 32)
    u16_hann32(),
    #elif (FFTN_2D == 64)
    u16_hann64(),
    #elif (FFTN_2D == 128)
    u16_hann128(),
    #elif (FFTN_2D == 256)
    u16_hann256(),
    #elif (FFTN_2D == 512)
    u16_hann512(),
    #elif (FFTN_2D == 1024)
    u16_hann1024(),
    #else
    #error "�����г��ȴ�����ע���飡"
    #endif
};

/* User FFTģʽ�µ�1D FFT������ */
fft_win_para_t GtRangeWin = {
    .enable = 1, /* 1=ʹ��Ӳ��FFT�Զ��Ӵ�,0=����Ӳ��FFT�Զ��Ӵ� */
    .fft_mode = FFT_USER_FFT_MODE, /* FFT���湤������ */
    .waddr = (u32)GusRangeWin, /* �������׵�ַ */
};

/* User FFTģʽ�µ�2D FFT������ */
fft_win_para_t GtVelocWin = {
    .enable = 1, /* 1=ʹ��Ӳ��FFT�Զ��Ӵ�,0=����Ӳ��FFT�Զ��Ӵ� */
    .fft_mode = FFT_USER_FFT_MODE, /* FFT���湤������ */
    .waddr = (u32)GusVelocWin, /* �������׵�ַ */
};

#if ((DUMP_TYPE == DUMP_RANGE) || (DUMP_TYPE == DUMP_VELOC))
/* User FFTģʽ�µ�1D FFT���� */
user_fft_para_t GtRangeCfg = {
    .data_type = REAL_DATA, /* FFT�任���ͣ�REAL_DATA=FFT�任����������Ϊʵ����COMPLEX_DATA=FFT�任����������Ϊ���� */
    .is_power_out  = 0, /* 0=�������,1=���ģƽ�� */
    .shift_value   = FFTS_1D, /* FFT���������λ�� */
    .fft_point     = FFTN_1D, /* FFT���� */
    .nframe        = CHIRPS * RX_ANTS, /* ��Ҫ��FFT����Ĵ�����1D FFT��ӦFrame�ڵ���chirp���� */
    .in_frame_size = SAMPLES, /* ÿ��FFT����������������1D FFT��Ӧchirp�ڲ��������� */
    .out_frame_size = FFTN_RB, /* ÿ��FFT�����������������1D FFT��Ӧ���Range Bin������ */
    .in_buf         = (u32)GsFrame, /* 1D FFT��������������׵�ַ */
    .out_buf        = (u32)GsRange, /* 1D FFT�������������׵�ַ */
    .end_callback   = rangeFftIsr, /* 1D FFTȫ��������ɺ�Ļص����� */
};
#endif

#if (DUMP_TYPE == DUMP_VELOC)
/* User FFTģʽ�µ�2D FFT���� */
user_fft_para_t GtVelocCfg = {
    .data_type = COMPLEX_DATA, /* FFT�任���ͣ�REAL_DATA=FFT�任����������Ϊʵ����COMPLEX_DATA=FFT�任����������Ϊ���� */
    .is_power_out  = 0, /* 0=�������,1=���ģƽ�� */
    .shift_value   = FFTS_2D, /* FFT���������λ�� */
    .fft_point     = FFTN_2D, /* FFT���� */
    .nframe        = FFTN_RB * RX_ANTS, /* ��Ҫ��FFT����Ĵ�����2D FFT��Ӧһ�׵���RangeBin���� */
    .in_frame_size = CHIRPS, /* ÿ��FFT����������������2D FFT��Ӧһ�׵�֡��Chirp���� */
    .out_frame_size = FFTN_VB, /* ÿ��FFT�����������������2D FFT��Ӧ�����VelocBin���� */
    .in_buf         = (u32)GsVecIn, /* 2D FFT��������������׵�ַ */
    .out_buf        = (u32)GsVeloc, /* 2D FFT�������������׵�ַ */
    .end_callback   = velocFftIsr, /* 2D FFTȫ��������ɺ�Ļص����� */
};
#endif

/* ��ƵADC�ɼ����ã���ƵADC��FFT���水֡�ɼ��� */
adc_rdat_para_t GtIfSampleCfg = {
    .enable = 1, /* 1=ʹ��FFT����ɼ���ƵADC,0=����FFT����ɼ���ƵADC */
    #if (RX_ANTS == 1)
    .num_adc_ch = FFT_MONO_ADC_CH, /* 0 = FFT_MONO_ADC_CH, 1 = FFT_DUAL_ADC_CH */
    #else
    .num_adc_ch = FFT_DUAL_ADC_CH, /* 0 = FFT_MONO_ADC_CH, 1 = FFT_DUAL_ADC_CH */
    #endif
    .nframe = CHIRPS, /* ÿ��ͨ����Ҫ�ɼ����ٸ�Chirp */
    .frame_size = SAMPLES, /* ÿ��Chirp��Ӧ�Ĳ��������� */
    .out_buf = (u32)GsIfAdc, /* ��Ƶ�ɼ���������׵�ַ */
    .end_callback = adc_rdat_end_isr, /* ��Ƶ�ɼ������ص�������half_int_en = 1ʱ����Ӧż��֡�ɼ����� */
    .half_callback = 0, /* adc_rdat_half_isr,//��Ƶ�ɼ������ص�������half_int_en = 1ʱ����Ӧ������֡�ɼ�������half_int_en = 0ʱ���˻ص��������ᱻ���� */
    .half_int_en = 0, /* 1=ʹ��˫֡���潻���Ųɼ����,0=����˫֡���潻���Ųɼ���� */
};

#define PI 3.14159265358979323846
double window[SAMPLES] = {0};
short  adc_value[SAMPLES] = {0};
/*********************************************************************************************************
* ��������adc_rdat_handle
* ������  ��Ƶ���ݣ�һ֡�����������ڴ˺����н��������г�Ӳ��FFT�����������˳��Ҫ��,������1D FFT����
*         ������FFT�任ǰ����Ƶ���ݽ���Ԥ��������ڴ˺��������
* ���룺  pvBuf��ָ��֡������׵�ַ
* ���أ�  ��
******************************************************************************************************** */
ATTR_RAM_SECTION void adc_rdat_handle (void *pvBuf)
{
    int i = 0, j = 0, k = 0;
    short (*sIfAdc)[CHIRPS][SAMPLES][RX_ANTS] = (short (*)[CHIRPS][SAMPLES][RX_ANTS])pvBuf;

    radar_rf_pwr_off(); /* Ϊ���͹��ģ��ɼ�����Ƶ�������ʱ�ر�RF RX��ص�Դ */

    #if ((DUMP_TYPE == DUMP_RANGE) || (DUMP_TYPE == DUMP_VELOC))
    /* ���������г�Ӳ��FFT�����������˳��Ҫ�� */
    for (i = 0; i < RX_ANTS; i++) {
        for (j = 0; j < CHIRPS; j++) {
            for (k = 0; k < SAMPLES; k++) {
                GsFrame[i][j][k] = ((*sIfAdc)[j][k][i] ) ;//* window[k];- adc_value[k]
//								if(i == 0 && j == 0)
//								{
//									adc_value[k] = (*sIfAdc)[j][k][i];
//								}
            }
        }
    }

    /* ��ʼ��FFT����������ò�����1D FFT���� */
    hal_fft_set_config(FFT_IDX_CONFIG_USER_WIN, (void *)&GtRangeWin); /* ��ʼ��FFT���������� */
    hal_fft_set_config(FFT_IDX_CONFIG_USER_FFT,(void*)&GtRangeCfg); /* ��GtRangeCfg���ó�ʼ��FFT���� */
    fft_process_trigger(FFT_USER_FFT_MODE); /* ����Ӳ��FFT�������FFT���� */
    #else
    g_frame_id++;
    #endif
}

/*********************************************************************************************************
* ��������adc_rdat_half_isr
* ������  �ɼ���һ֡��Ƶ���ݺ���жϴ�����(˫����ĵ�һ֡)
* ���룺  ��
* ���أ�  ��
******************************************************************************************************** */
ATTR_RAM_SECTION void adc_rdat_half_isr(void)
{
    adc_rdat_handle(GsIfAdc[0]);
}

/*********************************************************************************************************
* ��������adc_rdat_end_isr
* ������  �ɼ���һ֡��Ƶ���ݺ���жϴ�����(˫����ĵڶ�֡)
* ���룺  ��
* ���أ�  ��
******************************************************************************************************** */
ATTR_RAM_SECTION void adc_rdat_end_isr(void)
{
    adc_rdat_handle(GsIfAdc[0]);
}

/*********************************************************************************************************
* ��������rangeFftIsr
* ������  1D FFTȫ��������ɺ�Ļص�����,�ڴ˺����ж�1D��������������У�������2D FFT����
* ���룺  ��
* ���أ�  ��
******************************************************************************************************** */
ATTR_RAM_SECTION void rangeFftIsr(void)
{
    #if ((DUMP_TYPE == DUMP_RANGE) || (DUMP_TYPE == DUMP_VELOC))
    int i = 0, j = 0, k = 0;

    /* ���������г�Ӳ��FFT�����������˳��Ҫ�� */
    for (i = 0; i < RX_ANTS; i++) {
        for (j = 0; j < FFTN_RB; j++) {
            for (k = 0; k < CHIRPS; k++) {
                GsVecIn[i][j][k][0] = GsRange[i][k][j][0]; /* �鲿 */
                GsVecIn[i][j][k][1] = GsRange[i][k][j][1]; /* ʵ�� */
            }
        }
    }

    #if (DUMP_TYPE == DUMP_VELOC)
    /* ��ʼ��FFT����������ò�����2D FFT���� */
    hal_fft_set_config(FFT_IDX_CONFIG_USER_WIN, (void *)&GtVelocWin); /* ��ʼ��FFT���������� */
    hal_fft_set_config(FFT_IDX_CONFIG_USER_FFT,(void*)&GtVelocCfg); /* ��GtVelocCfg���ó�ʼ��FFT���� */
    fft_process_trigger(FFT_USER_FFT_MODE); /* ����Ӳ��FFT�������FFT���� */
    #else
    g_frame_id++;
    #endif
    #endif
}

/*********************************************************************************************************
* ��������velocFftIsr
* ������  2D FFTȫ��������ɺ�Ļص�����
* ���룺  ��
* �����  ����g_frame_id
* ���أ�  ��
******************************************************************************************************** */
ATTR_RAM_SECTION void velocFftIsr(void)
{
    g_frame_id++;
}

/*********************************************************************************************************
* ��������radar_clock_init
* ������  �״�RFϵͳ���ʱ�ӣ���DMA����ʼ������
* ���룺  ��
* ���أ�  ��
******************************************************************************************************** */
void radar_clock_init(void)
{
    sys_clk_disable_radar();
    sys_clk_enable_radar();
    sys_clk_enable_dma();
}

/*********************************************************************************************************
* ��������dump_frame_data
* ���ܣ�  ͨ��DMA����֡����
* ���룺  ulFrameId:֡ID
* ���أ�  ��
******************************************************************************************************** */
void dump_frame_data(unsigned long ulFrameId)
{
    /* ����֡ͷ���� */
    #if (DUMP_TYPE != DUMP_NONE)
    GtFrameHeader.radar_common_cfg.rxSubFrameId = ulFrameId;
    output_port_send_data((u32)&GtFrameHeader, sizeof(GtFrameHeader));
    #endif

    /* ����֡�������� */
    #if (DUMP_TYPE == DUMP_RAW) /* ���ԭʼ���� */
    output_port_send_data((u32)GsIfAdc[0], sizeof(GsIfAdc[0]));
    #elif (DUMP_TYPE == DUMP_RANGE)
    output_port_send_data((u32)GsVecIn, sizeof(GsVecIn));
    #elif (DUMP_TYPE == DUMP_VELOC)
    output_port_send_data((u32)GsVeloc, sizeof(GsVeloc));
    #endif
}

/*********************************************************************************************************
* ��������GetTickCount
* ���ܣ�  ��ȡ��ǰʱ��������,��λΪmS
* ���룺  ��
* �����  ��
* ���أ�  ��ǰʱ������ֵ
******************************************************************************************************** */
ATTR_RAM_SECTION unsigned long GetTickCount (void)
{
    return uwTick;
}


#define MAX_REGIONS 1
#define DEBUG   0
#define USE_PC_SOFT 0
#define  DEBUG_RANGE_BIN (1)
#define  DEBUG_VRANGE_BIN (1)
#define  TRESHOLD_VALUE  (-70)//2500000
#define  VTRESHOLD_VALUE  (6)//2500000
#define  START_INDEX  6
// ����һ���������жϴ�С��
int isLittleEndian() {
    uint32_t i = 1; // ����һ��32λ������
    return ((char*)&i)[0] == 1; // ����һ���ֽڵ�ֵ
}

// ����һ���������жϴ�С�˲���ӡ���
void checkEndianness() {
    if (isLittleEndian()) {
        printf("System is Little-Endian\n");
    } else {
        printf("System is Big-Endian\n");
    }
}


//frameN14��
int16 fileData_temp[2*FFTN_RB * CHIRPS * RX_ANTS] = 
{
0, 382, -6, 6, -3, 3, -3, 3, -1, 3, 0, 1, 0, 2, -1, 2, 
0, 2, 1, 2, 0, 2, 0, 1, 1, 3, 3, 3, 3, 3, 6, 6, 
-31, -202, 2, -4, 1, -2, 1, -2, 0, -1, 1, 0, 0, -1, 0, -1, 
0, -1, 0, 0, 0, -1, 0, -1, -1, -1, -2, -1, -2, -1, -4, -2, 
-10, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, -1, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 
13, -8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 
-1, 6, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 
17, -5, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 
0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-33, -4, 0, -1, 0, 0, -1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 
0, 0, 0, 1, 0, 0, -1, 0, -1, 0, 0, 0, -1, 0, -1, 0, 
14, 8, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 
8, 17, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 
0, -27, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, -1, 0, 0, 0, 
0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, 
-3, 8, 0, 0, 0, 0, 0, 0, -1, 1, -1, 0, 1, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 
-19, 7, 0, 0, -1, 0, 0, 0, 1, -1, 0, 0, 0, 0, -1, 0, 
0, 0, 0, 0, 0, 0, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
22, -2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 
0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, -1, 0, 0, 
-10, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 
7, -16, 0, 0, 0, -1, 0, -1, 0, -1, 0, -1, 0, 0, 0, 0, 
0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
3, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 
0, 0, 0, 0, -1, 0, 0, -1, 0, 0, 1, 0, 1, 0, 0, 0, 
-10, 38, -1, 0, 0, 0, 0, 0, 0, -1, -1, 0, 1, 0, -1, 0, 
1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 
8, -39, 1, 0, 0, 0, 0, -1, 0, 1, 0, 0, 0, 0, 1, 0, 
-1, 1, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, -1, -1, -1, 0, 
-10, 13, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, -1, 1, 0, 0, -1, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 
6, -20, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 
2, 34, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
-74, -43, 0, -2, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, -1, 0, 
0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 
110, 18, 1, 2, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 
0, 1, 0, 0, 1, 0, 0, -1, 1, 0, 0, 0, 1, -1, 1, -1, 
-35, 52, -1, 0, -2, 1, -1, 0, -1, 0, 0, 0, 0, 1, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 
-1, -96, 1, -1, 2, -1, 1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 
0, -1, 0, 0, 0, 0, -1, -1, 0, 0, -1, -1, -1, 0, -2, -1, 
58, 61, 0, 2, -1, 1, -1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 
0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, -1, 2, 0, 
-114, 9, -1, -2, 0, -1, 0, 0, -1, -1, -1, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, -1, 1, 0, 1, -1, 1, 0, 1, -2, 1, 
48, -3, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, 0, 0, 1, 0, 
20, -31, 1, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 1, 0, -1, 0, 0, 0, 0, -1, -1, 
28, 17, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1, 0, 
-103, -30, -1, -2, 0, -1, 0, -1, 0, -1, 0, -1, 0, 0, 0, 0, 
0, 0, 0, 0, -1, 0, 0, 0, 0, 1, -1, 1, 0, 1, -2, 1, 
60, 70, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 
0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, -1, 0, 0, 2, 0, 
13, -60, 0, 0, 0, 1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 
0, -1, 0, 0, 0, -1, 0, 0, -1, -1, 0, 0, 0, -1, -1, -1, 
-25, 6, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, 1, 0, 
0, 0, 0, 1, -1, 1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 1, 
-4, 51, -1, 1, 0, 0, -1, 0, 0, 0, -1, 0, 0, 0, -1, 0, 
0, 0, 0, 0, 1, -1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 
24, -80, 2, -1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -1, -1, 0, 0, 0, 0, -1, 0, -1, 0, -1, 0, -2, 
1, 59, -1, 1, 0, 0, -1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 
0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 
-9, -24, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 
0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-10, 11, -1, -1, 0, 0, 0, -1, 0, 0, 0, 0, 0, -1, 0, 0, 
0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 
15, -1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, -1, 1, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-23, -5, -1, 0, -1, -1, 0, 0, 0, -1, 0, -1, 0, 0, 0, -1, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
22, -15, 1, -1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 
0, 0, 0, 0, 0, -1, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 
-12, 27, 0, 0, 0, -1, -1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 
0, 0, -1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 
30, -20, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, -1, -1, 
0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 
-36, 8, -1, -1, 0, -1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 
0, 1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 1, 0, 1, 
18, -4, 1, 1, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, -1, 0, 
0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 
-14, 9, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 
-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 
22, -4, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 
1, -1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-36, 7, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 1, 
35, -14, 1, 0, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-21, 1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 
13, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 
-12, 2, 0, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 
7, -2, 1, 1, -1, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 
0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 
7, -4, 0, 0, 0, 0, 0, 1, 0, -1, 0, 1, 0, -1, -1, 0, 
0, 0, 1, -1, 0, -1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 
-10, -2, 0, 0, 0, -1, -1, -1, 1, 0, 1, 0, 0, 1, 1, 0, 
0, 0, 0, 0, 1, 0, -1, 0, -1, -1, 0, 0, 0, 0, 0, 0, 
-1, 13, 0, 1, 1, 1, 1, 2, -1, 1, -1, -1, 0, -1, -1, 2, 
0, 0, 0, 0, -1, 1, 1, 0, 2, 1, 0, 0, 0, 0, 1, 1, 
12, -8, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -1, 0, 
0, 0, 0, 1, 0, -1, 0, 0, -1, 1, 1, 0, 0, 0, 0, 0, 
-15, -3, 0, 0, 0, -1, 1, -3, 1, -1, 0, 0, 0, 2, 2, -2, 
0, 0, 0, 0, 1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 
10, 10, 0, 1, 0, 1, -1, 2, -1, 1, 0, 0, 1, -1, -1, 1, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 
-3, -6, 0, -1, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, -1, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 
-1, -2, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 1, 
9, 5, 0, 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 
0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, -1, 
-16, -5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
-1, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 
0, 477, -5, 5, -3, 2, -2, 2, -2, 2, 0, 1, 0, 1, 0, 2, 
0, 1, 0, 2, 0, 1, 0, 1, 2, 2, 2, 2, 3, 2, 5, 5, 
-21, -235, 2, -3, 1, -1, 1, -1, 1, -1, 0, -1, 0, 0, 0, -1, 
0, -1, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -2, -1, -3, -2, 
5, -7, 1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 
14, 7, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, -1, 
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 
-10, -11, 0, 0, 0, 0, -1, -1, 1, 0, 0, 0, 0, 0, 0, 0, 
-1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 
0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
1, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 
16, -21, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 
0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-28, 6, -1, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 
8, -16, 0, 0, 0, 1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 
0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, 
11, 30, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 
9, -9, 0, 0, -1, -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-22, -19, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -1, 
-5, 26, 1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 
0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 1, 
14, -15, -1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 
0, 0, 0, 1, 0, 1, -1, 0, -1, -1, 0, 0, 0, 0, 0, 0, 
-15, 6, 0, 0, 0, 0, -1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 
0, 0, 0, -1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 
13, -8, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 
0, 0, 0, 1, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 
11, -5, 0, 0, 0, 1, -1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 1, -1, 0, 0, 0, 0, 0, 0, 0, 
-15, 15, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, 0, 0, 
1, -1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 
1, 0, 0, -1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 
-7, -1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 
-1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
11, -33, 0, 0, 1, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
91, 92, -1, 2, -1, 1, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, 
0, 0, 1, 0, 1, 0, 1, 0, 1, -1, 0, 0, 1, 0, 2, -1, 
-152, -93, 0, -3, 1, -1, 0, -1, 0, 0, 0, 0, 0, -1, 0, 0, 
0, 0, -1, 0, 0, 0, -1, 0, -1, 0, 0, 0, -1, 1, -2, 1, 
62, -17, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 
0, 0, 0, -1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 1, -1, 
-28, 116, -1, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, -1, 0, 1, 
1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 
-6, -76, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 
-1, -1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 
62, -9, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-15, -8, 0, 0, -1, -1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, 
-34, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 
0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 
48, -62, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 
0, 0, 0, -1, 0, -1, 0, -1, 0, 0, 0, 0, 0, -1, 0, -1, 
-77, 72, -2, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 
69, -58, 1, 0, 1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
1, -1, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, -1, 
-51, 11, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 1, -1, 1, 0, -1, 0, 0, 1, 0, 0, -1, 1, 
44, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 
1, 0, 0, 0, -1, 1, 0, 0, 1, 0, -1, 0, 0, 0, 1, 0, 
5, -14, 0, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, 
-1, 0, -1, 0, 0, -1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 
-35, -2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -1, 1, -1, 0, 
13, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, 0, 
0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, 0, 1, -1, 0, 0, 
0, 18, 1, 0, -1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 
7, -8, 0, -1, 1, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, 
-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-13, -9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, 0, 0, -1, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -1, 
20, 10, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, -1, 0, 0, 
0, 0, 0, 0, -1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 
-18, -2, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 
0, 0, 1, 0, 1, 1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
10, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -1, 0, 
-9, -2, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, -1, 0, 1, 0, 
9, 7, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
-1, -12, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 1, -1, 0, -1, -1, 0, 0, 0, 0, -1, 0, 
-5, 15, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, -1, 0, 
0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 1, 0, 
5, -18, 0, -1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 
0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 
4, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 
-8, -5, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 
0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 
6, 13, -1, 0, 0, -1, -1, 0, 0, -1, 0, 0, 0, -1, -1, 0, 
0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 
-13, -15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 
0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
17, 4, 0, 1, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
0, -1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 
-10, 1, 0, -1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, -1, 
1, 0, 0, 0, 0, -1, 0, 0, -1, -1, 0, 1, 0, 0, -1, 0, 
-4, -3, 0, 0, 0, -1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
9, 12, 0, 0, -1, 1, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 
0, 0, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 
7, -22, 1, 0, 1, -1, 1, 0, 0, 1, 0, 0, -1, 1, 1, 1, 
0, 0, 0, 0, 1, 1, 0, 0, 1, -1, 1, 0, 0, 0, 1, -2, 
-18, 6, 0, 0, -1, 0, -1, 1, -1, -1, 0, 0, 0, 0, -1, -1, 
0, 0, 0, 1, -1, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 2, 
20, 11, 0, 1, -1, 1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, -1, 0, 
-14, -7, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 
4, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, 1, 
0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
2, 5, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 
-10, -6, 0, 0, 0, 0, 0, 0, 0, -1, 1, 0, 0, 1, 0, -1, 
0, 0, 0, 0, -1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 
18, 3, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, -1, 0, 0
};
typedef struct 
{
	uint8_t state;
	uint8_t small;
	uint16_t	distance;
	uint16_t	strength;
	uint8_t		lmd;
}MAGTX;

//�ظ���ѯ
void yihe_ack(MAGTX *magtx)
{
	uint8_t i;
	uint8_t sendbuf[30];
	uint8_t tx_i = 0;
	uint16_t crcdata = 0;
	
	sendbuf[tx_i++]	=	0x55;//֡ͷ
	sendbuf[tx_i++]	=	0xA5;//֡ͷ
	sendbuf[tx_i++]	=	0x0A;//����
	sendbuf[tx_i++]	=	0xd3;//g_Fmcpara.m_CurrentValue[3];//��ַ
	
	sendbuf[tx_i++]	=	magtx->distance >>8;	//����
	sendbuf[tx_i++]	=	magtx->distance;			//����
	sendbuf[tx_i++]	=	0;	//�ٶ�
	//sendbuf[tx_i++]	=	magtx->state;	//1:����   0������
	if(magtx->distance >0 )
	{
		sendbuf[tx_i++]	=	0X01;
	}
	else
	{
		sendbuf[tx_i++]	=	0X00;
	}
		
	sendbuf[tx_i++]	=	magtx->strength >>8;	//�ź�ǿ��
	sendbuf[tx_i++]	=	magtx->strength;			//�ź�ǿ��
//	sendbuf[tx_i++]	=	magtx->state ;				//״̬
	sendbuf[tx_i++]	=	magtx->small ;				//΢��
	sendbuf[tx_i++]	=	0;//radar_close;
	
	for(i=0;i<tx_i;i++)
	{
		crcdata += sendbuf[i];	
	}
//	sendbuf[tx_i++]	=	crcdata >>8;//У���
	sendbuf[tx_i++]	=	crcdata&0xFF;//У���
	 
	output_port_send_data((u32)sendbuf,tx_i);
	
}

// ���帴���ṹ��
typedef struct {
    int32_t real;
    int32_t imag;
} Complex;

// ���㸴���ķ�ֵ
double complexMagnitude(Complex c) {
    return sqrt((double)c.real * c.real + (double)c.imag * c.imag);
}
	
//�����
void DiffCalc(double *data, uint8_t offset_size, uint16_t chirpRbCn, double* diff, double* max_value, double* average_value, uint16_t* max_value_index) {
    double max_value_temp = (-DBL_MAX);
    uint16_t max_value_index_temp = 0;
    double sum = 0;

    // �����һ�����ֵ
    if (offset_size + 1 < chirpRbCn) {
        diff[0] = (data[offset_size + 1] - data[0]);
    }

    // �������һ�����ֵ
    if (chirpRbCn - offset_size - 1 >= 0) {
        diff[chirpRbCn - offset_size] = (data[chirpRbCn - 1] - data[chirpRbCn - offset_size - 1]);
    }

    // �����м���ֵ��Ѱ�����ֵ
    for (uint16_t i = 1; i < chirpRbCn - offset_size; i++) {
        diff[i] = data[i + offset_size] - data[i - 1];

        // �������ֵ��������
        if (data[i] > max_value_temp && i > START_INDEX) {
            max_value_index_temp = i;
            max_value_temp = data[i];
        }

        // �ۼ��������ڼ���ƽ��ֵ
        sum += data[i];
    }
		sum = sum - data[1] - data[2] - data[3];
 
    // �����������
    *max_value_index = max_value_index_temp;
    *max_value = max_value_temp;

    // ����ƽ��ֵ
    if (chirpRbCn - offset_size - 4 > 0) {
        *average_value = sum / (chirpRbCn - offset_size - 4);
    } else {
        *average_value = 0.0;
    }
}

/* ��Ĳ��� */
typedef struct {
    uint8_t bulge_head;       // ��ͷ����
    uint8_t bulge_tail;       // ��β����
//    uint8_t head_index;       // ��ͷ������������ bulge_head ��ͬ��
//    uint8_t tail_index;       // ��β������������ bulge_tail ��ͬ��
    uint8_t refline;          // �ο���
    uint8_t width;            // ��Ŀ��
    uint16_t extreme_index;    // ��ֵ����
    double  extreme_value;    // ��ֵ
    uint8_t centroid_value;   // ����ֵ
    uint8_t relative_altitude;// ��Ը߶�
    uint16_t centroid;        // ����λ��
		double  angle;						//Ŀ��Ƕ�
} PeakDef;

#define ABS(a,b) ((a>b)?(a-b):(b-a))
#define DIFF_OFFSET   1
#define PEAKS_NUM_MAX 5   //����������
#define PEAK_DIFF 15			//
#define PEAK_FILL 4
#define MAX_DECREASE 5
#define NORMAL_MAX_VALUE  3
#define THRES				  	4
/* ��Ĳ��� */
typedef struct {
	PeakDef peaks[PEAKS_NUM_MAX];
	double max_value;
	double average_value;
	uint16_t max_value_index;	
	uint8_t peak_count;
	double thres;
} PeakFiner;

PeakFiner FiderPeakStatue = {0};
PeakFiner FiderVPeakStatue = {0};
double dres = 3.22*2;//cm
double vres = 2.350;//m/s
// ��ӡ�����Ϣ
void PrintPeaks(PeakFiner *peakFiner,double res ,char * str) {
	//    for (uint8_t i = 0; i < peakFiner->peak_count; i++) {
	//        PeakDef *peak = &peakFiner->peaks[i];
	//        printf("%s�� %u: ��ʼλ��=%u, ��ֹλ��=%u, ���ֵ=%f, ���ֵλ��=%u,ȫ�����ֵ=%f,ȫ�����ֵλ��=%u,����=%fcm\n",
	//               str,i + 1, peak->bulge_head, peak->bulge_tail, peak->extreme_value, peak->extreme_index,peakFiner->max_value,peakFiner->max_value_index,(peakFiner->max_value_index)*res);
	//    }
		double speed = 0;
		double angle = 0;
		for (uint8_t i = 0; i < peakFiner->peak_count; i++) {
        PeakDef *peak = &peakFiner->peaks[i];
				uint16 speed_index =  FiderVPeakStatue.peaks[i].extreme_index;
				speed = speed_index < (CHIRPS/2)? speed_index * vres : -(CHIRPS - speed_index)*vres;
				if(speed_index == (CHIRPS/2))
				{
					speed = 0;
				}	
				angle = FiderPeakStatue.peaks[i].angle;
        printf("%s�� %u: ��ʼλ��=%u, ��ֹλ��=%u, ���ֵ=%f, ���ֵλ��=%u,����=%f cm,�ٶ�=%f m\\s,�Ƕ�=%f��\n",
               str,i + 1, peak->bulge_head, peak->bulge_tail, peak->extreme_value, peak->extreme_index,(peak->extreme_index)*res,speed,angle);
    }
}

// ��ӡ�����Ϣ
void PrintVPeaks(PeakFiner *peakFiner,double res ,char * str) {
    for (uint8_t i = 0; i < peakFiner->peak_count; i++) {
        PeakDef *peak = &peakFiner->peaks[i];
        printf("%s�� %u: ��ʼλ��=%u, ��ֹλ��=%u, ���ֵ=%f, ���ֵλ��=%u,ȫ�����ֵ=%f,ȫ�����ֵλ��=%u,�ٶ�=%f m/s\n",
               str,i + 1, peak->bulge_head, peak->bulge_tail, peak->extreme_value, peak->extreme_index,peakFiner->max_value,peakFiner->max_value_index,((double)(peakFiner->max_value_index) - FFTN_VB/2)*res);
    }
}

// �ֲ���ͻ����
void LocalHeadTailCalc(double *data, uint8_t *peak_count, double thres, uint16_t chirpRbCn, double* diff, PeakDef *peaks, uint16_t start, uint16_t end) {
    bool find_flag = false; // false������head��true������tail
    double partial_sum = 0;
    bool decline_flag = false;
    double extreme_value = 0;
		uint16 extreme_value_index = 0;
	
    for (uint16_t i = start; i < end; i++) {
        if (*peak_count == PEAKS_NUM_MAX) {
            break;
        }		
				if(find_flag == true){
					 if (i == chirpRbCn - 2) { // ���ҵ���β��¼Ϊtail����
							peaks[(*peak_count)++].bulge_tail = i;				
							find_flag = false;
							decline_flag = false;
					} else if (extreme_value > data[i] + MAX_DECREASE && data[i + 1] > data[i]) { // ���½�̫��ʱ�˳�
							peaks[(*peak_count)++].bulge_tail = i;
							find_flag = false;
							decline_flag = false;
					} else if (ABS(diff[i], 0) <= thres && (diff[i - 1] <= -thres || decline_flag == true) && data[i] < PEAK_FILL) { // Ѱ��peak��tail,��ȡ��abs
							partial_sum = 0;
							decline_flag = true;
							for (int j = i + 1; j < MIN(i + 4, chirpRbCn - 1); j++) {
									partial_sum += diff[j];
							}
							if ((ABS(partial_sum, 0) < thres * 3 && ABS(data[i + 1], data[i]) < NORMAL_MAX_VALUE) || (data[i + 1] < data[peaks[*peak_count].bulge_head])) {
									int j;
									for (j = i + 1; j >= i - 3; --j) {
											if ((data[j] * 2 <= data[j - 2] + data[j - 1])) {
													break;
											}
									}
									peaks[(*peak_count)++].bulge_tail = j;
									i = j - 1;
									find_flag = false;
									decline_flag = false;
							}
					}
					
					if(find_flag == false)
					{
							peaks[(*peak_count) - 1].extreme_index = extreme_value_index;
							peaks[(*peak_count) - 1].extreme_value = extreme_value;	
					}					
				}

        if (find_flag == false && diff[i] >= thres) { // Ѱ��peak��head
            extreme_value = data[i];
						extreme_value_index = i;
            peaks[*peak_count].bulge_head = i - 1;
            find_flag = true;
        }
        if (data[i] >= extreme_value && find_flag == true) {
            extreme_value = data[i]; // �Ҽ���ֵ
						extreme_value_index = i; // ����ֵ����
        }
    }
}

// �ҳ�ÿ�� chirp �з�ֵ���� Range Bin �������ͷ�ֵ
void findMaxMagnitudeIndex(uint32_t* fileData, uint16_t n_chirps, uint16_t chirpRbCn, uint8_t n_RX)
{
    // ����ÿ����������
    for (int rx = 0; rx < n_RX; rx++) {
        // ����ÿ�� chirp
        for (int chirp = 0; chirp < n_chirps; chirp++) {
            int maxIndex = 0; // ��ʼ������ֵ������
            double maxMagnitude = (-DBL_MAX); // ��ʼ������ֵ
						 // ���ڴ洢��ǰ chirp ������ Magnitude
            double magnitudes[chirpRbCn];
						double magnitudes_sqr[chirpRbCn];
						double diff[chirpRbCn]; //�����㹻���Ĳ������
            // ����ÿ�� Range Bin
            for (int rb = 0; rb < chirpRbCn; rb++) {
                // ���㵱ǰ Range Bin ������
                int index = (rx * n_chirps * chirpRbCn + chirp + n_chirps*rb);

                // ��ȡ I �� Q ����
                int16_t I = (int16_t)(fileData[index] & 0xFFFF); // ��ȡ�� 16 λ��Ϊ I
                int16_t Q = (int16_t)(fileData[index] >> 16); // ��ȡ�� 16 λ��Ϊ Q
							
								// ��ӡ��ǰ Range Bin ��ʵ�����鲿
								 //printf("Chirp %d, RX %d, RB %d: I = %d, Q = %d\n", chirp, rx, rb, I, Q);							

                // ��������
                Complex c = {I, Q};

                // �����ֵ
							  double magnitude = complexMagnitude(c);
                magnitude = magnitude*64;

								if(rb > 0)
								{
									magnitude *= 2;
								}
								magnitude /= SAMPLES;
								magnitudes_sqr[rb] = magnitude;

								// �����ֵ
							{		
								// magnitudes_sqr[chirp] = complexMagnitude(cVb[chirpRbCn_num][chirp]);
								if (magnitudes_sqr[rb] > VTRESHOLD_VALUE && magnitudes_sqr[rb] > maxMagnitude && rb > 6) {
										maxMagnitude = magnitudes_sqr[rb];
										maxIndex = rb;
								}
							}
															
								
								// ��ֹlog10(0)���µ���ѧ����
	//								if (magnitude == 0) {
	//										magnitude = -100.0;  // һ���ǳ�С��ֵ����ʾ��ֵΪ0
	//								} else {
	//										magnitude = 20 * log10(magnitude / 32768);
	//								}
	//								
	//								// ���浱ǰ Range Bin �ķ�ֵ
	//                magnitudes[rb] = magnitude;
            }
						
						if(rx == 0)
						{
							DiffCalc(magnitudes_sqr, DIFF_OFFSET, chirpRbCn, diff, 
								&FiderPeakStatue.max_value, &FiderPeakStatue.average_value, &FiderPeakStatue.max_value_index);

							LocalHeadTailCalc(magnitudes_sqr, &FiderPeakStatue.peak_count, FiderPeakStatue.thres, chirpRbCn, diff, FiderPeakStatue.peaks, 2,chirpRbCn-5);				
							
	 
//							if(maxMagnitude > 0)
//							{
//								if(!FiderPeakStatue.peak_count)
//									FiderPeakStatue.peak_count++;
//								FiderPeakStatue.max_value_index = maxIndex;
//								FiderPeakStatue.max_value = maxMagnitude;
//							}
							
						}

						#if DEBUG_RANGE_BIN
						// ��ӡ��ǰ chirp ������ Magnitude
            printf("Chirp %d, RX %d: Magnitudes = [", chirp, rx);
            for (int rb = 0; rb < chirpRbCn; rb++) {
                printf("%.2f", magnitudes_sqr[rb]);
                if (rb < chirpRbCn - 1) {
                    printf(", ");
                }
            }
	          printf("]\n");
						#endif
            // ��ӡ������Ϣ
            //printf("Chirp %d, RX %d: Max Magnitude Index = %d, Max Magnitude = %f\n", chirp, rx, maxIndex, maxMagnitude);
						break;	
				}
				//break;
    }
}

// ���ڴ洢����������
Complex cVb[FFTN_RB][CHIRPS];

// ����ʵ�� fftshift ����
void fftshift(Complex *data, int size) {
    Complex temp[size];
    for (int i = 0; i < size; i++) {
        temp[i] = data[i];
    }
    for (int i = 0; i < size / 2; i++) {
        data[i] = temp[i + size / 2];
        data[i + size / 2] = temp[i];
    }
}

void findVbMaxMagnitudeIndex(uint32_t* fileData, uint16_t n_chirps, uint16_t chirpRbCn, uint8_t n_RX_num,uint16_t chirpRbCn_num)
{
		int maxIndex = 0; // ��ʼ������ֵ������
		double maxMagnitude = (-DBL_MAX); // ��ʼ������ֵ
		// ���ڴ洢��ǰ chirp ������ Magnitude
		double magnitudes[chirpRbCn];
		double magnitudes_sqr[chirpRbCn];

		// ����ÿ�� chirp
		for (int chirp = 0; chirp < n_chirps; chirp++) {
				// ���㵱ǰ Range Bin ������
				int index = (n_RX_num * n_chirps * chirpRbCn + chirp + n_chirps*chirpRbCn_num);

				// ��ȡ I �� Q ����
				int16_t I = (int16_t)(fileData[index] & 0xFFFF); // ��ȡ�� 16 λ��Ϊ I
				int16_t Q = (int16_t)(fileData[index] >> 16); // ��ȡ�� 16 λ��Ϊ Q
			
				// ��ӡ��ǰ Range Bin ��ʵ�����鲿
			 // printf("Chirp %d, RX %d, RB %d: I = %d, Q = %d\n", chirp, n_RX_num, chirpRbCn_num, I, Q);							

				// ��������
				Complex c = {I, Q};
				cVb[chirpRbCn_num][chirp] = c;
		}
		
		// �����ֵ
		for (int chirp = 0; chirp < n_chirps; chirp++) {		
				 magnitudes_sqr[chirp] = complexMagnitude(cVb[chirpRbCn_num][chirp]);
		}
			 
//		#if DEBUG_VRANGE_BIN
//		// ��ӡ��ǰ chirp ������ Magnitude
//		printf("ǰchirpRbCn %d, RX %d: Magnitudes = [", chirpRbCn_num, n_RX_num);
//		for (int chirp = 0; chirp < n_chirps; chirp++) {	
//				printf("%.2f", magnitudes_sqr[chirp]);
//				if (chirp < n_chirps - 1) {
//						printf(", ");
//				}
//		}
//		printf("]\n");
//		#endif
		
		//fftshift(&cVb[chirpRbCn_num][0], n_chirps);
		// �����ֵ
		for (int chirp = 0; chirp < n_chirps; chirp++) {		
				 magnitudes_sqr[chirp] = complexMagnitude(cVb[chirpRbCn_num][chirp]);
				if (magnitudes_sqr[chirp] > VTRESHOLD_VALUE && magnitudes_sqr[chirp] > maxMagnitude && chirp > 0) {
						maxMagnitude = magnitudes_sqr[chirp];
						maxIndex = chirp;
				}
		}
			 
		if(maxMagnitude > 0)
		{
//			FiderVPeakStatue.max_value_index = maxIndex;
//			FiderVPeakStatue.max_value = maxMagnitude;
			FiderVPeakStatue.peaks[FiderVPeakStatue.peak_count].extreme_index = maxIndex;
			FiderVPeakStatue.peaks[FiderVPeakStatue.peak_count].extreme_value = maxMagnitude;
			FiderVPeakStatue.peak_count++;
		}
		
		#if DEBUG_VRANGE_BIN
		// ��ӡ��ǰ chirp ������ Magnitude
		printf("chirpRbCn %d, RX %d: Magnitudes = [", chirpRbCn_num, n_RX_num);
		for (int chirp = 0; chirp < n_chirps; chirp++) {	
				printf("%.2f", magnitudes_sqr[chirp]);
				if (chirp < n_chirps - 1) {
						printf(", ");
				}
		}
		printf("]\n");
		#endif
}


//��ȡ�Ƕ���Ϣ
double findAngleStatue(uint32_t* fileData, uint16_t n_chirps, uint16_t chirpRbCn, uint8_t n_RX,
	uint16_t n_chirps_num, uint16_t chirpRbCn_num)
{
		 // �洢ÿ�����ߵ���λ
    double phase[n_RX];
    // ����ÿ����������
    for (int rx = 0; rx < n_RX; rx++) {
			// ���㵱ǰ Range Bin ������
			int index = (rx * n_chirps * chirpRbCn + n_chirps_num + n_chirps*chirpRbCn_num);

			// ��ȡ I �� Q ����
			int16_t I = (int16_t)(fileData[index] & 0xFFFF); // ��ȡ�� 16 λ��Ϊ I
			int16_t Q = (int16_t)(fileData[index] >> 16); // ��ȡ�� 16 λ��Ϊ Q
			
			// ������λ����λ�����ȣ�
      phase[rx] = atan2(Q, I);
			
			// ��ӡÿ�����ߵ���λ��ת��Ϊ�ȣ�
			printf("���� %d: I = %d, Q = %d, ��λ = %.2f ��\n", rx, I, Q, phase[rx] * 180.0 / PI);
    }
    
		double phase_diff = 0;
    // ��ӡ��������֮�����λ��
    for (int i = 0; i < n_RX - 1; i++) {
        phase_diff = phase[i+1] - phase[i];
        
        // ��λ���һ���� [-��, ��]
        while (phase_diff > PI) phase_diff -= 2 * PI;
        while (phase_diff < -PI) phase_diff += 2 * PI;
        
        printf("���� %d �� %d ֮�����λ�� = %.2f ��\n", i, i+1, phase_diff * 180.0 / PI);
    }
		
		//������λ�ʽ����Ƕ�
		double lambda = 0.0051;
    double d = 	lambda/2;
		double estimated_angle = 0;
    estimated_angle = asin(phase_diff * lambda / ( 2 * PI * d ));
    
    // ������ת��Ϊ��
    estimated_angle =  estimated_angle * 180.0 / PI;  
  	if(estimated_angle > 0)
		{
			estimated_angle = 90 - estimated_angle;
		}
		else if(estimated_angle < 0)
		{
			estimated_angle += 90;
			estimated_angle = -estimated_angle;
		}
		
    return estimated_angle;
}

/*********************************************************************************************************
* ��������main
* ���ܣ�  ����������ʾ����Ӳ��FFT����ʵ����ƵADC�ɼ����Լ�1D FFT��2D FFT����
* ���룺  ��
* ���أ�  ��
******************************************************************************************************** */
int main(void)
{
    unsigned long ulFrameId = 0;
    unsigned long ulTmp = 0, ulStamp = 0;

    clock_init(); /* ϵͳʱ�� */
    radar_clock_init(); /* �״�RFϵͳ���ʱ�ӣ���DMA����ʼ������ */
    clk_mcu_set(CLK_SRC_PLL, 80000000U); /* ϵͳ��Ƶ��ʼ�� */
    hal_dualtimer_init(); /* Ĭ�϶�ʱ����ʼ�� */
    SysTick_Config(clk_mcu_get() / SYS_TICK_FREQ); /* ϵͳTick��ʱ����ʼ�� */
    hal_uart_init(UART_ID_0, &GtUartCfg); /* ��ͨLog���ڳ�ʼ�� */

    /* ���������س�ʼ����Dump�˿���Log���ڸ��ã�output_dma_config()�����ô��ڲ����� */
    #if (DUMP_TYPE != DUMP_NONE)
    output_dma_config(); /* �������DMA��ʼ�� */
    write_data_frame_header((u8*)&GtFrameHeader); /* ֡ͷ��Ϣ��ʼ�� */
    #if (DUMP_TYPE != DUMP_RAW)
    GtFrameHeader.radar_common_cfg.flag = (1<<5)| (1<<1) | (1<<6);
    #endif
    #endif

    hal_fft_init(); /* FFT ���������ʼ�� */
    radar_fmcw_cfg(); /* FMCW ɨƵ��ز�����ʼ�� */
    hal_fft_set_config(FFT_IDX_CONFIG_ADC_RDAT,(void*)&GtIfSampleCfg); /* ������������ */
    radar_frame_trigger_mode_set(TRIGGER_MODE_INTERNAL); /* RFɨƵ����Ϊ�ڲ����� */
    radar_frame_start(); /* ����RFɨƵ */
		//fres = 19.531KHz, dres = 0.032m, vres = 2.350m/s
		double last_distance[3] = {0};
		#if USE_PC_SOFT
		uart_cfg.baud_rate = UART_BAUD_RATE_115200;//UART_BAUD_RATE_921600;
		uart_cfg.parity = UART_PARITY_DISABLE;
		uart_cfg.stop_bits = UART_STOP_BITS_1;
		uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
		uart_cfg.fifo_cfg.fifo_en = 1;
		(void)hal_uart_init(UART_ID_0, &uart_cfg);
		#endif
		
		    // ����Hanning������
    for (int n = 0; n < SAMPLES; n++) {
        // MATLAB��hanning����ʵ�֣��Գƴ���
        window[n] = 0.5 * (1 - cos(2 * PI * n / (SAMPLES - 1)));
    }
		
    while(1) {
        if (ulFrameId != g_frame_id) { /* ����ɶ���FFT���� */
						#if 0
            dump_frame_data(ulFrameId); /* dump��������� */
						#else
            ulFrameId = g_frame_id;
						memset((uint8_t *)&FiderPeakStatue,0,sizeof(FiderPeakStatue));
						memset((uint8_t *)&FiderVPeakStatue,0,sizeof(FiderPeakStatue));
						//�����1 �� ����2			
						int n_chirps = CHIRPS; // chirp ����
						int chirpRbCn = FFTN_RB; // ÿ�� chirp �� Range Bin ����
						int n_RX = RX_ANTS; // ������������
						//printf("OUT_FRAME_HEADER_SIZE %d\n", OUT_FRAME_HEADER_SIZE);
						FiderPeakStatue.thres = THRES;
						// �ҳ�ÿ�� chirp �з�ֵ���� Range Bin ������
						findMaxMagnitudeIndex((uint32_t*)GsVecIn, n_chirps, chirpRbCn, n_RX);
//						for(int chirpRbCn_num = 0;chirpRbCn_num<chirpRbCn;chirpRbCn_num++)
//						{
//							findVbMaxMagnitudeIndex((uint32_t*)GsVeloc, n_chirps, chirpRbCn, 0,chirpRbCn_num);
//						}
					
						for(int peak_num = 0;peak_num < FiderPeakStatue.peak_count;peak_num++)
						{
							double angle = 0;
							findVbMaxMagnitudeIndex((uint32_t*)GsVeloc, n_chirps, chirpRbCn, 0,FiderPeakStatue.peaks[peak_num].extreme_index);
							angle = findAngleStatue((uint32_t*)GsVecIn, n_chirps, chirpRbCn,n_RX,	0, FiderPeakStatue.peaks[peak_num].extreme_index);
							FiderPeakStatue.peaks[peak_num].angle = angle;
						}
					
			//			findVbMaxMagnitudeIndex((uint32_t*)GsVeloc, n_chirps, chirpRbCn, 0,FiderPeakStatue.max_value_index);

						//findMaxMagnitudeIndex((uint32_t*)fileData_temp, n_chirps, chirpRbCn, n_RX, maxIndices,maxMagnitudes);
						MAGTX magtx = {0};
						double cur_distance = 0;
						// ��ӡ���
						PrintPeaks(&FiderPeakStatue,dres,"range/velociy");
					//	PrintVPeaks(&FiderVPeakStatue,2.350,"velociy");
//						for (int i = 0; i < n_RX * n_chirps; i++) {			
//							for(int j = 0; j < MAX_REGIONS; j++)
//							{
//								#if USE_PC_SOFT		
//								magtx.distance = cur_distance;
//								//magtx.strength = maxMagnitudes[i][j] + 100;
//								magtx.strength = maxMagnitudes[i][j]/1000;
//								yihe_ack(&magtx);
//								#else
//								printf("Chirp %d, RX %d Diatance%d : Max Magnitude Index = %d, Max Magnitude = %f,distance = %f cm\n", 
//											 i % n_chirps, i / n_chirps,j,maxIndices[i][j], maxMagnitudes[i][j],cur_distance);
//								#endif
//							}
//						}
						checkEndianness();	
						#endif
												
            /* ֡�ʿ��ƣ�֡�������Ϊ200mS */
            do {
                ulTmp = GetTickCount();
                ulTmp = (ulTmp >= ulStamp)? (ulTmp - ulStamp) : (0xFFFFFFFFUL - ulStamp + ulTmp);
            } while (ulTmp < 200);

            radar_rf_pwr_on(); /* ���ɼ�����Ƶ���йر�RF��ص�Դ�����ڴ�����ʹ��RF RX��ص�Դ */
            radar_frame_start(); /* ��RFɨƵ����Ϊ���δ�������ÿ��ɨƵ��������Ҫ���µ��ô˺��� */
            ulStamp = GetTickCount();
        }
    }
}


// �ҳ�ÿ�� chirp �з�ֵ���� Range Bin �������ͷ�ֵ
//void findMaxMagnitudeIndex(uint32_t* fileData, int n_chirps, int chirpRbCn, int n_RX,
//	int maxIndices[CHIRPS*RX_ANTS][MAX_REGIONS],
//		double maxMagnitudes[CHIRPS*RX_ANTS][MAX_REGIONS])
//{
//    // ����ÿ����������
//    for (int rx = 0; rx < n_RX; rx++) {
//        // ����ÿ�� chirp
//        for (int chirp = 0; chirp < n_chirps; chirp++) {
//            int maxIndex = 0; // ��ʼ������ֵ������
//            double maxMagnitude = (-DBL_MAX); // ��ʼ������ֵ
//						 // ���ڴ洢��ǰ chirp ������ Magnitude
//            double magnitudes[chirpRbCn];
//					  int start_index = -1;
//            int end_index = -1;
//						int in_region = 0;
//	          int region_count = 0;

//            // ���ڴ洢ǰ�������� -39 ������
//            int regions[MAX_REGIONS][2] = {{-1, -1}};	
//						
//            // ����ÿ�� Range Bin
//            for (int rb = 0; rb < chirpRbCn; rb++) {
//                // ���㵱ǰ Range Bin ������
//                int index = (rx * n_chirps * chirpRbCn + chirp + n_chirps*rb);

//                // ��ȡ I �� Q ����
//                int16_t I = (int16_t)(fileData[index] & 0xFFFF); // ��ȡ�� 16 λ��Ϊ I
//                int16_t Q = (int16_t)(fileData[index] >> 16); // ��ȡ�� 16 λ��Ϊ Q
//							
//								// ��ӡ��ǰ Range Bin ��ʵ�����鲿
//  //             printf("Chirp %d, RX %d, RB %d: I = %d, Q = %d\n", chirp, rx, rb, I, Q);							
//	//							
//	//								int32_t I_TEMP = I*64;
//	//								int32_t Q_TEMP = Q*64;
//	//							
//                // ��������
//                Complex c = {I, Q};

//								
//                // �����ֵ
//                double magnitude = complexMagnitude(c)*64;

//								if(rb > 0)
//								{
//									magnitude *= 2;
//								}
//								magnitude /= SAMPLES;
//								// ��ֹlog10(0)���µ���ѧ����
//								if (magnitude == 0) {
//										magnitude = -100.0;  // һ���ǳ�С��ֵ����ʾ��ֵΪ0
//								} else {
//										magnitude = 20 * log10(magnitude / 32768);
//								}
//								
//								// ���浱ǰ Range Bin �ķ�ֵ
//                magnitudes[rb] = magnitude;
//								
//								if(rb > 6)
//								{
//									// ������ -39 ������
//									if (magnitude > -70) {
//											if (!in_region) {
//													start_index = rb;
//													in_region = 1;
//											}
//											end_index = rb;
//									} else {
//											if (in_region) {
//													if (region_count < MAX_REGIONS) {
//															regions[region_count][0] = start_index;
//															regions[region_count][1] = end_index;
//															region_count++;
//													}
//													in_region = 0;
//											}
//									}
//								}
//            }

//            // ��������ֵ�������ͷ�ֵ
//						for(int i = 0; i < MAX_REGIONS;i++)
//						{
//							maxMagnitudes[chirp + rx * n_chirps][i] = -100;
//							if(regions[i][1] > 0)
//							{
//								maxIndices[chirp + rx * n_chirps][i] = (regions[i][1] + regions[i][0])/2 ;
//								maxMagnitudes[chirp + rx * n_chirps][i] = magnitudes[(regions[i][1] + regions[i][0])/2];
//							}
//						}
//						#if DEBUG_RANGE_BIN
//						// ��ӡ��ǰ chirp ������ Magnitude
//            printf("Chirp %d, RX %d: Magnitudes = [", chirp, rx);
//            for (int rb = 0; rb < chirpRbCn; rb++) {
//                printf("%.2f", magnitudes[rb]);
//                if (rb < chirpRbCn - 1) {
//                    printf(", ");
//                }
//            }
//	          printf("]\n");
//						#endif
//            // ��ӡ������Ϣ
//           // printf("Chirp %d, RX %d: Max Magnitude Index = %d, Max Magnitude = %f\n", chirp, rx, maxIndex, maxMagnitude);
//        }
//    }
//}



//// �ҳ�ÿ�� chirp �з�ֵ���� Range Bin �������ͷ�ֵ
//void findMaxMagnitudeIndex(uint32_t* fileData, int n_chirps, int chirpRbCn, int n_RX,
//	int maxIndices[CHIRPS*RX_ANTS][MAX_REGIONS],
//		double maxMagnitudes[CHIRPS*RX_ANTS][MAX_REGIONS])
//{
//    // ����ÿ����������
//    for (int rx = 0; rx < n_RX; rx++) {
//        // ����ÿ�� chirp
//        for (int chirp = 0; chirp < n_chirps; chirp++) {
//            int maxIndex = 0; // ��ʼ������ֵ������
//            double maxMagnitude = (-DBL_MAX); // ��ʼ������ֵ
//						 // ���ڴ洢��ǰ chirp ������ Magnitude
//            double magnitudes[chirpRbCn];
//					  int start_index = -1;
//            int end_index = -1;
//						int in_region = 0;
//	          int region_count = 0;

//            // ���ڴ洢ǰ�������� -39 ������
//            int regions[MAX_REGIONS][2] = {{-1, -1}};	
//						
//            // ����ÿ�� Range Bin
//            for (int rb = 0; rb < chirpRbCn; rb++) {
//                // ���㵱ǰ Range Bin ������
//                int index = (rx * n_chirps * chirpRbCn + chirp + n_chirps*rb);

//                // ��ȡ I �� Q ����
//                int16_t I = (int16_t)(fileData[index] & 0xFFFF); // ��ȡ�� 16 λ��Ϊ I
//                int16_t Q = (int16_t)(fileData[index] >> 16); // ��ȡ�� 16 λ��Ϊ Q
//							
//								// ��ӡ��ǰ Range Bin ��ʵ�����鲿
//								// printf("Chirp %d, RX %d, RB %d: I = %d, Q = %d\n", chirp, rx, rb, I, Q);													
//					
//                // ��������
//                Complex c = {I, Q};								

//								double magnitude = sqrt((double)c.real * c.real + (double)c.imag * c.imag);//complexMagnitude(c);
//								// ���浱ǰ Range Bin �ķ�ֵ
//                magnitudes[rb] = magnitude;               
//							 // ��������ֵ��������
//                if (magnitude > TRESHOLD_VALUE && magnitude > maxMagnitude && rb > 6) {
//	                  maxMagnitude = magnitude;
//	                  maxIndex = rb;
//									//	break;
//                }
//            }

//            // ��������ֵ�������ͷ�ֵ
//						for(int i = 0; i < MAX_REGIONS;i++)
//						{
//							{
//								maxIndices[chirp + rx * n_chirps][i] = maxIndex;
//								maxMagnitudes[chirp + rx * n_chirps][i]  = maxMagnitude;
//							}
//						}
//						#if DEBUG_RANGE_BIN
//						// ��ӡ��ǰ chirp ������ Magnitude
//            printf("Chirp %d, RX %d: Magnitudes = [", chirp, rx);
//            for (int rb = 0; rb < chirpRbCn; rb++) {
//                printf("%.2f", magnitudes[rb]);
//                if (rb < chirpRbCn - 1) {
//                    printf(", ");
//                }
//            }
//	          printf("]\n");
//						#endif
//            // ��ӡ������Ϣ
//            printf("Chirp %d, RX %d: Max Magnitude Index = %d, Max Magnitude = %f\n", chirp, rx, maxIndex, maxMagnitude);
//        }
//    }
//}


