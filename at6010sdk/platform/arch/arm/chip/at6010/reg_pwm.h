#ifndef _PWM_H_
#define _PWM_H_

#include "globals_macro.h"

#define REG_PWM_BASE            0x4000a000U

typedef struct
{
    REG32        pwt0_config;    // 0x0U
    REG32        pwt1_config;    // 0x4U
    REG32        pwt2_config;    // 0x8U
    REG32        pwt3_config;    // 0xcU
    REG32        pwt4_config;    // 0x10U
    REG32        pwt_config;    // 0x14U
} HWP_PWM_T;

#define hwp_pwm        ((HWP_PWM_T*)REG_PWM_BASE)

// 0x0U, pwt0_config
#define PWT0_CONFIG_OFFSET        0x0U
#define PWT0_CONFIG_EN_POS        0U
#define PWT0_CONFIG_EN_RST        0x0U
#define PWT0_CONFIG_EN_MSK        ( 0x1U << 0U )
#define PWT0_CONFIG_EN(x)          ( ((x) & 0x1U) << 0U )
#define PWT0_CONFIG_START_POS        1U
#define PWT0_CONFIG_START_RST        0x0U
#define PWT0_CONFIG_START_MSK        ( 0x1U << 1U )
#define PWT0_CONFIG_START(x)          ( ((x) & 0x1U) << 1U )
#define PWT0_CONFIG_INVERT_POS        2U
#define PWT0_CONFIG_INVERT_RST        0x0U
#define PWT0_CONFIG_INVERT_MSK        ( 0x1U << 2U )
#define PWT0_CONFIG_INVERT(x)          ( ((x) & 0x1U) << 2U )
#define PWT0_CONFIG_DUTY_POS        3U
#define PWT0_CONFIG_DUTY_RST        0x1fffU
#define PWT0_CONFIG_DUTY_MSK        ( 0x1fffU << 3U )
#define PWT0_CONFIG_DUTY(x)          ( ((x) & 0x1fffU) << 3U )
#define PWT0_CONFIG_PERIOD_POS        16U
#define PWT0_CONFIG_PERIOD_RST        0x3fffU
#define PWT0_CONFIG_PERIOD_MSK        ( 0x3fffU << 16U )
#define PWT0_CONFIG_PERIOD(x)          ( ((x) & 0x3fffU) << 16U )
#define PWT0_CONFIG_HOLD_POS        30U
#define PWT0_CONFIG_HOLD_RST        0x0U
#define PWT0_CONFIG_HOLD_MSK        ( 0x1U << 30U )
#define PWT0_CONFIG_HOLD(x)          ( ((x) & 0x1U) << 30U )

// 0x4U, pwt1_config
#define PWT1_CONFIG_OFFSET        0x4U
#define PWT1_CONFIG_EN_POS        0U
#define PWT1_CONFIG_EN_RST        0x0U
#define PWT1_CONFIG_EN_MSK        ( 0x1U << 0U )
#define PWT1_CONFIG_EN(x)          ( ((x) & 0x1U) << 0U )
#define PWT1_CONFIG_START_POS        1U
#define PWT1_CONFIG_START_RST        0x0U
#define PWT1_CONFIG_START_MSK        ( 0x1U << 1U )
#define PWT1_CONFIG_START(x)          ( ((x) & 0x1U) << 1U )
#define PWT1_CONFIG_INVERT_POS        2U
#define PWT1_CONFIG_INVERT_RST        0x0U
#define PWT1_CONFIG_INVERT_MSK        ( 0x1U << 2U )
#define PWT1_CONFIG_INVERT(x)          ( ((x) & 0x1U) << 2U )
#define PWT1_CONFIG_DUTY_POS        3U
#define PWT1_CONFIG_DUTY_RST        0x1fffU
#define PWT1_CONFIG_DUTY_MSK        ( 0x1fffU << 3U )
#define PWT1_CONFIG_DUTY(x)          ( ((x) & 0x1fffU) << 3U )
#define PWT1_CONFIG_PERIOD_POS        16U
#define PWT1_CONFIG_PERIOD_RST        0x3fffU
#define PWT1_CONFIG_PERIOD_MSK        ( 0x3fffU << 16U )
#define PWT1_CONFIG_PERIOD(x)          ( ((x) & 0x3fffU) << 16U )
#define PWT1_CONFIG_HOLD_POS        30U
#define PWT1_CONFIG_HOLD_RST        0x0U
#define PWT1_CONFIG_HOLD_MSK        ( 0x1U << 30U )
#define PWT1_CONFIG_HOLD(x)          ( ((x) & 0x1U) << 30U )

// 0x8U, pwt2_config
#define PWT2_CONFIG_OFFSET        0x8U
#define PWT2_CONFIG_EN_POS        0U
#define PWT2_CONFIG_EN_RST        0x0U
#define PWT2_CONFIG_EN_MSK        ( 0x1U << 0U )
#define PWT2_CONFIG_EN(x)          ( ((x) & 0x1U) << 0U )
#define PWT2_CONFIG_START_POS        1U
#define PWT2_CONFIG_START_RST        0x0U
#define PWT2_CONFIG_START_MSK        ( 0x1U << 1U )
#define PWT2_CONFIG_START(x)          ( ((x) & 0x1U) << 1U )
#define PWT2_CONFIG_INVERT_POS        2U
#define PWT2_CONFIG_INVERT_RST        0x0U
#define PWT2_CONFIG_INVERT_MSK        ( 0x1U << 2U )
#define PWT2_CONFIG_INVERT(x)          ( ((x) & 0x1U) << 2U )
#define PWT2_CONFIG_DUTY_POS        3U
#define PWT2_CONFIG_DUTY_RST        0x1fffU
#define PWT2_CONFIG_DUTY_MSK        ( 0x1fffU << 3U )
#define PWT2_CONFIG_DUTY(x)          ( ((x) & 0x1fffU) << 3U )
#define PWT2_CONFIG_PERIOD_POS        16U
#define PWT2_CONFIG_PERIOD_RST        0x3fffU
#define PWT2_CONFIG_PERIOD_MSK        ( 0x3fffU << 16U )
#define PWT2_CONFIG_PERIOD(x)          ( ((x) & 0x3fffU) << 16U )
#define PWT2_CONFIG_HOLD_POS        30U
#define PWT2_CONFIG_HOLD_RST        0x0U
#define PWT2_CONFIG_HOLD_MSK        ( 0x1U << 30U )
#define PWT2_CONFIG_HOLD(x)          ( ((x) & 0x1U) << 30U )

// 0xcU, pwt3_config
#define PWT3_CONFIG_OFFSET        0xcU
#define PWT3_CONFIG_EN_POS        0U
#define PWT3_CONFIG_EN_RST        0x0U
#define PWT3_CONFIG_EN_MSK        ( 0x1U << 0U )
#define PWT3_CONFIG_EN(x)          ( ((x) & 0x1U) << 0U )
#define PWT3_CONFIG_START_POS        1U
#define PWT3_CONFIG_START_RST        0x0U
#define PWT3_CONFIG_START_MSK        ( 0x1U << 1U )
#define PWT3_CONFIG_START(x)          ( ((x) & 0x1U) << 1U )
#define PWT3_CONFIG_INVERT_POS        2U
#define PWT3_CONFIG_INVERT_RST        0x0U
#define PWT3_CONFIG_INVERT_MSK        ( 0x1U << 2U )
#define PWT3_CONFIG_INVERT(x)          ( ((x) & 0x1U) << 2U )
#define PWT3_CONFIG_DUTY_POS        3U
#define PWT3_CONFIG_DUTY_RST        0x1fffU
#define PWT3_CONFIG_DUTY_MSK        ( 0x1fffU << 3U )
#define PWT3_CONFIG_DUTY(x)          ( ((x) & 0x1fffU) << 3U )
#define PWT3_CONFIG_PERIOD_POS        16U
#define PWT3_CONFIG_PERIOD_RST        0x3fffU
#define PWT3_CONFIG_PERIOD_MSK        ( 0x3fffU << 16U )
#define PWT3_CONFIG_PERIOD(x)          ( ((x) & 0x3fffU) << 16U )
#define PWT3_CONFIG_HOLD_POS        30U
#define PWT3_CONFIG_HOLD_RST        0x0U
#define PWT3_CONFIG_HOLD_MSK        ( 0x1U << 30U )
#define PWT3_CONFIG_HOLD(x)          ( ((x) & 0x1U) << 30U )

// 0x10U, pwt4_config
#define PWT4_CONFIG_OFFSET        0x10U
#define PWT4_CONFIG_EN_POS        0U
#define PWT4_CONFIG_EN_RST        0x0U
#define PWT4_CONFIG_EN_MSK        ( 0x1U << 0U )
#define PWT4_CONFIG_EN(x)          ( ((x) & 0x1U) << 0U )
#define PWT4_CONFIG_START_POS        1U
#define PWT4_CONFIG_START_RST        0x0U
#define PWT4_CONFIG_START_MSK        ( 0x1U << 1U )
#define PWT4_CONFIG_START(x)          ( ((x) & 0x1U) << 1U )
#define PWT4_CONFIG_INVERT_POS        2U
#define PWT4_CONFIG_INVERT_RST        0x0U
#define PWT4_CONFIG_INVERT_MSK        ( 0x1U << 2U )
#define PWT4_CONFIG_INVERT(x)          ( ((x) & 0x1U) << 2U )
#define PWT4_CONFIG_DUTY_POS        3U
#define PWT4_CONFIG_DUTY_RST        0x1fffU
#define PWT4_CONFIG_DUTY_MSK        ( 0x1fffU << 3U )
#define PWT4_CONFIG_DUTY(x)          ( ((x) & 0x1fffU) << 3U )
#define PWT4_CONFIG_PERIOD_POS        16U
#define PWT4_CONFIG_PERIOD_RST        0x3fffU
#define PWT4_CONFIG_PERIOD_MSK        ( 0x3fffU << 16U )
#define PWT4_CONFIG_PERIOD(x)          ( ((x) & 0x3fffU) << 16U )
#define PWT4_CONFIG_HOLD_POS        30U
#define PWT4_CONFIG_HOLD_RST        0x0U
#define PWT4_CONFIG_HOLD_MSK        ( 0x1U << 30U )
#define PWT4_CONFIG_HOLD(x)          ( ((x) & 0x1U) << 30U )

// 0x14U, pwt_config
#define PWT_CONFIG_OFFSET        0x14U
#define PWT_CONFIG_DIV_POS        0U
#define PWT_CONFIG_DIV_RST        0x0U
#define PWT_CONFIG_DIV_MSK        ( 0xffU << 0U )
#define PWT_CONFIG_DIV(x)          ( ((x) & 0xffU) << 0U )
#define PWT_CONFIG_DIV_EN_POS        8U
#define PWT_CONFIG_DIV_EN_RST        0x0U
#define PWT_CONFIG_DIV_EN_MSK        ( 0x1U << 8U )
#define PWT_CONFIG_DIV_EN(x)          ( ((x) & 0x1U) << 8U )

#endif
