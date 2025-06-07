#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "hal_fft.h"
#include "hal_uart.h"
#include "clock.h"
#include "common.h"
#include "cfg_radar_hwa.h"
#include "output_config.h"
#include "frame_header.h"
#include "fft_win.h"
#include "math.h"
#include "string.h"

#define FFT_MEM  __attribute__ ((section(".radar_mem"),__aligned__(32)))
#define WIN_MEM  __attribute__ ((section(".fft_win_mem"),__aligned__(32)))

#define CHIRPS                   8
#define SAMPLES                  256// Chirp内采样点数量，通常也是一维FFT点数
#define FFTS_1D                  8// FFT阶数，对应FFT点数为2^FFTS_1D
#define FFTN_1D                  (1UL << FFTS_1D)// FFT点数
#define FFTN_RB                  (SAMPLES)// 一阶FFT输出Range Bin数量
#define FFT_TYPE                 2//1=RFFT,FFT输入为仅实部；2=CFFT，FFT输入为复数，虚部在前，实部在后

#if (SAMPLES != FFTN_1D)
#warmming "FFT点数与采样点数量不一致，请注意检查！如确认无误，请删除此错误警告！"
#endif

/*********************************************************************************************************
* 以下宏定义与FFT测试信号相关
*********************************************************************************************************/
#define FT 0.25f// 测试信号频率
#define FS 4UL// 采样信号频率
#define PI 3.1415927f// Pi

/*********************************************************************************************************
* 函数声明
*********************************************************************************************************/
void usr_fft_isr(void);
void radar_clock_init(void);

/*********************************************************************************************************
* 全局变量或常量定义
*********************************************************************************************************/
volatile int GulFlg = 0;// 完成标识
volatile radar_frame_hdr_t GtFrameHeader = {0};
short GsRangeIn[CHIRPS][SAMPLES][FFT_TYPE] FFT_MEM = {0};// 定义能容纳一帧FFT输入的存储空间
short GsRangeOut[CHIRPS][FFTN_RB][2] FFT_MEM = {0};

// 窗序列定义,窗序列长度必须与FFT点数相同
unsigned short GusRangeWin[] WIN_MEM = {
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

// FFT窗参数配置
const fft_win_para_t  GtWinCfg = {
    .enable = 0,// 1=使能硬件FFT自动加窗运算，0=禁止硬件FFT自动加窗功能
    .fft_mode = FFT_USER_FFT_MODE,
    .waddr = (u32)GusRangeWin,// 窗序列首地址
};

// USER FFT 参数配置
user_fft_para_t GtRangeFftCfg = {
    .data_type = (FFT_TYPE == 1)? REAL_DATA : COMPLEX_DATA,// FFT类型,COMPLEX_DATA=复数FFT, REAL_DATA=实数FFT
    .is_power_out  = 0,// 0=输出复数（虚部在前）,1=输出模平方
    .shift_value   = FFTS_1D,// 移位次数
    .fft_point     = FFTN_1D,
    .nframe        = CHIRPS,
    .in_frame_size = SAMPLES,
    .out_frame_size = FFTN_RB,
    .in_buf         = (u32)GsRangeIn,// 输入缓存
    .out_buf        = (u32)GsRangeOut,// 输出缓存
    .end_callback   = usr_fft_isr,// 转换完成回调函数
};

// 串口参数配置,默认Log输出波特率为921600, 若吐数串口与Log串口复用，波特率将被output_dma_config()函数重置为5Mbps
uart_cfg_t GtUartCfg = {
    .baud_rate = UART_BAUD_RATE_921600,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .data_width = UART_DATA_WIDTH_8BIT,
    .fifo_cfg.fifo_en = 1U,
};

/*********************************************************************************************************
* 函数名：radar_clock_init
* 功能：  雷达相关外设时钟初始化
* 输入：  无
* 返回：  无
*********************************************************************************************************/
void radar_clock_init(void)
{
    sys_clk_disable_radar();
    sys_clk_enable_radar();
    sys_clk_enable_dma();
}

/*********************************************************************************************************
* 函数名：usr_fft_isr
* 功能：  USER FFT变换完成回调函数
* 输入：  无
* 输出：  修改GulFlg标志及帧ID
* 返回：  无
*********************************************************************************************************/
void usr_fft_isr(void)
{
    GulFlg = 1;
    GtFrameHeader.radar_common_cfg.rxSubFrameId++;
}

/*********************************************************************************************************
* 函数名：dataTestInit
* 功能：  FFT_TYPE=1，生成正弦测试信号，FFT_TYPE=2,生成正交的测试信号（复数，含实部和虚部）
* 输入：  无
* 输出：  生成CHIRPS组由SAMPLES个采样点组成的测试信号
*         存储到GsRangeIn数组中，以便用于FFT硬件运算测试
* 返回：  无
*********************************************************************************************************/
void dataTestInit (void)
{
    short *psBuf = (short *)GsRangeIn;
    int i =  0, j = 0;
    
    memset(psBuf, 0, sizeof(GsRangeIn));
    
    //生成测试信号
    for (j = 0; j < SAMPLES; j++) {// 输入数据虚部在前[15:0]Q，[31:16]I
        #if (FFT_TYPE == 1)
        *psBuf++ = 1000 * sin(2*PI*FT*j/FS);
        #else
        *psBuf++ = 1000 * sin(2*PI*FT*j/FS);
        *psBuf++ = 1000 * cos(2*PI*FT*j/FS);
        #endif
    }
    for (i = 1; i < CHIRPS; i++) {
        memcpy(GsRangeIn[i], GsRangeIn[0], sizeof(GsRangeIn[0]));
    }
}

/*********************************************************************************************************
* 函数名：main
* 功能：  主函数，调用硬件FFT引擎实现FFT变换，通过串口吐出FFT变换前后的数据
* 输入：  无
* 返回：  无
*********************************************************************************************************/
int main(void)
{
    int i = 0;
    radar_clock_init();
    clk_mcu_set(CLK_SRC_PLL, 80000000U);
    hal_uart_init(UART_ID_0, &GtUartCfg);
    hal_fft_init();
    output_dma_config();
    write_data_frame_header((u8*)&GtFrameHeader);
    
    printf("*** user fft example ***\r\n");
    dataTestInit();//生成测试数据
    hal_fft_set_config(FFT_IDX_CONFIG_USER_WIN, (void *)&GtWinCfg);
    hal_fft_set_config(FFT_IDX_CONFIG_USER_FFT,(void*)&GtRangeFftCfg);
    while (1) {
        fft_process_trigger(FFT_USER_FFT_MODE);
        while (GulFlg == 0) {
            ;
        }
        GulFlg = 0;
        output_port_send_data((u32)&GtFrameHeader, sizeof(GtFrameHeader));//输出帧头
        output_port_send_data((u32)GsRangeIn, sizeof(GsRangeIn));//输出原始数据
        output_port_send_data((u32)GsRangeOut, sizeof(GsRangeOut));//输出变换后的数据
    }
    
    return 0;
}
