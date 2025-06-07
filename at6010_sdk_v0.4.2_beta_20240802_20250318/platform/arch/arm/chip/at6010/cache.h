#ifndef _CACHE_H_
#define _CACHE_H_

#include "globals_macro.h"

#define REG_CACHE_BASE            0x40107000

typedef struct
{
    REG32        config;    // 0x0
    REG32        ctrl;    // 0x4
    REG32        addr_offset;    // 0x8
    REG32        status_hit;    // 0xc
    REG32        status_miss;    // 0x10
    REG32        status_error;    // 0x14
    REG32        status_irq;    // 0x18
    REG32        irq_mask;    // 0x1c
} HWP_CACHE_T;

#define hwp_cache        ((HWP_CACHE_T*)REG_CACHE_BASE)

// 0x0, config
#define CONFIG_OFFSET        0x0
#define CONFIG_RSVD_POS        0
#define CONFIG_RSVD_RST        0x0
#define CONFIG_RSVD_MSK        ( 0xf << 0 )
#define CONFIG_RSVD(x)          ( ((x) & 0xf) << 0 )
#define CONFIG_CACHE_SIZE_POS        4
#define CONFIG_CACHE_SIZE_RST        0x0
#define CONFIG_CACHE_SIZE_MSK        ( 0x3 << 4 )
#define CONFIG_CACHE_SIZE(x)          ( ((x) & 0x3) << 4 )
#define CONFIG_CACHE_EN_POS        6
#define CONFIG_CACHE_EN_RST        0x0
#define CONFIG_CACHE_EN_MSK        ( 0x1 << 6 )
#define CONFIG_CACHE_EN(x)          ( ((x) & 0x1) << 6 )
#define CONFIG_IRQ_QUICK_RESP_POS        7
#define CONFIG_IRQ_QUICK_RESP_RST        0x0
#define CONFIG_IRQ_QUICK_RESP_MSK        ( 0x1 << 7 )
#define CONFIG_IRQ_QUICK_RESP(x)          ( ((x) & 0x1) << 7 )
#define CONFIG_FLASH_MAP_POS        8
#define CONFIG_FLASH_MAP_RST        0x0
#define CONFIG_FLASH_MAP_MSK        ( 0x7 << 8 )
#define CONFIG_FLASH_MAP(x)          ( ((x) & 0x7) << 8 )
#define CONFIG_MISS_CNT_EN_POS        12
#define CONFIG_MISS_CNT_EN_RST        0x0
#define CONFIG_MISS_CNT_EN_MSK        ( 0x1 << 12 )
#define CONFIG_MISS_CNT_EN(x)          ( ((x) & 0x1) << 12 )
#define CONFIG_HIT_CNT_EN_POS        13
#define CONFIG_HIT_CNT_EN_RST        0x0
#define CONFIG_HIT_CNT_EN_MSK        ( 0x1 << 13 )
#define CONFIG_HIT_CNT_EN(x)          ( ((x) & 0x1) << 13 )
#define CONFIG_RAM_CLK_FORCE_ON_POS        14
#define CONFIG_RAM_CLK_FORCE_ON_RST        0x0
#define CONFIG_RAM_CLK_FORCE_ON_MSK        ( 0x1 << 14 )
#define CONFIG_RAM_CLK_FORCE_ON(x)          ( ((x) & 0x1) << 14 )
#define CONFIG_CACHE_LINE_NOFLUSH_N_POS        15
#define CONFIG_CACHE_LINE_NOFLUSH_N_RST        0x1
#define CONFIG_CACHE_LINE_NOFLUSH_N_MSK        ( 0x1 << 15 )
#define CONFIG_CACHE_LINE_NOFLUSH_N(x)          ( ((x) & 0x1) << 15 )
#define CONFIG_INT_VALID_POS        16
#define CONFIG_INT_VALID_RST        0x0
#define CONFIG_INT_VALID_MSK        ( 0x1 << 16 )
#define CONFIG_INT_VALID(x)          ( ((x) & 0x1) << 16 )

// 0x4, ctrl
#define CTRL_OFFSET        0x4
#define CTRL_CACHE_LINE_FLUSH_POS        0
#define CTRL_CACHE_LINE_FLUSH_RST        0x0
#define CTRL_CACHE_LINE_FLUSH_MSK        ( 0x1 << 0 )
#define CTRL_CACHE_LINE_FLUSH(x)          ( ((x) & 0x1) << 0 )

// 0x8, addr_offset
#define ADDR_OFFSET_OFFSET        0x8
#define ADDR_OFFSET_OFST_POS        0
#define ADDR_OFFSET_OFST_RST        0x0
#define ADDR_OFFSET_OFST_MSK        ( 0xffffff << 0 )
#define ADDR_OFFSET_OFST(x)          ( ((x) & 0xffffff) << 0 )
#define ADDR_OFFSET_EN_POS        31
#define ADDR_OFFSET_EN_RST        0x0
#define ADDR_OFFSET_EN_MSK        ( 0x1 << 31 )
#define ADDR_OFFSET_EN(x)          ( ((x) & 0x1) << 31 )

// 0xc, status_hit
#define STATUS_HIT_OFFSET        0xc
#define STATUS_HIT_HIT_CNT_POS        0
#define STATUS_HIT_HIT_CNT_RST        0x0
#define STATUS_HIT_HIT_CNT_MSK        ( 0xffff << 0 )
#define STATUS_HIT_HIT_CNT(x)          ( ((x) & 0xffff) << 0 )

// 0x10, status_miss
#define STATUS_MISS_OFFSET        0x10
#define STATUS_MISS_MISS_CNT_POS        0
#define STATUS_MISS_MISS_CNT_RST        0x0
#define STATUS_MISS_MISS_CNT_MSK        ( 0xffff << 0 )
#define STATUS_MISS_MISS_CNT(x)          ( ((x) & 0xffff) << 0 )

// 0x14, status_error
#define STATUS_ERROR_OFFSET        0x14
#define STATUS_ERROR_ADDR_RANGE_ERROR_POS        0
#define STATUS_ERROR_ADDR_RANGE_ERROR_RST        0x0
#define STATUS_ERROR_ADDR_RANGE_ERROR_MSK        ( 0x1 << 0 )
#define STATUS_ERROR_ADDR_RANGE_ERROR(x)          ( ((x) & 0x1) << 0 )
#define STATUS_ERROR_WRITE_ERROR_POS        1
#define STATUS_ERROR_WRITE_ERROR_RST        0x0
#define STATUS_ERROR_WRITE_ERROR_MSK        ( 0x1 << 1 )
#define STATUS_ERROR_WRITE_ERROR(x)          ( ((x) & 0x1) << 1 )
#define STATUS_ERROR_CACHE_DISABLE_ERROR_POS        2
#define STATUS_ERROR_CACHE_DISABLE_ERROR_RST        0x0
#define STATUS_ERROR_CACHE_DISABLE_ERROR_MSK        ( 0x1 << 2 )
#define STATUS_ERROR_CACHE_DISABLE_ERROR(x)          ( ((x) & 0x1) << 2 )
#define STATUS_ERROR_FLUSH_DONE_POS        3
#define STATUS_ERROR_FLUSH_DONE_RST        0x0
#define STATUS_ERROR_FLUSH_DONE_MSK        ( 0x1 << 3 )
#define STATUS_ERROR_FLUSH_DONE(x)          ( ((x) & 0x1) << 3 )

// 0x18, status_irq
#define STATUS_IRQ_OFFSET        0x18
#define STATUS_IRQ_ADDR_RANGE_ERROR_IRQ_POS        0
#define STATUS_IRQ_ADDR_RANGE_ERROR_IRQ_RST        0x0
#define STATUS_IRQ_ADDR_RANGE_ERROR_IRQ_MSK        ( 0x1 << 0 )
#define STATUS_IRQ_ADDR_RANGE_ERROR_IRQ(x)          ( ((x) & 0x1) << 0 )
#define STATUS_IRQ_WRITE_ERROR_IRQ_POS        1
#define STATUS_IRQ_WRITE_ERROR_IRQ_RST        0x0
#define STATUS_IRQ_WRITE_ERROR_IRQ_MSK        ( 0x1 << 1 )
#define STATUS_IRQ_WRITE_ERROR_IRQ(x)          ( ((x) & 0x1) << 1 )
#define STATUS_IRQ_CACHE_DISABLE_ERROR_IRQ_POS        2
#define STATUS_IRQ_CACHE_DISABLE_ERROR_IRQ_RST        0x0
#define STATUS_IRQ_CACHE_DISABLE_ERROR_IRQ_MSK        ( 0x1 << 2 )
#define STATUS_IRQ_CACHE_DISABLE_ERROR_IRQ(x)          ( ((x) & 0x1) << 2 )
#define STATUS_IRQ_FLUSH_DONE_IRQ_POS        3
#define STATUS_IRQ_FLUSH_DONE_IRQ_RST        0x0
#define STATUS_IRQ_FLUSH_DONE_IRQ_MSK        ( 0x1 << 3 )
#define STATUS_IRQ_FLUSH_DONE_IRQ(x)          ( ((x) & 0x1) << 3 )

// 0x1c, irq_mask
#define IRQ_MASK_OFFSET        0x1c
#define IRQ_MASK_ADDR_RANGE_ERRWR_MASK_POS        0
#define IRQ_MASK_ADDR_RANGE_ERRWR_MASK_RST        0x1
#define IRQ_MASK_ADDR_RANGE_ERRWR_MASK_MSK        ( 0x1 << 0 )
#define IRQ_MASK_ADDR_RANGE_ERRWR_MASK(x)          ( ((x) & 0x1) << 0 )
#define IRQ_MASK_WRITE_ERRWR_MASK_POS        1
#define IRQ_MASK_WRITE_ERRWR_MASK_RST        0x1
#define IRQ_MASK_WRITE_ERRWR_MASK_MSK        ( 0x1 << 1 )
#define IRQ_MASK_WRITE_ERRWR_MASK(x)          ( ((x) & 0x1) << 1 )
#define IRQ_MASK_CACHE_DISABLE_ERRWR_MASK_POS        2
#define IRQ_MASK_CACHE_DISABLE_ERRWR_MASK_RST        0x1
#define IRQ_MASK_CACHE_DISABLE_ERRWR_MASK_MSK        ( 0x1 << 2 )
#define IRQ_MASK_CACHE_DISABLE_ERRWR_MASK(x)          ( ((x) & 0x1) << 2 )
#define IRQ_MASK_FLUSH_DONE_MASK_POS        3
#define IRQ_MASK_FLUSH_DONE_MASK_RST        0x1
#define IRQ_MASK_FLUSH_DONE_MASK_MSK        ( 0x1 << 3 )
#define IRQ_MASK_FLUSH_DONE_MASK(x)          ( ((x) & 0x1) << 3 )

#endif
