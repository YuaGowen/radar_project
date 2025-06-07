#ifndef _CHIRP_H_
#define _CHIRP_H_

#include "globals_macro.h"

#define REG_CHIRP_BASE            0x40012800

typedef struct
{
    REG32        c0;    // 0x0
    REG32        c1;    // 0x4
    REG32        c2;    // 0x8
    REG32        c3;    // 0xc
    REG32        c4;    // 0x10
    REG32        c5;    // 0x14
    REG32        c6;    // 0x18
    REG32        c7;    // 0x1c
} HWP_CHIRP_T;

#define hwp_chirp        ((HWP_CHIRP_T*)REG_CHIRP_BASE)

// 0x0, c0
#define C0_START_FREQ_POS        0
#define C0_START_FREQ_RST        0x0
#define C0_START_FREQ_MSK        ( 0xffffffff << 0 )
#define C0_START_FREQ(x)          ( (x & 0xffffffffU) << 0 )

// 0x4, c1
#define C1_RISE_STEP_TIME_POS        16
#define C1_RISE_STEP_TIME_RST        0x0
#define C1_RISE_STEP_TIME_MSK        ( 0xffff << 16 )
#define C1_RISE_STEP_TIME(x)          ( (x & 0xffff) << 16 )
#define C1_NUM_CHIRP_POS        0
#define C1_NUM_CHIRP_RST        0x0
#define C1_NUM_CHIRP_MSK        ( 0xffff << 0 )
#define C1_NUM_CHIRP(x)          ( (x & 0xffff) << 0 )

// 0x8, c2
#define C2_RISE_NUM_STEP_POS        16
#define C2_RISE_NUM_STEP_RST        0x0
#define C2_RISE_NUM_STEP_MSK        ( 0xffff << 16 )
#define C2_RISE_NUM_STEP(x)          ( (x & 0xffff) << 16 )
#define C2_RISE_STEP_FREQ_POS        0
#define C2_RISE_STEP_FREQ_RST        0x0
#define C2_RISE_STEP_FREQ_MSK        ( 0xffff << 0 )
#define C2_RISE_STEP_FREQ(x)          ( (x & 0xffff) << 0 )

// 0xc, c3
#define C3_FALL_STEP_FREQ_POS        16
#define C3_FALL_STEP_FREQ_RST        0x0
#define C3_FALL_STEP_FREQ_MSK        ( 0xffff << 16 )
#define C3_FALL_STEP_FREQ(x)          ( (x & 0xffff) << 16 )
#define C3_FALL_STEP_TIME_POS        0
#define C3_FALL_STEP_TIME_RST        0x0
#define C3_FALL_STEP_TIME_MSK        ( 0xffff << 0 )
#define C3_FALL_STEP_TIME(x)          ( (x & 0xffff) << 0 )

// 0x10, c4
#define C4_HPF_CONFIG_POS        16
#define C4_HPF_CONFIG_RST        0x0
#define C4_HPF_CONFIG_MSK        ( 0xffff << 16 )
#define C4_HPF_CONFIG(x)          ( (x & 0xffff) << 16 )
#define C4_FALL_NUM_STEP_POS        0
#define C4_FALL_NUM_STEP_RST        0x0
#define C4_FALL_NUM_STEP_MSK        ( 0xffff << 0 )
#define C4_FALL_NUM_STEP(x)          ( (x & 0xffff) << 0 )

// 0x14, c5
#define C5_FALL_PLL_BW_POS        28
#define C5_FALL_PLL_BW_RST        0x0
#define C5_FALL_PLL_BW_MSK        ( 0x7 << 28 )
#define C5_FALL_PLL_BW(x)          ( (x & 0x7) << 28 )
#define C5_RISE_PLL_BW_POS        24
#define C5_RISE_PLL_BW_RST        0x0
#define C5_RISE_PLL_BW_MSK        ( 0x7 << 24 )
#define C5_RISE_PLL_BW(x)          ( (x & 0x7) << 24 )
#define C5_IDLE_TIME_POS        0
#define C5_IDLE_TIME_RST        0x0
#define C5_IDLE_TIME_MSK        ( 0xffffff << 0 )
#define C5_IDLE_TIME(x)          ( (x & 0xffffff) << 0 )

// 0x18, c6
#define C6_RX_GAIN_POS        0
#define C6_RX_GAIN_RST        0x0
#define C6_RX_GAIN_MSK        ( 0xffffffff << 0 )
#define C6_RX_GAIN(x)          ( (x & 0xffffffff) << 0 )

// 0x1c, c7
#define C7_JUMP_NUM_STEP_POS        16
#define C7_JUMP_NUM_STEP_RST        0x0
#define C7_JUMP_NUM_STEP_MSK        ( 0xffff << 16 )
#define C7_JUMP_NUM_STEP(x)          ( (x & 0xffff) << 16 )
#define C7_JUMP_FREQ_POS        0
#define C7_JUMP_FREQ_RST        0x0
#define C7_JUMP_FREQ_MSK        ( 0xffff << 0 )
#define C7_JUMP_FREQ(x)          ( (x & 0xffff) << 0 )

#endif
