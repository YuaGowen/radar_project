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
* 函数声明
******************************************************************************************************** */
void adc_rdat_end_isr(void);
void adc_rdat_half_isr(void);
void rangeFftIsr(void);
void velocFftIsr(void);

/*********************************************************************************************************
* 宏定义
******************************************************************************************************** */
#define CHIRPS                   NUM_CHIRP /* 帧内chirp数量 */
#define SAMPLES                  NUM_SAMPLE_PER_CHIRP /* Chirp内采样点数量，通常也是一维FFT点数 */
#define FFTS_1D                  9 /* 1D FFT阶数，对应FFT点数为2^FFTS_1D */
#define FFTS_2D                  4 /* 2D FFT阶数，对应FFT点数为2^FFTS_2D */
#define FFTN_RB                  64 /* 一阶FFT输出Range Bin数量，一般根据最大测距范围设定 */
#define FFTN_VB                  CHIRPS /* 二阶FFT输出Velocity Bin数量，一般根据最大测速需求设定 */

/*********************************************************************************************************
* 以下宏定义请勿修改
******************************************************************************************************** */
#define FFTN_1D                  (1UL << FFTS_1D) /* 1D FFT点数，通常与Chirp内采样点数量相同 */
#define FFTN_2D                  (1UL << FFTS_2D) /* 2D FFT点数，通常与帧内Chirp数量相同 */
#define RX_ANTS                  (1 + (RADAR_PRF_RFE_RX_NUM == RADAR_RX_NUM_2)) /* 接收通道数 */
#define DUMP_NONE                0 /* 不输出任何帧数据 */
#define DUMP_RAW                 1 /* 输出RAW DATA数据 */
#define DUMP_RANGE               2 /* 输出RANGE BIN数据 */
#define DUMP_VELOC               3 /* 输出VELOC BIN数据 */
#define SYS_TICK_FREQ            1000UL /* 系统定时器频率,默认为1KHz,请勿修改 */

/*********************************************************************************************************
* 数据输出类型选择
******************************************************************************************************** */
#define DUMP_TYPE                DUMP_VELOC /* DUMP_RAW=输出RAW DATA数据,DUMP_RANGE=输出RANGE BIN数据,DUMP_VELOC=输出VELOC BIN数据 */

/*********************************************************************************************************
* 宏定义防呆检查
******************************************************************************************************** */
#if (SAMPLES != FFTN_1D) /* CHIRP采样点数与1D FFT点数不同，如符合设计预期，可忽略此警告！ */
#pragma message("The value of SAMPLES is different from FFTN_1D. If it meets the expectations, this warning can be ignored!")
#endif
#if (CHIRPS != FFTN_2D) /* CHIRP数量与2D FFT点数不同，如符合设计预期，可忽略此警告！ */
#pragma message("The value of CHIRPS is different from FFTN_2D. If it meets the expectations, this warning can be ignored!")
#endif
#if (FFTN_RB > (FFTN_1D >> 1)) /* 输出Range Bin的数量大于FFT点数的一半，如符合设计预期，可忽略此警告！ */
#pragma message("The value of FFTN_RB is greater than (FFTN_1D >> 1). If it meets the expectations, this warning can be ignored!")
#endif

/*********************************************************************************************************
* 全局变量或常量定义
******************************************************************************************************** */
short GsIfAdc[1][CHIRPS][SAMPLES][RX_ANTS] FFT_BUF_ATTRIBUTES = {0}; /* 容纳1帧ADC数据的缓存 */
#if ((DUMP_TYPE == DUMP_RANGE) || (DUMP_TYPE == DUMP_VELOC))
short GsFrame[RX_ANTS][CHIRPS][SAMPLES] FFT_BUF_ATTRIBUTES = {0}; /* 一阶FFT变换的输入(帧)缓存 */
short GsRange[RX_ANTS][CHIRPS][FFTN_RB][2] FFT_BUF_ATTRIBUTES = {0}; /* 一阶FFT变换后的输出缓存 */
short GsVecIn[RX_ANTS][FFTN_RB][CHIRPS][2] FFT_BUF_ATTRIBUTES = {0}; /* 二阶FFT变换前的输入缓存 */
#if (DUMP_TYPE == DUMP_VELOC)
short GsVeloc[RX_ANTS][FFTN_RB][FFTN_VB][2] FFT_BUF_ATTRIBUTES = {0}; /* 二阶FFT变换后的输出缓存 */
#endif
#endif
volatile unsigned long g_frame_id = 0; /* 帧ID */
#if (DUMP_TYPE != DUMP_NONE)
volatile radar_frame_hdr_t GtFrameHeader = {0}; /* 数据 */
#endif

volatile unsigned long uwTick = 0; /* 计时变量 */

/* 串口参数配置,默认Log输出波特率为921600, 若吐数串口与Log串口复用，波特率将被output_dma_config()函数重置 */
uart_cfg_t GtUartCfg = {
    .baud_rate = UART_BAUD_RATE_921600,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .data_width = UART_DATA_WIDTH_8BIT,
    .fifo_cfg.fifo_en = 1U,
};

/* 1D FFT窗序列定义,窗序列长度必须与FFT点数相同 */
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
    #error "窗序列长度错误，请注意检查！"
    #endif
};

/* 2D FFT窗序列定义,窗序列长度必须与FFT点数相同 */
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
    #error "窗序列长度错误，请注意检查！"
    #endif
};

/* User FFT模式下的1D FFT窗配置 */
fft_win_para_t GtRangeWin = {
    .enable = 1, /* 1=使能硬件FFT自动加窗,0=禁能硬件FFT自动加窗 */
    .fft_mode = FFT_USER_FFT_MODE, /* FFT引擎工作类型 */
    .waddr = (u32)GusRangeWin, /* 窗序列首地址 */
};

/* User FFT模式下的2D FFT窗配置 */
fft_win_para_t GtVelocWin = {
    .enable = 1, /* 1=使能硬件FFT自动加窗,0=禁能硬件FFT自动加窗 */
    .fft_mode = FFT_USER_FFT_MODE, /* FFT引擎工作类型 */
    .waddr = (u32)GusVelocWin, /* 窗序列首地址 */
};

#if ((DUMP_TYPE == DUMP_RANGE) || (DUMP_TYPE == DUMP_VELOC))
/* User FFT模式下的1D FFT配置 */
user_fft_para_t GtRangeCfg = {
    .data_type = REAL_DATA, /* FFT变换类型，REAL_DATA=FFT变换的输入数据为实数，COMPLEX_DATA=FFT变换的输入数据为复数 */
    .is_power_out  = 0, /* 0=输出复数,1=输出模平方 */
    .shift_value   = FFTS_1D, /* FFT结果向右移位数 */
    .fft_point     = FFTN_1D, /* FFT点数 */
    .nframe        = CHIRPS * RX_ANTS, /* 需要做FFT运算的次数，1D FFT对应Frame内的总chirp数量 */
    .in_frame_size = SAMPLES, /* 每次FFT运算的输入点数量，1D FFT对应chirp内采样点数量 */
    .out_frame_size = FFTN_RB, /* 每次FFT运算后的输出点数量，1D FFT对应输出Range Bin的数量 */
    .in_buf         = (u32)GsFrame, /* 1D FFT运算的输入数据首地址 */
    .out_buf        = (u32)GsRange, /* 1D FFT运算的输出数据首地址 */
    .end_callback   = rangeFftIsr, /* 1D FFT全部运算完成后的回调函数 */
};
#endif

#if (DUMP_TYPE == DUMP_VELOC)
/* User FFT模式下的2D FFT配置 */
user_fft_para_t GtVelocCfg = {
    .data_type = COMPLEX_DATA, /* FFT变换类型，REAL_DATA=FFT变换的输入数据为实数，COMPLEX_DATA=FFT变换的输入数据为复数 */
    .is_power_out  = 0, /* 0=输出复数,1=输出模平方 */
    .shift_value   = FFTS_2D, /* FFT结果向右移位数 */
    .fft_point     = FFTN_2D, /* FFT点数 */
    .nframe        = FFTN_RB * RX_ANTS, /* 需要做FFT运算的次数，2D FFT对应一阶的总RangeBin数量 */
    .in_frame_size = CHIRPS, /* 每次FFT运算的输入点数量，2D FFT对应一阶的帧内Chirp数量 */
    .out_frame_size = FFTN_VB, /* 每次FFT运算后的输出点数量，2D FFT对应所需的VelocBin数量 */
    .in_buf         = (u32)GsVecIn, /* 2D FFT运算的输入数据首地址 */
    .out_buf        = (u32)GsVeloc, /* 2D FFT运算的输出数据首地址 */
    .end_callback   = velocFftIsr, /* 2D FFT全部运算完成后的回调函数 */
};
#endif

/* 中频ADC采集配置（中频ADC由FFT引擎按帧采集） */
adc_rdat_para_t GtIfSampleCfg = {
    .enable = 1, /* 1=使能FFT引擎采集中频ADC,0=禁能FFT引擎采集中频ADC */
    #if (RX_ANTS == 1)
    .num_adc_ch = FFT_MONO_ADC_CH, /* 0 = FFT_MONO_ADC_CH, 1 = FFT_DUAL_ADC_CH */
    #else
    .num_adc_ch = FFT_DUAL_ADC_CH, /* 0 = FFT_MONO_ADC_CH, 1 = FFT_DUAL_ADC_CH */
    #endif
    .nframe = CHIRPS, /* 每个通道需要采集多少个Chirp */
    .frame_size = SAMPLES, /* 每个Chirp对应的采样点数量 */
    .out_buf = (u32)GsIfAdc, /* 中频采集结果缓存首地址 */
    .end_callback = adc_rdat_end_isr, /* 中频采集结束回调函数，half_int_en = 1时，对应偶数帧采集结束 */
    .half_callback = 0, /* adc_rdat_half_isr,//中频采集结束回调函数，half_int_en = 1时，对应奇数数帧采集结束，half_int_en = 0时，此回调函数不会被调用 */
    .half_int_en = 0, /* 1=使能双帧缓存交替存放采集结果,0=禁能双帧缓存交替存放采集结果 */
};

#define PI 3.14159265358979323846
double window[SAMPLES] = {0};
short  adc_value[SAMPLES] = {0};
/*********************************************************************************************************
* 函数名：adc_rdat_handle
* 描述：  中频数据（一帧）处理函数，在此函数中将数据排列成硬件FFT加速器所需的顺序要求,并触发1D FFT运算
*         如需在FFT变换前对中频数据进行预处理，亦可在此函数中完成
* 输入：  pvBuf：指向帧缓存的首地址
* 返回：  无
******************************************************************************************************** */
ATTR_RAM_SECTION void adc_rdat_handle (void *pvBuf)
{
    int i = 0, j = 0, k = 0;
    short (*sIfAdc)[CHIRPS][SAMPLES][RX_ANTS] = (short (*)[CHIRPS][SAMPLES][RX_ANTS])pvBuf;

    radar_rf_pwr_off(); /* 为降低功耗，采集完中频后可以暂时关闭RF RX相关电源 */

    #if ((DUMP_TYPE == DUMP_RANGE) || (DUMP_TYPE == DUMP_VELOC))
    /* 将数据排列成硬件FFT加速器所需的顺序要求 */
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

    /* 初始化FFT引擎相关配置并触发1D FFT运算 */
    hal_fft_set_config(FFT_IDX_CONFIG_USER_WIN, (void *)&GtRangeWin); /* 初始化FFT窗序列配置 */
    hal_fft_set_config(FFT_IDX_CONFIG_USER_FFT,(void*)&GtRangeCfg); /* 按GtRangeCfg配置初始化FFT引擎 */
    fft_process_trigger(FFT_USER_FFT_MODE); /* 触发硬件FFT引擎进行FFT运算 */
    #else
    g_frame_id++;
    #endif
}

/*********************************************************************************************************
* 函数名：adc_rdat_half_isr
* 描述：  采集完一帧中频数据后的中断处理函数(双缓存的第一帧)
* 输入：  无
* 返回：  无
******************************************************************************************************** */
ATTR_RAM_SECTION void adc_rdat_half_isr(void)
{
    adc_rdat_handle(GsIfAdc[0]);
}

/*********************************************************************************************************
* 函数名：adc_rdat_end_isr
* 描述：  采集完一帧中频数据后的中断处理函数(双缓存的第二帧)
* 输入：  无
* 返回：  无
******************************************************************************************************** */
ATTR_RAM_SECTION void adc_rdat_end_isr(void)
{
    adc_rdat_handle(GsIfAdc[0]);
}

/*********************************************************************************************************
* 函数名：rangeFftIsr
* 描述：  1D FFT全部运算完成后的回调函数,在此函数中对1D结果进行重新排列，并触发2D FFT运算
* 输入：  无
* 返回：  无
******************************************************************************************************** */
ATTR_RAM_SECTION void rangeFftIsr(void)
{
    #if ((DUMP_TYPE == DUMP_RANGE) || (DUMP_TYPE == DUMP_VELOC))
    int i = 0, j = 0, k = 0;

    /* 将数据排列成硬件FFT加速器所需的顺序要求 */
    for (i = 0; i < RX_ANTS; i++) {
        for (j = 0; j < FFTN_RB; j++) {
            for (k = 0; k < CHIRPS; k++) {
                GsVecIn[i][j][k][0] = GsRange[i][k][j][0]; /* 虚部 */
                GsVecIn[i][j][k][1] = GsRange[i][k][j][1]; /* 实部 */
            }
        }
    }

    #if (DUMP_TYPE == DUMP_VELOC)
    /* 初始化FFT引擎相关配置并触发2D FFT运算 */
    hal_fft_set_config(FFT_IDX_CONFIG_USER_WIN, (void *)&GtVelocWin); /* 初始化FFT窗序列配置 */
    hal_fft_set_config(FFT_IDX_CONFIG_USER_FFT,(void*)&GtVelocCfg); /* 按GtVelocCfg配置初始化FFT引擎 */
    fft_process_trigger(FFT_USER_FFT_MODE); /* 触发硬件FFT引擎进行FFT运算 */
    #else
    g_frame_id++;
    #endif
    #endif
}

/*********************************************************************************************************
* 函数名：velocFftIsr
* 描述：  2D FFT全部运算完成后的回调函数
* 输入：  无
* 输出：  递增g_frame_id
* 返回：  无
******************************************************************************************************** */
ATTR_RAM_SECTION void velocFftIsr(void)
{
    g_frame_id++;
}

/*********************************************************************************************************
* 函数名：radar_clock_init
* 描述：  雷达RF系统相关时钟（含DMA）初始化函数
* 输入：  无
* 返回：  无
******************************************************************************************************** */
void radar_clock_init(void)
{
    sys_clk_disable_radar();
    sys_clk_enable_radar();
    sys_clk_enable_dma();
}

/*********************************************************************************************************
* 函数名：dump_frame_data
* 功能：  通过DMA发送帧数据
* 输入：  ulFrameId:帧ID
* 返回：  无
******************************************************************************************************** */
void dump_frame_data(unsigned long ulFrameId)
{
    /* 发送帧头数据 */
    #if (DUMP_TYPE != DUMP_NONE)
    GtFrameHeader.radar_common_cfg.rxSubFrameId = ulFrameId;
    output_port_send_data((u32)&GtFrameHeader, sizeof(GtFrameHeader));
    #endif

    /* 发送帧内容数据 */
    #if (DUMP_TYPE == DUMP_RAW) /* 输出原始数据 */
    output_port_send_data((u32)GsIfAdc[0], sizeof(GsIfAdc[0]));
    #elif (DUMP_TYPE == DUMP_RANGE)
    output_port_send_data((u32)GsVecIn, sizeof(GsVecIn));
    #elif (DUMP_TYPE == DUMP_VELOC)
    output_port_send_data((u32)GsVeloc, sizeof(GsVeloc));
    #endif
}

/*********************************************************************************************************
* 函数名：GetTickCount
* 功能：  读取当前时基计数器,单位为mS
* 输入：  无
* 输出：  无
* 返回：  当前时基计数值
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
// 定义一个函数来判断大小端
int isLittleEndian() {
    uint32_t i = 1; // 设置一个32位的整数
    return ((char*)&i)[0] == 1; // 检查第一个字节的值
}

// 定义一个函数来判断大小端并打印结果
void checkEndianness() {
    if (isLittleEndian()) {
        printf("System is Little-Endian\n");
    } else {
        printf("System is Big-Endian\n");
    }
}


//frameN14：
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

//回复查询
void yihe_ack(MAGTX *magtx)
{
	uint8_t i;
	uint8_t sendbuf[30];
	uint8_t tx_i = 0;
	uint16_t crcdata = 0;
	
	sendbuf[tx_i++]	=	0x55;//帧头
	sendbuf[tx_i++]	=	0xA5;//帧头
	sendbuf[tx_i++]	=	0x0A;//长度
	sendbuf[tx_i++]	=	0xd3;//g_Fmcpara.m_CurrentValue[3];//地址
	
	sendbuf[tx_i++]	=	magtx->distance >>8;	//距离
	sendbuf[tx_i++]	=	magtx->distance;			//距离
	sendbuf[tx_i++]	=	0;	//速度
	//sendbuf[tx_i++]	=	magtx->state;	//1:有人   0：无人
	if(magtx->distance >0 )
	{
		sendbuf[tx_i++]	=	0X01;
	}
	else
	{
		sendbuf[tx_i++]	=	0X00;
	}
		
	sendbuf[tx_i++]	=	magtx->strength >>8;	//信号强度
	sendbuf[tx_i++]	=	magtx->strength;			//信号强度
//	sendbuf[tx_i++]	=	magtx->state ;				//状态
	sendbuf[tx_i++]	=	magtx->small ;				//微动
	sendbuf[tx_i++]	=	0;//radar_close;
	
	for(i=0;i<tx_i;i++)
	{
		crcdata += sendbuf[i];	
	}
//	sendbuf[tx_i++]	=	crcdata >>8;//校验和
	sendbuf[tx_i++]	=	crcdata&0xFF;//校验和
	 
	output_port_send_data((u32)sendbuf,tx_i);
	
}

// 定义复数结构体
typedef struct {
    int32_t real;
    int32_t imag;
} Complex;

// 计算复数的幅值
double complexMagnitude(Complex c) {
    return sqrt((double)c.real * c.real + (double)c.imag * c.imag);
}
	
//做差分
void DiffCalc(double *data, uint8_t offset_size, uint16_t chirpRbCn, double* diff, double* max_value, double* average_value, uint16_t* max_value_index) {
    double max_value_temp = (-DBL_MAX);
    uint16_t max_value_index_temp = 0;
    double sum = 0;

    // 计算第一个差分值
    if (offset_size + 1 < chirpRbCn) {
        diff[0] = (data[offset_size + 1] - data[0]);
    }

    // 计算最后一个差分值
    if (chirpRbCn - offset_size - 1 >= 0) {
        diff[chirpRbCn - offset_size] = (data[chirpRbCn - 1] - data[chirpRbCn - offset_size - 1]);
    }

    // 计算中间差分值并寻找最大值
    for (uint16_t i = 1; i < chirpRbCn - offset_size; i++) {
        diff[i] = data[i + offset_size] - data[i - 1];

        // 更新最大值及其索引
        if (data[i] > max_value_temp && i > START_INDEX) {
            max_value_index_temp = i;
            max_value_temp = data[i];
        }

        // 累加数据用于计算平均值
        sum += data[i];
    }
		sum = sum - data[1] - data[2] - data[3];
 
    // 设置输出参数
    *max_value_index = max_value_index_temp;
    *max_value = max_value_temp;

    // 计算平均值
    if (chirpRbCn - offset_size - 4 > 0) {
        *average_value = sum / (chirpRbCn - offset_size - 4);
    } else {
        *average_value = 0.0;
    }
}

/* 峰的参数 */
typedef struct {
    uint8_t bulge_head;       // 峰头索引
    uint8_t bulge_tail;       // 峰尾索引
//    uint8_t head_index;       // 峰头索引（可能与 bulge_head 相同）
//    uint8_t tail_index;       // 峰尾索引（可能与 bulge_tail 相同）
    uint8_t refline;          // 参考线
    uint8_t width;            // 峰的宽度
    uint16_t extreme_index;    // 极值索引
    double  extreme_value;    // 极值
    uint8_t centroid_value;   // 重心值
    uint8_t relative_altitude;// 相对高度
    uint16_t centroid;        // 重心位置
		double  angle;						//目标角度
} PeakDef;

#define ABS(a,b) ((a>b)?(a-b):(b-a))
#define DIFF_OFFSET   1
#define PEAKS_NUM_MAX 5   //最大距离数据
#define PEAK_DIFF 15			//
#define PEAK_FILL 4
#define MAX_DECREASE 5
#define NORMAL_MAX_VALUE  3
#define THRES				  	4
/* 峰的参数 */
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
// 打印峰的信息
void PrintPeaks(PeakFiner *peakFiner,double res ,char * str) {
	//    for (uint8_t i = 0; i < peakFiner->peak_count; i++) {
	//        PeakDef *peak = &peakFiner->peaks[i];
	//        printf("%s峰 %u: 起始位置=%u, 终止位置=%u, 最大值=%f, 最大值位置=%u,全局最大值=%f,全局最大值位置=%u,距离=%fcm\n",
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
        printf("%s峰 %u: 起始位置=%u, 终止位置=%u, 最大值=%f, 最大值位置=%u,距离=%f cm,速度=%f m\\s,角度=%f度\n",
               str,i + 1, peak->bulge_head, peak->bulge_tail, peak->extreme_value, peak->extreme_index,(peak->extreme_index)*res,speed,angle);
    }
}

// 打印峰的信息
void PrintVPeaks(PeakFiner *peakFiner,double res ,char * str) {
    for (uint8_t i = 0; i < peakFiner->peak_count; i++) {
        PeakDef *peak = &peakFiner->peaks[i];
        printf("%s峰 %u: 起始位置=%u, 终止位置=%u, 最大值=%f, 最大值位置=%u,全局最大值=%f,全局最大值位置=%u,速度=%f m/s\n",
               str,i + 1, peak->bulge_head, peak->bulge_tail, peak->extreme_value, peak->extreme_index,peakFiner->max_value,peakFiner->max_value_index,((double)(peakFiner->max_value_index) - FFTN_VB/2)*res);
    }
}

// 局部找突起函数
void LocalHeadTailCalc(double *data, uint8_t *peak_count, double thres, uint16_t chirpRbCn, double* diff, PeakDef *peaks, uint16_t start, uint16_t end) {
    bool find_flag = false; // false代表找head，true代表找tail
    double partial_sum = 0;
    bool decline_flag = false;
    double extreme_value = 0;
		uint16 extreme_value_index = 0;
	
    for (uint16_t i = start; i < end; i++) {
        if (*peak_count == PEAKS_NUM_MAX) {
            break;
        }		
				if(find_flag == true){
					 if (i == chirpRbCn - 2) { // 查找到结尾记录为tail结束
							peaks[(*peak_count)++].bulge_tail = i;				
							find_flag = false;
							decline_flag = false;
					} else if (extreme_value > data[i] + MAX_DECREASE && data[i + 1] > data[i]) { // 峰下降太多时退出
							peaks[(*peak_count)++].bulge_tail = i;
							find_flag = false;
							decline_flag = false;
					} else if (ABS(diff[i], 0) <= thres && (diff[i - 1] <= -thres || decline_flag == true) && data[i] < PEAK_FILL) { // 寻找peak的tail,带取消abs
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

        if (find_flag == false && diff[i] >= thres) { // 寻找peak的head
            extreme_value = data[i];
						extreme_value_index = i;
            peaks[*peak_count].bulge_head = i - 1;
            find_flag = true;
        }
        if (data[i] >= extreme_value && find_flag == true) {
            extreme_value = data[i]; // 找极大值
						extreme_value_index = i; // 极大值索引
        }
    }
}

// 找出每个 chirp 中幅值最大的 Range Bin 的索引和幅值
void findMaxMagnitudeIndex(uint32_t* fileData, uint16_t n_chirps, uint16_t chirpRbCn, uint8_t n_RX)
{
    // 遍历每个接收天线
    for (int rx = 0; rx < n_RX; rx++) {
        // 遍历每个 chirp
        for (int chirp = 0; chirp < n_chirps; chirp++) {
            int maxIndex = 0; // 初始化最大幅值的索引
            double maxMagnitude = (-DBL_MAX); // 初始化最大幅值
						 // 用于存储当前 chirp 的所有 Magnitude
            double magnitudes[chirpRbCn];
						double magnitudes_sqr[chirpRbCn];
						double diff[chirpRbCn]; //创建足够长的差分数组
            // 遍历每个 Range Bin
            for (int rb = 0; rb < chirpRbCn; rb++) {
                // 计算当前 Range Bin 的索引
                int index = (rx * n_chirps * chirpRbCn + chirp + n_chirps*rb);

                // 提取 I 和 Q 部分
                int16_t I = (int16_t)(fileData[index] & 0xFFFF); // 提取低 16 位作为 I
                int16_t Q = (int16_t)(fileData[index] >> 16); // 提取高 16 位作为 Q
							
								// 打印当前 Range Bin 的实部和虚部
								 //printf("Chirp %d, RX %d, RB %d: I = %d, Q = %d\n", chirp, rx, rb, I, Q);							

                // 创建复数
                Complex c = {I, Q};

                // 计算幅值
							  double magnitude = complexMagnitude(c);
                magnitude = magnitude*64;

								if(rb > 0)
								{
									magnitude *= 2;
								}
								magnitude /= SAMPLES;
								magnitudes_sqr[rb] = magnitude;

								// 计算幅值
							{		
								// magnitudes_sqr[chirp] = complexMagnitude(cVb[chirpRbCn_num][chirp]);
								if (magnitudes_sqr[rb] > VTRESHOLD_VALUE && magnitudes_sqr[rb] > maxMagnitude && rb > 6) {
										maxMagnitude = magnitudes_sqr[rb];
										maxIndex = rb;
								}
							}
															
								
								// 防止log10(0)导致的数学错误
	//								if (magnitude == 0) {
	//										magnitude = -100.0;  // 一个非常小的值，表示幅值为0
	//								} else {
	//										magnitude = 20 * log10(magnitude / 32768);
	//								}
	//								
	//								// 保存当前 Range Bin 的幅值
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
						// 打印当前 chirp 的所有 Magnitude
            printf("Chirp %d, RX %d: Magnitudes = [", chirp, rx);
            for (int rb = 0; rb < chirpRbCn; rb++) {
                printf("%.2f", magnitudes_sqr[rb]);
                if (rb < chirpRbCn - 1) {
                    printf(", ");
                }
            }
	          printf("]\n");
						#endif
            // 打印调试信息
            //printf("Chirp %d, RX %d: Max Magnitude Index = %d, Max Magnitude = %f\n", chirp, rx, maxIndex, maxMagnitude);
						break;	
				}
				//break;
    }
}

// 用于存储处理后的数据
Complex cVb[FFTN_RB][CHIRPS];

// 用于实现 fftshift 操作
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
		int maxIndex = 0; // 初始化最大幅值的索引
		double maxMagnitude = (-DBL_MAX); // 初始化最大幅值
		// 用于存储当前 chirp 的所有 Magnitude
		double magnitudes[chirpRbCn];
		double magnitudes_sqr[chirpRbCn];

		// 遍历每个 chirp
		for (int chirp = 0; chirp < n_chirps; chirp++) {
				// 计算当前 Range Bin 的索引
				int index = (n_RX_num * n_chirps * chirpRbCn + chirp + n_chirps*chirpRbCn_num);

				// 提取 I 和 Q 部分
				int16_t I = (int16_t)(fileData[index] & 0xFFFF); // 提取低 16 位作为 I
				int16_t Q = (int16_t)(fileData[index] >> 16); // 提取高 16 位作为 Q
			
				// 打印当前 Range Bin 的实部和虚部
			 // printf("Chirp %d, RX %d, RB %d: I = %d, Q = %d\n", chirp, n_RX_num, chirpRbCn_num, I, Q);							

				// 创建复数
				Complex c = {I, Q};
				cVb[chirpRbCn_num][chirp] = c;
		}
		
		// 计算幅值
		for (int chirp = 0; chirp < n_chirps; chirp++) {		
				 magnitudes_sqr[chirp] = complexMagnitude(cVb[chirpRbCn_num][chirp]);
		}
			 
//		#if DEBUG_VRANGE_BIN
//		// 打印当前 chirp 的所有 Magnitude
//		printf("前chirpRbCn %d, RX %d: Magnitudes = [", chirpRbCn_num, n_RX_num);
//		for (int chirp = 0; chirp < n_chirps; chirp++) {	
//				printf("%.2f", magnitudes_sqr[chirp]);
//				if (chirp < n_chirps - 1) {
//						printf(", ");
//				}
//		}
//		printf("]\n");
//		#endif
		
		//fftshift(&cVb[chirpRbCn_num][0], n_chirps);
		// 计算幅值
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
		// 打印当前 chirp 的所有 Magnitude
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


//获取角度信息
double findAngleStatue(uint32_t* fileData, uint16_t n_chirps, uint16_t chirpRbCn, uint8_t n_RX,
	uint16_t n_chirps_num, uint16_t chirpRbCn_num)
{
		 // 存储每个天线的相位
    double phase[n_RX];
    // 遍历每个接收天线
    for (int rx = 0; rx < n_RX; rx++) {
			// 计算当前 Range Bin 的索引
			int index = (rx * n_chirps * chirpRbCn + n_chirps_num + n_chirps*chirpRbCn_num);

			// 提取 I 和 Q 部分
			int16_t I = (int16_t)(fileData[index] & 0xFFFF); // 提取低 16 位作为 I
			int16_t Q = (int16_t)(fileData[index] >> 16); // 提取高 16 位作为 Q
			
			// 计算相位（单位：弧度）
      phase[rx] = atan2(Q, I);
			
			// 打印每个天线的相位（转换为度）
			printf("天线 %d: I = %d, Q = %d, 相位 = %.2f 度\n", rx, I, Q, phase[rx] * 180.0 / PI);
    }
    
		double phase_diff = 0;
    // 打印相邻天线之间的相位差
    for (int i = 0; i < n_RX - 1; i++) {
        phase_diff = phase[i+1] - phase[i];
        
        // 相位差归一化到 [-π, π]
        while (phase_diff > PI) phase_diff -= 2 * PI;
        while (phase_diff < -PI) phase_diff += 2 * PI;
        
        printf("天线 %d 和 %d 之间的相位差 = %.2f 度\n", i, i+1, phase_diff * 180.0 / PI);
    }
		
		//基于相位差公式计算角度
		double lambda = 0.0051;
    double d = 	lambda/2;
		double estimated_angle = 0;
    estimated_angle = asin(phase_diff * lambda / ( 2 * PI * d ));
    
    // 将弧度转换为度
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
* 函数名：main
* 功能：  主函数，演示调用硬件FFT引擎实现中频ADC采集，以及1D FFT及2D FFT处理
* 输入：  无
* 返回：  无
******************************************************************************************************** */
int main(void)
{
    unsigned long ulFrameId = 0;
    unsigned long ulTmp = 0, ulStamp = 0;

    clock_init(); /* 系统时钟 */
    radar_clock_init(); /* 雷达RF系统相关时钟（含DMA）初始化函数 */
    clk_mcu_set(CLK_SRC_PLL, 80000000U); /* 系统主频初始化 */
    hal_dualtimer_init(); /* 默认定时器初始化 */
    SysTick_Config(clk_mcu_get() / SYS_TICK_FREQ); /* 系统Tick定时器初始化 */
    hal_uart_init(UART_ID_0, &GtUartCfg); /* 普通Log串口初始化 */

    /* 数据输出相关初始化，Dump端口与Log串口复用，output_dma_config()将重置串口波特率 */
    #if (DUMP_TYPE != DUMP_NONE)
    output_dma_config(); /* 数据输出DMA初始化 */
    write_data_frame_header((u8*)&GtFrameHeader); /* 帧头信息初始化 */
    #if (DUMP_TYPE != DUMP_RAW)
    GtFrameHeader.radar_common_cfg.flag = (1<<5)| (1<<1) | (1<<6);
    #endif
    #endif

    hal_fft_init(); /* FFT 加速引擎初始化 */
    radar_fmcw_cfg(); /* FMCW 扫频相关参数初始化 */
    hal_fft_set_config(FFT_IDX_CONFIG_ADC_RDAT,(void*)&GtIfSampleCfg); /* 加速引擎配置 */
    radar_frame_trigger_mode_set(TRIGGER_MODE_INTERNAL); /* RF扫频配置为内部触发 */
    radar_frame_start(); /* 启动RF扫频 */
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
		
		    // 生成Hanning窗序列
    for (int n = 0; n < SAMPLES; n++) {
        // MATLAB的hanning函数实现（对称窗）
        window[n] = 0.5 * (1 - cos(2 * PI * n / (SAMPLES - 1)));
    }
		
    while(1) {
        if (ulFrameId != g_frame_id) { /* 已完成二阶FFT运算 */
						#if 0
            dump_frame_data(ulFrameId); /* dump出相关数据 */
						#else
            ulFrameId = g_frame_id;
						memset((uint8_t *)&FiderPeakStatue,0,sizeof(FiderPeakStatue));
						memset((uint8_t *)&FiderVPeakStatue,0,sizeof(FiderPeakStatue));
						//求距离1 和 距离2			
						int n_chirps = CHIRPS; // chirp 数量
						int chirpRbCn = FFTN_RB; // 每个 chirp 的 Range Bin 数量
						int n_RX = RX_ANTS; // 接收天线数量
						//printf("OUT_FRAME_HEADER_SIZE %d\n", OUT_FRAME_HEADER_SIZE);
						FiderPeakStatue.thres = THRES;
						// 找出每个 chirp 中幅值最大的 Range Bin 的索引
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
						// 打印结果
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
												
            /* 帧率控制，帧间隔控制为200mS */
            do {
                ulTmp = GetTickCount();
                ulTmp = (ulTmp >= ulStamp)? (ulTmp - ulStamp) : (0xFFFFFFFFUL - ulStamp + ulTmp);
            } while (ulTmp < 200);

            radar_rf_pwr_on(); /* 若采集完中频后有关闭RF相关电源，则在此重新使能RF RX相关电源 */
            radar_frame_start(); /* 若RF扫频配置为单次触发，则每次扫频结束后需要重新调用此函数 */
            ulStamp = GetTickCount();
        }
    }
}


// 找出每个 chirp 中幅值最大的 Range Bin 的索引和幅值
//void findMaxMagnitudeIndex(uint32_t* fileData, int n_chirps, int chirpRbCn, int n_RX,
//	int maxIndices[CHIRPS*RX_ANTS][MAX_REGIONS],
//		double maxMagnitudes[CHIRPS*RX_ANTS][MAX_REGIONS])
//{
//    // 遍历每个接收天线
//    for (int rx = 0; rx < n_RX; rx++) {
//        // 遍历每个 chirp
//        for (int chirp = 0; chirp < n_chirps; chirp++) {
//            int maxIndex = 0; // 初始化最大幅值的索引
//            double maxMagnitude = (-DBL_MAX); // 初始化最大幅值
//						 // 用于存储当前 chirp 的所有 Magnitude
//            double magnitudes[chirpRbCn];
//					  int start_index = -1;
//            int end_index = -1;
//						int in_region = 0;
//	          int region_count = 0;

//            // 用于存储前三个大于 -39 的区域
//            int regions[MAX_REGIONS][2] = {{-1, -1}};	
//						
//            // 遍历每个 Range Bin
//            for (int rb = 0; rb < chirpRbCn; rb++) {
//                // 计算当前 Range Bin 的索引
//                int index = (rx * n_chirps * chirpRbCn + chirp + n_chirps*rb);

//                // 提取 I 和 Q 部分
//                int16_t I = (int16_t)(fileData[index] & 0xFFFF); // 提取低 16 位作为 I
//                int16_t Q = (int16_t)(fileData[index] >> 16); // 提取高 16 位作为 Q
//							
//								// 打印当前 Range Bin 的实部和虚部
//  //             printf("Chirp %d, RX %d, RB %d: I = %d, Q = %d\n", chirp, rx, rb, I, Q);							
//	//							
//	//								int32_t I_TEMP = I*64;
//	//								int32_t Q_TEMP = Q*64;
//	//							
//                // 创建复数
//                Complex c = {I, Q};

//								
//                // 计算幅值
//                double magnitude = complexMagnitude(c)*64;

//								if(rb > 0)
//								{
//									magnitude *= 2;
//								}
//								magnitude /= SAMPLES;
//								// 防止log10(0)导致的数学错误
//								if (magnitude == 0) {
//										magnitude = -100.0;  // 一个非常小的值，表示幅值为0
//								} else {
//										magnitude = 20 * log10(magnitude / 32768);
//								}
//								
//								// 保存当前 Range Bin 的幅值
//                magnitudes[rb] = magnitude;
//								
//								if(rb > 6)
//								{
//									// 检测大于 -39 的区域
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

//            // 保存最大幅值的索引和幅值
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
//						// 打印当前 chirp 的所有 Magnitude
//            printf("Chirp %d, RX %d: Magnitudes = [", chirp, rx);
//            for (int rb = 0; rb < chirpRbCn; rb++) {
//                printf("%.2f", magnitudes[rb]);
//                if (rb < chirpRbCn - 1) {
//                    printf(", ");
//                }
//            }
//	          printf("]\n");
//						#endif
//            // 打印调试信息
//           // printf("Chirp %d, RX %d: Max Magnitude Index = %d, Max Magnitude = %f\n", chirp, rx, maxIndex, maxMagnitude);
//        }
//    }
//}



//// 找出每个 chirp 中幅值最大的 Range Bin 的索引和幅值
//void findMaxMagnitudeIndex(uint32_t* fileData, int n_chirps, int chirpRbCn, int n_RX,
//	int maxIndices[CHIRPS*RX_ANTS][MAX_REGIONS],
//		double maxMagnitudes[CHIRPS*RX_ANTS][MAX_REGIONS])
//{
//    // 遍历每个接收天线
//    for (int rx = 0; rx < n_RX; rx++) {
//        // 遍历每个 chirp
//        for (int chirp = 0; chirp < n_chirps; chirp++) {
//            int maxIndex = 0; // 初始化最大幅值的索引
//            double maxMagnitude = (-DBL_MAX); // 初始化最大幅值
//						 // 用于存储当前 chirp 的所有 Magnitude
//            double magnitudes[chirpRbCn];
//					  int start_index = -1;
//            int end_index = -1;
//						int in_region = 0;
//	          int region_count = 0;

//            // 用于存储前三个大于 -39 的区域
//            int regions[MAX_REGIONS][2] = {{-1, -1}};	
//						
//            // 遍历每个 Range Bin
//            for (int rb = 0; rb < chirpRbCn; rb++) {
//                // 计算当前 Range Bin 的索引
//                int index = (rx * n_chirps * chirpRbCn + chirp + n_chirps*rb);

//                // 提取 I 和 Q 部分
//                int16_t I = (int16_t)(fileData[index] & 0xFFFF); // 提取低 16 位作为 I
//                int16_t Q = (int16_t)(fileData[index] >> 16); // 提取高 16 位作为 Q
//							
//								// 打印当前 Range Bin 的实部和虚部
//								// printf("Chirp %d, RX %d, RB %d: I = %d, Q = %d\n", chirp, rx, rb, I, Q);													
//					
//                // 创建复数
//                Complex c = {I, Q};								

//								double magnitude = sqrt((double)c.real * c.real + (double)c.imag * c.imag);//complexMagnitude(c);
//								// 保存当前 Range Bin 的幅值
//                magnitudes[rb] = magnitude;               
//							 // 更新最大幅值及其索引
//                if (magnitude > TRESHOLD_VALUE && magnitude > maxMagnitude && rb > 6) {
//	                  maxMagnitude = magnitude;
//	                  maxIndex = rb;
//									//	break;
//                }
//            }

//            // 保存最大幅值的索引和幅值
//						for(int i = 0; i < MAX_REGIONS;i++)
//						{
//							{
//								maxIndices[chirp + rx * n_chirps][i] = maxIndex;
//								maxMagnitudes[chirp + rx * n_chirps][i]  = maxMagnitude;
//							}
//						}
//						#if DEBUG_RANGE_BIN
//						// 打印当前 chirp 的所有 Magnitude
//            printf("Chirp %d, RX %d: Magnitudes = [", chirp, rx);
//            for (int rb = 0; rb < chirpRbCn; rb++) {
//                printf("%.2f", magnitudes[rb]);
//                if (rb < chirpRbCn - 1) {
//                    printf(", ");
//                }
//            }
//	          printf("]\n");
//						#endif
//            // 打印调试信息
//            printf("Chirp %d, RX %d: Max Magnitude Index = %d, Max Magnitude = %f\n", chirp, rx, maxIndex, maxMagnitude);
//        }
//    }
//}


