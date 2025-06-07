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

#include "math.h"
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
#include "clk_cfg.h"
#include "cpu.h"
#include "cfg_radar_hwa.h"
#include "common.h"
#include <float.h>

#define FRAME_HEADER_LEN ((sizeof(ATCommV2_RadarCommonDef)+sizeof(ATCommV2_RadarSystem)+sizeof(ATCommV2_SubframeConfig)*8) >> 2)


#define OUT_FRAME_HEADER_SIZE   (FRAME_HEADER_LEN*4)
#define OUT_DAT_SIZE            (FFT_OUT_VALID_RB*FFT_NFRAME *4*FFT_NUM_ADC_CH)


#define FFT_BUF_ATTRIBUTES  __attribute__ ((section(".radar_mem"),__aligned__(32)))
#define FFT_WIN_ATTRIBUTES  __attribute__ ((section(".fft_win_mem"),__aligned__(32)))

enum enum_fifo_wp_status {
	DEV_FIFO_WP_NA = 0,
	DEV_FIFO_WP_HALF = 1,
	DEV_FIFO_WP_END = 2,
};

#ifdef WIN_ENABLE
#include "fft_win.h"
static u16 win[] FFT_WIN_ATTRIBUTES = {
	#if (FFT_RSIZE == 8)
    u16_hann8(),
    #elif (FFT_RSIZE == 16)
    u16_hann16(),
    #elif (FFT_RSIZE == 32)
    u16_hann32(),
    #elif (FFT_RSIZE == 64)
    u16_hann64(),
    #elif (FFT_RSIZE == 128)
    u16_hann128(),
    #elif (FFT_RSIZE == 256)
    u16_hann256(),
    #elif (FFT_RSIZE == 512)
    u16_hann512(),
    #elif (FFT_RSIZE == 1024)
    u16_hann1024(),
    #else
    #error "窗序列长度错误，请注意检查！"
    #endif
};
#endif

static u32 fft_out_buf[FRAME_HEADER_LEN + FFT_OUT_VALID_RB*FFT_NUM_ADC_CH*2*2] FFT_BUF_ATTRIBUTES = {0};
static volatile unsigned int fifo_state = 0;
static radar_frame_hdr_t *frame_header =  (radar_frame_hdr_t*)fft_out_buf;
static u32 *range_fft_buffer;

extern void radar_fmcw_cfg(void);

static void adc_rfft_end_isr(void)
{
	fifo_state = DEV_FIFO_WP_END;
	frame_header->radar_common_cfg.rxSubFrameId++;
}

static void adc_rfft_half_isr(void)
{
	fifo_state = DEV_FIFO_WP_HALF;
	frame_header->radar_common_cfg.rxSubFrameId++;
}

static u32 get_current_data_addr(void)
{
	u32 d_update = 0;

	critical_section_start();

	if (fifo_state == DEV_FIFO_WP_HALF) {
		d_update = (u32)range_fft_buffer;
	} else if (fifo_state == DEV_FIFO_WP_END) {
		d_update = (u32)range_fft_buffer + OUT_DAT_SIZE;
	}
	fifo_state = DEV_FIFO_WP_NA;

	critical_section_end();

	return d_update;
}

static void radar_clock_init(void)
{
	sys_clk_disable_radar();
	sys_clk_enable_radar();
	sys_clk_enable_dma();
}

static void adc_fft_win_cfg(void)
{
	fft_win_para_t win_cfg = {
#ifdef WIN_ENABLE
		.enable = 1,
		.waddr    = (u32)win,
#else
		.enable = 0,
		.waddr    = (u32)NULL,
#endif
		.fft_mode = FFT_ADC_RANGE_FFT_MODE,
	};

	hal_fft_set_config(FFT_IDX_CONFIG_ADC_WIN,(void*)&win_cfg);
}

#define MAX_REGIONS 1
//void findMaxMagnitudeIndex(uint32_t* fileData, int n_chirps, int chirpRbCn, int n_RX, int* maxIndices);
void findMaxMagnitudeIndex(uint32_t* fileData, int n_chirps, int chirpRbCn, int n_RX,int maxIndices[FFT_NFRAME*FFT_NUM_ADC_CH][MAX_REGIONS], double maxMagnitudes[FFT_NFRAME*FFT_NUM_ADC_CH][MAX_REGIONS]);

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


//frameN69：
int16 fileData_temp[2*FFT_OUT_VALID_RB * FFT_NFRAME * FFT_NUM_ADC_CH] = 
//{
//	0, -9315, 0, -9320, 0, -9359, 0, -9348, 0, -9310, 0, -9420, -926, 3738, -980, 3801, -308, 2134, -303, 2091, 369, -1035, 397, -1015, -196, -378, -194, -327, 586, -25, 637, -61, -784, 1107, -859, 1106, 489, -1711, 496, -1697, -153, 920, -163, 939, 504, -238, 494, -259, -788, 378, -756, 303, 480, -285, 480, -248, 551, -254, 112, -171, 115, -176, 105, -196, 
//142, -207, 14, -127, 71, -90, 159, -187, -615, 628, -630, 679, -611, 668, -629, 727, -615, 698, -611, 652, 662, -795, 641, -808, -65, 586, -36, 557, -406, -191, -400, -136, 388, -344, 360, -337, -284, 415, -261, 330, 314, -112, 265, -12, -242, -134, -232, -181, 245, 42, 312, 65, -523, 454, -587, 439, 97, -420, 92, -408, 991, -393, 1052, -370, 
//-151, 2718, -161, 2633, -2512, -3573, -2570, -3464, 2525, 385, 2519, 271, -754, 1228, -676, 1299, 393, -127, 340, -181, -753, -267, -763, -206, 518, -62, 600, -130, 799, -105, 648, -68, -799, 483, -658, 491, -677, -174, -667, -129, 554, -224, 439, -303, 593, 10, 685, 67, -807, 512, -875, 514, 667, -435, 728, -488, -681, -218, -698, -203, 858, 838, 823, 884, 
//-995, -983, -953, -1014, 723, 507, 680, 506, -746, -25, -702, -63, 615, 133, 577, 160, -152, -293, -85, -215, 214, -93, 102, -168, 65, 183, 118, 162, -502, -79, -407, -14, 381, 236, 218, 188, -75, 33, 6, 15, -161, -199, -124, -120, 199, -70, 110, -170, -243, 115, -184, 161, 164, 30, 141, 70, -24, 30, 91, -23, 112, -102, -84, -38
//};

{
	0, -9527, 0, -9620, 0, -9658, 0, -9688, 0, -9635, 0, -9592, -862, 4024, -955, 4001, -502, 1849, -503, 1828, 430, -900, 504, -907, -186, -67, -227, -42, 273, -590, 263, -554, -147, 229, -120, 189, 1252, 767, 1259, 785, -1384, -705, -1443, -718, -49, 184, -31, 241, 415, 305, 490, 269, 342, 42, 243, -47, 293, 131, -415, -383, -409, -464, -360, -388, 
-341, -377, -411, -424, -460, -339, -395, -443, 604, 228, 477, 255, 646, 281, 506, 280, 534, 236, 569, 294, -963, -807, -996, -746, 178, 1248, 171, 1220, 581, -943, 628, -997, -989, 408, -1080, 400, 1308, -7, 1470, 77, -1109, 147, -1203, 98, 728, -1200, 645, -1211, 441, 1698, 581, 1746, -954, -788, -953, -844, 373, -116, 238, -137, 82, 195, 141, 244, 
-228, 844, -219, 819, -383, -1318, -398, -1254, 644, 253, 684, 227, 152, 397, 153, 371, -665, -194, -720, -193, 447, 369, 466, 414, 95, -269, 160, -339, -454, -206, -543, -183, 128, 253, 163, 294, 7, -528, 2, -577, 256, 588, 256, 595, -165, 131, -161, 165, -89, -647, -101, -636, 197, 368, 240, 260, 59, -20, 36, 70, -488, 182, -466, 196, 
595, -713, 522, -691, -145, 848, -100, 729, -78, -104, -82, -5, -42, -422, -52, -450, 55, 481, 55, 439, -335, -345, -313, -313, 490, -42, 444, -14, -387, -123, -290, -123, 303, 447, 245, 472, -157, -166, -209, -236, 7, -113, 92, -139, 75, 165, 37, 219, 92, -247, 86, -195, -243, 166, -236, 87, 118, 142, 46, 100, -117, -256, 44, -129
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

int main(void)
{
	uart_cfg_t uart_cfg;
	adc_range_fft_para_t fft_cfg = {0};
	u32 fft_dat = 0;
	radar_clock_init();

	range_fft_buffer = &fft_out_buf[FRAME_HEADER_LEN];
#if SYSTEM_CLK_80M
	clk_mcu_set(CLK_SRC_PLL, 80000000U);
#else
	clk_mcu_set(CLK_SRC_XTAL, 40000000U);
#endif
	uart_cfg.baud_rate = UART_BAUD_RATE_115200;//UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	hal_dualtimer_init();

	radar_clock_init();
	write_data_frame_header((u8*)fft_out_buf);

	hal_fft_init();
	radar_fmcw_cfg();

	adc_fft_win_cfg();

	/* adc fft config*/
	fft_cfg.enable =  1;
#ifdef FFT_DC_CUT_ENABLE
	fft_cfg.dc_cut = 1;
#endif
	fft_cfg.num_adc_ch = FFT_MONO_ADC_CH;
#if FFT_NUM_ADC_CH == 2
	fft_cfg.num_adc_ch = FFT_DUAL_ADC_CH;
#endif
	fft_cfg.shift_value = 0x4;
	fft_cfg.fft_point = FFT_RSIZE;
	fft_cfg.nframe = FFT_NFRAME;
	fft_cfg.rfft_upper_rb = FFT_OUT_VALID_RB;
	fft_cfg.in_frame_size = FFT_INPUT_SIZE;
	fft_cfg.out_buf = (u32)range_fft_buffer;
	fft_cfg.half_int_en = 1;
	fft_cfg.end_callback = adc_rfft_end_isr;
	fft_cfg.half_callback = adc_rfft_half_isr;

	hal_fft_set_config(FFT_IDX_CONFIG_ADC_RANGE_FFT,(void*)&fft_cfg);
	// output config
	output_dma_config();

	radar_frame_trigger_mode_set(TRIGGER_MODE_INTERNAL);
	radar_frame_start();
	#define DEBUG   0
	#define USE_PC_SOFT 0
	#define  DEBUG_RANGE_BIN 0
	#define  TRESHOLD_VALUE  2000 //2500000
	
	double dres = 3.22;
	double last_distance[3] = {0};
	#if USE_PC_SOFT
	uart_cfg.baud_rate = UART_BAUD_RATE_115200;//UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1;
	(void)hal_uart_init(UART_ID_0, &uart_cfg);
	#endif
	
	while(1) {
		while(fifo_state == DEV_FIFO_WP_NA) {
			__NOP();
		}
		fft_dat = get_current_data_addr();
		if (fft_dat != 0) {		
			//printf("Chirp1 \n");
			#if  DEBUG
			output_port_send_data((u32)frame_header,OUT_FRAME_HEADER_SIZE);
			output_port_send_data((u32)fft_dat,OUT_DAT_SIZE);
			#else 
			//求距离1 和 距离2			
			int n_chirps = FFT_NFRAME; // chirp 数量
			int chirpRbCn = FFT_OUT_VALID_RB; // 每个 chirp 的 Range Bin 数量
			int n_RX = FFT_NUM_ADC_CH; // 接收天线数量
			//printf("OUT_FRAME_HEADER_SIZE %d\n", OUT_FRAME_HEADER_SIZE);
			// 用于存储每个 chirp 中最大幅值的 Range Bin 索引
			int maxIndices[FFT_NFRAME*FFT_NUM_ADC_CH][MAX_REGIONS] = {0};
			double maxMagnitudes[FFT_NFRAME*FFT_NUM_ADC_CH][MAX_REGIONS] = {0};
			
			// 找出每个 chirp 中幅值最大的 Range Bin 的索引
			findMaxMagnitudeIndex((uint32_t*)fft_dat, n_chirps, chirpRbCn, n_RX, maxIndices,maxMagnitudes);
			//findMaxMagnitudeIndex((uint32_t*)fileData_temp, n_chirps, chirpRbCn, n_RX, maxIndices,maxMagnitudes);
			MAGTX magtx = {0};
			double cur_distance = 0;
			// 打印结果
			for (int i = 0; i < n_RX * n_chirps; i++) {			
				for(int j = 0; j < MAX_REGIONS; j++)
				{
					cur_distance = MIN(last_distance[0]/*MIN(last_distance[1],last_distance[0])*/,maxIndices[i][j]*dres);

					#if USE_PC_SOFT		
					magtx.distance = cur_distance;
					//magtx.strength = maxMagnitudes[i][j] + 100;
				  magtx.strength = maxMagnitudes[i][j]/1000;
					yihe_ack(&magtx);
					#else
					printf("Chirp %d, RX %d Diatance%d : Max Magnitude Index = %d, Max Magnitude = %f,distance = %f cm\n", 
								 i % n_chirps, i / n_chirps,j,maxIndices[i][j], maxMagnitudes[i][j],cur_distance);
					#endif
					last_distance[2] = last_distance[1];
					last_distance[1] = last_distance[0];
					last_distance[0] = maxIndices[i][j]*dres;
				}
			}

			
		//			checkEndianness();
			#endif
      radar_frame_start();
			//printf("Chirp2 \n");
		}
	}
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
	

// 找出每个 chirp 中幅值最大的 Range Bin 的索引和幅值
void findMaxMagnitudeIndex(uint32_t* fileData, int n_chirps, int chirpRbCn, int n_RX,
	int maxIndices[FFT_NFRAME*FFT_NUM_ADC_CH][MAX_REGIONS],
		double maxMagnitudes[FFT_NFRAME*FFT_NUM_ADC_CH][MAX_REGIONS])
{
    // 遍历每个接收天线
    for (int rx = 0; rx < n_RX; rx++) {
        // 遍历每个 chirp
        for (int chirp = 0; chirp < n_chirps; chirp++) {
            int maxIndex = 0; // 初始化最大幅值的索引
            double maxMagnitude = (-DBL_MAX); // 初始化最大幅值
						 // 用于存储当前 chirp 的所有 Magnitude
            double magnitudes[chirpRbCn];
					  int start_index = -1;
            int end_index = -1;
						int in_region = 0;
	          int region_count = 0;

            // 用于存储前三个大于 -39 的区域
            int regions[MAX_REGIONS][2] = {{-1, -1}};	
						
            // 遍历每个 Range Bin
            for (int rb = 0; rb < chirpRbCn; rb++) {
                // 计算当前 Range Bin 的索引
                int index = (rx * n_chirps * chirpRbCn + chirp + n_chirps*rb);

                // 提取 I 和 Q 部分
                int16_t I = (int16_t)(fileData[index] & 0xFFFF); // 提取低 16 位作为 I
                int16_t Q = (int16_t)(fileData[index] >> 16); // 提取高 16 位作为 Q
							
								// 打印当前 Range Bin 的实部和虚部
//                printf("Chirp %d, RX %d, RB %d: I = %d, Q = %d\n", chirp, rx, rb, I, Q);							
	//							
	//								int32_t I_TEMP = I*64;
	//								int32_t Q_TEMP = Q*64;
	//							
                // 创建复数
                Complex c = {I, Q};

								
//                // 计算幅值
//                double magnitude = complexMagnitude(c)*64;

//								if(rb > 0)
//								{
//									magnitude *= 2;
//								}
//								magnitude /= FFT_RSIZE;
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
//									if (magnitude > -39) {
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
//													break;
//													in_region = 0;
//											}
//									}
//								}

								 // 打印前三个大于 -39 的区域

								double magnitude = sqrt((double)c.real * c.real + (double)c.imag * c.imag);//complexMagnitude(c);
								// 保存当前 Range Bin 的幅值
                magnitudes[rb] = magnitude;               
							 // 更新最大幅值及其索引
                if (magnitude > TRESHOLD_VALUE && magnitude > maxMagnitude && rb > 6) {
	                  maxMagnitude = magnitude;
	                  maxIndex = rb;
									//	break;
                }
            }

            // 保存最大幅值的索引和幅值
						for(int i = 0; i < MAX_REGIONS;i++)
						{
//							maxMagnitudes[chirp + rx * n_chirps][i] = -100;
//							if(regions[i][1] > 0)
							{
//								maxIndices[chirp + rx * n_chirps][i] = (regions[i][1] + regions[i][0])/2 ;
//								maxMagnitudes[chirp + rx * n_chirps][i] = magnitudes[(regions[i][1] + regions[i][0])/2];
								maxIndices[chirp + rx * n_chirps][i] = maxIndex;
								maxMagnitudes[chirp + rx * n_chirps][i]  = maxMagnitude;
							}
						}
						#if DEBUG_RANGE_BIN
						// 打印当前 chirp 的所有 Magnitude
            printf("Chirp %d, RX %d: Magnitudes = [", chirp, rx);
            for (int rb = 0; rb < chirpRbCn; rb++) {
                printf("%.2f", magnitudes[rb]);
                if (rb < chirpRbCn - 1) {
                    printf(", ");
                }
            }
	          printf("]\n");
						#endif
            // 打印调试信息
           // printf("Chirp %d, RX %d: Max Magnitude Index = %d, Max Magnitude = %f\n", chirp, rx, maxIndex, maxMagnitude);
        }
    }
}

/*
void findMaxMagnitudeIndex(uint32_t* fileData, int n_chirps, int chirpRbCn, int n_RX, int* maxIndices) {
    for (int rx = 0; rx < n_RX; rx++) {
        for (int chirp = 0; chirp < n_chirps; chirp++) {
            int maxIndex = 0;
            double maxMagnitude = 0.0;

            for (int rb = 0; rb < chirpRbCn; rb++) {
                int index = (rx * n_chirps * chirpRbCn + chirp * chirpRbCn + rb);

                int16_t I = (int16_t)(fileData[index] & 0xFFFF);
                int16_t Q = (int16_t)(fileData[index] >> 16);

                Complex c = {I, Q};
                double magnitude = complexMagnitude(c);

                if (magnitude > maxMagnitude) {
                    maxMagnitude = magnitude;
                    maxIndex = rb;
                }

            //   printf("rx = %d, chirp = %d, rb = %d, index = %d, I = %d, Q = %d, magnitude = %f\n", rx, chirp, rb, index, I, Q, magnitude);
            }

            maxIndices[chirp + rx * n_chirps] = maxIndex;
           // printf("Chirp %d, RX %d: Max Magnitude Index = %d, Max Magnitude = %f\n", chirp, rx, maxIndex, maxMagnitude);
        }
    }
}

*/
/*
// 找出每个 chirp 中幅值最大的 Range Bin 的索引
void findMaxMagnitudeIndex(uint32_t* fileData, int n_chirps, int chirpRbCn, int n_RX, int* maxIndices) {
    // 遍历每个接收天线
    for (int rx = 0; rx < n_RX; rx++) {
        // 遍历每个 chirp
        for (int chirp = 0; chirp < n_chirps; chirp++) {
            int maxIndex = 0; // 初始化最大幅值的索引
            double maxMagnitude = 0.0; // 初始化最大幅值

            // 遍历每个 Range Bin
            for (int rb = 0; rb < chirpRbCn; rb++) {
                // 计算当前 Range Bin 的索引
                int index = (rx * n_chirps * chirpRbCn + chirp * chirpRbCn + rb);

                // 提取 I 和 Q 部分
                int16_t I = (int16_t)(fileData[index] & 0xFFFF); // 提取低 16 位作为 I
                int16_t Q = (int16_t)(fileData[index] >> 16); // 提取高 16 位作为 Q

                // 创建复数
                Complex c = {I, Q};

                // 计算幅值
                double magnitude = complexMagnitude(c);

                // 更新最大幅值及其索引
                if (magnitude > maxMagnitude) {
                    maxMagnitude = magnitude;
                    maxIndex = rb;
                }
            }

            // 保存最大幅值的索引
            maxIndices[chirp + rx * n_chirps] = maxIndex;
        }
    }
}*/