#ifndef _AON_H_
#define _AON_H_

#include "globals_macro.h"

#define REG_AON_BASE            0x40103c00

typedef struct
{
    REG8        device_id_rf;    // 0x0
    REG8        device_id_pmu;    // 0x1
    REG8        device_id_ana;    // 0x2
    REG8        device_id_aon;    // 0x3
    REG8        aon_rst_info;    // 0x4
    REG8        wdt_ctrl;    // 0x5
    REG8        wdt_rpl;    // 0x6
    REG8        wdt_to_0;    // 0x7
    REG8        wdt_to_1;    // 0x8
    REG8        wdt_to_2;    // 0x9
    REG8        wdt_to_3;    // 0xa
    REG8        wdt_cnt_0;    // 0xb
    REG8        wdt_cnt_1;    // 0xc
    REG8        wdt_cnt_2;    // 0xd
    REG8        wdt_cnt_3;    // 0xe
} HWP_AON_T;

#define hwp_aon        ((HWP_AON_T*)REG_AON_BASE)

// 0x0, device_id_rf
#define DEVICE_ID_RF_OFFSET        0x0
#define DEVICE_ID_RF_ID_POS        0
#define DEVICE_ID_RF_ID_RST        0x24
#define DEVICE_ID_RF_ID_MSK        ( 0x7f << 0 )
#define DEVICE_ID_RF_ID(x)          ( ((x) & 0x7f) << 0 )

// 0x1, device_id_pmu
#define DEVICE_ID_PMU_OFFSET        0x1
#define DEVICE_ID_PMU_ID_POS        0
#define DEVICE_ID_PMU_ID_RST        0x25
#define DEVICE_ID_PMU_ID_MSK        ( 0x7f << 0 )
#define DEVICE_ID_PMU_ID(x)          ( ((x) & 0x7f) << 0 )

// 0x2, device_id_ana
#define DEVICE_ID_ANA_OFFSET        0x2
#define DEVICE_ID_ANA_ID_POS        0
#define DEVICE_ID_ANA_ID_RST        0x26
#define DEVICE_ID_ANA_ID_MSK        ( 0x7f << 0 )
#define DEVICE_ID_ANA_ID(x)          ( ((x) & 0x7f) << 0 )

// 0x3, device_id_aon
#define DEVICE_ID_AON_OFFSET        0x3
#define DEVICE_ID_AON_ID_POS        0
#define DEVICE_ID_AON_ID_RST        0x27
#define DEVICE_ID_AON_ID_MSK        ( 0x7f << 0 )
#define DEVICE_ID_AON_ID(x)          ( ((x) & 0x7f) << 0 )

// 0x4, aon_rst_info
#define AON_RST_INFO_OFFSET        0x4
#define AON_RST_INFO_MCU_POR_POS        0
#define AON_RST_INFO_MCU_POR_RST        0x0
#define AON_RST_INFO_MCU_POR_MSK        ( 0x1 << 0 )
#define AON_RST_INFO_MCU_POR(x)          ( ((x) & 0x1) << 0 )
#define AON_RST_INFO_MCU_POR_CLR_POS        1
#define AON_RST_INFO_MCU_POR_CLR_RST        0x0
#define AON_RST_INFO_MCU_POR_CLR_MSK        ( 0x1 << 1 )
#define AON_RST_INFO_MCU_POR_CLR(x)          ( ((x) & 0x1) << 1 )
#define AON_RST_INFO_WDT_POS        2
#define AON_RST_INFO_WDT_RST        0x0
#define AON_RST_INFO_WDT_MSK        ( 0x1 << 2 )
#define AON_RST_INFO_WDT(x)          ( ((x) & 0x1) << 2 )
#define AON_RST_INFO_WDT_CLR_POS        3
#define AON_RST_INFO_WDT_CLR_RST        0x0
#define AON_RST_INFO_WDT_CLR_MSK        ( 0x1 << 3 )
#define AON_RST_INFO_WDT_CLR(x)          ( ((x) & 0x1) << 3 )

// 0x5, wdt_ctrl
#define WDT_CTRL_OFFSET        0x5
#define WDT_CTRL_EN_POS        0
#define WDT_CTRL_EN_RST        0x0
#define WDT_CTRL_EN_MSK        ( 0x1 << 0 )
#define WDT_CTRL_EN(x)          ( ((x) & 0x1) << 0 )
#define WDT_CTRL_RESTART_POS        1
#define WDT_CTRL_RESTART_RST        0x0
#define WDT_CTRL_RESTART_MSK        ( 0x1 << 1 )
#define WDT_CTRL_RESTART(x)          ( ((x) & 0x1) << 1 )
#define WDT_CTRL_EOI_EN_POS        2
#define WDT_CTRL_EOI_EN_RST        0x0
#define WDT_CTRL_EOI_EN_MSK        ( 0x1 << 2 )
#define WDT_CTRL_EOI_EN(x)          ( ((x) & 0x1) << 2 )
#define WDT_CTRL_RMOD_POS        3
#define WDT_CTRL_RMOD_RST        0x0
#define WDT_CTRL_RMOD_MSK        ( 0x1 << 3 )
#define WDT_CTRL_RMOD(x)          ( ((x) & 0x1) << 3 )
#define WDT_CTRL_SPEED_UP_POS        4
#define WDT_CTRL_SPEED_UP_RST        0x0
#define WDT_CTRL_SPEED_UP_MSK        ( 0x1 << 4 )
#define WDT_CTRL_SPEED_UP(x)          ( ((x) & 0x1) << 4 )
#define WDT_CTRL_IRQ_POS        5
#define WDT_CTRL_IRQ_RST        0x0
#define WDT_CTRL_IRQ_MSK        ( 0x1 << 5 )
#define WDT_CTRL_IRQ(x)          ( ((x) & 0x1) << 5 )
#define WDT_CTRL_SYS_RSTN_POS        6
#define WDT_CTRL_SYS_RSTN_RST        0x1
#define WDT_CTRL_SYS_RSTN_MSK        ( 0x1 << 6 )
#define WDT_CTRL_SYS_RSTN(x)          ( ((x) & 0x1) << 6 )

// 0x6, wdt_rpl
#define WDT_RPL_OFFSET        0x6
#define WDT_RPL_LEN_POS        0
#define WDT_RPL_LEN_RST        0x10
#define WDT_RPL_LEN_MSK        ( 0xff << 0 )
#define WDT_RPL_LEN(x)          ( ((x) & 0xff) << 0 )

// 0x7, wdt_to_0
#define WDT_TO_0_OFFSET        0x7
#define WDT_TO_0_CNT_POS        0
#define WDT_TO_0_CNT_RST        0xff
#define WDT_TO_0_CNT_MSK        ( 0xff << 0 )
#define WDT_TO_0_CNT(x)          ( ((x) & 0xff) << 0 )

// 0x8, wdt_to_1
#define WDT_TO_1_OFFSET        0x8
#define WDT_TO_1_CNT_POS        0
#define WDT_TO_1_CNT_RST        0xff
#define WDT_TO_1_CNT_MSK        ( 0xff << 0 )
#define WDT_TO_1_CNT(x)          ( ((x) & 0xff) << 0 )

// 0x9, wdt_to_2
#define WDT_TO_2_OFFSET        0x9
#define WDT_TO_2_CNT_POS        0
#define WDT_TO_2_CNT_RST        0x0
#define WDT_TO_2_CNT_MSK        ( 0xff << 0 )
#define WDT_TO_2_CNT(x)          ( ((x) & 0xff) << 0 )

// 0xa, wdt_to_3
#define WDT_TO_3_OFFSET        0xa
#define WDT_TO_3_CNT_POS        0
#define WDT_TO_3_CNT_RST        0x0
#define WDT_TO_3_CNT_MSK        ( 0xff << 0 )
#define WDT_TO_3_CNT(x)          ( ((x) & 0xff) << 0 )

// 0xb, wdt_cnt_0
#define WDT_CNT_0_OFFSET        0xb
#define WDT_CNT_0_CNT_POS        0
#define WDT_CNT_0_CNT_RST        0x0
#define WDT_CNT_0_CNT_MSK        ( 0xff << 0 )
#define WDT_CNT_0_CNT(x)          ( ((x) & 0xff) << 0 )

// 0xc, wdt_cnt_1
#define WDT_CNT_1_OFFSET        0xc
#define WDT_CNT_1_CNT_POS        0
#define WDT_CNT_1_CNT_RST        0x0
#define WDT_CNT_1_CNT_MSK        ( 0xff << 0 )
#define WDT_CNT_1_CNT(x)          ( ((x) & 0xff) << 0 )

// 0xd, wdt_cnt_2
#define WDT_CNT_2_OFFSET        0xd
#define WDT_CNT_2_CNT_POS        0
#define WDT_CNT_2_CNT_RST        0x0
#define WDT_CNT_2_CNT_MSK        ( 0xff << 0 )
#define WDT_CNT_2_CNT(x)          ( ((x) & 0xff) << 0 )

// 0xe, wdt_cnt_3
#define WDT_CNT_3_OFFSET        0xe
#define WDT_CNT_3_CNT_POS        0
#define WDT_CNT_3_CNT_RST        0x0
#define WDT_CNT_3_CNT_MSK        ( 0xff << 0 )
#define WDT_CNT_3_CNT(x)          ( ((x) & 0xff) << 0 )

#endif
