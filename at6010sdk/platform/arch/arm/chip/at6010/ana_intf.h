#ifndef _ANA_INTF_H_
#define _ANA_INTF_H_

#include "globals_macro.h"

#define REG_ANA_INTF_BASE            0x40103800U

typedef struct
{
    REG8        reset_ctrl;    // 0x0
    REG8        efuse_ctrl;    // 0x1
    REG8        efuse_time_pgm_0;    // 0x2
    REG8        efuse_time_pgm_1;    // 0x3
    REG8        efuse_time_vddq_pu;    // 0x4
    REG8        efuse_time_hold_sclk;    // 0x5
    REG8        efuse_wdata_0;    // 0x6
    REG8        efuse_wdata_1;    // 0x7
    REG8        efuse_wdata_2;    // 0x8
    REG8        efuse_wdata_3;    // 0x9
    REG8        efuse_wdata_4;    // 0xa
    REG8        efuse_wdata_5;    // 0xb
    REG8        efuse_wdata_6;    // 0xc
    REG8        efuse_wdata_7;    // 0xd
    REG8        efuse_wdata_8;    // 0xe
    REG8        efuse_wdata_9;    // 0xf
    REG8        efuse_wdata_10;    // 0x10
    REG8        efuse_wdata_11;    // 0x11
    REG8        efuse_wdata_12;    // 0x12
    REG8        efuse_wdata_13;    // 0x13
    REG8        efuse_wdata_14;    // 0x14
    REG8        efuse_wdata_15;    // 0x15
    REG8        efuse_rdata_0;    // 0x16
    REG8        efuse_rdata_1;    // 0x17
    REG8        efuse_rdata_2;    // 0x18
    REG8        efuse_rdata_3;    // 0x19
    REG8        efuse_rdata_4;    // 0x1a
    REG8        efuse_rdata_5;    // 0x1b
    REG8        efuse_rdata_6;    // 0x1c
    REG8        efuse_rdata_7;    // 0x1d
    REG8        efuse_rdata_8;    // 0x1e
    REG8        efuse_rdata_9;    // 0x1f
    REG8        efuse_rdata_10;    // 0x20
    REG8        efuse_rdata_11;    // 0x21
    REG8        efuse_rdata_12;    // 0x22
    REG8        efuse_rdata_13;    // 0x23
    REG8        efuse_rdata_14;    // 0x24
    REG8        efuse_rdata_15;    // 0x25
    REG8        efuse_status;    // 0x26
    REG8        low_bat_ctrl;    // 0x27
    REG8        low_bat_smooth_high;    // 0x28
    REG8        low_bat_smooth_low;    // 0x29
    REG8        bor_ctrl;    // 0x2a
    REG8        bor_smooth_high;    // 0x2b
    REG8        bor_smooth_low;    // 0x2c
    REG8        pll_lock_det_ctrl;    // 0x2d
    REG8        pll_lock_det_lock_th;    // 0x2e
    REG8        pll_lock_det_unlock_th;    // 0x2f
    REG8        pll_lock_det_status;    // 0x30
    REG8        aux_adc_ctrl;    // 0x31
    REG8        aux_adc_div_ctrl;    // 0x32
    REG8        aux_adc_init;    // 0x33
    REG8        aux_adc_status_0;    // 0x34
    REG8        aux_adc_status_1;    // 0x35
    REG8        key_ctrl;    // 0x36
    REG8        key_smooth_high;    // 0x37
    REG8        key_smooth_low;    // 0x38
    REG8        light_ctrl;    // 0x39
    REG8        light_th_low_l;    // 0x3a
    REG8        light_th_high_l;    // 0x3b
    REG8        light_th_h;    // 0x3c
    REG8        light_status;    // 0x3d
    REG8        time_light_sleep_0;    // 0x3e
    REG8        time_light_sleep_1;    // 0x3f
    REG8        time_light_sleep_2;    // 0x40
    REG8        time_light_sleep_3;    // 0x41
    REG8        ana_irq_clr;    // 0x42
    REG8        ana_irq_msk;    // 0x43
    REG8        ana_irq_status;    // 0x44
    REG8        iomux_ctrl;    // 0x45
    REG8        iomux_sel_0;    // 0x46
    REG8        iomux_sel_1;    // 0x47
    REG8        iomux_sel_2;    // 0x48
    REG8        iomux_sel_3;    // 0x49
    REG8        gpio_aon_sel;    // 0x4a
    REG8        debug_aon_en;    // 0x4b
    REG8        debug_aon_sel;    // 0x4c
    REG8        gpio_ctrl;    // 0x4d
    REG8        gpio_aon_0;    // 0x4e
    REG8        gpio_aon_1;    // 0x4f
    REG8        gpio_aon_2;    // 0x50
    REG8        gpio_aon_3;    // 0x51
    REG8        gpio_aon_4;    // 0x52
    REG8        gpio_aon_5;    // 0x53
    REG8        gpio_aon_6;    // 0x54
    REG8        gpio_aon_7;    // 0x55
    REG8        gpio_aon_status;    // 0x56
} HWP_ANA_INTF_T;

#define hwp_ana_intf        ((HWP_ANA_INTF_T*)REG_ANA_INTF_BASE)

// 0x0, reset_ctrl
#define RESET_CTRL_OFFSET        0x0U
#define RESET_CTRL_RSTN_SW_POS        0U
#define RESET_CTRL_RSTN_SW_RST        0x1U
#define RESET_CTRL_RSTN_SW_MSK        ( 0x1U << 0U )
#define RESET_CTRL_RSTN_SW(x)          ( ((x) & 0x1U) << 0U )

// 0x1, efuse_ctrl
#define EFUSE_CTRL_OFFSET        0x1U
#define EFUSE_CTRL_RD_POS        0U
#define EFUSE_CTRL_RD_RST        0x0U
#define EFUSE_CTRL_RD_MSK        ( 0x1U << 0U )
#define EFUSE_CTRL_RD(x)          ( ((x) & 0x1U) << 0U )
#define EFUSE_CTRL_WR_POS        1U
#define EFUSE_CTRL_WR_RST        0x0U
#define EFUSE_CTRL_WR_MSK        ( 0x1U << 1U )
#define EFUSE_CTRL_WR(x)          ( ((x) & 0x1U) << 1U )
#define EFUSE_CTRL_VDDQ_REG_POS        2U
#define EFUSE_CTRL_VDDQ_REG_RST        0x0U
#define EFUSE_CTRL_VDDQ_REG_MSK        ( 0x1U << 2U )
#define EFUSE_CTRL_VDDQ_REG(x)          ( ((x) & 0x1U) << 2U )
#define EFUSE_CTRL_VDDQ_DR_POS        3U
#define EFUSE_CTRL_VDDQ_DR_RST        0x0U
#define EFUSE_CTRL_VDDQ_DR_MSK        ( 0x1U << 3U )
#define EFUSE_CTRL_VDDQ_DR(x)          ( ((x) & 0x1U) << 3U )
#define EFUSE_CTRL_VDD_POS        4U
#define EFUSE_CTRL_VDD_RST        0x1U
#define EFUSE_CTRL_VDD_MSK        ( 0x1U << 4U )
#define EFUSE_CTRL_VDD(x)          ( ((x) & 0x1U) << 4U )
#define EFUSE_CTRL_TIMER_CLR_POS        5U
#define EFUSE_CTRL_TIMER_CLR_RST        0x0U
#define EFUSE_CTRL_TIMER_CLR_MSK        ( 0x1U << 5U )
#define EFUSE_CTRL_TIMER_CLR(x)          ( ((x) & 0x1U) << 5U )
#define EFUSE_CTRL_SERIAL_MODE_POS        6U
#define EFUSE_CTRL_SERIAL_MODE_RST        0x1U
#define EFUSE_CTRL_SERIAL_MODE_MSK        ( 0x1U << 6U )
#define EFUSE_CTRL_SERIAL_MODE(x)          ( ((x) & 0x1U) << 6U )

// 0x2, efuse_time_pgm_0
#define EFUSE_TIME_PGM_0_OFFSET        0x2U
#define EFUSE_TIME_PGM_0_CNT_POS        0U
#define EFUSE_TIME_PGM_0_CNT_RST        0x05U
#define EFUSE_TIME_PGM_0_CNT_MSK        ( 0xffU << 0U )
#define EFUSE_TIME_PGM_0_CNT(x)          ( ((x) & 0xffU) << 0U )

// 0x3, efuse_time_pgm_1
#define EFUSE_TIME_PGM_1_OFFSET        0x3U
#define EFUSE_TIME_PGM_1_CNT_POS        0U
#define EFUSE_TIME_PGM_1_CNT_RST        0x01U
#define EFUSE_TIME_PGM_1_CNT_MSK        ( 0xffU << 0U )
#define EFUSE_TIME_PGM_1_CNT(x)          ( ((x) & 0xffU) << 0U )

// 0x4, efuse_time_vddq_pu
#define EFUSE_TIME_VDDQ_PU_OFFSET        0x4U
#define EFUSE_TIME_VDDQ_PU_CNT_POS        0U
#define EFUSE_TIME_VDDQ_PU_CNT_RST        0x19U
#define EFUSE_TIME_VDDQ_PU_CNT_MSK        ( 0xffU << 0U )
#define EFUSE_TIME_VDDQ_PU_CNT(x)          ( ((x) & 0xffU) << 0U )

// 0x5, efuse_time_hold_sclk
#define EFUSE_TIME_HOLD_SCLK_OFFSET        0x5U
#define EFUSE_TIME_HOLD_SCLK_CNT_POS        0U
#define EFUSE_TIME_HOLD_SCLK_CNT_RST        0xeU
#define EFUSE_TIME_HOLD_SCLK_CNT_MSK        ( 0xffU << 0U )
#define EFUSE_TIME_HOLD_SCLK_CNT(x)          ( ((x) & 0xffU) << 0U )

// 0x6, efuse_wdata_0
#define EFUSE_WDATA_0_OFFSET        0x6U
#define EFUSE_WDATA_0_DATA_POS        0U
#define EFUSE_WDATA_0_DATA_RST        0x0U
#define EFUSE_WDATA_0_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_0_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x7, efuse_wdata_1
#define EFUSE_WDATA_1_OFFSET        0x7U
#define EFUSE_WDATA_1_DATA_POS        0U
#define EFUSE_WDATA_1_DATA_RST        0x0U
#define EFUSE_WDATA_1_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_1_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x8, efuse_wdata_2
#define EFUSE_WDATA_2_OFFSET        0x8U
#define EFUSE_WDATA_2_DATA_POS        0U
#define EFUSE_WDATA_2_DATA_RST        0x0U
#define EFUSE_WDATA_2_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_2_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x9, efuse_wdata_3
#define EFUSE_WDATA_3_OFFSET        0x9U
#define EFUSE_WDATA_3_DATA_POS        0U
#define EFUSE_WDATA_3_DATA_RST        0x0U
#define EFUSE_WDATA_3_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_3_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0xa, efuse_wdata_4
#define EFUSE_WDATA_4_OFFSET        0xaU
#define EFUSE_WDATA_4_DATA_POS        0U
#define EFUSE_WDATA_4_DATA_RST        0x0U
#define EFUSE_WDATA_4_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_4_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0xb, efuse_wdata_5
#define EFUSE_WDATA_5_OFFSET        0xbU
#define EFUSE_WDATA_5_DATA_POS        0U
#define EFUSE_WDATA_5_DATA_RST        0x0U
#define EFUSE_WDATA_5_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_5_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0xc, efuse_wdata_6
#define EFUSE_WDATA_6_OFFSET        0xcU
#define EFUSE_WDATA_6_DATA_POS        0U
#define EFUSE_WDATA_6_DATA_RST        0x0U
#define EFUSE_WDATA_6_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_6_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0xd, efuse_wdata_7
#define EFUSE_WDATA_7_OFFSET        0xdU
#define EFUSE_WDATA_7_DATA_POS        0U
#define EFUSE_WDATA_7_DATA_RST        0x0U
#define EFUSE_WDATA_7_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_7_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0xe, efuse_wdata_8
#define EFUSE_WDATA_8_OFFSET        0xeU
#define EFUSE_WDATA_8_DATA_POS        0U
#define EFUSE_WDATA_8_DATA_RST        0x0U
#define EFUSE_WDATA_8_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_8_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0xf, efuse_wdata_9
#define EFUSE_WDATA_9_OFFSET        0xfU
#define EFUSE_WDATA_9_DATA_POS        0U
#define EFUSE_WDATA_9_DATA_RST        0x0U
#define EFUSE_WDATA_9_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_9_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x10, efuse_wdata_10
#define EFUSE_WDATA_10_OFFSET        0x10U
#define EFUSE_WDATA_10_DATA_POS        0U
#define EFUSE_WDATA_10_DATA_RST        0x0U
#define EFUSE_WDATA_10_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_10_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x11, efuse_wdata_11
#define EFUSE_WDATA_11_OFFSET        0x11U
#define EFUSE_WDATA_11_DATA_POS        0U
#define EFUSE_WDATA_11_DATA_RST        0x0U
#define EFUSE_WDATA_11_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_11_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x12, efuse_wdata_12
#define EFUSE_WDATA_12_OFFSET        0x12U
#define EFUSE_WDATA_12_DATA_POS        0U
#define EFUSE_WDATA_12_DATA_RST        0x0U
#define EFUSE_WDATA_12_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_12_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x13, efuse_wdata_13
#define EFUSE_WDATA_13_OFFSET        0x13U
#define EFUSE_WDATA_13_DATA_POS        0U
#define EFUSE_WDATA_13_DATA_RST        0x0U
#define EFUSE_WDATA_13_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_13_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x14, efuse_wdata_14
#define EFUSE_WDATA_14_OFFSET        0x14U
#define EFUSE_WDATA_14_DATA_POS        0U
#define EFUSE_WDATA_14_DATA_RST        0x0U
#define EFUSE_WDATA_14_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_14_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x15, efuse_wdata_15
#define EFUSE_WDATA_15_OFFSET        0x15U
#define EFUSE_WDATA_15_DATA_POS        0U
#define EFUSE_WDATA_15_DATA_RST        0x0U
#define EFUSE_WDATA_15_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_WDATA_15_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x16, efuse_rdata_0
#define EFUSE_RDATA_0_OFFSET        0x16U
#define EFUSE_RDATA_0_DATA_POS        0U
#define EFUSE_RDATA_0_DATA_RST        0x0U
#define EFUSE_RDATA_0_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_0_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x17, efuse_rdata_1
#define EFUSE_RDATA_1_OFFSET        0x17U
#define EFUSE_RDATA_1_DATA_POS        0U
#define EFUSE_RDATA_1_DATA_RST        0x0U
#define EFUSE_RDATA_1_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_1_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x18, efuse_rdata_2
#define EFUSE_RDATA_2_OFFSET        0x18U
#define EFUSE_RDATA_2_DATA_POS        0U
#define EFUSE_RDATA_2_DATA_RST        0x0U
#define EFUSE_RDATA_2_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_2_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x19, efuse_rdata_3
#define EFUSE_RDATA_3_OFFSET        0x19U
#define EFUSE_RDATA_3_DATA_POS        0U
#define EFUSE_RDATA_3_DATA_RST        0x0U
#define EFUSE_RDATA_3_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_3_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x1a, efuse_rdata_4
#define EFUSE_RDATA_4_OFFSET        0x1aU
#define EFUSE_RDATA_4_DATA_POS        0U
#define EFUSE_RDATA_4_DATA_RST        0x0U
#define EFUSE_RDATA_4_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_4_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x1b, efuse_rdata_5
#define EFUSE_RDATA_5_OFFSET        0x1bU
#define EFUSE_RDATA_5_DATA_POS        0U
#define EFUSE_RDATA_5_DATA_RST        0x0U
#define EFUSE_RDATA_5_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_5_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x1c, efuse_rdata_6
#define EFUSE_RDATA_6_OFFSET        0x1cU
#define EFUSE_RDATA_6_DATA_POS        0U
#define EFUSE_RDATA_6_DATA_RST        0x0U
#define EFUSE_RDATA_6_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_6_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x1d, efuse_rdata_7
#define EFUSE_RDATA_7_OFFSET        0x1dU
#define EFUSE_RDATA_7_DATA_POS        0U
#define EFUSE_RDATA_7_DATA_RST        0x0U
#define EFUSE_RDATA_7_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_7_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x1e, efuse_rdata_8
#define EFUSE_RDATA_8_OFFSET        0x1eU
#define EFUSE_RDATA_8_DATA_POS        0U
#define EFUSE_RDATA_8_DATA_RST        0x0U
#define EFUSE_RDATA_8_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_8_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x1f, efuse_rdata_9
#define EFUSE_RDATA_9_OFFSET        0x1fU
#define EFUSE_RDATA_9_DATA_POS        0U
#define EFUSE_RDATA_9_DATA_RST        0x0U
#define EFUSE_RDATA_9_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_9_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x20, efuse_rdata_10
#define EFUSE_RDATA_10_OFFSET        0x20U
#define EFUSE_RDATA_10_DATA_POS        0U
#define EFUSE_RDATA_10_DATA_RST        0x0U
#define EFUSE_RDATA_10_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_10_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x21, efuse_rdata_11
#define EFUSE_RDATA_11_OFFSET        0x21U
#define EFUSE_RDATA_11_DATA_POS        0U
#define EFUSE_RDATA_11_DATA_RST        0x0U
#define EFUSE_RDATA_11_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_11_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x22, efuse_rdata_12
#define EFUSE_RDATA_12_OFFSET        0x22U
#define EFUSE_RDATA_12_DATA_POS        0U
#define EFUSE_RDATA_12_DATA_RST        0x0U
#define EFUSE_RDATA_12_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_12_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x23, efuse_rdata_13
#define EFUSE_RDATA_13_OFFSET        0x23U
#define EFUSE_RDATA_13_DATA_POS        0U
#define EFUSE_RDATA_13_DATA_RST        0x0U
#define EFUSE_RDATA_13_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_13_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x24, efuse_rdata_14
#define EFUSE_RDATA_14_OFFSET        0x24U
#define EFUSE_RDATA_14_DATA_POS        0U
#define EFUSE_RDATA_14_DATA_RST        0x0U
#define EFUSE_RDATA_14_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_14_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x25, efuse_rdata_15
#define EFUSE_RDATA_15_OFFSET        0x25U
#define EFUSE_RDATA_15_DATA_POS        0U
#define EFUSE_RDATA_15_DATA_RST        0x0U
#define EFUSE_RDATA_15_DATA_MSK        ( 0xffU << 0U )
#define EFUSE_RDATA_15_DATA(x)          ( ((x) & 0xffU) << 0U )

// 0x26, efuse_status
#define EFUSE_STATUS_OFFSET        0x26U
#define EFUSE_STATUS_WR_DONE_POS        0U
#define EFUSE_STATUS_WR_DONE_RST        0x0U
#define EFUSE_STATUS_WR_DONE_MSK        ( 0x1U << 0U )
#define EFUSE_STATUS_WR_DONE(x)          ( ((x) & 0x1U) << 0U )
#define EFUSE_STATUS_RD_DONE_POS        1U
#define EFUSE_STATUS_RD_DONE_RST        0x0U
#define EFUSE_STATUS_RD_DONE_MSK        ( 0x1U << 1U )
#define EFUSE_STATUS_RD_DONE(x)          ( ((x) & 0x1U) << 1U )
#define EFUSE_STATUS_CK_OK_POS        2U
#define EFUSE_STATUS_CK_OK_RST        0x0U
#define EFUSE_STATUS_CK_OK_MSK        ( 0x1U << 2U )
#define EFUSE_STATUS_CK_OK(x)          ( ((x) & 0x1U) << 2U )

// 0x27, low_bat_ctrl
#define LOW_BAT_CTRL_OFFSET        0x27U
#define LOW_BAT_CTRL_EN_DR_POS        0U
#define LOW_BAT_CTRL_EN_DR_RST        0x0U
#define LOW_BAT_CTRL_EN_DR_MSK        ( 0x1U << 0U )
#define LOW_BAT_CTRL_EN_DR(x)          ( ((x) & 0x1U) << 0U )
#define LOW_BAT_CTRL_EN_REG_POS        1U
#define LOW_BAT_CTRL_EN_REG_RST        0x0U
#define LOW_BAT_CTRL_EN_REG_MSK        ( 0x1U << 1U )
#define LOW_BAT_CTRL_EN_REG(x)          ( ((x) & 0x1U) << 1U )
#define LOW_BAT_CTRL_INV_POS        2U
#define LOW_BAT_CTRL_INV_RST        0x1U
#define LOW_BAT_CTRL_INV_MSK        ( 0x1U << 2U )
#define LOW_BAT_CTRL_INV(x)          ( ((x) & 0x1U) << 2U )
#define LOW_BAT_CTRL_SMOOTH_EN_POS        3U
#define LOW_BAT_CTRL_SMOOTH_EN_RST        0x1U
#define LOW_BAT_CTRL_SMOOTH_EN_MSK        ( 0x1U << 3U )
#define LOW_BAT_CTRL_SMOOTH_EN(x)          ( ((x) & 0x1U) << 3U )
#define LOW_BAT_CTRL_DETECTED_POS        4U
#define LOW_BAT_CTRL_DETECTED_RST        0x0U
#define LOW_BAT_CTRL_DETECTED_MSK        ( 0x1U << 4U )
#define LOW_BAT_CTRL_DETECTED(x)          ( ((x) & 0x1U) << 4U )

// 0x28, low_bat_smooth_high
#define LOW_BAT_SMOOTH_HIGH_OFFSET        0x28U
#define LOW_BAT_SMOOTH_HIGH_CNT_POS        0U
#define LOW_BAT_SMOOTH_HIGH_CNT_RST        0x4U
#define LOW_BAT_SMOOTH_HIGH_CNT_MSK        ( 0xffU << 0U )
#define LOW_BAT_SMOOTH_HIGH_CNT(x)          ( ((x) & 0xffU) << 0U )

// 0x29, low_bat_smooth_low
#define LOW_BAT_SMOOTH_LOW_OFFSET        0x29U
#define LOW_BAT_SMOOTH_LOW_CNT_POS        0U
#define LOW_BAT_SMOOTH_LOW_CNT_RST        0x4U
#define LOW_BAT_SMOOTH_LOW_CNT_MSK        ( 0xffU << 0U )
#define LOW_BAT_SMOOTH_LOW_CNT(x)          ( ((x) & 0xffU) << 0U )

// 0x2a, bor_ctrl
#define BOR_CTRL_OFFSET        0x2aU
#define BOR_CTRL_EN_POS        0U
#define BOR_CTRL_EN_RST        0x0U
#define BOR_CTRL_EN_MSK        ( 0x1U << 0U )
#define BOR_CTRL_EN(x)          ( ((x) & 0x1U) << 0U )
#define BOR_CTRL_INV_POS        1U
#define BOR_CTRL_INV_RST        0x1U
#define BOR_CTRL_INV_MSK        ( 0x1U << 1U )
#define BOR_CTRL_INV(x)          ( ((x) & 0x1U) << 1U )
#define BOR_CTRL_SMOOTH_EN_POS        2U
#define BOR_CTRL_SMOOTH_EN_RST        0x0U
#define BOR_CTRL_SMOOTH_EN_MSK        ( 0x1U << 2U )
#define BOR_CTRL_SMOOTH_EN(x)          ( ((x) & 0x1U) << 2U )
#define BOR_CTRL_DETECTED_POS        3U
#define BOR_CTRL_DETECTED_RST        0x0U
#define BOR_CTRL_DETECTED_MSK        ( 0x1U << 3U )
#define BOR_CTRL_DETECTED(x)          ( ((x) & 0x1U) << 3U )

// 0x2b, bor_smooth_high
#define BOR_SMOOTH_HIGH_OFFSET        0x2bU
#define BOR_SMOOTH_HIGH_CNT_POS        0U
#define BOR_SMOOTH_HIGH_CNT_RST        0x4U
#define BOR_SMOOTH_HIGH_CNT_MSK        ( 0xffU << 0U )
#define BOR_SMOOTH_HIGH_CNT(x)          ( ((x) & 0xffU) << 0U )

// 0x2c, bor_smooth_low
#define BOR_SMOOTH_LOW_OFFSET        0x2cU
#define BOR_SMOOTH_LOW_CNT_POS        0U
#define BOR_SMOOTH_LOW_CNT_RST        0x4U
#define BOR_SMOOTH_LOW_CNT_MSK        ( 0xffU << 0U )
#define BOR_SMOOTH_LOW_CNT(x)          ( ((x) & 0xffU) << 0U )

// 0x2d, pll_lock_det_ctrl
#define PLL_LOCK_DET_CTRL_OFFSET        0x2dU
#define PLL_LOCK_DET_CTRL_EN_POS        0U
#define PLL_LOCK_DET_CTRL_EN_RST        0x0U
#define PLL_LOCK_DET_CTRL_EN_MSK        ( 0x1U << 0U )
#define PLL_LOCK_DET_CTRL_EN(x)          ( ((x) & 0x1U) << 0U )

// 0x2e, pll_lock_det_lock_th
#define PLL_LOCK_DET_LOCK_TH_OFFSET        0x2eU
#define PLL_LOCK_DET_LOCK_TH_TH_POS        0U
#define PLL_LOCK_DET_LOCK_TH_TH_RST        0xffU
#define PLL_LOCK_DET_LOCK_TH_TH_MSK        ( 0xffU << 0U )
#define PLL_LOCK_DET_LOCK_TH_TH(x)          ( ((x) & 0xffU) << 0U )

// 0x2f, pll_lock_det_unlock_th
#define PLL_LOCK_DET_UNLOCK_TH_OFFSET        0x2fU
#define PLL_LOCK_DET_UNLOCK_TH_TH_POS        0U
#define PLL_LOCK_DET_UNLOCK_TH_TH_RST        0x10U
#define PLL_LOCK_DET_UNLOCK_TH_TH_MSK        ( 0xffU << 0U )
#define PLL_LOCK_DET_UNLOCK_TH_TH(x)          ( ((x) & 0xffU) << 0U )

// 0x30, pll_lock_det_status
#define PLL_LOCK_DET_STATUS_OFFSET        0x30U
#define PLL_LOCK_DET_STATUS_LOCK_POS        0U
#define PLL_LOCK_DET_STATUS_LOCK_RST        0x0U
#define PLL_LOCK_DET_STATUS_LOCK_MSK        ( 0x1U << 0U )
#define PLL_LOCK_DET_STATUS_LOCK(x)          ( ((x) & 0x1U) << 0U )
#define PLL_LOCK_DET_STATUS_LOCK_FREE_POS        1U
#define PLL_LOCK_DET_STATUS_LOCK_FREE_RST        0x0U
#define PLL_LOCK_DET_STATUS_LOCK_FREE_MSK        ( 0x1U << 1U )
#define PLL_LOCK_DET_STATUS_LOCK_FREE(x)          ( ((x) & 0x1U) << 1U )

// 0x31, aux_adc_ctrl
#define AUX_ADC_CTRL_OFFSET        0x31U
#define AUX_ADC_CTRL_CLK_EN_POS        0U
#define AUX_ADC_CTRL_CLK_EN_RST        0x0U
#define AUX_ADC_CTRL_CLK_EN_MSK        ( 0x1U << 0U )
#define AUX_ADC_CTRL_CLK_EN(x)          ( ((x) & 0x1U) << 0U )
#define AUX_ADC_CTRL_SAMPLE_EN_DR_POS        1U
#define AUX_ADC_CTRL_SAMPLE_EN_DR_RST        0x0U
#define AUX_ADC_CTRL_SAMPLE_EN_DR_MSK        ( 0x1U << 1U )
#define AUX_ADC_CTRL_SAMPLE_EN_DR(x)          ( ((x) & 0x1U) << 1U )
#define AUX_ADC_CTRL_SAMPLE_EN_REG_POS        2U
#define AUX_ADC_CTRL_SAMPLE_EN_REG_RST        0x0U
#define AUX_ADC_CTRL_SAMPLE_EN_REG_MSK        ( 0x1U << 2U )
#define AUX_ADC_CTRL_SAMPLE_EN_REG(x)          ( ((x) & 0x1U) << 2U )
#define AUX_ADC_CTRL_SAMPLE_ONCE_POS        3U
#define AUX_ADC_CTRL_SAMPLE_ONCE_RST        0x1U
#define AUX_ADC_CTRL_SAMPLE_ONCE_MSK        ( 0x1U << 3U )
#define AUX_ADC_CTRL_SAMPLE_ONCE(x)          ( ((x) & 0x1U) << 3U )
#define AUX_ADC_CTRL_EN_DR_POS        4U
#define AUX_ADC_CTRL_EN_DR_RST        0x0U
#define AUX_ADC_CTRL_EN_DR_MSK        ( 0x1U << 4U )
#define AUX_ADC_CTRL_EN_DR(x)          ( ((x) & 0x1U) << 4U )

// 0x32, aux_adc_div_ctrl
#define AUX_ADC_DIV_CTRL_OFFSET        0x32U
#define AUX_ADC_DIV_CTRL_DIV_BYPASS_POS        0U
#define AUX_ADC_DIV_CTRL_DIV_BYPASS_RST        0x0U
#define AUX_ADC_DIV_CTRL_DIV_BYPASS_MSK        ( 0x1U << 0U )
#define AUX_ADC_DIV_CTRL_DIV_BYPASS(x)          ( ((x) & 0x1U) << 0U )
#define AUX_ADC_DIV_CTRL_DIV_LOAD_POS        1U
#define AUX_ADC_DIV_CTRL_DIV_LOAD_RST        0x0U
#define AUX_ADC_DIV_CTRL_DIV_LOAD_MSK        ( 0x1U << 1U )
#define AUX_ADC_DIV_CTRL_DIV_LOAD(x)          ( ((x) & 0x1U) << 1U )
#define AUX_ADC_DIV_CTRL_DIV_EN_POS        2U
#define AUX_ADC_DIV_CTRL_DIV_EN_RST        0x1U
#define AUX_ADC_DIV_CTRL_DIV_EN_MSK        ( 0x1U << 2U )
#define AUX_ADC_DIV_CTRL_DIV_EN(x)          ( ((x) & 0x1U) << 2U )
#define AUX_ADC_DIV_CTRL_DIV_POS        3U
#define AUX_ADC_DIV_CTRL_DIV_RST        0x0U
#define AUX_ADC_DIV_CTRL_DIV_MSK        ( 0x1fU << 3U )
#define AUX_ADC_DIV_CTRL_DIV(x)          ( ((x) & 0x1fU) << 3U )

// 0x33, aux_adc_init
#define AUX_ADC_INIT_OFFSET        0x33U
#define AUX_ADC_INIT_WIN_POS        0U
#define AUX_ADC_INIT_WIN_RST        0x36U
#define AUX_ADC_INIT_WIN_MSK        ( 0xffU << 0U )
#define AUX_ADC_INIT_WIN(x)          ( ((x) & 0xffU) << 0U )

// 0x34, aux_adc_status_0
#define AUX_ADC_STATUS_0_OFFSET        0x34U
#define AUX_ADC_STATUS_0_ADC_OUT_L_POS        0U
#define AUX_ADC_STATUS_0_ADC_OUT_L_RST        0x0U
#define AUX_ADC_STATUS_0_ADC_OUT_L_MSK        ( 0xffU << 0U )
#define AUX_ADC_STATUS_0_ADC_OUT_L(x)          ( ((x) & 0xffU) << 0U )

// 0x35, aux_adc_status_1
#define AUX_ADC_STATUS_1_OFFSET        0x35U
#define AUX_ADC_STATUS_1_ADC_OUT_H_POS        0U
#define AUX_ADC_STATUS_1_ADC_OUT_H_RST        0x0U
#define AUX_ADC_STATUS_1_ADC_OUT_H_MSK        ( 0xfU << 0U )
#define AUX_ADC_STATUS_1_ADC_OUT_H(x)          ( ((x) & 0xfU) << 0U )
#define AUX_ADC_STATUS_1_ADC_OUT_VLD_POS        4U
#define AUX_ADC_STATUS_1_ADC_OUT_VLD_RST        0x0U
#define AUX_ADC_STATUS_1_ADC_OUT_VLD_MSK        ( 0x1U << 4U )
#define AUX_ADC_STATUS_1_ADC_OUT_VLD(x)          ( ((x) & 0x1U) << 4U )

// 0x36, key_ctrl
#define KEY_CTRL_OFFSET        0x36U
#define KEY_CTRL_EN_POS        0U
#define KEY_CTRL_EN_RST        0x0U
#define KEY_CTRL_EN_MSK        ( 0x1U << 0U )
#define KEY_CTRL_EN(x)          ( ((x) & 0x1U) << 0U )
#define KEY_CTRL_INV_POS        1U
#define KEY_CTRL_INV_RST        0x1U
#define KEY_CTRL_INV_MSK        ( 0x1U << 1U )
#define KEY_CTRL_INV(x)          ( ((x) & 0x1U) << 1U )
#define KEY_CTRL_SMOOTH_EN_POS        2U
#define KEY_CTRL_SMOOTH_EN_RST        0x1U
#define KEY_CTRL_SMOOTH_EN_MSK        ( 0x1U << 2U )
#define KEY_CTRL_SMOOTH_EN(x)          ( ((x) & 0x1U) << 2U )
#define KEY_CTRL_DIN_POS        3U
#define KEY_CTRL_DIN_RST        0x0U
#define KEY_CTRL_DIN_MSK        ( 0x1U << 3U )
#define KEY_CTRL_DIN(x)          ( ((x) & 0x1U) << 3U )

// 0x37, key_smooth_high
#define KEY_SMOOTH_HIGH_OFFSET        0x37U
#define KEY_SMOOTH_HIGH_CNT_POS        0U
#define KEY_SMOOTH_HIGH_CNT_RST        0x20U
#define KEY_SMOOTH_HIGH_CNT_MSK        ( 0x3fU << 0U )
#define KEY_SMOOTH_HIGH_CNT(x)          ( ((x) & 0x3fU) << 0U )

// 0x38, key_smooth_low
#define KEY_SMOOTH_LOW_OFFSET        0x38U
#define KEY_SMOOTH_LOW_CNT_POS        0U
#define KEY_SMOOTH_LOW_CNT_RST        0x20U
#define KEY_SMOOTH_LOW_CNT_MSK        ( 0x3fU << 0U )
#define KEY_SMOOTH_LOW_CNT(x)          ( ((x) & 0x3fU) << 0U )

// 0x39, light_ctrl
#define LIGHT_CTRL_OFFSET        0x39U
#define LIGHT_CTRL_EN_POS        0U
#define LIGHT_CTRL_EN_RST        0x0U
#define LIGHT_CTRL_EN_MSK        ( 0x1U << 0U )
#define LIGHT_CTRL_EN(x)          ( ((x) & 0x1U) << 0U )
#define LIGHT_CTRL_INV_POS        1U
#define LIGHT_CTRL_INV_RST        0x0U
#define LIGHT_CTRL_INV_MSK        ( 0x1U << 1U )
#define LIGHT_CTRL_INV(x)          ( ((x) & 0x1U) << 1U )
#define LIGHT_CTRL_DR_POS        2U
#define LIGHT_CTRL_DR_RST        0x0U
#define LIGHT_CTRL_DR_MSK        ( 0x1U << 2U )
#define LIGHT_CTRL_DR(x)          ( ((x) & 0x1U) << 2U )
#define LIGHT_CTRL_LIGHT_REG_POS        3U
#define LIGHT_CTRL_LIGHT_REG_RST        0x0U
#define LIGHT_CTRL_LIGHT_REG_MSK        ( 0x1U << 3U )
#define LIGHT_CTRL_LIGHT_REG(x)          ( ((x) & 0x1U) << 3U )
#define LIGHT_CTRL_SMOOTH_LEN_POS        4U
#define LIGHT_CTRL_SMOOTH_LEN_RST        0x4U
#define LIGHT_CTRL_SMOOTH_LEN_MSK        ( 0xfU << 4U )
#define LIGHT_CTRL_SMOOTH_LEN(x)          ( ((x) & 0xfU) << 4U )

// 0x3a, light_th_low_l
#define LIGHT_TH_LOW_L_OFFSET        0x3aU
#define LIGHT_TH_LOW_L_TH_POS        0U
#define LIGHT_TH_LOW_L_TH_RST        0x00U
#define LIGHT_TH_LOW_L_TH_MSK        ( 0xffU << 0U )
#define LIGHT_TH_LOW_L_TH(x)          ( ((x) & 0xffU) << 0U )

// 0x3b, light_th_high_l
#define LIGHT_TH_HIGH_L_OFFSET        0x3bU
#define LIGHT_TH_HIGH_L_TH_POS        0U
#define LIGHT_TH_HIGH_L_TH_RST        0x00U
#define LIGHT_TH_HIGH_L_TH_MSK        ( 0xffU << 0U )
#define LIGHT_TH_HIGH_L_TH(x)          ( ((x) & 0xffU) << 0U )

// 0x3c, light_th_h
#define LIGHT_TH_H_OFFSET        0x3cU
#define LIGHT_TH_H_LOW_H_POS        0U
#define LIGHT_TH_H_LOW_H_RST        0x6U
#define LIGHT_TH_H_LOW_H_MSK        ( 0xfU << 0U )
#define LIGHT_TH_H_LOW_H(x)          ( ((x) & 0xfU) << 0U )
#define LIGHT_TH_H_HIGH_H_POS        4U
#define LIGHT_TH_H_HIGH_H_RST        0x8U
#define LIGHT_TH_H_HIGH_H_MSK        ( 0xfU << 4U )
#define LIGHT_TH_H_HIGH_H(x)          ( ((x) & 0xfU) << 4U )

// 0x3d, light_status
#define LIGHT_STATUS_OFFSET        0x3dU
#define LIGHT_STATUS_LIGHT_IMM_POS        0U
#define LIGHT_STATUS_LIGHT_IMM_RST        0x0U
#define LIGHT_STATUS_LIGHT_IMM_MSK        ( 0x1U << 0U )
#define LIGHT_STATUS_LIGHT_IMM(x)          ( ((x) & 0x1U) << 0U )
#define LIGHT_STATUS_LIGHT_SMOOTH_POS        1U
#define LIGHT_STATUS_LIGHT_SMOOTH_RST        0x0U
#define LIGHT_STATUS_LIGHT_SMOOTH_MSK        ( 0x1U << 1U )
#define LIGHT_STATUS_LIGHT_SMOOTH(x)          ( ((x) & 0x1U) << 1U )

// 0x3e, time_light_sleep_0
#define TIME_LIGHT_SLEEP_0_OFFSET        0x3eU
#define TIME_LIGHT_SLEEP_0_CNT_POS        0U
#define TIME_LIGHT_SLEEP_0_CNT_RST        0x40U
#define TIME_LIGHT_SLEEP_0_CNT_MSK        ( 0xffU << 0U )
#define TIME_LIGHT_SLEEP_0_CNT(x)          ( ((x) & 0xffU) << 0U )

// 0x3f, time_light_sleep_1
#define TIME_LIGHT_SLEEP_1_OFFSET        0x3fU
#define TIME_LIGHT_SLEEP_1_CNT_POS        0U
#define TIME_LIGHT_SLEEP_1_CNT_RST        0x06U
#define TIME_LIGHT_SLEEP_1_CNT_MSK        ( 0xffU << 0U )
#define TIME_LIGHT_SLEEP_1_CNT(x)          ( ((x) & 0xffU) << 0U )

// 0x40, time_light_sleep_2
#define TIME_LIGHT_SLEEP_2_OFFSET        0x40U
#define TIME_LIGHT_SLEEP_2_CNT_POS        0U
#define TIME_LIGHT_SLEEP_2_CNT_RST        0x0U
#define TIME_LIGHT_SLEEP_2_CNT_MSK        ( 0xffU << 0U )
#define TIME_LIGHT_SLEEP_2_CNT(x)          ( ((x) & 0xffU) << 0U )

// 0x41, time_light_sleep_3
#define TIME_LIGHT_SLEEP_3_OFFSET        0x41U
#define TIME_LIGHT_SLEEP_3_CNT_POS        0U
#define TIME_LIGHT_SLEEP_3_CNT_RST        0x0U
#define TIME_LIGHT_SLEEP_3_CNT_MSK        ( 0xffU << 0U )
#define TIME_LIGHT_SLEEP_3_CNT(x)          ( ((x) & 0xffU) << 0U )

// 0x42, ana_irq_clr
#define ANA_IRQ_CLR_OFFSET        0x42U
#define ANA_IRQ_CLR_EFUSE_WR_POS        0U
#define ANA_IRQ_CLR_EFUSE_WR_RST        0x0U
#define ANA_IRQ_CLR_EFUSE_WR_MSK        ( 0x1U << 0U )
#define ANA_IRQ_CLR_EFUSE_WR(x)          ( ((x) & 0x1U) << 0U )
#define ANA_IRQ_CLR_EFUSE_RD_POS        1U
#define ANA_IRQ_CLR_EFUSE_RD_RST        0x0U
#define ANA_IRQ_CLR_EFUSE_RD_MSK        ( 0x1U << 1U )
#define ANA_IRQ_CLR_EFUSE_RD(x)          ( ((x) & 0x1U) << 1U )
#define ANA_IRQ_CLR_PLL_LOCK_POS        2U
#define ANA_IRQ_CLR_PLL_LOCK_RST        0x0U
#define ANA_IRQ_CLR_PLL_LOCK_MSK        ( 0x1U << 2U )
#define ANA_IRQ_CLR_PLL_LOCK(x)          ( ((x) & 0x1U) << 2U )
#define ANA_IRQ_CLR_PLL_UNLOCK_POS        3U
#define ANA_IRQ_CLR_PLL_UNLOCK_RST        0x0U
#define ANA_IRQ_CLR_PLL_UNLOCK_MSK        ( 0x1U << 3U )
#define ANA_IRQ_CLR_PLL_UNLOCK(x)          ( ((x) & 0x1U) << 3U )
#define ANA_IRQ_CLR_AUX_ADC_POS        4U
#define ANA_IRQ_CLR_AUX_ADC_RST        0x0U
#define ANA_IRQ_CLR_AUX_ADC_MSK        ( 0x1U << 4U )
#define ANA_IRQ_CLR_AUX_ADC(x)          ( ((x) & 0x1U) << 4U )
#define ANA_IRQ_CLR_KEY_POS        5U
#define ANA_IRQ_CLR_KEY_RST        0x0U
#define ANA_IRQ_CLR_KEY_MSK        ( 0x1U << 5U )
#define ANA_IRQ_CLR_KEY(x)          ( ((x) & 0x1U) << 5U )
#define ANA_IRQ_CLR_LOW_BAT_POS        6U
#define ANA_IRQ_CLR_LOW_BAT_RST        0x0U
#define ANA_IRQ_CLR_LOW_BAT_MSK        ( 0x1U << 6U )
#define ANA_IRQ_CLR_LOW_BAT(x)          ( ((x) & 0x1U) << 6U )
#define ANA_IRQ_CLR_BOR_POS        7U
#define ANA_IRQ_CLR_BOR_RST        0x0U
#define ANA_IRQ_CLR_BOR_MSK        ( 0x1U << 7U )
#define ANA_IRQ_CLR_BOR(x)          ( ((x) & 0x1U) << 7U )

// 0x43, ana_irq_msk
#define ANA_IRQ_MSK_OFFSET        0x43U
#define ANA_IRQ_MSK_EFUSE_WR_POS        0U
#define ANA_IRQ_MSK_EFUSE_WR_RST        0x1U
#define ANA_IRQ_MSK_EFUSE_WR_MSK        ( 0x1U << 0U )
#define ANA_IRQ_MSK_EFUSE_WR(x)          ( ((x) & 0x1U) << 0U )
#define ANA_IRQ_MSK_EFUSE_RD_POS        1U
#define ANA_IRQ_MSK_EFUSE_RD_RST        0x1U
#define ANA_IRQ_MSK_EFUSE_RD_MSK        ( 0x1U << 1U )
#define ANA_IRQ_MSK_EFUSE_RD(x)          ( ((x) & 0x1U) << 1U )
#define ANA_IRQ_MSK_PLL_LOCK_POS        2U
#define ANA_IRQ_MSK_PLL_LOCK_RST        0x1U
#define ANA_IRQ_MSK_PLL_LOCK_MSK        ( 0x1U << 2U )
#define ANA_IRQ_MSK_PLL_LOCK(x)          ( ((x) & 0x1U) << 2U )
#define ANA_IRQ_MSK_PLL_UNLOCK_POS        3U
#define ANA_IRQ_MSK_PLL_UNLOCK_RST        0x1U
#define ANA_IRQ_MSK_PLL_UNLOCK_MSK        ( 0x1U << 3U )
#define ANA_IRQ_MSK_PLL_UNLOCK(x)          ( ((x) & 0x1U) << 3U )
#define ANA_IRQ_MSK_AUX_ADC_POS        4U
#define ANA_IRQ_MSK_AUX_ADC_RST        0x1U
#define ANA_IRQ_MSK_AUX_ADC_MSK        ( 0x1U << 4U )
#define ANA_IRQ_MSK_AUX_ADC(x)          ( ((x) & 0x1U) << 4U )
#define ANA_IRQ_MSK_KEY_POS        5U
#define ANA_IRQ_MSK_KEY_RST        0x1U
#define ANA_IRQ_MSK_KEY_MSK        ( 0x1U << 5U )
#define ANA_IRQ_MSK_KEY(x)          ( ((x) & 0x1U) << 5U )
#define ANA_IRQ_MSK_LOW_BAT_POS        6U
#define ANA_IRQ_MSK_LOW_BAT_RST        0x1U
#define ANA_IRQ_MSK_LOW_BAT_MSK        ( 0x1U << 6U )
#define ANA_IRQ_MSK_LOW_BAT(x)          ( ((x) & 0x1U) << 6U )
#define ANA_IRQ_MSK_BOR_POS        7U
#define ANA_IRQ_MSK_BOR_RST        0x1U
#define ANA_IRQ_MSK_BOR_MSK        ( 0x1U << 7U )
#define ANA_IRQ_MSK_BOR(x)          ( ((x) & 0x1U) << 7U )

// 0x44, ana_irq_status
#define ANA_IRQ_STATUS_OFFSET        0x44U
#define ANA_IRQ_STATUS_EFUSE_WR_IRQ_POS        0U
#define ANA_IRQ_STATUS_EFUSE_WR_IRQ_RST        0x0U
#define ANA_IRQ_STATUS_EFUSE_WR_IRQ_MSK        ( 0x1U << 0U )
#define ANA_IRQ_STATUS_EFUSE_WR_IRQ(x)          ( ((x) & 0x1U) << 0U )
#define ANA_IRQ_STATUS_EFUSE_RD_IRQ_POS        1U
#define ANA_IRQ_STATUS_EFUSE_RD_IRQ_RST        0x0U
#define ANA_IRQ_STATUS_EFUSE_RD_IRQ_MSK        ( 0x1U << 1U )
#define ANA_IRQ_STATUS_EFUSE_RD_IRQ(x)          ( ((x) & 0x1U) << 1U )
#define ANA_IRQ_STATUS_PLL_LOCK_IRQ_POS        2U
#define ANA_IRQ_STATUS_PLL_LOCK_IRQ_RST        0x0U
#define ANA_IRQ_STATUS_PLL_LOCK_IRQ_MSK        ( 0x1U << 2U )
#define ANA_IRQ_STATUS_PLL_LOCK_IRQ(x)          ( ((x) & 0x1U) << 2U )
#define ANA_IRQ_STATUS_PLL_UNLOCK_IRQ_POS        3U
#define ANA_IRQ_STATUS_PLL_UNLOCK_IRQ_RST        0x0U
#define ANA_IRQ_STATUS_PLL_UNLOCK_IRQ_MSK        ( 0x1U << 3U )
#define ANA_IRQ_STATUS_PLL_UNLOCK_IRQ(x)          ( ((x) & 0x1U) << 3U )
#define ANA_IRQ_STATUS_AUX_ADC_IRQ_POS        4U
#define ANA_IRQ_STATUS_AUX_ADC_IRQ_RST        0x0U
#define ANA_IRQ_STATUS_AUX_ADC_IRQ_MSK        ( 0x1U << 4U )
#define ANA_IRQ_STATUS_AUX_ADC_IRQ(x)          ( ((x) & 0x1U) << 4U )
#define ANA_IRQ_STATUS_KEY_IRQ_POS        5U
#define ANA_IRQ_STATUS_KEY_IRQ_RST        0x0U
#define ANA_IRQ_STATUS_KEY_IRQ_MSK        ( 0x1U << 5U )
#define ANA_IRQ_STATUS_KEY_IRQ(x)          ( ((x) & 0x1U) << 5U )
#define ANA_IRQ_STATUS_LOW_BAT_IRQ_POS        6U
#define ANA_IRQ_STATUS_LOW_BAT_IRQ_RST        0x0U
#define ANA_IRQ_STATUS_LOW_BAT_IRQ_MSK        ( 0x1U << 6U )
#define ANA_IRQ_STATUS_LOW_BAT_IRQ(x)          ( ((x) & 0x1U) << 6U )
#define ANA_IRQ_STATUS_BOR_IRQ_POS        7U
#define ANA_IRQ_STATUS_BOR_IRQ_RST        0x0U
#define ANA_IRQ_STATUS_BOR_IRQ_MSK        ( 0x1U << 7U )
#define ANA_IRQ_STATUS_BOR_IRQ(x)          ( ((x) & 0x1U) << 7U )

// 0x45, iomux_ctrl
#define IOMUX_CTRL_OFFSET        0x45U
#define IOMUX_CTRL_SPI_SEL_POS        0U
#define IOMUX_CTRL_SPI_SEL_RST        0x1U
#define IOMUX_CTRL_SPI_SEL_MSK        ( 0x1U << 0U )
#define IOMUX_CTRL_SPI_SEL(x)          ( ((x) & 0x1U) << 0U )

// 0x46, iomux_sel_0
#define IOMUX_SEL_0_OFFSET        0x46U
#define IOMUX_SEL_0_SEL_0_POS        0U
#define IOMUX_SEL_0_SEL_0_RST        0x0U
#define IOMUX_SEL_0_SEL_0_MSK        ( 0x3U << 0U )
#define IOMUX_SEL_0_SEL_0(x)          ( ((x) & 0x3U) << 0U )
#define IOMUX_SEL_0_SEL_1_POS        2U
#define IOMUX_SEL_0_SEL_1_RST        0x0U
#define IOMUX_SEL_0_SEL_1_MSK        ( 0x3U << 2U )
#define IOMUX_SEL_0_SEL_1(x)          ( ((x) & 0x3U) << 2U )
#define IOMUX_SEL_0_SEL_2_POS        4U
#define IOMUX_SEL_0_SEL_2_RST        0x0U
#define IOMUX_SEL_0_SEL_2_MSK        ( 0x3U << 4U )
#define IOMUX_SEL_0_SEL_2(x)          ( ((x) & 0x3U) << 4U )
#define IOMUX_SEL_0_SEL_3_POS        6U
#define IOMUX_SEL_0_SEL_3_RST        0x0U
#define IOMUX_SEL_0_SEL_3_MSK        ( 0x3U << 6U )
#define IOMUX_SEL_0_SEL_3(x)          ( ((x) & 0x3U) << 6U )

// 0x47, iomux_sel_1
#define IOMUX_SEL_1_OFFSET        0x47U
#define IOMUX_SEL_1_SEL_4_POS        0U
#define IOMUX_SEL_1_SEL_4_RST        0x0U
#define IOMUX_SEL_1_SEL_4_MSK        ( 0x3U << 0U )
#define IOMUX_SEL_1_SEL_4(x)          ( ((x) & 0x3U) << 0U )
#define IOMUX_SEL_1_SEL_5_POS        2U
#define IOMUX_SEL_1_SEL_5_RST        0x0U
#define IOMUX_SEL_1_SEL_5_MSK        ( 0x3U << 2U )
#define IOMUX_SEL_1_SEL_5(x)          ( ((x) & 0x3U) << 2U )
#define IOMUX_SEL_1_SEL_6_POS        4U
#define IOMUX_SEL_1_SEL_6_RST        0x0U
#define IOMUX_SEL_1_SEL_6_MSK        ( 0x3U << 4U )
#define IOMUX_SEL_1_SEL_6(x)          ( ((x) & 0x3U) << 4U )
#define IOMUX_SEL_1_SEL_7_POS        6U
#define IOMUX_SEL_1_SEL_7_RST        0x0U
#define IOMUX_SEL_1_SEL_7_MSK        ( 0x3U << 6U )
#define IOMUX_SEL_1_SEL_7(x)          ( ((x) & 0x3U) << 6U )

// 0x48, iomux_sel_2
#define IOMUX_SEL_2_OFFSET        0x48U
#define IOMUX_SEL_2_SEL_8_POS        0U
#define IOMUX_SEL_2_SEL_8_RST        0x0U
#define IOMUX_SEL_2_SEL_8_MSK        ( 0x3U << 0U )
#define IOMUX_SEL_2_SEL_8(x)          ( ((x) & 0x3U) << 0U )
#define IOMUX_SEL_2_SEL_9_POS        2U
#define IOMUX_SEL_2_SEL_9_RST        0x0U
#define IOMUX_SEL_2_SEL_9_MSK        ( 0x3U << 2U )
#define IOMUX_SEL_2_SEL_9(x)          ( ((x) & 0x3U) << 2U )
#define IOMUX_SEL_2_SEL_10_POS        4U
#define IOMUX_SEL_2_SEL_10_RST        0x0U
#define IOMUX_SEL_2_SEL_10_MSK        ( 0x3U << 4U )
#define IOMUX_SEL_2_SEL_10(x)          ( ((x) & 0x3U) << 4U )
#define IOMUX_SEL_2_SEL_11_POS        6U
#define IOMUX_SEL_2_SEL_11_RST        0x0U
#define IOMUX_SEL_2_SEL_11_MSK        ( 0x3U << 6U )
#define IOMUX_SEL_2_SEL_11(x)          ( ((x) & 0x3U) << 6U )

// 0x49, iomux_sel_3
#define IOMUX_SEL_3_OFFSET        0x49U
#define IOMUX_SEL_3_SEL_12_POS        0U
#define IOMUX_SEL_3_SEL_12_RST        0x0U
#define IOMUX_SEL_3_SEL_12_MSK        ( 0x3U << 0U )
#define IOMUX_SEL_3_SEL_12(x)          ( ((x) & 0x3U) << 0U )
#define IOMUX_SEL_3_SEL_13_POS        2U
#define IOMUX_SEL_3_SEL_13_RST        0x0U
#define IOMUX_SEL_3_SEL_13_MSK        ( 0x3U << 2U )
#define IOMUX_SEL_3_SEL_13(x)          ( ((x) & 0x3U) << 2U )
#define IOMUX_SEL_3_SEL_14_POS        4U
#define IOMUX_SEL_3_SEL_14_RST        0x0U
#define IOMUX_SEL_3_SEL_14_MSK        ( 0x3U << 4U )
#define IOMUX_SEL_3_SEL_14(x)          ( ((x) & 0x3U) << 4U )
#define IOMUX_SEL_3_SEL_15_POS        6U
#define IOMUX_SEL_3_SEL_15_RST        0x0U
#define IOMUX_SEL_3_SEL_15_MSK        ( 0x3U << 6U )
#define IOMUX_SEL_3_SEL_15(x)          ( ((x) & 0x3U) << 6U )

// 0x4a, gpio_aon_sel
#define GPIO_AON_SEL_OFFSET        0x4aU
#define GPIO_AON_SEL_SELECT_POS        0U
#define GPIO_AON_SEL_SELECT_RST        0x7U
#define GPIO_AON_SEL_SELECT_MSK        ( 0x7fU << 0U )
#define GPIO_AON_SEL_SELECT(x)          ( ((x) & 0x7fU) << 0U )

// 0x4b, debug_aon_en
#define DEBUG_AON_EN_OFFSET        0x4bU
#define DEBUG_AON_EN_EN_POS        0U
#define DEBUG_AON_EN_EN_RST        0x0U
#define DEBUG_AON_EN_EN_MSK        ( 0x7fU << 0U )
#define DEBUG_AON_EN_EN(x)          ( ((x) & 0x7fU) << 0U )

// 0x4c, debug_aon_sel
#define DEBUG_AON_SEL_OFFSET        0x4cU
#define DEBUG_AON_SEL_SELECT_POS        0U
#define DEBUG_AON_SEL_SELECT_RST        0x0U
#define DEBUG_AON_SEL_SELECT_MSK        ( 0x7U << 0U )
#define DEBUG_AON_SEL_SELECT(x)          ( ((x) & 0x7U) << 0U )

// 0x4d, gpio_ctrl
#define GPIO_CTRL_OFFSET        0x4dU
#define GPIO_CTRL_DIG_GPIO_5OR3_SW_POS        0U
#define GPIO_CTRL_DIG_GPIO_5OR3_SW_RST        0x1U
#define GPIO_CTRL_DIG_GPIO_5OR3_SW_MSK        ( 0x1U << 0U )
#define GPIO_CTRL_DIG_GPIO_5OR3_SW(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_CTRL_BY_MCU_DR_POS        1U
#define GPIO_CTRL_BY_MCU_DR_RST        0x0U
#define GPIO_CTRL_BY_MCU_DR_MSK        ( 0x1U << 1U )
#define GPIO_CTRL_BY_MCU_DR(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_CTRL_BY_MCU_REG_POS        2U
#define GPIO_CTRL_BY_MCU_REG_RST        0x0U
#define GPIO_CTRL_BY_MCU_REG_MSK        ( 0x1U << 2U )
#define GPIO_CTRL_BY_MCU_REG(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_CTRL_RVD_POS        3U
#define GPIO_CTRL_RVD_RST        0xaU
#define GPIO_CTRL_RVD_MSK        ( 0x1fU << 3U )
#define GPIO_CTRL_RVD(x)          ( ((x) & 0x1fU) << 3U )

// 0x4e, gpio_aon_0
#define GPIO_AON_0_OFFSET        0x4eU
#define GPIO_AON_0_GPIO_AON0_O_POS        0U
#define GPIO_AON_0_GPIO_AON0_O_RST        0x0U
#define GPIO_AON_0_GPIO_AON0_O_MSK        ( 0x1U << 0U )
#define GPIO_AON_0_GPIO_AON0_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_AON_0_GPIO_AON0_OE_N_POS        1U
#define GPIO_AON_0_GPIO_AON0_OE_N_RST        0x1U
#define GPIO_AON_0_GPIO_AON0_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_AON_0_GPIO_AON0_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_AON_0_GPIO_AON0_TIE_UP_POS        2U
#define GPIO_AON_0_GPIO_AON0_TIE_UP_RST        0x1U
#define GPIO_AON_0_GPIO_AON0_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_AON_0_GPIO_AON0_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_AON_0_GPIO_AON0_TIE_DN_POS        3U
#define GPIO_AON_0_GPIO_AON0_TIE_DN_RST        0x0U
#define GPIO_AON_0_GPIO_AON0_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_AON_0_GPIO_AON0_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_AON_0_GPIO_AON0_DRV_SEL_POS        4U
#define GPIO_AON_0_GPIO_AON0_DRV_SEL_RST        0x3U
#define GPIO_AON_0_GPIO_AON0_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_AON_0_GPIO_AON0_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_AON_0_GPIO_AON0_ANA_EN_POS        6U
#define GPIO_AON_0_GPIO_AON0_ANA_EN_RST        0x0U
#define GPIO_AON_0_GPIO_AON0_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_AON_0_GPIO_AON0_ANA_EN(x)          ( ((x) & 0x1U) << 6U )

// 0x4f, gpio_aon_1
#define GPIO_AON_1_OFFSET        0x4fU
#define GPIO_AON_1_GPIO_AON1_O_POS        0U
#define GPIO_AON_1_GPIO_AON1_O_RST        0x0U
#define GPIO_AON_1_GPIO_AON1_O_MSK        ( 0x1U << 0U )
#define GPIO_AON_1_GPIO_AON1_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_AON_1_GPIO_AON1_OE_N_POS        1U
#define GPIO_AON_1_GPIO_AON1_OE_N_RST        0x1U
#define GPIO_AON_1_GPIO_AON1_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_AON_1_GPIO_AON1_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_AON_1_GPIO_AON1_TIE_UP_POS        2U
#define GPIO_AON_1_GPIO_AON1_TIE_UP_RST        0x1U
#define GPIO_AON_1_GPIO_AON1_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_AON_1_GPIO_AON1_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_AON_1_GPIO_AON1_TIE_DN_POS        3U
#define GPIO_AON_1_GPIO_AON1_TIE_DN_RST        0x0U
#define GPIO_AON_1_GPIO_AON1_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_AON_1_GPIO_AON1_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_AON_1_GPIO_AON1_DRV_SEL_POS        4U
#define GPIO_AON_1_GPIO_AON1_DRV_SEL_RST        0x3U
#define GPIO_AON_1_GPIO_AON1_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_AON_1_GPIO_AON1_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_AON_1_GPIO_AON1_ANA_EN_POS        6U
#define GPIO_AON_1_GPIO_AON1_ANA_EN_RST        0x0U
#define GPIO_AON_1_GPIO_AON1_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_AON_1_GPIO_AON1_ANA_EN(x)          ( ((x) & 0x1U) << 6U )

// 0x50, gpio_aon_2
#define GPIO_AON_2_OFFSET        0x50U
#define GPIO_AON_2_GPIO_AON2_O_POS        0U
#define GPIO_AON_2_GPIO_AON2_O_RST        0x0U
#define GPIO_AON_2_GPIO_AON2_O_MSK        ( 0x1U << 0U )
#define GPIO_AON_2_GPIO_AON2_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_AON_2_GPIO_AON2_OE_N_POS        1U
#define GPIO_AON_2_GPIO_AON2_OE_N_RST        0x1U
#define GPIO_AON_2_GPIO_AON2_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_AON_2_GPIO_AON2_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_AON_2_GPIO_AON2_TIE_UP_POS        2U
#define GPIO_AON_2_GPIO_AON2_TIE_UP_RST        0x1U
#define GPIO_AON_2_GPIO_AON2_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_AON_2_GPIO_AON2_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_AON_2_GPIO_AON2_TIE_DN_POS        3U
#define GPIO_AON_2_GPIO_AON2_TIE_DN_RST        0x0U
#define GPIO_AON_2_GPIO_AON2_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_AON_2_GPIO_AON2_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_AON_2_GPIO_AON2_DRV_SEL_POS        4U
#define GPIO_AON_2_GPIO_AON2_DRV_SEL_RST        0x3U
#define GPIO_AON_2_GPIO_AON2_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_AON_2_GPIO_AON2_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_AON_2_GPIO_AON2_ANA_EN_POS        6U
#define GPIO_AON_2_GPIO_AON2_ANA_EN_RST        0x0U
#define GPIO_AON_2_GPIO_AON2_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_AON_2_GPIO_AON2_ANA_EN(x)          ( ((x) & 0x1U) << 6U )

// 0x51, gpio_aon_3
#define GPIO_AON_3_OFFSET        0x51U
#define GPIO_AON_3_GPIO_AON3_O_POS        0U
#define GPIO_AON_3_GPIO_AON3_O_RST        0x0U
#define GPIO_AON_3_GPIO_AON3_O_MSK        ( 0x1U << 0U )
#define GPIO_AON_3_GPIO_AON3_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_AON_3_GPIO_AON3_OE_N_POS        1U
#define GPIO_AON_3_GPIO_AON3_OE_N_RST        0x1U
#define GPIO_AON_3_GPIO_AON3_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_AON_3_GPIO_AON3_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_AON_3_GPIO_AON3_TIE_UP_POS        2U
#define GPIO_AON_3_GPIO_AON3_TIE_UP_RST        0x1U
#define GPIO_AON_3_GPIO_AON3_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_AON_3_GPIO_AON3_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_AON_3_GPIO_AON3_TIE_DN_POS        3U
#define GPIO_AON_3_GPIO_AON3_TIE_DN_RST        0x0U
#define GPIO_AON_3_GPIO_AON3_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_AON_3_GPIO_AON3_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_AON_3_GPIO_AON3_DRV_SEL_POS        4U
#define GPIO_AON_3_GPIO_AON3_DRV_SEL_RST        0x3U
#define GPIO_AON_3_GPIO_AON3_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_AON_3_GPIO_AON3_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_AON_3_GPIO_AON3_ANA_EN_POS        6U
#define GPIO_AON_3_GPIO_AON3_ANA_EN_RST        0x0U
#define GPIO_AON_3_GPIO_AON3_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_AON_3_GPIO_AON3_ANA_EN(x)          ( ((x) & 0x1U) << 6U )

// 0x52, gpio_aon_4
#define GPIO_AON_4_OFFSET        0x52U
#define GPIO_AON_4_GPIO_AON4_O_POS        0U
#define GPIO_AON_4_GPIO_AON4_O_RST        0x0U
#define GPIO_AON_4_GPIO_AON4_O_MSK        ( 0x1U << 0U )
#define GPIO_AON_4_GPIO_AON4_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_AON_4_GPIO_AON4_OE_N_POS        1U
#define GPIO_AON_4_GPIO_AON4_OE_N_RST        0x1U
#define GPIO_AON_4_GPIO_AON4_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_AON_4_GPIO_AON4_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_AON_4_GPIO_AON4_TIE_UP_POS        2U
#define GPIO_AON_4_GPIO_AON4_TIE_UP_RST        0x1U
#define GPIO_AON_4_GPIO_AON4_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_AON_4_GPIO_AON4_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_AON_4_GPIO_AON4_TIE_DN_POS        3U
#define GPIO_AON_4_GPIO_AON4_TIE_DN_RST        0x0U
#define GPIO_AON_4_GPIO_AON4_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_AON_4_GPIO_AON4_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_AON_4_GPIO_AON4_DRV_SEL_POS        4U
#define GPIO_AON_4_GPIO_AON4_DRV_SEL_RST        0x3U
#define GPIO_AON_4_GPIO_AON4_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_AON_4_GPIO_AON4_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_AON_4_GPIO_AON4_ANA_EN_POS        6U
#define GPIO_AON_4_GPIO_AON4_ANA_EN_RST        0x0U
#define GPIO_AON_4_GPIO_AON4_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_AON_4_GPIO_AON4_ANA_EN(x)          ( ((x) & 0x1U) << 6U )

// 0x53, gpio_aon_5
#define GPIO_AON_5_OFFSET        0x53U
#define GPIO_AON_5_GPIO_AON5_O_POS        0U
#define GPIO_AON_5_GPIO_AON5_O_RST        0x0U
#define GPIO_AON_5_GPIO_AON5_O_MSK        ( 0x1U << 0U )
#define GPIO_AON_5_GPIO_AON5_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_AON_5_GPIO_AON5_OE_N_POS        1U
#define GPIO_AON_5_GPIO_AON5_OE_N_RST        0x1U
#define GPIO_AON_5_GPIO_AON5_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_AON_5_GPIO_AON5_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_AON_5_GPIO_AON5_TIE_UP_POS        2U
#define GPIO_AON_5_GPIO_AON5_TIE_UP_RST        0x0U
#define GPIO_AON_5_GPIO_AON5_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_AON_5_GPIO_AON5_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_AON_5_GPIO_AON5_TIE_DN_POS        3U
#define GPIO_AON_5_GPIO_AON5_TIE_DN_RST        0x1U
#define GPIO_AON_5_GPIO_AON5_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_AON_5_GPIO_AON5_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_AON_5_GPIO_AON5_DRV_SEL_POS        4U
#define GPIO_AON_5_GPIO_AON5_DRV_SEL_RST        0x3U
#define GPIO_AON_5_GPIO_AON5_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_AON_5_GPIO_AON5_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_AON_5_GPIO_AON5_ANA_EN_POS        6U
#define GPIO_AON_5_GPIO_AON5_ANA_EN_RST        0x0U
#define GPIO_AON_5_GPIO_AON5_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_AON_5_GPIO_AON5_ANA_EN(x)          ( ((x) & 0x1U) << 6U )

// 0x54, gpio_aon_6
#define GPIO_AON_6_OFFSET        0x54U
#define GPIO_AON_6_GPIO_AON6_O_POS        0U
#define GPIO_AON_6_GPIO_AON6_O_RST        0x0U
#define GPIO_AON_6_GPIO_AON6_O_MSK        ( 0x1U << 0U )
#define GPIO_AON_6_GPIO_AON6_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_AON_6_GPIO_AON6_OE_N_POS        1U
#define GPIO_AON_6_GPIO_AON6_OE_N_RST        0x1U
#define GPIO_AON_6_GPIO_AON6_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_AON_6_GPIO_AON6_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_AON_6_GPIO_AON6_TIE_UP_POS        2U
#define GPIO_AON_6_GPIO_AON6_TIE_UP_RST        0x0U
#define GPIO_AON_6_GPIO_AON6_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_AON_6_GPIO_AON6_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_AON_6_GPIO_AON6_TIE_DN_POS        3U
#define GPIO_AON_6_GPIO_AON6_TIE_DN_RST        0x1U
#define GPIO_AON_6_GPIO_AON6_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_AON_6_GPIO_AON6_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_AON_6_GPIO_AON6_DRV_SEL_POS        4U
#define GPIO_AON_6_GPIO_AON6_DRV_SEL_RST        0x3U
#define GPIO_AON_6_GPIO_AON6_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_AON_6_GPIO_AON6_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_AON_6_GPIO_AON6_ANA_EN_POS        6U
#define GPIO_AON_6_GPIO_AON6_ANA_EN_RST        0x0U
#define GPIO_AON_6_GPIO_AON6_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_AON_6_GPIO_AON6_ANA_EN(x)          ( ((x) & 0x1U) << 6U )

// 0x55, gpio_aon_7
#define GPIO_AON_7_OFFSET        0x55U
#define GPIO_AON_7_GPIO_AON7_O_POS        0U
#define GPIO_AON_7_GPIO_AON7_O_RST        0x0U
#define GPIO_AON_7_GPIO_AON7_O_MSK        ( 0x1U << 0U )
#define GPIO_AON_7_GPIO_AON7_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_AON_7_GPIO_AON7_OE_N_POS        1U
#define GPIO_AON_7_GPIO_AON7_OE_N_RST        0x1U
#define GPIO_AON_7_GPIO_AON7_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_AON_7_GPIO_AON7_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_AON_7_GPIO_AON7_TIE_UP_POS        2U
#define GPIO_AON_7_GPIO_AON7_TIE_UP_RST        0x0U
#define GPIO_AON_7_GPIO_AON7_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_AON_7_GPIO_AON7_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_AON_7_GPIO_AON7_TIE_DN_POS        3U
#define GPIO_AON_7_GPIO_AON7_TIE_DN_RST        0x1U
#define GPIO_AON_7_GPIO_AON7_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_AON_7_GPIO_AON7_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_AON_7_GPIO_AON7_DRV_SEL_POS        4U
#define GPIO_AON_7_GPIO_AON7_DRV_SEL_RST        0x3U
#define GPIO_AON_7_GPIO_AON7_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_AON_7_GPIO_AON7_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_AON_7_GPIO_AON7_ANA_EN_POS        6U
#define GPIO_AON_7_GPIO_AON7_ANA_EN_RST        0x0U
#define GPIO_AON_7_GPIO_AON7_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_AON_7_GPIO_AON7_ANA_EN(x)          ( ((x) & 0x1U) << 6U )

// 0x56, gpio_aon_status
#define GPIO_AON_STATUS_OFFSET        0x56U
#define GPIO_AON_STATUS_IN_POS        0U
#define GPIO_AON_STATUS_IN_RST        0x0U
#define GPIO_AON_STATUS_IN_MSK        ( 0xffU << 0U )
#define GPIO_AON_STATUS_IN(x)          ( ((x) & 0xffU) << 0U )

#endif
