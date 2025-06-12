#ifndef _MEMORY_MAP_H_
#define _MEMORY_MAP_H_


//***********************************
//*** For sram
//***********************************

#define SRAM_BASE_0             0x20000000
#define SRAM_BASE_1             0x20008000
#define SRAM_BASE_2             0x20010000
#define SRAM_BASE_3             0x20020000


//***********************************
//*** For uart_0/1/2
//***********************************

#define REG_UART_BASE_0         0x40006000
#define REG_UART_BASE_1         0x40007000
#define REG_UART_BASE_2         0x40008000

#define hwp_uart_0              ((HWP_UART_T*)REG_UART_BASE_0)
#define hwp_uart_1              ((HWP_UART_T*)REG_UART_BASE_1)
#define hwp_uart_2              ((HWP_UART_T*)REG_UART_BASE_2)

//***********************************
//*** For DMA
//***********************************
#define REG_DMA_BASE            0x40100000


//***********************************
//*** For spi_0/1
//***********************************

#define REG_SSI_BASE_0          0x40101000
#define REG_SSI_BASE_1          0x40102000

#define hwp_ssi_0               ((HWP_SSI_T*)REG_SSI_BASE_0)
#define hwp_ssi_1               ((HWP_SSI_T*)REG_SSI_BASE_1)


//***********************************
//*** For radar
//***********************************

// radar dsp register, 4KB
// #define REG_RADAR_DSP_BASE    0x40010000

// radar dsp coeff ram, 4KB
#define REG_RADAR_DSP_COEF_BASE   0x40011000

// radar fmcw register, 4KB
// #define REG_FRAME_BASE        0x40012000
// #define REG_CHIRP_BASE        0x40012800
// #define REG_TX_BASE           0x40012900

// radar filter register, 4KB
// #define REG_RADAR_FLT_BASE    0x40013000


#endif
