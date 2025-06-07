#include "clock.h"
#include "hal_uart.h"
#include "reg_uart.h"
#include "memory_map.h"
#include "common.h"
#include "spi_flash_host.h"

#define UART_PORT                       UART_ID_0
#define UART_BAUDRATE                   UART_BAUD_RATE_921600
#define RXBUF_2PWD                      10
#define RXBUF_SIZE                      (1UL << RXBUF_2PWD)
#define RXBUF_MASK                      (RXBUF_SIZE - 1)
#define  atPrintf(...)                  printf(__VA_ARGS__)

/*********************************************************************************************************
** ���º궨�������޸�
*********************************************************************************************************/
#define FLASH_TYPE_ID                   0
#define TOTAL_FLASH_ID                  3
#define SPIFLS_CMD_READ_UID             0x4B //Read Identification

/*********************************************************************************************************
* �汾��Ϣ�ṹ����
*********************************************************************************************************/
typedef struct __lib_info {
    const char *pcDisc;//�����ַ���
    const char *pcDate;//��������
    const char *pcTime;//����ʱ��
} VER_INFO;

/*********************************************************************************************************
* ȫ�ֱ�����������
*********************************************************************************************************/
unsigned char GucRxBuf[RXBUF_SIZE] = {0};
unsigned long GulWrIdx = 0;
unsigned long GulReIdx = 0;
HWP_UART_T *const uart_hwd[UART_ID_QTY] = {hwp_uart_0, hwp_uart_1, hwp_uart_2};

/*********************************************************************************************************
* �汾��������Ϣ
*********************************************************************************************************/
__attribute__((used)) const VER_INFO GtVerInfo = {
    "Radar Service Routine For AT6010, Build at:",
    __DATE__,
    __TIME__,
};

/*********************************************************************************************************
* ����Ϊ΢��/C����ӳ����ش���
*********************************************************************************************************/
#include <stdio.h>
#if (defined (__CC_ARM)) || (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
#ifndef __MICROLIB //��ʹ��Micro Lib
#pragma import(__use_no_semihosting) 
/* ���� _sys_exit() �Ա���ʹ�ð�����ģʽ */ 
void _sys_exit(int x)
{
    x = x; 
}
 
/* ��׼����Ҫ��֧������ */
struct __FILE { 
    int handle; 
};

FILE __stdout;
#endif

//ͨ�����Դ��ڷ���һ���ַ�
ATTR_RAM_SECTION int fputc(int ch, FILE *f)
{
    uart_send_byte(UART_PORT, (u8)ch);
    return (ch);
}
#endif

/*********************************************************************************************************
* ��������getUuid64
* ���ܣ�  ��ȡ64λUUID
* ���룺  ��
* ���أ�  unsigned long long ��ΨһID
*********************************************************************************************************/
ATTR_RAM_SECTION unsigned long long getUuid64(void)
{
    unsigned char op_mode = 0;
    unsigned long long ullUid = 0;

    if (FLASH_TYPE_ID == 0) {
        op_mode = 0;
    } else if (FLASH_TYPE_ID >= TOTAL_FLASH_ID) {
        op_mode = 3;
    } else {
        op_mode = 3;
    }
    critical_section_start();
    spifls_host_clear_fifo_data(TRUE, TRUE);
    spifls_host_send_cmd(SPIFLS_CMD_READ_UID, 0, op_mode, 8);
    spifls_host_get_rx_data((u8 *)&ullUid, 8); //��ȡ8���ֽ�UID
    critical_section_end();

    return ullUid;
}

/*********************************************************************************************************
* ��������uart_rx_hdl
* ���ܣ�  �����жϻص�����
* ���룺  ��
* ���أ�  ��
*********************************************************************************************************/
ATTR_RAM_SECTION void uart_rx_hdl(void)
{
    HWP_UART_T *uart_base = uart_hwd[UART_PORT];    
    while (uart_base->uart_usr & UART_USR_RFNE_MSK) {
        GucRxBuf[GulWrIdx++] = uart_base->uart_rbr_dll_thr;
        GulWrIdx &= RXBUF_MASK;
    }	
}

/*********************************************************************************************************
* ��������main
* ���ܣ�  ������
* ���룺  ��
* ���أ�  ��
*********************************************************************************************************/
s32 main(void)
{
    uart_cfg_t cfg = {
        .baud_rate = UART_BAUDRATE,
        .data_width = UART_DATA_WIDTH_8BIT,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        
        .dma_mode = 0,
        .irq_enable = 1,
        .padding = {0},

        .fifo_cfg.rx_fifo_level = UART_RX_FIFO_QUARTER_FULL,//FIFO�������
        .fifo_cfg.tx_fifo_level = UART_TX_FIFO_EMPTY,
        .fifo_cfg.fifo_en = 1U,/* default enable */
    };

	clock_init();
    clk_mcu_set(CLK_SRC_PLL, 80000000U); /* ϵͳ��Ƶ��ʼ�� */
    
	hal_uart_init(UART_PORT, &cfg);    
	uart_set_hdl(UART_PORT, UART_RX_INTR, uart_rx_hdl);
    uart_set_hdl(UART_PORT, UART_RX_IDLE_INTR, uart_rx_hdl);
	uart_irq_enable(UART_PORT, UART_RX_INTR);
    
    atPrintf("\r\n%s %s %s\r\nUUID:%016llX\r\n", GtVerInfo.pcDisc, GtVerInfo.pcDate, GtVerInfo.pcTime, getUuid64());
    while(1) {
        if (GulReIdx != GulWrIdx) {
            uart_send_byte(UART_PORT, GucRxBuf[GulReIdx++]);
            GulReIdx &= RXBUF_MASK;
        }
    }	
}
