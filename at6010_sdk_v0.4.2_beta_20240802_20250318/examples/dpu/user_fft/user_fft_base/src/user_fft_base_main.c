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
#define SAMPLES                  256// Chirp�ڲ�����������ͨ��Ҳ��һάFFT����
#define FFTS_1D                  8// FFT��������ӦFFT����Ϊ2^FFTS_1D
#define FFTN_1D                  (1UL << FFTS_1D)// FFT����
#define FFTN_RB                  (SAMPLES)// һ��FFT���Range Bin����
#define FFT_TYPE                 2//1=RFFT,FFT����Ϊ��ʵ����2=CFFT��FFT����Ϊ�������鲿��ǰ��ʵ���ں�

#if (SAMPLES != FFTN_1D)
#warmming "FFT�����������������һ�£���ע���飡��ȷ��������ɾ���˴��󾯸棡"
#endif

/*********************************************************************************************************
* ���º궨����FFT�����ź����
*********************************************************************************************************/
#define FT 0.25f// �����ź�Ƶ��
#define FS 4UL// �����ź�Ƶ��
#define PI 3.1415927f// Pi

/*********************************************************************************************************
* ��������
*********************************************************************************************************/
void usr_fft_isr(void);
void radar_clock_init(void);

/*********************************************************************************************************
* ȫ�ֱ�����������
*********************************************************************************************************/
volatile int GulFlg = 0;// ��ɱ�ʶ
volatile radar_frame_hdr_t GtFrameHeader = {0};
short GsRangeIn[CHIRPS][SAMPLES][FFT_TYPE] FFT_MEM = {0};// ����������һ֡FFT����Ĵ洢�ռ�
short GsRangeOut[CHIRPS][FFTN_RB][2] FFT_MEM = {0};

// �����ж���,�����г��ȱ�����FFT������ͬ
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
    #error "�����г��ȴ�����ע���飡"
    #endif
};

// FFT����������
const fft_win_para_t  GtWinCfg = {
    .enable = 0,// 1=ʹ��Ӳ��FFT�Զ��Ӵ����㣬0=��ֹӲ��FFT�Զ��Ӵ�����
    .fft_mode = FFT_USER_FFT_MODE,
    .waddr = (u32)GusRangeWin,// �������׵�ַ
};

// USER FFT ��������
user_fft_para_t GtRangeFftCfg = {
    .data_type = (FFT_TYPE == 1)? REAL_DATA : COMPLEX_DATA,// FFT����,COMPLEX_DATA=����FFT, REAL_DATA=ʵ��FFT
    .is_power_out  = 0,// 0=����������鲿��ǰ��,1=���ģƽ��
    .shift_value   = FFTS_1D,// ��λ����
    .fft_point     = FFTN_1D,
    .nframe        = CHIRPS,
    .in_frame_size = SAMPLES,
    .out_frame_size = FFTN_RB,
    .in_buf         = (u32)GsRangeIn,// ���뻺��
    .out_buf        = (u32)GsRangeOut,// �������
    .end_callback   = usr_fft_isr,// ת����ɻص�����
};

// ���ڲ�������,Ĭ��Log���������Ϊ921600, ������������Log���ڸ��ã������ʽ���output_dma_config()��������Ϊ5Mbps
uart_cfg_t GtUartCfg = {
    .baud_rate = UART_BAUD_RATE_921600,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .data_width = UART_DATA_WIDTH_8BIT,
    .fifo_cfg.fifo_en = 1U,
};

/*********************************************************************************************************
* ��������radar_clock_init
* ���ܣ�  �״��������ʱ�ӳ�ʼ��
* ���룺  ��
* ���أ�  ��
*********************************************************************************************************/
void radar_clock_init(void)
{
    sys_clk_disable_radar();
    sys_clk_enable_radar();
    sys_clk_enable_dma();
}

/*********************************************************************************************************
* ��������usr_fft_isr
* ���ܣ�  USER FFT�任��ɻص�����
* ���룺  ��
* �����  �޸�GulFlg��־��֡ID
* ���أ�  ��
*********************************************************************************************************/
void usr_fft_isr(void)
{
    GulFlg = 1;
    GtFrameHeader.radar_common_cfg.rxSubFrameId++;
}

/*********************************************************************************************************
* ��������dataTestInit
* ���ܣ�  FFT_TYPE=1���������Ҳ����źţ�FFT_TYPE=2,���������Ĳ����źţ���������ʵ�����鲿��
* ���룺  ��
* �����  ����CHIRPS����SAMPLES����������ɵĲ����ź�
*         �洢��GsRangeIn�����У��Ա�����FFTӲ���������
* ���أ�  ��
*********************************************************************************************************/
void dataTestInit (void)
{
    short *psBuf = (short *)GsRangeIn;
    int i =  0, j = 0;
    
    memset(psBuf, 0, sizeof(GsRangeIn));
    
    //���ɲ����ź�
    for (j = 0; j < SAMPLES; j++) {// ���������鲿��ǰ[15:0]Q��[31:16]I
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
* ��������main
* ���ܣ�  ������������Ӳ��FFT����ʵ��FFT�任��ͨ�������³�FFT�任ǰ�������
* ���룺  ��
* ���أ�  ��
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
    dataTestInit();//���ɲ�������
    hal_fft_set_config(FFT_IDX_CONFIG_USER_WIN, (void *)&GtWinCfg);
    hal_fft_set_config(FFT_IDX_CONFIG_USER_FFT,(void*)&GtRangeFftCfg);
    while (1) {
        fft_process_trigger(FFT_USER_FFT_MODE);
        while (GulFlg == 0) {
            ;
        }
        GulFlg = 0;
        output_port_send_data((u32)&GtFrameHeader, sizeof(GtFrameHeader));//���֡ͷ
        output_port_send_data((u32)GsRangeIn, sizeof(GsRangeIn));//���ԭʼ����
        output_port_send_data((u32)GsRangeOut, sizeof(GsRangeOut));//����任�������
    }
    
    return 0;
}
