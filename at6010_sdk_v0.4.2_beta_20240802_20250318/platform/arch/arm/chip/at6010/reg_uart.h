#ifndef _REG_UART_H_
#define _REG_UART_H_

#include "globals_macro.h"

#define REG_UART_BASE            0x40006000U

typedef struct
{
    REG32        uart_rbr_dll_thr;    // 0x0
    REG32        uart_dlh_ier;    // 0x4
    REG32        uart_fcr_iir;    // 0x8
    REG32        uart_lcr;    // 0xc
    REG32        uart_mcr;    // 0x10
    REG32        uart_lsr;    // 0x14
    REG32        uart_msr;    // 0x18
    REG32        uart_scr;    // 0x1c
    REG32        uart_lpdll;    // 0x20
    REG32        uart_lpdlh;    // 0x24
    REG32        RESERVE0[2];
    REG32        uart_srbr0;    // 0x30
    REG32        uart_srbr1;    // 0x34
    REG32        uart_srbr2;    // 0x38
    REG32        uart_srbr3;    // 0x3c
    REG32        uart_srbr4;    // 0x40
    REG32        uart_srbr5;    // 0x44
    REG32        uart_srbr6;    // 0x48
    REG32        uart_srbr7;    // 0x4c
    REG32        uart_srbr8;    // 0x50
    REG32        uart_srbr9;    // 0x54
    REG32        uart_srbr10;    // 0x58
    REG32        uart_srbr11;    // 0x5c
    REG32        uart_srbr12;    // 0x60
    REG32        uart_srbr13;    // 0x64
    REG32        uart_srbr14;    // 0x68
    REG32        uart_srbr15;    // 0x6c
    REG32        uart_far;    // 0x70
    REG32        uart_tfr;    // 0x74
    REG32        uart_rfw;    // 0x78
    REG32        uart_usr;    // 0x7c
    REG32        uart_tfl;    // 0x80
    REG32        uart_rfl;    // 0x84
    REG32        uart_srr;    // 0x88
    REG32        uart_srts;    // 0x8c
    REG32        uart_sbcr;    // 0x90
    REG32        uart_sdmam;    // 0x94
    REG32        uart_sfe;    // 0x98
    REG32        uart_srt;    // 0x9c
    REG32        uart_stet;    // 0xa0
    REG32        uart_htx;    // 0xa4
    REG32        uart_dmasa;    // 0xa8
    REG32        uart_tcr;    // 0xac
    REG32        uart_de_en;    // 0xb0
    REG32        uart_re_en;    // 0xb4
    REG32        uart_det;    // 0xb8
    REG32        uart_tat;    // 0xbc
    REG32        uart_dlf;    // 0xc0
    REG32        uart_rar;    // 0xc4
    REG32        uart_tar;    // 0xc8
    REG32        uart_lcr_ext;    // 0xcc
    REG32        RESERVE1[9];
    REG32        uart_cpr;    // 0xf4
    REG32        uart_ucv;    // 0xf8
    REG32        uart_ctr;    // 0xfc
} HWP_UART_T;

#define hwp_uart        ((HWP_UART_T*)REG_UART_BASE)

// 0x0, uart_rbr_dll_thr
#define UART_RBR_DLL_THR_OFFSET        0x0U
#define UART_RBR_DLL_THR_THR_POS        0U
#define UART_RBR_DLL_THR_THR_RST        0x0U
#define UART_RBR_DLL_THR_THR_MSK        ( 0x1ffU << 0U )
#define UART_RBR_DLL_THR_THR(x)          ( ((x) & 0x1ffU) << 0U )

// 0x4, uart_dlh_ier
#define UART_DLH_IER_OFFSET        0x4U
#define UART_DLH_IER_ERBFI_POS        0U
#define UART_DLH_IER_ERBFI_RST        0x0U
#define UART_DLH_IER_ERBFI_MSK        ( 0x1U << 0U )
#define UART_DLH_IER_ERBFI(x)          ( ((x) & 0x1U) << 0U )
#define UART_DLH_IER_ETBEI_POS        1
#define UART_DLH_IER_ETBEI_RST        0x0U
#define UART_DLH_IER_ETBEI_MSK        ( 0x1U << 1U )
#define UART_DLH_IER_ETBEI(x)          ( ((x) & 0x1U) << 1U )
#define UART_DLH_IER_ELSI_POS        2U
#define UART_DLH_IER_ELSI_RST        0x0U
#define UART_DLH_IER_ELSI_MSK        ( 0x1U << 2U )
#define UART_DLH_IER_ELSI(x)          ( ((x) & 0x1) << 2U )
#define UART_DLH_IER_EDSSI_POS        3U
#define UART_DLH_IER_EDSSI_RST        0x0U
#define UART_DLH_IER_EDSSI_MSK        ( 0x1U << 3U )
#define UART_DLH_IER_EDSSI(x)          ( ((x) & 0x1U) << 3U )
#define UART_DLH_IER_ELCOLR_POS        4U
#define UART_DLH_IER_ELCOLR_RST        0x0U
#define UART_DLH_IER_ELCOLR_MSK        ( 0x1U << 4U )
#define UART_DLH_IER_ELCOLR(x)          ( ((x) & 0x1U) << 4U )
#define UART_DLH_IER_PTIME_POS        7U
#define UART_DLH_IER_PTIME_RST        0x0U
#define UART_DLH_IER_PTIME_MSK        ( 0x1U << 7U )
#define UART_DLH_IER_PTIME(x)          ( ((x) & 0x1U) << 7U )

// 0x8, uart_fcr_iir
#define UART_FCR_IIR_OFFSET        0x8U
#define UART_FCR_IIR_FIFOE_POS        0U
#define UART_FCR_IIR_FIFOE_RST        0x0U
#define UART_FCR_IIR_FIFOE_MSK        ( 0x1U << 0U )
#define UART_FCR_IIR_FIFOE(x)          ( ((x) & 0x1U) << 0U )
#define UART_FCR_IIR_RFIFOR_POS        1U
#define UART_FCR_IIR_RFIFOR_RST        0x0U
#define UART_FCR_IIR_RFIFOR_MSK        ( 0x1U << 1U )
#define UART_FCR_IIR_RFIFOR(x)          ( ((x) & 0x1U) << 1U )
#define UART_FCR_IIR_XFIFOR_POS        2U
#define UART_FCR_IIR_XFIFOR_RST        0x0U
#define UART_FCR_IIR_XFIFOR_MSK        ( 0x1U << 2U )
#define UART_FCR_IIR_XFIFOR(x)          ( ((x) & 0x1U) << 2U )
#define UART_FCR_IIR_DMAM_POS        3U
#define UART_FCR_IIR_DMAM_RST        0x0U
#define UART_FCR_IIR_DMAM_MSK        ( 0x1U << 3U )
#define UART_FCR_IIR_DMAM(x)          ( ((x) & 0x1U) << 3U )
#define UART_FCR_IIR_TET_POS        4U
#define UART_FCR_IIR_TET_RST        0x0U
#define UART_FCR_IIR_TET_MSK        ( 0x3U << 4U )
#define UART_FCR_IIR_TET(x)          ( ((x) & 0x3U) << 4U )
#define UART_FCR_IIR_RT_POS        6U
#define UART_FCR_IIR_RT_RST        0x0U
#define UART_FCR_IIR_RT_MSK        ( 0x3U << 6U )
#define UART_FCR_IIR_RT(x)          ( ((x) & 0x3U) << 6U )

// 0xc, uart_lcr
#define UART_LCR_OFFSET        0xcU
#define UART_LCR_DLS_POS        0U
#define UART_LCR_DLS_RST        0x0U
#define UART_LCR_DLS_MSK        ( 0x3U << 0U )
#define UART_LCR_DLS(x)          ( ((x) & 0x3U) << 0U )
#define UART_LCR_STOP_POS        2U
#define UART_LCR_STOP_RST        0x0U
#define UART_LCR_STOP_MSK        ( 0x1U << 2U )
#define UART_LCR_STOP(x)          ( ((x) & 0x1U) << 2U )
#define UART_LCR_PEN_POS        3U
#define UART_LCR_PEN_RST        0x0U
#define UART_LCR_PEN_MSK        ( 0x1U << 3U )
#define UART_LCR_PEN(x)          ( ((x) & 0x1U) << 3U )
#define UART_LCR_EPS_POS        4U
#define UART_LCR_EPS_RST        0x0U
#define UART_LCR_EPS_MSK        ( 0x1U << 4U )
#define UART_LCR_EPS(x)          ( ((x) & 0x1U) << 4U )
#define UART_LCR_SP_POS        5U
#define UART_LCR_SP_RST        0x0U
#define UART_LCR_SP_MSK        ( 0x1U << 5U )
#define UART_LCR_SP(x)          ( ((x) & 0x1U) << 5U )
#define UART_LCR_BC_POS        6U
#define UART_LCR_BC_RST        0x0U
#define UART_LCR_BC_MSK        ( 0x1U << 6U )
#define UART_LCR_BC(x)          ( ((x) & 0x1U) << 6U )
#define UART_LCR_DLAB_POS        7U
#define UART_LCR_DLAB_RST        0x0U
#define UART_LCR_DLAB_MSK        ( 0x1U << 7U )
#define UART_LCR_DLAB(x)          ( ((x) & 0x1U) << 7U )

// 0x10, uart_mcr
#define UART_MCR_OFFSET        0x10U
#define UART_MCR_DTR_POS        0U
#define UART_MCR_DTR_RST        0x0U
#define UART_MCR_DTR_MSK        ( 0x1U << 0U )
#define UART_MCR_DTR(x)          ( ((x) & 0x1U) << 0U )
#define UART_MCR_RTS_POS        1U
#define UART_MCR_RTS_RST        0x0U
#define UART_MCR_RTS_MSK        ( 0x1U << 1U )
#define UART_MCR_RTS(x)          ( ((x) & 0x1U) << 1U )
#define UART_MCR_OUT1_POS        2U
#define UART_MCR_OUT1_RST        0x0U
#define UART_MCR_OUT1_MSK        ( 0x1U << 2U )
#define UART_MCR_OUT1(x)          ( ((x) & 0x1U) << 2U )
#define UART_MCR_OUT2_POS        3U
#define UART_MCR_OUT2_RST        0x0U
#define UART_MCR_OUT2_MSK        ( 0x1U << 3U )
#define UART_MCR_OUT2(x)          ( ((x) & 0x1U) << 3U )
#define UART_MCR_LOOPBACK_POS        4U
#define UART_MCR_LOOPBACK_RST        0x0U
#define UART_MCR_LOOPBACK_MSK        ( 0x1U << 4U )
#define UART_MCR_LOOPBACK(x)          ( ((x) & 0x1U) << 4U )
#define UART_MCR_AFCE_POS        5U
#define UART_MCR_AFCE_RST        0x0U
#define UART_MCR_AFCE_MSK        ( 0x1U << 5U )
#define UART_MCR_AFCE(x)          ( ((x) & 0x1U) << 5U )
#define UART_MCR_SIRE_POS        6U
#define UART_MCR_SIRE_RST        0x0U
#define UART_MCR_SIRE_MSK        ( 0x1U << 6U )
#define UART_MCR_SIRE(x)          ( ((x) & 0x1U) << 6U )

// 0x14, uart_lsr
#define UART_LSR_OFFSET        0x14U
#define UART_LSR_DR_POS        0U
#define UART_LSR_DR_RST        0x0U
#define UART_LSR_DR_MSK        ( 0x1U << 0U )
#define UART_LSR_DR(x)          ( ((x) & 0x1U) << 0U )
#define UART_LSR_OE_POS        1U
#define UART_LSR_OE_RST        0x0U
#define UART_LSR_OE_MSK        ( 0x1U << 1U )
#define UART_LSR_OE(x)          ( ((x) & 0x1U) << 1U )
#define UART_LSR_PE_POS        2U
#define UART_LSR_PE_RST        0x0U
#define UART_LSR_PE_MSK        ( 0x1U << 2U )
#define UART_LSR_PE(x)          ( ((x) & 0x1U) << 2U )
#define UART_LSR_FE_POS        3U
#define UART_LSR_FE_RST        0x0U
#define UART_LSR_FE_MSK        ( 0x1U << 3U )
#define UART_LSR_FE(x)          ( ((x) & 0x1U) << 3U )
#define UART_LSR_BI_POS        4U
#define UART_LSR_BI_RST        0x0U
#define UART_LSR_BI_MSK        ( 0x1U << 4U )
#define UART_LSR_BI(x)          ( ((x) & 0x1U) << 4U )
#define UART_LSR_THRE_POS        5U
#define UART_LSR_THRE_RST        0x1U
#define UART_LSR_THRE_MSK        ( 0x1U << 5U )
#define UART_LSR_THRE(x)          ( ((x) & 0x1U) << 5U )
#define UART_LSR_TEMT_POS        6U
#define UART_LSR_TEMT_RST        0x1U
#define UART_LSR_TEMT_MSK        ( 0x1U << 6U )
#define UART_LSR_TEMT(x)          ( ((x) & 0x1U) << 6U )
#define UART_LSR_RFE_POS        7U
#define UART_LSR_RFE_RST        0x0U
#define UART_LSR_RFE_MSK        ( 0x1U << 7U )
#define UART_LSR_RFE(x)          ( ((x) & 0x1U) << 7U )
#define UART_LSR_ADDR_RCVD_POS        8U
#define UART_LSR_ADDR_RCVD_RST        0x0U
#define UART_LSR_ADDR_RCVD_MSK        ( 0x1U << 8U )
#define UART_LSR_ADDR_RCVD(x)          ( ((x) & 0x1U) << 8U )

// 0x18, uart_msr
#define UART_MSR_OFFSET        0x18U
#define UART_MSR_DCTS_POS        0U
#define UART_MSR_DCTS_RST        0x0U
#define UART_MSR_DCTS_MSK        ( 0x1U << 0U )
#define UART_MSR_DCTS(x)          ( ((x) & 0x1U) << 0U )
#define UART_MSR_DDSR_POS        1U
#define UART_MSR_DDSR_RST        0x0U
#define UART_MSR_DDSR_MSK        ( 0x1U << 1U )
#define UART_MSR_DDSR(x)          ( ((x) & 0x1U) << 1U )
#define UART_MSR_TERI_POS        2U
#define UART_MSR_TERI_RST        0x0U
#define UART_MSR_TERI_MSK        ( 0x1U << 2U )
#define UART_MSR_TERI(x)          ( ((x) & 0x1U) << 2U )
#define UART_MSR_DDCD_POS        3U
#define UART_MSR_DDCD_RST        0x0U
#define UART_MSR_DDCD_MSK        ( 0x1U << 3U )
#define UART_MSR_DDCD(x)          ( ((x) & 0x1U) << 3U )
#define UART_MSR_CTS_POS        4U
#define UART_MSR_CTS_RST        0x0U
#define UART_MSR_CTS_MSK        ( 0x1U << 4U )
#define UART_MSR_CTS(x)          ( ((x) & 0x1U) << 4U )
#define UART_MSR_DSR_POS        5U
#define UART_MSR_DSR_RST        0x0U
#define UART_MSR_DSR_MSK        ( 0x1U << 5U )
#define UART_MSR_DSR(x)          ( ((x) & 0x1U) << 5U )
#define UART_MSR_RI_POS        6U
#define UART_MSR_RI_RST        0x0U
#define UART_MSR_RI_MSK        ( 0x1U << 6U )
#define UART_MSR_RI(x)          ( ((x) & 0x1U) << 6U )
#define UART_MSR_DCD_POS        7U
#define UART_MSR_DCD_RST        0x0U
#define UART_MSR_DCD_MSK        ( 0x1U << 7U )
#define UART_MSR_DCD(x)          ( ((x) & 0x1U) << 7U )

// 0x1c, uart_scr
#define UART_SCR_OFFSET        0x1cU
#define UART_SCR_SCR_POS        0U
#define UART_SCR_SCR_RST        0x0U
#define UART_SCR_SCR_MSK        ( 0xffU << 0U )
#define UART_SCR_SCR(x)          ( ((x) & 0xffU) << 0U )

// 0x20, uart_lpdll
#define UART_LPDLL_OFFSET        0x20U
#define UART_LPDLL_LPDLL_POS        0U
#define UART_LPDLL_LPDLL_RST        0x0U
#define UART_LPDLL_LPDLL_MSK        ( 0xffU << 0U )
#define UART_LPDLL_LPDLL(x)          ( ((x) & 0xffU) << 0U )

// 0x24, uart_lpdlh
#define UART_LPDLH_OFFSET        0x24U
#define UART_LPDLH_LPDLH_POS        0U
#define UART_LPDLH_LPDLH_RST        0x0U
#define UART_LPDLH_LPDLH_MSK        ( 0xffU << 0U )
#define UART_LPDLH_LPDLH(x)          ( ((x) & 0xffU) << 0U )

// 0x30, uart_srbr0
#define UART_SRBR0_OFFSET        0x30U
#define UART_SRBR0_SRBRN_POS        0U
#define UART_SRBR0_SRBRN_RST        0x0U
#define UART_SRBR0_SRBRN_MSK        ( 0x1ffU << 0U )
#define UART_SRBR0_SRBRN(x)          ( ((x) & 0x1ffU) << 0U )

// 0x34, uart_srbr1
#define UART_SRBR1_OFFSET        0x34U
#define UART_SRBR1_SRBR1_POS        0U
#define UART_SRBR1_SRBR1_RST        0x0U
#define UART_SRBR1_SRBR1_MSK        ( 0x1ffU << 0U )
#define UART_SRBR1_SRBR1(x)          ( ((x) & 0x1ffU) << 0U )

// 0x38, uart_srbr2
#define UART_SRBR2_OFFSET        0x38U
#define UART_SRBR2_SRBR2_POS        0U
#define UART_SRBR2_SRBR2_RST        0x0U
#define UART_SRBR2_SRBR2_MSK        ( 0x1ffU << 0U )
#define UART_SRBR2_SRBR2(x)          ( ((x) & 0x1ffU) << 0U )

// 0x3c, uart_srbr3
#define UART_SRBR3_OFFSET        0x3cU
#define UART_SRBR3_SRBR3_POS        0U
#define UART_SRBR3_SRBR3_RST        0x0U
#define UART_SRBR3_SRBR3_MSK        ( 0x1ffU << 0U )
#define UART_SRBR3_SRBR3(x)          ( ((x) & 0x1ffU) << 0U )

// 0x40, uart_srbr4
#define UART_SRBR4_OFFSET        0x40U
#define UART_SRBR4_SRBR4_POS        0U
#define UART_SRBR4_SRBR4_RST        0x0U
#define UART_SRBR4_SRBR4_MSK        ( 0x1ffU << 0U )
#define UART_SRBR4_SRBR4(x)          ( ((x) & 0x1ffU) << 0U )

// 0x44, uart_srbr5
#define UART_SRBR5_OFFSET        0x44U
#define UART_SRBR5_SRBR5_POS        0U
#define UART_SRBR5_SRBR5_RST        0x0U
#define UART_SRBR5_SRBR5_MSK        ( 0x1ff << 0U )
#define UART_SRBR5_SRBR5(x)          ( ((x) & 0x1ffU) << 0U )

// 0x48, uart_srbr6
#define UART_SRBR6_OFFSET        0x48U
#define UART_SRBR6_SRBR6_POS        0U
#define UART_SRBR6_SRBR6_RST        0x0U
#define UART_SRBR6_SRBR6_MSK        ( 0x1ffU << 0U )
#define UART_SRBR6_SRBR6(x)          ( ((x) & 0x1ffU) << 0U )

// 0x4c, uart_srbr7
#define UART_SRBR7_OFFSET        0x4cU
#define UART_SRBR7_SRBR7_POS        0U
#define UART_SRBR7_SRBR7_RST        0x0U
#define UART_SRBR7_SRBR7_MSK        ( 0x1ffU << 0U )
#define UART_SRBR7_SRBR7(x)          ( ((x) & 0x1ffU) << 0U )

// 0x50, uart_srbr8
#define UART_SRBR8_OFFSET        0x50U
#define UART_SRBR8_SRBR8_POS        0U
#define UART_SRBR8_SRBR8_RST        0x0U
#define UART_SRBR8_SRBR8_MSK        ( 0x1ffU << 0U )
#define UART_SRBR8_SRBR8(x)          ( ((x) & 0x1ffU) << 0U )

// 0x54, uart_srbr9
#define UART_SRBR9_OFFSET        0x54U
#define UART_SRBR9_SRBR9_POS        0U
#define UART_SRBR9_SRBR9_RST        0x0U
#define UART_SRBR9_SRBR9_MSK        ( 0x1ffU << 0U )
#define UART_SRBR9_SRBR9(x)          ( ((x) & 0x1ffU) << 0U )

// 0x58, uart_srbr10
#define UART_SRBR10_OFFSET        0x58U
#define UART_SRBR10_SRBR10_POS        0U
#define UART_SRBR10_SRBR10_RST        0x0U
#define UART_SRBR10_SRBR10_MSK        ( 0x1ffU << 0U )
#define UART_SRBR10_SRBR10(x)          ( ((x) & 0x1ffU) << 0U )

// 0x5c, uart_srbr11
#define UART_SRBR11_OFFSET        0x5cU
#define UART_SRBR11_SRBR11_POS        0U
#define UART_SRBR11_SRBR11_RST        0x0U
#define UART_SRBR11_SRBR11_MSK        ( 0x1ffU << 0U )
#define UART_SRBR11_SRBR11(x)          ( ((x) & 0x1ffU) << 0U )

// 0x60, uart_srbr12
#define UART_SRBR12_OFFSET        0x60U
#define UART_SRBR12_SRBR12_POS        0U
#define UART_SRBR12_SRBR12_RST        0x0U
#define UART_SRBR12_SRBR12_MSK        ( 0x1ffU << 0U )
#define UART_SRBR12_SRBR12(x)          ( ((x) & 0x1ffU) << 0U )

// 0x64, uart_srbr13
#define UART_SRBR13_OFFSET        0x64U
#define UART_SRBR13_SRBR13_POS        0U
#define UART_SRBR13_SRBR13_RST        0x0U
#define UART_SRBR13_SRBR13_MSK        ( 0x1ffU << 0U )
#define UART_SRBR13_SRBR13(x)          ( ((x) & 0x1ffU) << 0U )

// 0x68, uart_srbr14
#define UART_SRBR14_OFFSET        0x68U
#define UART_SRBR14_SRBR14_POS        0U
#define UART_SRBR14_SRBR14_RST        0x0U
#define UART_SRBR14_SRBR14_MSK        ( 0x1ffU << 0U )
#define UART_SRBR14_SRBR14(x)          ( ((x) & 0x1ffU) << 0U )

// 0x6c, uart_srbr15
#define UART_SRBR15_OFFSET        0x6cU
#define UART_SRBR15_SRBR15_POS        0U
#define UART_SRBR15_SRBR15_RST        0x0U
#define UART_SRBR15_SRBR15_MSK        ( 0x1ffU << 0U )
#define UART_SRBR15_SRBR15(x)          ( ((x) & 0x1ffU) << 0U )

// 0x70, uart_far
#define UART_FAR_OFFSET        0x70U
#define UART_FAR_FAR_POS        0U
#define UART_FAR_FAR_RST        0x0U
#define UART_FAR_FAR_MSK        ( 0x1U << 0U )
#define UART_FAR_FAR(x)          ( ((x) & 0x1U) << 0U )

// 0x74, uart_tfr
#define UART_TFR_OFFSET        0x74U
#define UART_TFR_TFR_POS        0U
#define UART_TFR_TFR_RST        0x0U
#define UART_TFR_TFR_MSK        ( 0xff << 0U )
#define UART_TFR_TFR(x)          ( ((x) & 0xffU) << 0U )

// 0x78, uart_rfw
#define UART_RFW_OFFSET        0x78U
#define UART_RFW_RFWD_POS        0U
#define UART_RFW_RFWD_RST        0x0U
#define UART_RFW_RFWD_MSK        ( 0xffU << 0U )
#define UART_RFW_RFWD(x)          ( ((x) & 0xffU) << 0U )
#define UART_RFW_RFPE_POS        8U
#define UART_RFW_RFPE_RST        0x0U
#define UART_RFW_RFPE_MSK        ( 0x1U << 8U )
#define UART_RFW_RFPE(x)          ( ((x) & 0x1U) << 8U )
#define UART_RFW_RFFE_POS        9U
#define UART_RFW_RFFE_RST        0x0U
#define UART_RFW_RFFE_MSK        ( 0x1U << 9U )
#define UART_RFW_RFFE(x)          ( ((x) & 0x1U) << 9U )

// 0x7c, uart_usr
#define UART_USR_OFFSET        0x7cU
#define UART_USR_BUSY_POS        0U
#define UART_USR_BUSY_RST        0x0U
#define UART_USR_BUSY_MSK        ( 0x1U << 0U )
#define UART_USR_BUSY(x)          ( ((x) & 0x1U) << 0U )
#define UART_USR_TFNF_POS        1U
#define UART_USR_TFNF_RST        0x1U
#define UART_USR_TFNF_MSK        ( 0x1U << 1U )
#define UART_USR_TFNF(x)          ( ((x) & 0x1U) << 1U )
#define UART_USR_TFE_POS        2U
#define UART_USR_TFE_RST        0x1U
#define UART_USR_TFE_MSK        ( 0x1U << 2U )
#define UART_USR_TFE(x)          ( ((x) & 0x1U) << 2U )
#define UART_USR_RFNE_POS        3U
#define UART_USR_RFNE_RST        0x0U
#define UART_USR_RFNE_MSK        ( 0x1U << 3U )
#define UART_USR_RFNE(x)          ( ((x) & 0x1U) << 3U )
#define UART_USR_RFF_POS        4U
#define UART_USR_RFF_RST        0x0U
#define UART_USR_RFF_MSK        ( 0x1U << 4U )
#define UART_USR_RFF(x)          ( ((x) & 0x1U) << 4U )

// 0x80, uart_tfl
#define UART_TFL_OFFSET        0x80U
#define UART_TFL_TFL_POS        0U
#define UART_TFL_TFL_RST        0x0U
#define UART_TFL_TFL_MSK        ( 0x7fU << 0U )
#define UART_TFL_TFL(x)          ( ((x) & 0x7fU) << 0U )

// 0x84, uart_rfl
#define UART_RFL_OFFSET        0x84U
#define UART_RFL_RFL_POS        0U
#define UART_RFL_RFL_RST        0x0U
#define UART_RFL_RFL_MSK        ( 0x7fU << 0U )
#define UART_RFL_RFL(x)          ( ((x) & 0x7fU) << 0U )

// 0x88, uart_srr
#define UART_SRR_OFFSET        0x88U
#define UART_SRR_UR_POS        0U
#define UART_SRR_UR_RST        0x0U
#define UART_SRR_UR_MSK        ( 0x1U << 0U )
#define UART_SRR_UR(x)          ( ((x) & 0x1U) << 0U )
#define UART_SRR_RFR_POS        1U
#define UART_SRR_RFR_RST        0x0U
#define UART_SRR_RFR_MSK        ( 0x1U << 1U )
#define UART_SRR_RFR(x)          ( ((x) & 0x1U) << 1U )
#define UART_SRR_XFR_POS        2U
#define UART_SRR_XFR_RST        0x0U
#define UART_SRR_XFR_MSK        ( 0x1U << 2U )
#define UART_SRR_XFR(x)          ( ((x) & 0x1U) << 2U )

// 0x8c, uart_srts
#define UART_SRTS_OFFSET        0x8cU
#define UART_SRTS_SRTS_POS        0U
#define UART_SRTS_SRTS_RST        0x0U
#define UART_SRTS_SRTS_MSK        ( 0x1U << 0U )
#define UART_SRTS_SRTS(x)          ( ((x) & 0x1U) << 0U )

// 0x90, uart_sbcr
#define UART_SBCR_OFFSET        0x90U
#define UART_SBCR_SBCB_POS        0U
#define UART_SBCR_SBCB_RST        0x0U
#define UART_SBCR_SBCB_MSK        ( 0x1U << 0U )
#define UART_SBCR_SBCB(x)          ( ((x) & 0x1U) << 0U )

// 0x94, uart_sdmam
#define UART_SDMAM_OFFSET        0x94U
#define UART_SDMAM_SDMAM_POS        0U
#define UART_SDMAM_SDMAM_RST        0x0U
#define UART_SDMAM_SDMAM_MSK        ( 0x1U << 0U )
#define UART_SDMAM_SDMAM(x)          ( ((x) & 0x1U) << 0U )

// 0x98, uart_sfe
#define UART_SFE_OFFSET        0x98U
#define UART_SFE_SFE_POS        0U
#define UART_SFE_SFE_RST        0x0U
#define UART_SFE_SFE_MSK        ( 0x1U << 0U )
#define UART_SFE_SFE(x)          ( ((x) & 0x1U) << 0U )

// 0x9c, uart_srt
#define UART_SRT_OFFSET        0x9cU
#define UART_SRT_SRT_POS        0U
#define UART_SRT_SRT_RST        0x0U
#define UART_SRT_SRT_MSK        ( 0x3U << 0U )
#define UART_SRT_SRT(x)          ( ((x) & 0x3U) << 0U )

// 0xa0, uart_stet
#define UART_STET_OFFSET        0xa0U
#define UART_STET_STET_POS        0U
#define UART_STET_STET_RST        0x0U
#define UART_STET_STET_MSK        ( 0x3U << 0U )
#define UART_STET_STET(x)          ( ((x) & 0x3U) << 0U )

// 0xa4, uart_htx
#define UART_HTX_OFFSET        0xa4U
#define UART_HTX_HTX_POS        0U
#define UART_HTX_HTX_RST        0x0U
#define UART_HTX_HTX_MSK        ( 0x1U << 0U )
#define UART_HTX_HTX(x)          ( ((x) & 0x1U) << 0U )

// 0xa8, uart_dmasa
#define UART_DMASA_OFFSET        0xa8U
#define UART_DMASA_DMASA_POS        0U
#define UART_DMASA_DMASA_RST        0x0U
#define UART_DMASA_DMASA_MSK        ( 0x1U << 0U )
#define UART_DMASA_DMASA(x)          ( ((x) & 0x1U) << 0U )

// 0xac, uart_tcr
#define UART_TCR_OFFSET        0xacU
#define UART_TCR_RS485_EN_POS        0U
#define UART_TCR_RS485_EN_RST        0x0U
#define UART_TCR_RS485_EN_MSK        ( 0x1U << 0U )
#define UART_TCR_RS485_EN(x)          ( ((x) & 0x1U) << 0U )
#define UART_TCR_PE_POL_POS        1U
#define UART_TCR_PE_POL_RST        0x1U
#define UART_TCR_PE_POL_MSK        ( 0x1U << 1U )
#define UART_TCR_PE_POL(x)          ( ((x) & 0x1U) << 1U )
#define UART_TCR_DE_POL_POS        2U
#define UART_TCR_DE_POL_RST        0x1U
#define UART_TCR_DE_POL_MSK        ( 0x1U << 2U )
#define UART_TCR_DE_POL(x)          ( ((x) & 0x1U) << 2U )
#define UART_TCR_XFER_MODE_POS        3U
#define UART_TCR_XFER_MODE_RST        0x0U
#define UART_TCR_XFER_MODE_MSK        ( 0x3U << 3U )
#define UART_TCR_XFER_MODE(x)          ( ((x) & 0x3U) << 3U )

// 0xb0, uart_de_en
#define UART_DE_EN_OFFSET        0xb0U
#define UART_DE_EN_DE_ENABLE_POS        0U
#define UART_DE_EN_DE_ENABLE_RST        0x0U
#define UART_DE_EN_DE_ENABLE_MSK        ( 0x1U << 0U )
#define UART_DE_EN_DE_ENABLE(x)          ( ((x) & 0x1U) << 0U )

// 0xb4, uart_re_en
#define UART_RE_EN_OFFSET        0xb4U
#define UART_RE_EN_RE_ENABLE_POS        0U
#define UART_RE_EN_RE_ENABLE_RST        0x0U
#define UART_RE_EN_RE_ENABLE_MSK        ( 0x1U << 0U )
#define UART_RE_EN_RE_ENABLE(x)          ( ((x) & 0x1U) << 0U )

// 0xb8, uart_det
#define UART_DET_OFFSET        0xb8U
#define UART_DET_DE_ASSERTION_TIME_POS        0U
#define UART_DET_DE_ASSERTION_TIME_RST        0x0U
#define UART_DET_DE_ASSERTION_TIME_MSK        ( 0xffU << 0U )
#define UART_DET_DE_ASSERTION_TIME(x)          ( ((x) & 0xffU) << 0U )
#define UART_DET_DE_DE_ASSERTION_TIME_POS        16U
#define UART_DET_DE_DE_ASSERTION_TIME_RST        0x0U
#define UART_DET_DE_DE_ASSERTION_TIME_MSK        ( 0xffU << 16U )
#define UART_DET_DE_DE_ASSERTION_TIME(x)          ( ((x) & 0xffU) << 16U )

// 0xbc, uart_tat
#define UART_TAT_OFFSET        0xbcU
#define UART_TAT_DE_TO_RE_POS        0U
#define UART_TAT_DE_TO_RE_RST        0x0U
#define UART_TAT_DE_TO_RE_MSK        ( 0xffff << 0U )
#define UART_TAT_DE_TO_RE(x)          ( ((x) & 0xffffU) << 0U )
#define UART_TAT_RE_TO_DE_POS        16U
#define UART_TAT_RE_TO_DE_RST        0x0U
#define UART_TAT_RE_TO_DE_MSK        ( 0xffffU << 16U )
#define UART_TAT_RE_TO_DE(x)          ( ((x) & 0xffffU) << 16U )

// 0xc0, uart_dlf
#define UART_DLF_OFFSET        0xc0U
#define UART_DLF_DLF_POS        0U
#define UART_DLF_DLF_RST        0x0U
#define UART_DLF_DLF_MSK        ( 0x3fU << 0U )
#define UART_DLF_DLF(x)          ( ((x) & 0x3fU) << 0U )

// 0xc4, uart_rar
#define UART_RAR_OFFSET        0xc4U
#define UART_RAR_RAR_POS        0U
#define UART_RAR_RAR_RST        0x0U
#define UART_RAR_RAR_MSK        ( 0xffU << 0U )
#define UART_RAR_RAR(x)          ( ((x) & 0xffU) << 0U )

// 0xc8, uart_tar
#define UART_TAR_OFFSET        0xc8U
#define UART_TAR_TAR_POS        0U
#define UART_TAR_TAR_RST        0x0U
#define UART_TAR_TAR_MSK        ( 0xffU << 0U )
#define UART_TAR_TAR(x)          ( ((x) & 0xffU) << 0U )

// 0xcc, uart_lcr_ext
#define UART_LCR_EXT_OFFSET        0xccU
#define UART_LCR_EXT_DLS_E_POS        0U
#define UART_LCR_EXT_DLS_E_RST        0x0U
#define UART_LCR_EXT_DLS_E_MSK        ( 0x1U << 0U )
#define UART_LCR_EXT_DLS_E(x)          ( ((x) & 0x1U) << 0U )
#define UART_LCR_EXT_ADDR_MATCH_POS        1U
#define UART_LCR_EXT_ADDR_MATCH_RST        0x0U
#define UART_LCR_EXT_ADDR_MATCH_MSK        ( 0x1U << 1U )
#define UART_LCR_EXT_ADDR_MATCH(x)          ( ((x) & 0x1U) << 1U )
#define UART_LCR_EXT_SEND_ADDR_POS        2U
#define UART_LCR_EXT_SEND_ADDR_RST        0x0U
#define UART_LCR_EXT_SEND_ADDR_MSK        ( 0x1U << 2U )
#define UART_LCR_EXT_SEND_ADDR(x)          ( ((x) & 0x1U) << 2U )
#define UART_LCR_EXT_TRANSMIT_MODE_POS        3U
#define UART_LCR_EXT_TRANSMIT_MODE_RST        0x0U
#define UART_LCR_EXT_TRANSMIT_MODE_MSK        ( 0x1U << 3U )
#define UART_LCR_EXT_TRANSMIT_MODE(x)          ( ((x) & 0x1U) << 3U )

// 0xf4, uart_cpr
#define UART_CPR_OFFSET        0xf4U
#define UART_CPR_APB_DATA_WIDTH_POS        0U
#define UART_CPR_APB_DATA_WIDTH_RST        0x2U
#define UART_CPR_APB_DATA_WIDTH_MSK        ( 0x3U << 0U )
#define UART_CPR_APB_DATA_WIDTH(x)          ( ((x) & 0x3U) << 0U )
#define UART_CPR_AFCE_MODE_POS        4U
#define UART_CPR_AFCE_MODE_RST        0x1U
#define UART_CPR_AFCE_MODE_MSK        ( 0x1U << 4U )
#define UART_CPR_AFCE_MODE(x)          ( ((x) & 0x1U) << 4U )
#define UART_CPR_THRE_MODE_POS        5U
#define UART_CPR_THRE_MODE_RST        0x1U
#define UART_CPR_THRE_MODE_MSK        ( 0x1U << 5U )
#define UART_CPR_THRE_MODE(x)          ( ((x) & 0x1U) << 5U )
#define UART_CPR_SIR_MODE_POS        6U
#define UART_CPR_SIR_MODE_RST        0x1U
#define UART_CPR_SIR_MODE_MSK        ( 0x1U << 6U )
#define UART_CPR_SIR_MODE(x)          ( ((x) & 0x1U) << 6U )
#define UART_CPR_SIR_LP_MODE_POS        7U
#define UART_CPR_SIR_LP_MODE_RST        0x1U
#define UART_CPR_SIR_LP_MODE_MSK        ( 0x1U << 7U )
#define UART_CPR_SIR_LP_MODE(x)          ( ((x) & 0x1U) << 7U )
#define UART_CPR_ADDITIONAL_FEAT_POS        8U
#define UART_CPR_ADDITIONAL_FEAT_RST        0x1U
#define UART_CPR_ADDITIONAL_FEAT_MSK        ( 0x1U << 8U )
#define UART_CPR_ADDITIONAL_FEAT(x)          ( ((x) & 0x1U) << 8U )
#define UART_CPR_FIFO_ACCESS_POS        9U
#define UART_CPR_FIFO_ACCESS_RST        0x1U
#define UART_CPR_FIFO_ACCESS_MSK        ( 0x1U << 9U )
#define UART_CPR_FIFO_ACCESS(x)          ( ((x) & 0x1U) << 9U )
#define UART_CPR_FIFO_STAT_POS        10U
#define UART_CPR_FIFO_STAT_RST        0x1U
#define UART_CPR_FIFO_STAT_MSK        ( 0x1U << 10U )
#define UART_CPR_FIFO_STAT(x)          ( ((x) & 0x1U) << 10U )
#define UART_CPR_SHADOW_POS        11U
#define UART_CPR_SHADOW_RST        0x1U
#define UART_CPR_SHADOW_MSK        ( 0x1U << 11U )
#define UART_CPR_SHADOW(x)          ( ((x) & 0x1U) << 11U )
#define UART_CPR_ADD_ENCODED_PARAMS_POS        12U
#define UART_CPR_ADD_ENCODED_PARAMS_RST        0x1U
#define UART_CPR_ADD_ENCODED_PARAMS_MSK        ( 0x1U << 12U )
#define UART_CPR_ADD_ENCODED_PARAMS(x)          ( ((x) & 0x1U) << 12U )
#define UART_CPR_DMA_EXTRA_POS        13U
#define UART_CPR_DMA_EXTRA_RST        0x1U
#define UART_CPR_DMA_EXTRA_MSK        ( 0x1U << 13U )
#define UART_CPR_DMA_EXTRA(x)          ( ((x) & 0x1U) << 13U )
#define UART_CPR_FIFO_MODE_POS        16U
#define UART_CPR_FIFO_MODE_RST        0x4U
#define UART_CPR_FIFO_MODE_MSK        ( 0xffU << 16 )
#define UART_CPR_FIFO_MODE(x)          ( ((x) & 0xffU) << 16U )

// 0xf8, uart_ucv
#define UART_UCV_OFFSET        0xf8U
#define UART_UCV_COMPONENT_VERSION_POS        0U
#define UART_UCV_COMPONENT_VERSION_RST        0x3430322aU
#define UART_UCV_COMPONENT_VERSION_MSK        ( 0xffffffffU << 0U )
#define UART_UCV_COMPONENT_VERSION(x)          ( ((x) & 0xffffffffU) << 0U )

// 0xfc, uart_ctr
#define UART_CTR_OFFSET        0xfcU
#define UART_CTR_PERIPHERAL_ID_POS        0U
#define UART_CTR_PERIPHERAL_ID_RST        0x44570110U
#define UART_CTR_PERIPHERAL_ID_MSK        ( 0xffffffffU << 0U )
#define UART_CTR_PERIPHERAL_ID(x)          ( ((x) & 0xffffffffU) << 0U )

#endif
