#ifndef _I2C_H_
#define _I2C_H_

#include "globals_macro.h"

#define REG_I2C_BASE            0x40009000U

typedef struct
{
    REG32        ctrl_1;    // 0x0U
    REG32        ctrl_2;    // 0x4U
    REG32        cmd;    // 0x8U
    REG32        status;    // 0xcU
} HWP_I2C_T;

#define hwp_i2c        ((HWP_I2C_T*)REG_I2C_BASE)

// 0x0U, ctrl_1
#define CTRL_1_OFFSET        0x0U
#define CTRL_1_EN_POS        0U
#define CTRL_1_EN_RST        0x0U
#define CTRL_1_EN_MSK        ( 0x1U << 0U )
#define CTRL_1_EN(x)          ( ((x) & 0x1U) << 0U )
#define CTRL_1_RST_POS        1U
#define CTRL_1_RST_RST        0x0U
#define CTRL_1_RST_MSK        ( 0x1U << 1U )
#define CTRL_1_RST(x)          ( ((x) & 0x1U) << 1U )
#define CTRL_1_MASTER_POS        2U
#define CTRL_1_MASTER_RST        0x0U
#define CTRL_1_MASTER_MSK        ( 0x1U << 2U )
#define CTRL_1_MASTER(x)          ( ((x) & 0x1U) << 2U )
#define CTRL_1_IRQ_EN_POS        5U
#define CTRL_1_IRQ_EN_RST        0x0U
#define CTRL_1_IRQ_EN_MSK        ( 0x1U << 5U )
#define CTRL_1_IRQ_EN(x)          ( ((x) & 0x1U) << 5U )
#define CTRL_1_IRQ_CLR_POS        6U
#define CTRL_1_IRQ_CLR_RST        0x0U
#define CTRL_1_IRQ_CLR_MSK        ( 0x1U << 6U )
#define CTRL_1_IRQ_CLR(x)          ( ((x) & 0x1U) << 6U )
#define CTRL_1_SCL_I_SEL_POS        9U
#define CTRL_1_SCL_I_SEL_RST        0x0U
#define CTRL_1_SCL_I_SEL_MSK        ( 0x7U << 9U )
#define CTRL_1_SCL_I_SEL(x)          ( ((x) & 0x7U) << 9U )
#define CTRL_1_SDA_I_SEL_POS        12U
#define CTRL_1_SDA_I_SEL_RST        0x0U
#define CTRL_1_SDA_I_SEL_MSK        ( 0x7U << 12U )
#define CTRL_1_SDA_I_SEL(x)          ( ((x) & 0x7U) << 12U )
#define CTRL_1_ADDR_POS        15U
#define CTRL_1_ADDR_RST        0x0U
#define CTRL_1_ADDR_MSK        ( 0x7fU << 15U )
#define CTRL_1_ADDR(x)          ( ((x) & 0x7fU) << 15U )
#define CTRL_1_DIV_POS        22U
#define CTRL_1_DIV_RST        0x0U
#define CTRL_1_DIV_MSK        ( 0x3ffU << 22U )
#define CTRL_1_DIV(x)          ( ((x) & 0x3ffU) << 22U )

// 0x4U, ctrl_2
#define CTRL_2_OFFSET        0x4U
#define CTRL_2_SDA_O_SEL_POS        0U
#define CTRL_2_SDA_O_SEL_RST        0x7U
#define CTRL_2_SDA_O_SEL_MSK        ( 0x7U << 0U )
#define CTRL_2_SDA_O_SEL(x)          ( ((x) & 0x7U) << 0U )
#define CTRL_2_DMA_RX_EN_POS        4U
#define CTRL_2_DMA_RX_EN_RST        0x0U
#define CTRL_2_DMA_RX_EN_MSK        ( 0x1U << 4U )
#define CTRL_2_DMA_RX_EN(x)          ( ((x) & 0x1U) << 4U )
#define CTRL_2_DMA_TX_EN_POS        5U
#define CTRL_2_DMA_TX_EN_RST        0x0U
#define CTRL_2_DMA_TX_EN_MSK        ( 0x1U << 5U )
#define CTRL_2_DMA_TX_EN(x)          ( ((x) & 0x1U) << 5U )
#define CTRL_2_IRQ_ADDR_HIT_EN_POS        6U
#define CTRL_2_IRQ_ADDR_HIT_EN_RST        0x0U
#define CTRL_2_IRQ_ADDR_HIT_EN_MSK        ( 0x1U << 6U )
#define CTRL_2_IRQ_ADDR_HIT_EN(x)          ( ((x) & 0x1U) << 6U )
#define CTRL_2_IRQ_EMPTY_EN_POS        9U
#define CTRL_2_IRQ_EMPTY_EN_RST        0x0U
#define CTRL_2_IRQ_EMPTY_EN_MSK        ( 0x1U << 9U )
#define CTRL_2_IRQ_EMPTY_EN(x)          ( ((x) & 0x1U) << 9U )
#define CTRL_2_IRQ_FULL_EN_POS        10U
#define CTRL_2_IRQ_FULL_EN_RST        0x0U
#define CTRL_2_IRQ_FULL_EN_MSK        ( 0x1U << 10U )
#define CTRL_2_IRQ_FULL_EN(x)          ( ((x) & 0x1U) << 10U )
#define CTRL_2_IRQ_STOP_EN_POS        11U
#define CTRL_2_IRQ_STOP_EN_RST        0x0U
#define CTRL_2_IRQ_STOP_EN_MSK        ( 0x1U << 11U )
#define CTRL_2_IRQ_STOP_EN(x)          ( ((x) & 0x1U) << 11U )

// 0x8U, cmd
#define CMD_OFFSET        0x8U
#define CMD_START_POS        0U
#define CMD_START_RST        0x0U
#define CMD_START_MSK        ( 0x1U << 0U )
#define CMD_START(x)          ( ((x) & 0x1U) << 0U )
#define CMD_STOP_POS        1U
#define CMD_STOP_RST        0x0U
#define CMD_STOP_MSK        ( 0x1U << 1U )
#define CMD_STOP(x)          ( ((x) & 0x1U) << 1U )
#define CMD_TRANS_POS        2U
#define CMD_TRANS_RST        0x0U
#define CMD_TRANS_MSK        ( 0x1U << 2U )
#define CMD_TRANS(x)          ( ((x) & 0x1U) << 2U )
#define CMD_RECEIVE_POS        3U
#define CMD_RECEIVE_RST        0x0U
#define CMD_RECEIVE_MSK        ( 0x1U << 3U )
#define CMD_RECEIVE(x)          ( ((x) & 0x1U) << 3U )
#define CMD_ACK_POS        4U
#define CMD_ACK_RST        0x0U
#define CMD_ACK_MSK        ( 0x1U << 4U )
#define CMD_ACK(x)          ( ((x) & 0x1U) << 4U )
#define CMD_DATA_POS        24U
#define CMD_DATA_RST        0x0U
#define CMD_DATA_MSK        ( 0xffU << 24U )
#define CMD_DATA(x)          ( ((x) & 0xffU) << 24U )

// 0xcU, status
#define STATUS_OFFSET        0xcU
#define STATUS_ADDR_HIT_POS        0U
#define STATUS_ADDR_HIT_RST        0x0U
#define STATUS_ADDR_HIT_MSK        ( 0x1U << 0U )
#define STATUS_ADDR_HIT(x)          ( ((x) & 0x1U) << 0U )
#define STATUS_EMPTY_POS        1U
#define STATUS_EMPTY_RST        0x0U
#define STATUS_EMPTY_MSK        ( 0x1U << 1U )
#define STATUS_EMPTY(x)          ( ((x) & 0x1U) << 1U )
#define STATUS_FULL_POS        2U
#define STATUS_FULL_RST        0x0U
#define STATUS_FULL_MSK        ( 0x1U << 2U )
#define STATUS_FULL(x)          ( ((x) & 0x1U) << 2U )
#define STATUS_ERR_POS        3U
#define STATUS_ERR_RST        0x0U
#define STATUS_ERR_MSK        ( 0x1U << 3U )
#define STATUS_ERR(x)          ( ((x) & 0x1U) << 3U )
#define STATUS_BUSY_POS        4U
#define STATUS_BUSY_RST        0x0U
#define STATUS_BUSY_MSK        ( 0x1U << 4U )
#define STATUS_BUSY(x)          ( ((x) & 0x1U) << 4U )
#define STATUS_ADDR_MATCH_POS        8U
#define STATUS_ADDR_MATCH_RST        0x0U
#define STATUS_ADDR_MATCH_MSK        ( 0x1U << 8U )
#define STATUS_ADDR_MATCH(x)          ( ((x) & 0x1U) << 8U )
#define STATUS_ACK_POS        9U
#define STATUS_ACK_RST        0x0U
#define STATUS_ACK_MSK        ( 0x1U << 9U )
#define STATUS_ACK(x)          ( ((x) & 0x1U) << 9U )
#define STATUS_STATE_POS        11U
#define STATUS_STATE_RST        0x0U
#define STATUS_STATE_MSK        ( 0x1fU << 11U )
#define STATUS_STATE(x)          ( ((x) & 0x1fU) << 11U )
#define STATUS_RW_POS        16U
#define STATUS_RW_RST        0x0U
#define STATUS_RW_MSK        ( 0x1U << 16U )
#define STATUS_RW(x)          ( ((x) & 0x1U) << 16U )
#define STATUS_ADDR_POS        17U
#define STATUS_ADDR_RST        0x0U
#define STATUS_ADDR_MSK        ( 0x7fU << 17U )
#define STATUS_ADDR(x)          ( ((x) & 0x7fU) << 17U )
#define STATUS_DATA_POS        24U
#define STATUS_DATA_RST        0x0U
#define STATUS_DATA_MSK        ( 0xffU << 24U )
#define STATUS_DATA(x)          ( ((x) & 0xffU) << 24U )

#endif
