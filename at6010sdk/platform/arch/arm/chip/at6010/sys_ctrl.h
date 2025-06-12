#ifndef _SYS_CTRL_H_
#define _SYS_CTRL_H_

#include "globals_macro.h"

#define REG_SYS_CTRL_BASE            0x40105000U

typedef struct
{
    REG32        rst_info;    // 0x0
    REG32        pmu_ctrl;    // 0x4
    REG32        clk_div_ctrl_0;    // 0x8
    REG32        clk_div_ctrl_1;    // 0xc
    REG32        clk_div_ctrl_2;    // 0x10
    REG32        clk_div_ctrl_3;    // 0x14
    REG32        clk_gate_ahb;    // 0x18
    REG32        clk_gate_apb;    // 0x1c
    REG32        soft_reset_ahb;    // 0x20
    REG32        soft_reset_apb;    // 0x24
    REG32        sys_ctrl_status;    // 0x28
    REG32        iomux_sel;    // 0x2c
    REG32        gpio_mcu_sel;    // 0x30
    REG32        debug_mcu;    // 0x34
    REG32        gpio_mcu_0_3;    // 0x38
    REG32        gpio_mcu_4_7;    // 0x3c
    REG32        gpio_mcu_8_11;    // 0x40
    REG32        gpio_mcu_12_15;    // 0x44
    REG32        gpio_mcu_16_19;    // 0x48
    REG32        gpio_status;    // 0x4c
    REG32        bus_ctrl;    // 0x50
} HWP_SYS_CTRL_T;

#define hwp_sys_ctrl        ((HWP_SYS_CTRL_T*)REG_SYS_CTRL_BASE)

// 0x0, rst_info
#define RST_INFO_OFFSET        0x0U
#define RST_INFO_SYS_POS        0U
#define RST_INFO_SYS_RST        0x0U
#define RST_INFO_SYS_MSK        ( 0x1U << 0U )
#define RST_INFO_SYS(x)          ( ((x) & 0x1U) << 0U )
#define RST_INFO_WDG_POS        1U
#define RST_INFO_WDG_RST        0x0U
#define RST_INFO_WDG_MSK        ( 0x1U << 1U )
#define RST_INFO_WDG(x)          ( ((x) & 0x1U) << 1U )
#define RST_INFO_LOCKUP_POS        2U
#define RST_INFO_LOCKUP_RST        0x0U
#define RST_INFO_LOCKUP_MSK        ( 0x1U << 2U )
#define RST_INFO_LOCKUP(x)          ( ((x) & 0x1U) << 2U )

// 0x4, pmu_ctrl
#define PMU_CTRL_OFFSET        0x4U
#define PMU_CTRL_CM0P_PMU_EN_POS        0U
#define PMU_CTRL_CM0P_PMU_EN_RST        0x0U
#define PMU_CTRL_CM0P_PMU_EN_MSK        ( 0x1U << 0U )
#define PMU_CTRL_CM0P_PMU_EN(x)          ( ((x) & 0x1U) << 0U )
#define PMU_CTRL_LOCKUP_RST_EN_POS        1U
#define PMU_CTRL_LOCKUP_RST_EN_RST        0x0U
#define PMU_CTRL_LOCKUP_RST_EN_MSK        ( 0x1U << 1U )
#define PMU_CTRL_LOCKUP_RST_EN(x)          ( ((x) & 0x1U) << 1U )
#define PMU_CTRL_CLK_ADC_INV_POS        2U
#define PMU_CTRL_CLK_ADC_INV_RST        0x0U
#define PMU_CTRL_CLK_ADC_INV_MSK        ( 0x1U << 2U )
#define PMU_CTRL_CLK_ADC_INV(x)          ( ((x) & 0x1U) << 2U )
#define PMU_CTRL_CLK_SDM_INV_POS        3U
#define PMU_CTRL_CLK_SDM_INV_RST        0x0U
#define PMU_CTRL_CLK_SDM_INV_MSK        ( 0x1U << 3U )
#define PMU_CTRL_CLK_SDM_INV(x)          ( ((x) & 0x1U) << 3U )

// 0x8, clk_div_ctrl_0
#define CLK_DIV_CTRL_0_OFFSET        0x8U
#define CLK_DIV_CTRL_0_DIV_BYPASS_XTAL_POS        0U
#define CLK_DIV_CTRL_0_DIV_BYPASS_XTAL_RST        0x1U
#define CLK_DIV_CTRL_0_DIV_BYPASS_XTAL_MSK        ( 0x1U << 0U )
#define CLK_DIV_CTRL_0_DIV_BYPASS_XTAL(x)          ( ((x) & 0x1U) << 0U )
#define CLK_DIV_CTRL_0_DIV_LOAD_XTAL_POS        1U
#define CLK_DIV_CTRL_0_DIV_LOAD_XTAL_RST        0x0U
#define CLK_DIV_CTRL_0_DIV_LOAD_XTAL_MSK        ( 0x1U << 1U )
#define CLK_DIV_CTRL_0_DIV_LOAD_XTAL(x)          ( ((x) & 0x1U) << 1U )
#define CLK_DIV_CTRL_0_DIV_EN_XTAL_POS        2U
#define CLK_DIV_CTRL_0_DIV_EN_XTAL_RST        0x1U
#define CLK_DIV_CTRL_0_DIV_EN_XTAL_MSK        ( 0x1U << 2U )
#define CLK_DIV_CTRL_0_DIV_EN_XTAL(x)          ( ((x) & 0x1U) << 2U )
#define CLK_DIV_CTRL_0_DIV_XTAL_POS        4U
#define CLK_DIV_CTRL_0_DIV_XTAL_RST        0x0U
#define CLK_DIV_CTRL_0_DIV_XTAL_MSK        ( 0xfffU << 4U )
#define CLK_DIV_CTRL_0_DIV_XTAL(x)          ( ((x) & 0xfffU) << 4U )
#define CLK_DIV_CTRL_0_DIV_BYPASS_PLL_POS        16U
#define CLK_DIV_CTRL_0_DIV_BYPASS_PLL_RST        0x0U
#define CLK_DIV_CTRL_0_DIV_BYPASS_PLL_MSK        ( 0x1U << 16U )
#define CLK_DIV_CTRL_0_DIV_BYPASS_PLL(x)          ( ((x) & 0x1U) << 16U )
#define CLK_DIV_CTRL_0_DIV_LOAD_PLL_POS        17U
#define CLK_DIV_CTRL_0_DIV_LOAD_PLL_RST        0x0U
#define CLK_DIV_CTRL_0_DIV_LOAD_PLL_MSK        ( 0x1U << 17U )
#define CLK_DIV_CTRL_0_DIV_LOAD_PLL(x)          ( ((x) & 0x1U) << 17U )
#define CLK_DIV_CTRL_0_DIV_EN_PLL_POS        18U
#define CLK_DIV_CTRL_0_DIV_EN_PLL_RST        0x1U
#define CLK_DIV_CTRL_0_DIV_EN_PLL_MSK        ( 0x1U << 18U )
#define CLK_DIV_CTRL_0_DIV_EN_PLL(x)          ( ((x) & 0x1U) << 18U )
#define CLK_DIV_CTRL_0_DIV_PLL_POS        20U
#define CLK_DIV_CTRL_0_DIV_PLL_RST        0x0U
#define CLK_DIV_CTRL_0_DIV_PLL_MSK        ( 0xffU << 20U )
#define CLK_DIV_CTRL_0_DIV_PLL(x)          ( ((x) & 0xffU) << 20U )
#define CLK_DIV_CTRL_0_CLK_XTAL_SEL_POS        28U
#define CLK_DIV_CTRL_0_CLK_XTAL_SEL_RST        0x1U
#define CLK_DIV_CTRL_0_CLK_XTAL_SEL_MSK        ( 0x1U << 28U )
#define CLK_DIV_CTRL_0_CLK_XTAL_SEL(x)          ( ((x) & 0x1U) << 28U )
#define CLK_DIV_CTRL_0_CLK_PLL_SEL_POS        29U
#define CLK_DIV_CTRL_0_CLK_PLL_SEL_RST        0x0U
#define CLK_DIV_CTRL_0_CLK_PLL_SEL_MSK        ( 0x1U << 29U )
#define CLK_DIV_CTRL_0_CLK_PLL_SEL(x)          ( ((x) & 0x1U) << 29U )

// 0xc, clk_div_ctrl_1
#define CLK_DIV_CTRL_1_OFFSET        0xcU
#define CLK_DIV_CTRL_1_DIV_BYPASS_FCLK_POS        0U
#define CLK_DIV_CTRL_1_DIV_BYPASS_FCLK_RST        0x1U
#define CLK_DIV_CTRL_1_DIV_BYPASS_FCLK_MSK        ( 0x1U << 0U )
#define CLK_DIV_CTRL_1_DIV_BYPASS_FCLK(x)          ( ((x) & 0x1U) << 0U )
#define CLK_DIV_CTRL_1_DIV_LOAD_FCLK_POS        1U
#define CLK_DIV_CTRL_1_DIV_LOAD_FCLK_RST        0x0U
#define CLK_DIV_CTRL_1_DIV_LOAD_FCLK_MSK        ( 0x1U << 1U )
#define CLK_DIV_CTRL_1_DIV_LOAD_FCLK(x)          ( ((x) & 0x1U) << 1U )
#define CLK_DIV_CTRL_1_DIV_EN_FCLK_POS        2U
#define CLK_DIV_CTRL_1_DIV_EN_FCLK_RST        0x1U
#define CLK_DIV_CTRL_1_DIV_EN_FCLK_MSK        ( 0x1U << 2U )
#define CLK_DIV_CTRL_1_DIV_EN_FCLK(x)          ( ((x) & 0x1U) << 2U )
#define CLK_DIV_CTRL_1_DIV_FCLK_POS        4U
#define CLK_DIV_CTRL_1_DIV_FCLK_RST        0x0U
#define CLK_DIV_CTRL_1_DIV_FCLK_MSK        ( 0xffU << 4U )
#define CLK_DIV_CTRL_1_DIV_FCLK(x)          ( ((x) & 0xffU) << 4U )
#define CLK_DIV_CTRL_1_DIV_BYPASS_SCLK_POS        12U
#define CLK_DIV_CTRL_1_DIV_BYPASS_SCLK_RST        0x1U
#define CLK_DIV_CTRL_1_DIV_BYPASS_SCLK_MSK        ( 0x1U << 12U )
#define CLK_DIV_CTRL_1_DIV_BYPASS_SCLK(x)          ( ((x) & 0x1U) << 12U )
#define CLK_DIV_CTRL_1_DIV_LOAD_SCLK_POS        13U
#define CLK_DIV_CTRL_1_DIV_LOAD_SCLK_RST        0x0U
#define CLK_DIV_CTRL_1_DIV_LOAD_SCLK_MSK        ( 0x1U << 13U )
#define CLK_DIV_CTRL_1_DIV_LOAD_SCLK(x)          ( ((x) & 0x1U) << 13U )
#define CLK_DIV_CTRL_1_DIV_EN_SCLK_POS        14U
#define CLK_DIV_CTRL_1_DIV_EN_SCLK_RST        0x1U
#define CLK_DIV_CTRL_1_DIV_EN_SCLK_MSK        ( 0x1U << 14U )
#define CLK_DIV_CTRL_1_DIV_EN_SCLK(x)          ( ((x) & 0x1U) << 14U )
#define CLK_DIV_CTRL_1_DIV_SCLK_POS        16U
#define CLK_DIV_CTRL_1_DIV_SCLK_RST        0x0U
#define CLK_DIV_CTRL_1_DIV_SCLK_MSK        ( 0xffU << 16U )
#define CLK_DIV_CTRL_1_DIV_SCLK(x)          ( ((x) & 0xffU) << 16U )
#define CLK_DIV_CTRL_1_SCLK_FLASH_EN_POS        24U
#define CLK_DIV_CTRL_1_SCLK_FLASH_EN_RST        0x1U
#define CLK_DIV_CTRL_1_SCLK_FLASH_EN_MSK        ( 0x1U << 24U )
#define CLK_DIV_CTRL_1_SCLK_FLASH_EN(x)          ( ((x) & 0x1U) << 24U )

// 0x10, clk_div_ctrl_2
#define CLK_DIV_CTRL_2_OFFSET        0x10U
#define CLK_DIV_CTRL_2_DIV_BYPASS_32K_POS        0U
#define CLK_DIV_CTRL_2_DIV_BYPASS_32K_RST        0x0U
#define CLK_DIV_CTRL_2_DIV_BYPASS_32K_MSK        ( 0x1U << 0U )
#define CLK_DIV_CTRL_2_DIV_BYPASS_32K(x)          ( ((x) & 0x1U) << 0U )
#define CLK_DIV_CTRL_2_DIV_LOAD_32K_POS        1U
#define CLK_DIV_CTRL_2_DIV_LOAD_32K_RST        0x0U
#define CLK_DIV_CTRL_2_DIV_LOAD_32K_MSK        ( 0x1U << 1U )
#define CLK_DIV_CTRL_2_DIV_LOAD_32K(x)          ( ((x) & 0x1U) << 1U )
#define CLK_DIV_CTRL_2_DIV_EN_32K_POS        2U
#define CLK_DIV_CTRL_2_DIV_EN_32K_RST        0x1U
#define CLK_DIV_CTRL_2_DIV_EN_32K_MSK        ( 0x1U << 2U )
#define CLK_DIV_CTRL_2_DIV_EN_32K(x)          ( ((x) & 0x1U) << 2U )
#define CLK_DIV_CTRL_2_DIV_32K_POS        4U
#define CLK_DIV_CTRL_2_DIV_32K_RST        0x270U
#define CLK_DIV_CTRL_2_DIV_32K_MSK        ( 0xfffU << 4U )
#define CLK_DIV_CTRL_2_DIV_32K(x)          ( ((x) & 0xfffU) << 4U )

// 0x14, clk_div_ctrl_3
#define CLK_DIV_CTRL_3_OFFSET        0x14U
#define CLK_DIV_CTRL_3_DIV_BYPASS_SPI_0_POS        0U
#define CLK_DIV_CTRL_3_DIV_BYPASS_SPI_0_RST        0x1U
#define CLK_DIV_CTRL_3_DIV_BYPASS_SPI_0_MSK        ( 0x1U << 0U )
#define CLK_DIV_CTRL_3_DIV_BYPASS_SPI_0(x)          ( ((x) & 0x1U) << 0U )
#define CLK_DIV_CTRL_3_DIV_LOAD_SPI_0_POS        1U
#define CLK_DIV_CTRL_3_DIV_LOAD_SPI_0_RST        0x0U
#define CLK_DIV_CTRL_3_DIV_LOAD_SPI_0_MSK        ( 0x1U << 1U )
#define CLK_DIV_CTRL_3_DIV_LOAD_SPI_0(x)          ( ((x) & 0x1U) << 1U )
#define CLK_DIV_CTRL_3_DIV_EN_SPI_0_POS        2U
#define CLK_DIV_CTRL_3_DIV_EN_SPI_0_RST        0x1U
#define CLK_DIV_CTRL_3_DIV_EN_SPI_0_MSK        ( 0x1U << 2U )
#define CLK_DIV_CTRL_3_DIV_EN_SPI_0(x)          ( ((x) & 0x1U) << 2U )
#define CLK_DIV_CTRL_3_DIV_SPI_0_POS        4U
#define CLK_DIV_CTRL_3_DIV_SPI_0_RST        0x0U
#define CLK_DIV_CTRL_3_DIV_SPI_0_MSK        ( 0xffU << 4U )
#define CLK_DIV_CTRL_3_DIV_SPI_0(x)          ( ((x) & 0xffU) << 4U )
#define CLK_DIV_CTRL_3_DIV_BYPASS_SPI_1_POS        12U
#define CLK_DIV_CTRL_3_DIV_BYPASS_SPI_1_RST        0x1U
#define CLK_DIV_CTRL_3_DIV_BYPASS_SPI_1_MSK        ( 0x1U << 12U )
#define CLK_DIV_CTRL_3_DIV_BYPASS_SPI_1(x)          ( ((x) & 0x1U) << 12U )
#define CLK_DIV_CTRL_3_DIV_LOAD_SPI_1_POS        13U
#define CLK_DIV_CTRL_3_DIV_LOAD_SPI_1_RST        0x0U
#define CLK_DIV_CTRL_3_DIV_LOAD_SPI_1_MSK        ( 0x1U << 13U )
#define CLK_DIV_CTRL_3_DIV_LOAD_SPI_1(x)          ( ((x) & 0x1U) << 13U )
#define CLK_DIV_CTRL_3_DIV_EN_SPI_1_POS        14U
#define CLK_DIV_CTRL_3_DIV_EN_SPI_1_RST        0x1U
#define CLK_DIV_CTRL_3_DIV_EN_SPI_1_MSK        ( 0x1U << 14U )
#define CLK_DIV_CTRL_3_DIV_EN_SPI_1(x)          ( ((x) & 0x1U) << 14U )
#define CLK_DIV_CTRL_3_DIV_SPI_1_POS        16U
#define CLK_DIV_CTRL_3_DIV_SPI_1_RST        0x0U
#define CLK_DIV_CTRL_3_DIV_SPI_1_MSK        ( 0xffU << 16U )
#define CLK_DIV_CTRL_3_DIV_SPI_1(x)          ( ((x) & 0xffU) << 16U )

// 0x18, clk_gate_ahb
#define CLK_GATE_AHB_OFFSET        0x18U
#define CLK_GATE_AHB_HCLK_DMA_EN_POS        0U
#define CLK_GATE_AHB_HCLK_DMA_EN_RST        0x0U
#define CLK_GATE_AHB_HCLK_DMA_EN_MSK        ( 0x1U << 0U )
#define CLK_GATE_AHB_HCLK_DMA_EN(x)          ( ((x) & 0x1U) << 0U )
#define CLK_GATE_AHB_HCLK_ROM_EN_POS        1U
#define CLK_GATE_AHB_HCLK_ROM_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_ROM_EN_MSK        ( 0x1U << 1U )
#define CLK_GATE_AHB_HCLK_ROM_EN(x)          ( ((x) & 0x1U) << 1U )
#define CLK_GATE_AHB_HCLK_CACHE_EN_POS        2U
#define CLK_GATE_AHB_HCLK_CACHE_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_CACHE_EN_MSK        ( 0x1U << 2U )
#define CLK_GATE_AHB_HCLK_CACHE_EN(x)          ( ((x) & 0x1U) << 2U )
#define CLK_GATE_AHB_HCLK_FLASH_EN_POS        3U
#define CLK_GATE_AHB_HCLK_FLASH_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_FLASH_EN_MSK        ( 0x1U << 3U )
#define CLK_GATE_AHB_HCLK_FLASH_EN(x)          ( ((x) & 0x1U) << 3U )
#define CLK_GATE_AHB_HCLK_SRAM_0_EN_POS        4U
#define CLK_GATE_AHB_HCLK_SRAM_0_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_SRAM_0_EN_MSK        ( 0x1U << 4U )
#define CLK_GATE_AHB_HCLK_SRAM_0_EN(x)          ( ((x) & 0x1U) << 4U )
#define CLK_GATE_AHB_HCLK_SRAM_1_EN_POS        5U
#define CLK_GATE_AHB_HCLK_SRAM_1_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_SRAM_1_EN_MSK        ( 0x1U << 5U )
#define CLK_GATE_AHB_HCLK_SRAM_1_EN(x)          ( ((x) & 0x1U) << 5U )
#define CLK_GATE_AHB_HCLK_SRAM_2_EN_POS        6U
#define CLK_GATE_AHB_HCLK_SRAM_2_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_SRAM_2_EN_MSK        ( 0x1U << 6U )
#define CLK_GATE_AHB_HCLK_SRAM_2_EN(x)          ( ((x) & 0x1U) << 6U )
#define CLK_GATE_AHB_HCLK_SRAM_3_EN_POS        7U
#define CLK_GATE_AHB_HCLK_SRAM_3_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_SRAM_3_EN_MSK        ( 0x1U << 7U )
#define CLK_GATE_AHB_HCLK_SRAM_3_EN(x)          ( ((x) & 0x1U) << 7U )
#define CLK_GATE_AHB_HCLK_RADAR_EN_POS        8U
#define CLK_GATE_AHB_HCLK_RADAR_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_RADAR_EN_MSK        ( 0x1U << 8U )
#define CLK_GATE_AHB_HCLK_RADAR_EN(x)          ( ((x) & 0x1U) << 8U )
#define CLK_GATE_AHB_HCLK_SPI_0_EN_POS        9U
#define CLK_GATE_AHB_HCLK_SPI_0_EN_RST        0x0U
#define CLK_GATE_AHB_HCLK_SPI_0_EN_MSK        ( 0x1U << 9U )
#define CLK_GATE_AHB_HCLK_SPI_0_EN(x)          ( ((x) & 0x1U) << 9U )
#define CLK_GATE_AHB_HCLK_SPI_1_EN_POS        10U
#define CLK_GATE_AHB_HCLK_SPI_1_EN_RST        0x0U
#define CLK_GATE_AHB_HCLK_SPI_1_EN_MSK        ( 0x1U << 10U )
#define CLK_GATE_AHB_HCLK_SPI_1_EN(x)          ( ((x) & 0x1U) << 10U )
#define CLK_GATE_AHB_HCLK_EXT_EN_POS        11U
#define CLK_GATE_AHB_HCLK_EXT_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_EXT_EN_MSK        ( 0x1U << 11U )
#define CLK_GATE_AHB_HCLK_EXT_EN(x)          ( ((x) & 0x1U) << 11U )
#define CLK_GATE_AHB_HCLK_APB_EN_POS        12U
#define CLK_GATE_AHB_HCLK_APB_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_APB_EN_MSK        ( 0x1U << 12U )
#define CLK_GATE_AHB_HCLK_APB_EN(x)          ( ((x) & 0x1U) << 12U )
#define CLK_GATE_AHB_HCLK_GPIO_EN_POS        13U
#define CLK_GATE_AHB_HCLK_GPIO_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_GPIO_EN_MSK        ( 0x1U << 13U )
#define CLK_GATE_AHB_HCLK_GPIO_EN(x)          ( ((x) & 0x1U) << 13U )
#define CLK_GATE_AHB_HCLK_SYS_CTRL_EN_POS        14U
#define CLK_GATE_AHB_HCLK_SYS_CTRL_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_SYS_CTRL_EN_MSK        ( 0x1U << 14U )
#define CLK_GATE_AHB_HCLK_SYS_CTRL_EN(x)          ( ((x) & 0x1U) << 14U )
#define CLK_GATE_AHB_HCLK_ROM_TABLE_EN_POS        15U
#define CLK_GATE_AHB_HCLK_ROM_TABLE_EN_RST        0x1U
#define CLK_GATE_AHB_HCLK_ROM_TABLE_EN_MSK        ( 0x1U << 15U )
#define CLK_GATE_AHB_HCLK_ROM_TABLE_EN(x)          ( ((x) & 0x1U) << 15U )

// 0x1c, clk_gate_apb
#define CLK_GATE_APB_OFFSET        0x1cU
#define CLK_GATE_APB_PCLK_WDG_EN_POS        0U
#define CLK_GATE_APB_PCLK_WDG_EN_RST        0x1U
#define CLK_GATE_APB_PCLK_WDG_EN_MSK        ( 0x1U << 0U )
#define CLK_GATE_APB_PCLK_WDG_EN(x)          ( ((x) & 0x1U) << 0U )
#define CLK_GATE_APB_PCLK_TIMER_0_EN_POS        1U
#define CLK_GATE_APB_PCLK_TIMER_0_EN_RST        0x1U
#define CLK_GATE_APB_PCLK_TIMER_0_EN_MSK        ( 0x1U << 1U )
#define CLK_GATE_APB_PCLK_TIMER_0_EN(x)          ( ((x) & 0x1U) << 1U )
#define CLK_GATE_APB_PCLK_TIMER_1_EN_POS        2U
#define CLK_GATE_APB_PCLK_TIMER_1_EN_RST        0x1U
#define CLK_GATE_APB_PCLK_TIMER_1_EN_MSK        ( 0x1U << 2U )
#define CLK_GATE_APB_PCLK_TIMER_1_EN(x)          ( ((x) & 0x1U) << 2U )
#define CLK_GATE_APB_PCLK_TIMER_2_EN_POS        3U
#define CLK_GATE_APB_PCLK_TIMER_2_EN_RST        0x1U
#define CLK_GATE_APB_PCLK_TIMER_2_EN_MSK        ( 0x1U << 3U )
#define CLK_GATE_APB_PCLK_TIMER_2_EN(x)          ( ((x) & 0x1U) << 3U )
#define CLK_GATE_APB_PCLK_TIMER_3_EN_POS        4U
#define CLK_GATE_APB_PCLK_TIMER_3_EN_RST        0x1U
#define CLK_GATE_APB_PCLK_TIMER_3_EN_MSK        ( 0x1U << 4U )
#define CLK_GATE_APB_PCLK_TIMER_3_EN(x)          ( ((x) & 0x1U) << 4U )
#define CLK_GATE_APB_PCLK_DUALTIMER_EN_POS        5U
#define CLK_GATE_APB_PCLK_DUALTIMER_EN_RST        0x1U
#define CLK_GATE_APB_PCLK_DUALTIMER_EN_MSK        ( 0x1U << 5U )
#define CLK_GATE_APB_PCLK_DUALTIMER_EN(x)          ( ((x) & 0x1U) << 5U )
#define CLK_GATE_APB_PCLK_UART_0_EN_POS        6U
#define CLK_GATE_APB_PCLK_UART_0_EN_RST        0x1U
#define CLK_GATE_APB_PCLK_UART_0_EN_MSK        ( 0x1U << 6U )
#define CLK_GATE_APB_PCLK_UART_0_EN(x)          ( ((x) & 0x1U) << 6U )
#define CLK_GATE_APB_PCLK_UART_1_EN_POS        7U
#define CLK_GATE_APB_PCLK_UART_1_EN_RST        0x0U
#define CLK_GATE_APB_PCLK_UART_1_EN_MSK        ( 0x1U << 7U )
#define CLK_GATE_APB_PCLK_UART_1_EN(x)          ( ((x) & 0x1U) << 7U )
#define CLK_GATE_APB_PCLK_UART_2_EN_POS        8U
#define CLK_GATE_APB_PCLK_UART_2_EN_RST        0x0U
#define CLK_GATE_APB_PCLK_UART_2_EN_MSK        ( 0x1U << 8U )
#define CLK_GATE_APB_PCLK_UART_2_EN(x)          ( ((x) & 0x1U) << 8U )
#define CLK_GATE_APB_PCLK_I2C_EN_POS        9U
#define CLK_GATE_APB_PCLK_I2C_EN_RST        0x0U
#define CLK_GATE_APB_PCLK_I2C_EN_MSK        ( 0x1U << 9U )
#define CLK_GATE_APB_PCLK_I2C_EN(x)          ( ((x) & 0x1U) << 9U )
#define CLK_GATE_APB_PCLK_PWM_EN_POS        10U
#define CLK_GATE_APB_PCLK_PWM_EN_RST        0x0U
#define CLK_GATE_APB_PCLK_PWM_EN_MSK        ( 0x1U << 10U )
#define CLK_GATE_APB_PCLK_PWM_EN(x)          ( ((x) & 0x1U) << 10U )

// 0x20, soft_reset_ahb
#define SOFT_RESET_AHB_OFFSET        0x20U
#define SOFT_RESET_AHB_RSTN_SW_DMA_POS        0U
#define SOFT_RESET_AHB_RSTN_SW_DMA_RST        0x0U
#define SOFT_RESET_AHB_RSTN_SW_DMA_MSK        ( 0x1U << 0U )
#define SOFT_RESET_AHB_RSTN_SW_DMA(x)          ( ((x) & 0x1U) << 0U )
#define SOFT_RESET_AHB_RSTN_SW_ROM_POS        1U
#define SOFT_RESET_AHB_RSTN_SW_ROM_RST        0x1U
#define SOFT_RESET_AHB_RSTN_SW_ROM_MSK        ( 0x1U << 1U )
#define SOFT_RESET_AHB_RSTN_SW_ROM(x)          ( ((x) & 0x1U) << 1U )
#define SOFT_RESET_AHB_RSTN_SW_CACHE_POS        2U
#define SOFT_RESET_AHB_RSTN_SW_CACHE_RST        0x1U
#define SOFT_RESET_AHB_RSTN_SW_CACHE_MSK        ( 0x1U << 2U )
#define SOFT_RESET_AHB_RSTN_SW_CACHE(x)          ( ((x) & 0x1U) << 2U )
#define SOFT_RESET_AHB_RSTN_SW_FLASH_POS        3U
#define SOFT_RESET_AHB_RSTN_SW_FLASH_RST        0x1U
#define SOFT_RESET_AHB_RSTN_SW_FLASH_MSK        ( 0x1U << 3U )
#define SOFT_RESET_AHB_RSTN_SW_FLASH(x)          ( ((x) & 0x1U) << 3U )
#define SOFT_RESET_AHB_RSTN_SW_SRAM_0_POS        4U
#define SOFT_RESET_AHB_RSTN_SW_SRAM_0_RST        0x1U
#define SOFT_RESET_AHB_RSTN_SW_SRAM_0_MSK        ( 0x1U << 4U )
#define SOFT_RESET_AHB_RSTN_SW_SRAM_0(x)          ( ((x) & 0x1U) << 4U )
#define SOFT_RESET_AHB_RSTN_SW_SRAM_1_POS        5U
#define SOFT_RESET_AHB_RSTN_SW_SRAM_1_RST        0x1U
#define SOFT_RESET_AHB_RSTN_SW_SRAM_1_MSK        ( 0x1U << 5U )
#define SOFT_RESET_AHB_RSTN_SW_SRAM_1(x)          ( ((x) & 0x1U) << 5U )
#define SOFT_RESET_AHB_RSTN_SW_SRAM_2_POS        6U
#define SOFT_RESET_AHB_RSTN_SW_SRAM_2_RST        0x1U
#define SOFT_RESET_AHB_RSTN_SW_SRAM_2_MSK        ( 0x1U << 6U )
#define SOFT_RESET_AHB_RSTN_SW_SRAM_2(x)          ( ((x) & 0x1U) << 6U )
#define SOFT_RESET_AHB_RSTN_SW_SRAM_3_POS        7U
#define SOFT_RESET_AHB_RSTN_SW_SRAM_3_RST        0x1U
#define SOFT_RESET_AHB_RSTN_SW_SRAM_3_MSK        ( 0x1U << 7U )
#define SOFT_RESET_AHB_RSTN_SW_SRAM_3(x)          ( ((x) & 0x1U) << 7U )
#define SOFT_RESET_AHB_RSTN_SW_RADAR_POS        8U
#define SOFT_RESET_AHB_RSTN_SW_RADAR_RST        0x1U
#define SOFT_RESET_AHB_RSTN_SW_RADAR_MSK        ( 0x1U << 8U )
#define SOFT_RESET_AHB_RSTN_SW_RADAR(x)          ( ((x) & 0x1U) << 8U )
#define SOFT_RESET_AHB_RSTN_SW_SPI_0_POS        9U
#define SOFT_RESET_AHB_RSTN_SW_SPI_0_RST        0x0U
#define SOFT_RESET_AHB_RSTN_SW_SPI_0_MSK        ( 0x1U << 9U )
#define SOFT_RESET_AHB_RSTN_SW_SPI_0(x)          ( ((x) & 0x1U) << 9U )
#define SOFT_RESET_AHB_RSTN_SW_SPI_1_POS        10U
#define SOFT_RESET_AHB_RSTN_SW_SPI_1_RST        0x0U
#define SOFT_RESET_AHB_RSTN_SW_SPI_1_MSK        ( 0x1U << 10U )
#define SOFT_RESET_AHB_RSTN_SW_SPI_1(x)          ( ((x) & 0x1U) << 10U )
#define SOFT_RESET_AHB_RSTN_SW_EXT_POS        11U
#define SOFT_RESET_AHB_RSTN_SW_EXT_RST        0x1U
#define SOFT_RESET_AHB_RSTN_SW_EXT_MSK        ( 0x1U << 11U )
#define SOFT_RESET_AHB_RSTN_SW_EXT(x)          ( ((x) & 0x1U) << 11U )
#define SOFT_RESET_AHB_RSTN_SW_APB_POS        12U
#define SOFT_RESET_AHB_RSTN_SW_APB_RST        0x1U
#define SOFT_RESET_AHB_RSTN_SW_APB_MSK        ( 0x1U << 12U )
#define SOFT_RESET_AHB_RSTN_SW_APB(x)          ( ((x) & 0x1U) << 12U )
#define SOFT_RESET_AHB_RSTN_SW_GPIO_POS        13U
#define SOFT_RESET_AHB_RSTN_SW_GPIO_RST        0x1U
#define SOFT_RESET_AHB_RSTN_SW_GPIO_MSK        ( 0x1U << 13U )
#define SOFT_RESET_AHB_RSTN_SW_GPIO(x)          ( ((x) & 0x1U) << 13U )
#define SOFT_RESET_AHB_RSTN_SW_SYS_CTRL_POS        14U
#define SOFT_RESET_AHB_RSTN_SW_SYS_CTRL_RST        0x1U
#define SOFT_RESET_AHB_RSTN_SW_SYS_CTRL_MSK        ( 0x1U << 14U )
#define SOFT_RESET_AHB_RSTN_SW_SYS_CTRL(x)          ( ((x) & 0x1U) << 14U )

// 0x24, soft_reset_apb
#define SOFT_RESET_APB_OFFSET        0x24U
#define SOFT_RESET_APB_RSTN_SW_WDG_POS        0U
#define SOFT_RESET_APB_RSTN_SW_WDG_RST        0x1U
#define SOFT_RESET_APB_RSTN_SW_WDG_MSK        ( 0x1U << 0U )
#define SOFT_RESET_APB_RSTN_SW_WDG(x)          ( ((x) & 0x1U) << 0U )
#define SOFT_RESET_APB_RSTN_SW_TIMER_0_POS        1U
#define SOFT_RESET_APB_RSTN_SW_TIMER_0_RST        0x1U
#define SOFT_RESET_APB_RSTN_SW_TIMER_0_MSK        ( 0x1U << 1U )
#define SOFT_RESET_APB_RSTN_SW_TIMER_0(x)          ( ((x) & 0x1U) << 1U )
#define SOFT_RESET_APB_RSTN_SW_TIMER_1_POS        2U
#define SOFT_RESET_APB_RSTN_SW_TIMER_1_RST        0x1U
#define SOFT_RESET_APB_RSTN_SW_TIMER_1_MSK        ( 0x1U << 2U )
#define SOFT_RESET_APB_RSTN_SW_TIMER_1(x)          ( ((x) & 0x1U) << 2U )
#define SOFT_RESET_APB_RSTN_SW_TIMER_2_POS        3U
#define SOFT_RESET_APB_RSTN_SW_TIMER_2_RST        0x1U
#define SOFT_RESET_APB_RSTN_SW_TIMER_2_MSK        ( 0x1U << 3U )
#define SOFT_RESET_APB_RSTN_SW_TIMER_2(x)          ( ((x) & 0x1U) << 3U )
#define SOFT_RESET_APB_RSTN_SW_TIMER_3_POS        4U
#define SOFT_RESET_APB_RSTN_SW_TIMER_3_RST        0x1U
#define SOFT_RESET_APB_RSTN_SW_TIMER_3_MSK        ( 0x1U << 4U )
#define SOFT_RESET_APB_RSTN_SW_TIMER_3(x)          ( ((x) & 0x1U) << 4U )
#define SOFT_RESET_APB_RSTN_SW_DUALTIMER_POS        5U
#define SOFT_RESET_APB_RSTN_SW_DUALTIMER_RST        0x1U
#define SOFT_RESET_APB_RSTN_SW_DUALTIMER_MSK        ( 0x1U << 5U )
#define SOFT_RESET_APB_RSTN_SW_DUALTIMER(x)          ( ((x) & 0x1U) << 5U )
#define SOFT_RESET_APB_RSTN_SW_UART_0_POS        6U
#define SOFT_RESET_APB_RSTN_SW_UART_0_RST        0x1U
#define SOFT_RESET_APB_RSTN_SW_UART_0_MSK        ( 0x1U << 6U )
#define SOFT_RESET_APB_RSTN_SW_UART_0(x)          ( ((x) & 0x1U) << 6U )
#define SOFT_RESET_APB_RSTN_SW_UART_1_POS        7U
#define SOFT_RESET_APB_RSTN_SW_UART_1_RST        0x0U
#define SOFT_RESET_APB_RSTN_SW_UART_1_MSK        ( 0x1U << 7U )
#define SOFT_RESET_APB_RSTN_SW_UART_1(x)          ( ((x) & 0x1U) << 7U )
#define SOFT_RESET_APB_RSTN_SW_UART_2_POS        8U
#define SOFT_RESET_APB_RSTN_SW_UART_2_RST        0x0U
#define SOFT_RESET_APB_RSTN_SW_UART_2_MSK        ( 0x1U << 8U )
#define SOFT_RESET_APB_RSTN_SW_UART_2(x)          ( ((x) & 0x1U) << 8U )
#define SOFT_RESET_APB_RSTN_SW_I2C_POS        9U
#define SOFT_RESET_APB_RSTN_SW_I2C_RST        0x0U
#define SOFT_RESET_APB_RSTN_SW_I2C_MSK        ( 0x1U << 9U )
#define SOFT_RESET_APB_RSTN_SW_I2C(x)          ( ((x) & 0x1U) << 9U )
#define SOFT_RESET_APB_RSTN_SW_PWM_POS        10U
#define SOFT_RESET_APB_RSTN_SW_PWM_RST        0x0U
#define SOFT_RESET_APB_RSTN_SW_PWM_MSK        ( 0x1U << 10U )
#define SOFT_RESET_APB_RSTN_SW_PWM(x)          ( ((x) & 0x1U) << 10U )

// 0x28, sys_ctrl_status
#define SYS_CTRL_STATUS_OFFSET        0x28U
#define SYS_CTRL_STATUS_SPI_SLEEP_0_POS        0U
#define SYS_CTRL_STATUS_SPI_SLEEP_0_RST        0x1U
#define SYS_CTRL_STATUS_SPI_SLEEP_0_MSK        ( 0x1U << 0U )
#define SYS_CTRL_STATUS_SPI_SLEEP_0(x)          ( ((x) & 0x1U) << 0U )
#define SYS_CTRL_STATUS_SPI_SLEEP_1_POS        1U
#define SYS_CTRL_STATUS_SPI_SLEEP_1_RST        0x1U
#define SYS_CTRL_STATUS_SPI_SLEEP_1_MSK        ( 0x1U << 1U )
#define SYS_CTRL_STATUS_SPI_SLEEP_1(x)          ( ((x) & 0x1U) << 1U )

// 0x2c, iomux_sel
#define IOMUX_SEL_OFFSET        0x2cU
#define IOMUX_SEL_SEL_MCU_0_POS        0U
#define IOMUX_SEL_SEL_MCU_0_RST        0x0U
#define IOMUX_SEL_SEL_MCU_0_MSK        ( 0x3U << 0U )
#define IOMUX_SEL_SEL_MCU_0(x)          ( ((x) & 0x3U) << 0U )
#define IOMUX_SEL_SEL_MCU_1_POS        2U
#define IOMUX_SEL_SEL_MCU_1_RST        0x0U
#define IOMUX_SEL_SEL_MCU_1_MSK        ( 0x3U << 2U )
#define IOMUX_SEL_SEL_MCU_1(x)          ( ((x) & 0x3U) << 2U )
#define IOMUX_SEL_SEL_MCU_2_POS        4U
#define IOMUX_SEL_SEL_MCU_2_RST        0x0U
#define IOMUX_SEL_SEL_MCU_2_MSK        ( 0x3U << 4U )
#define IOMUX_SEL_SEL_MCU_2(x)          ( ((x) & 0x3U) << 4U )
#define IOMUX_SEL_SEL_MCU_3_POS        6U
#define IOMUX_SEL_SEL_MCU_3_RST        0x0U
#define IOMUX_SEL_SEL_MCU_3_MSK        ( 0x3U << 6U )
#define IOMUX_SEL_SEL_MCU_3(x)          ( ((x) & 0x3U) << 6U )
#define IOMUX_SEL_SEL_MCU_4_POS        8U
#define IOMUX_SEL_SEL_MCU_4_RST        0x0U
#define IOMUX_SEL_SEL_MCU_4_MSK        ( 0x3U << 8U )
#define IOMUX_SEL_SEL_MCU_4(x)          ( ((x) & 0x3U) << 8U )
#define IOMUX_SEL_SEL_MCU_5_POS        10U
#define IOMUX_SEL_SEL_MCU_5_RST        0x0U
#define IOMUX_SEL_SEL_MCU_5_MSK        ( 0x3U << 10U )
#define IOMUX_SEL_SEL_MCU_5(x)          ( ((x) & 0x3U) << 10U )
#define IOMUX_SEL_SEL_MCU_6_POS        12U
#define IOMUX_SEL_SEL_MCU_6_RST        0x0U
#define IOMUX_SEL_SEL_MCU_6_MSK        ( 0x3U << 12U )
#define IOMUX_SEL_SEL_MCU_6(x)          ( ((x) & 0x3U) << 12U )
#define IOMUX_SEL_SEL_MCU_7_POS        14U
#define IOMUX_SEL_SEL_MCU_7_RST        0x0U
#define IOMUX_SEL_SEL_MCU_7_MSK        ( 0x3U << 14U )
#define IOMUX_SEL_SEL_MCU_7(x)          ( ((x) & 0x3U) << 14U )
#define IOMUX_SEL_SEL_MCU_8_POS        16U
#define IOMUX_SEL_SEL_MCU_8_RST        0x0U
#define IOMUX_SEL_SEL_MCU_8_MSK        ( 0x3U << 16U )
#define IOMUX_SEL_SEL_MCU_8(x)          ( ((x) & 0x3U) << 16U )
#define IOMUX_SEL_SEL_MCU_9_POS        18U
#define IOMUX_SEL_SEL_MCU_9_RST        0x0U
#define IOMUX_SEL_SEL_MCU_9_MSK        ( 0x3U << 18U )
#define IOMUX_SEL_SEL_MCU_9(x)          ( ((x) & 0x3U) << 18U )
#define IOMUX_SEL_SEL_AON_0_POS        20U
#define IOMUX_SEL_SEL_AON_0_RST        0x0U
#define IOMUX_SEL_SEL_AON_0_MSK        ( 0x3U << 20U )
#define IOMUX_SEL_SEL_AON_0(x)          ( ((x) & 0x3U) << 20U )
#define IOMUX_SEL_SEL_AON_1_POS        22U
#define IOMUX_SEL_SEL_AON_1_RST        0x0U
#define IOMUX_SEL_SEL_AON_1_MSK        ( 0x3U << 22U )
#define IOMUX_SEL_SEL_AON_1(x)          ( ((x) & 0x3U) << 22U )
#define IOMUX_SEL_SEL_AON_3_POS        24U
#define IOMUX_SEL_SEL_AON_3_RST        0x0U
#define IOMUX_SEL_SEL_AON_3_MSK        ( 0x3U << 24U )
#define IOMUX_SEL_SEL_AON_3(x)          ( ((x) & 0x3U) << 24U )
#define IOMUX_SEL_SEL_AON_4_POS        26U
#define IOMUX_SEL_SEL_AON_4_RST        0x0U
#define IOMUX_SEL_SEL_AON_4_MSK        ( 0x3U << 26U )
#define IOMUX_SEL_SEL_AON_4(x)          ( ((x) & 0x3U) << 26U )
#define IOMUX_SEL_SEL_AON_5_POS        28U
#define IOMUX_SEL_SEL_AON_5_RST        0x0U
#define IOMUX_SEL_SEL_AON_5_MSK        ( 0x3U << 28U )
#define IOMUX_SEL_SEL_AON_5(x)          ( ((x) & 0x3U) << 28U )
#define IOMUX_SEL_SEL_AON_6_POS        30U
#define IOMUX_SEL_SEL_AON_6_RST        0x0U
#define IOMUX_SEL_SEL_AON_6_MSK        ( 0x3U << 30U )
#define IOMUX_SEL_SEL_AON_6(x)          ( ((x) & 0x3U) << 30U )

// 0x30, gpio_mcu_sel
#define GPIO_MCU_SEL_OFFSET        0x30U
#define GPIO_MCU_SEL_SELECT_POS        0U
#define GPIO_MCU_SEL_SELECT_RST        0x0U
#define GPIO_MCU_SEL_SELECT_MSK        ( 0xfffffU << 0U )
#define GPIO_MCU_SEL_SELECT(x)          ( ((x) & 0xfffffU) << 0U )
#define GPIO_MCU_SEL_GPIO_IN_SEL_POS        20U
#define GPIO_MCU_SEL_GPIO_IN_SEL_RST        0x0U
#define GPIO_MCU_SEL_GPIO_IN_SEL_MSK        ( 0x1U << 20U )
#define GPIO_MCU_SEL_GPIO_IN_SEL(x)          ( ((x) & 0x1U) << 20U )
#define GPIO_MCU_SEL_SEL_AON_2_POS        21U
#define GPIO_MCU_SEL_SEL_AON_2_RST        0x2U
#define GPIO_MCU_SEL_SEL_AON_2_MSK        ( 0x3U << 21U )
#define GPIO_MCU_SEL_SEL_AON_2(x)          ( ((x) & 0x3U) << 21U )
#define GPIO_MCU_SEL_SEL_AON_7_POS        23U
#define GPIO_MCU_SEL_SEL_AON_7_RST        0x2U
#define GPIO_MCU_SEL_SEL_AON_7_MSK        ( 0x3U << 23U )
#define GPIO_MCU_SEL_SEL_AON_7(x)          ( ((x) & 0x3U) << 23U )

// 0x34, debug_mcu
#define DEBUG_MCU_OFFSET        0x34U
#define DEBUG_MCU_EN_POS        0U
#define DEBUG_MCU_EN_RST        0x0U
#define DEBUG_MCU_EN_MSK        ( 0x3ffU << 0U )
#define DEBUG_MCU_EN(x)          ( ((x) & 0x3ffU) << 0U )
#define DEBUG_MCU_SELECT_POS        10U
#define DEBUG_MCU_SELECT_RST        0x0U
#define DEBUG_MCU_SELECT_MSK        ( 0x7U << 10U )
#define DEBUG_MCU_SELECT(x)          ( ((x) & 0x7U) << 10U )

// 0x38, gpio_mcu_0_3
#define GPIO_MCU_0_3_OFFSET        0x38U
#define GPIO_MCU_0_3_GPIO_MCU0_O_POS        0U
#define GPIO_MCU_0_3_GPIO_MCU0_O_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU0_O_MSK        ( 0x1U << 0U )
#define GPIO_MCU_0_3_GPIO_MCU0_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_MCU_0_3_GPIO_MCU0_OE_N_POS        1U
#define GPIO_MCU_0_3_GPIO_MCU0_OE_N_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU0_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_MCU_0_3_GPIO_MCU0_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_MCU_0_3_GPIO_MCU0_TIE_UP_POS        2U
#define GPIO_MCU_0_3_GPIO_MCU0_TIE_UP_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU0_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_MCU_0_3_GPIO_MCU0_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_MCU_0_3_GPIO_MCU0_TIE_DN_POS        3U
#define GPIO_MCU_0_3_GPIO_MCU0_TIE_DN_RST        0x1U
#define GPIO_MCU_0_3_GPIO_MCU0_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_MCU_0_3_GPIO_MCU0_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_MCU_0_3_GPIO_MCU0_DRV_SEL_POS        4U
#define GPIO_MCU_0_3_GPIO_MCU0_DRV_SEL_RST        0x3U
#define GPIO_MCU_0_3_GPIO_MCU0_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_MCU_0_3_GPIO_MCU0_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_MCU_0_3_GPIO_MCU0_ANA_EN_POS        6U
#define GPIO_MCU_0_3_GPIO_MCU0_ANA_EN_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU0_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_MCU_0_3_GPIO_MCU0_ANA_EN(x)          ( ((x) & 0x1U) << 6U )
#define GPIO_MCU_0_3_GPIO_MCU1_O_POS        8U
#define GPIO_MCU_0_3_GPIO_MCU1_O_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU1_O_MSK        ( 0x1U << 8U )
#define GPIO_MCU_0_3_GPIO_MCU1_O(x)          ( ((x) & 0x1U) << 8U )
#define GPIO_MCU_0_3_GPIO_MCU1_OE_N_POS        9U
#define GPIO_MCU_0_3_GPIO_MCU1_OE_N_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU1_OE_N_MSK        ( 0x1U << 9U )
#define GPIO_MCU_0_3_GPIO_MCU1_OE_N(x)          ( ((x) & 0x1U) << 9U )
#define GPIO_MCU_0_3_GPIO_MCU1_TIE_UP_POS        10U
#define GPIO_MCU_0_3_GPIO_MCU1_TIE_UP_RST        0x1U
#define GPIO_MCU_0_3_GPIO_MCU1_TIE_UP_MSK        ( 0x1U << 10U )
#define GPIO_MCU_0_3_GPIO_MCU1_TIE_UP(x)          ( ((x) & 0x1U) << 10U )
#define GPIO_MCU_0_3_GPIO_MCU1_TIE_DN_POS        11U
#define GPIO_MCU_0_3_GPIO_MCU1_TIE_DN_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU1_TIE_DN_MSK        ( 0x1U << 11U )
#define GPIO_MCU_0_3_GPIO_MCU1_TIE_DN(x)          ( ((x) & 0x1U) << 11U )
#define GPIO_MCU_0_3_GPIO_MCU1_DRV_SEL_POS        12U
#define GPIO_MCU_0_3_GPIO_MCU1_DRV_SEL_RST        0x3U
#define GPIO_MCU_0_3_GPIO_MCU1_DRV_SEL_MSK        ( 0x3U << 12U )
#define GPIO_MCU_0_3_GPIO_MCU1_DRV_SEL(x)          ( ((x) & 0x3U) << 12U )
#define GPIO_MCU_0_3_GPIO_MCU1_ANA_EN_POS        14U
#define GPIO_MCU_0_3_GPIO_MCU1_ANA_EN_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU1_ANA_EN_MSK        ( 0x1U << 14U )
#define GPIO_MCU_0_3_GPIO_MCU1_ANA_EN(x)          ( ((x) & 0x1U) << 14U )
#define GPIO_MCU_0_3_GPIO_MCU2_O_POS        16U
#define GPIO_MCU_0_3_GPIO_MCU2_O_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU2_O_MSK        ( 0x1U << 16U )
#define GPIO_MCU_0_3_GPIO_MCU2_O(x)          ( ((x) & 0x1U) << 16U )
#define GPIO_MCU_0_3_GPIO_MCU2_OE_N_POS        17U
#define GPIO_MCU_0_3_GPIO_MCU2_OE_N_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU2_OE_N_MSK        ( 0x1U << 17U )
#define GPIO_MCU_0_3_GPIO_MCU2_OE_N(x)          ( ((x) & 0x1U) << 17U )
#define GPIO_MCU_0_3_GPIO_MCU2_TIE_UP_POS        18U
#define GPIO_MCU_0_3_GPIO_MCU2_TIE_UP_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU2_TIE_UP_MSK        ( 0x1U << 18U )
#define GPIO_MCU_0_3_GPIO_MCU2_TIE_UP(x)          ( ((x) & 0x1U) << 18U )
#define GPIO_MCU_0_3_GPIO_MCU2_TIE_DN_POS        19U
#define GPIO_MCU_0_3_GPIO_MCU2_TIE_DN_RST        0x1U
#define GPIO_MCU_0_3_GPIO_MCU2_TIE_DN_MSK        ( 0x1U << 19U )
#define GPIO_MCU_0_3_GPIO_MCU2_TIE_DN(x)          ( ((x) & 0x1U) << 19U )
#define GPIO_MCU_0_3_GPIO_MCU2_DRV_SEL_POS        20U
#define GPIO_MCU_0_3_GPIO_MCU2_DRV_SEL_RST        0x3U
#define GPIO_MCU_0_3_GPIO_MCU2_DRV_SEL_MSK        ( 0x3U << 20U )
#define GPIO_MCU_0_3_GPIO_MCU2_DRV_SEL(x)          ( ((x) & 0x3U) << 20U )
#define GPIO_MCU_0_3_GPIO_MCU2_ANA_EN_POS        22U
#define GPIO_MCU_0_3_GPIO_MCU2_ANA_EN_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU2_ANA_EN_MSK        ( 0x1U << 22U )
#define GPIO_MCU_0_3_GPIO_MCU2_ANA_EN(x)          ( ((x) & 0x1U) << 22U )
#define GPIO_MCU_0_3_GPIO_MCU3_O_POS        24U
#define GPIO_MCU_0_3_GPIO_MCU3_O_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU3_O_MSK        ( 0x1U << 24U )
#define GPIO_MCU_0_3_GPIO_MCU3_O(x)          ( ((x) & 0x1U) << 24U )
#define GPIO_MCU_0_3_GPIO_MCU3_OE_N_POS        25U
#define GPIO_MCU_0_3_GPIO_MCU3_OE_N_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU3_OE_N_MSK        ( 0x1U << 25U )
#define GPIO_MCU_0_3_GPIO_MCU3_OE_N(x)          ( ((x) & 0x1U) << 25U )
#define GPIO_MCU_0_3_GPIO_MCU3_TIE_UP_POS        26U
#define GPIO_MCU_0_3_GPIO_MCU3_TIE_UP_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU3_TIE_UP_MSK        ( 0x1U << 26U )
#define GPIO_MCU_0_3_GPIO_MCU3_TIE_UP(x)          ( ((x) & 0x1U) << 26U )
#define GPIO_MCU_0_3_GPIO_MCU3_TIE_DN_POS        27U
#define GPIO_MCU_0_3_GPIO_MCU3_TIE_DN_RST        0x1U
#define GPIO_MCU_0_3_GPIO_MCU3_TIE_DN_MSK        ( 0x1U << 27U )
#define GPIO_MCU_0_3_GPIO_MCU3_TIE_DN(x)          ( ((x) & 0x1U) << 27U )
#define GPIO_MCU_0_3_GPIO_MCU3_DRV_SEL_POS        28U
#define GPIO_MCU_0_3_GPIO_MCU3_DRV_SEL_RST        0x3U
#define GPIO_MCU_0_3_GPIO_MCU3_DRV_SEL_MSK        ( 0x3U << 28U )
#define GPIO_MCU_0_3_GPIO_MCU3_DRV_SEL(x)          ( ((x) & 0x3U) << 28U )
#define GPIO_MCU_0_3_GPIO_MCU3_ANA_EN_POS        30U
#define GPIO_MCU_0_3_GPIO_MCU3_ANA_EN_RST        0x0U
#define GPIO_MCU_0_3_GPIO_MCU3_ANA_EN_MSK        ( 0x1U << 30U )
#define GPIO_MCU_0_3_GPIO_MCU3_ANA_EN(x)          ( ((x) & 0x1U) << 30U )

// 0x3c, gpio_mcu_4_7
#define GPIO_MCU_4_7_OFFSET        0x3cU
#define GPIO_MCU_4_7_GPIO_MCU4_O_POS        0U
#define GPIO_MCU_4_7_GPIO_MCU4_O_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU4_O_MSK        ( 0x1U << 0U )
#define GPIO_MCU_4_7_GPIO_MCU4_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_MCU_4_7_GPIO_MCU4_OE_N_POS        1U
#define GPIO_MCU_4_7_GPIO_MCU4_OE_N_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU4_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_MCU_4_7_GPIO_MCU4_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_MCU_4_7_GPIO_MCU4_TIE_UP_POS        2U
#define GPIO_MCU_4_7_GPIO_MCU4_TIE_UP_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU4_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_MCU_4_7_GPIO_MCU4_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_MCU_4_7_GPIO_MCU4_TIE_DN_POS        3U
#define GPIO_MCU_4_7_GPIO_MCU4_TIE_DN_RST        0x1U
#define GPIO_MCU_4_7_GPIO_MCU4_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_MCU_4_7_GPIO_MCU4_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_MCU_4_7_GPIO_MCU4_DRV_SEL_POS        4U
#define GPIO_MCU_4_7_GPIO_MCU4_DRV_SEL_RST        0x3U
#define GPIO_MCU_4_7_GPIO_MCU4_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_MCU_4_7_GPIO_MCU4_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_MCU_4_7_GPIO_MCU4_ANA_EN_POS        6U
#define GPIO_MCU_4_7_GPIO_MCU4_ANA_EN_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU4_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_MCU_4_7_GPIO_MCU4_ANA_EN(x)          ( ((x) & 0x1U) << 6U )
#define GPIO_MCU_4_7_GPIO_MCU5_O_POS        8U
#define GPIO_MCU_4_7_GPIO_MCU5_O_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU5_O_MSK        ( 0x1U << 8U )
#define GPIO_MCU_4_7_GPIO_MCU5_O(x)          ( ((x) & 0x1U) << 8U )
#define GPIO_MCU_4_7_GPIO_MCU5_OE_N_POS        9U
#define GPIO_MCU_4_7_GPIO_MCU5_OE_N_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU5_OE_N_MSK        ( 0x1U << 9U )
#define GPIO_MCU_4_7_GPIO_MCU5_OE_N(x)          ( ((x) & 0x1U) << 9U )
#define GPIO_MCU_4_7_GPIO_MCU5_TIE_UP_POS        10U
#define GPIO_MCU_4_7_GPIO_MCU5_TIE_UP_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU5_TIE_UP_MSK        ( 0x1U << 10U )
#define GPIO_MCU_4_7_GPIO_MCU5_TIE_UP(x)          ( ((x) & 0x1U) << 10U )
#define GPIO_MCU_4_7_GPIO_MCU5_TIE_DN_POS        11U
#define GPIO_MCU_4_7_GPIO_MCU5_TIE_DN_RST        0x1U
#define GPIO_MCU_4_7_GPIO_MCU5_TIE_DN_MSK        ( 0x1U << 11U )
#define GPIO_MCU_4_7_GPIO_MCU5_TIE_DN(x)          ( ((x) & 0x1U) << 11U )
#define GPIO_MCU_4_7_GPIO_MCU5_DRV_SEL_POS        12U
#define GPIO_MCU_4_7_GPIO_MCU5_DRV_SEL_RST        0x3U
#define GPIO_MCU_4_7_GPIO_MCU5_DRV_SEL_MSK        ( 0x3U << 12U )
#define GPIO_MCU_4_7_GPIO_MCU5_DRV_SEL(x)          ( ((x) & 0x3U) << 12U )
#define GPIO_MCU_4_7_GPIO_MCU5_ANA_EN_POS        14U
#define GPIO_MCU_4_7_GPIO_MCU5_ANA_EN_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU5_ANA_EN_MSK        ( 0x1U << 14U )
#define GPIO_MCU_4_7_GPIO_MCU5_ANA_EN(x)          ( ((x) & 0x1U) << 14U )
#define GPIO_MCU_4_7_GPIO_MCU6_O_POS        16U
#define GPIO_MCU_4_7_GPIO_MCU6_O_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU6_O_MSK        ( 0x1U << 16U )
#define GPIO_MCU_4_7_GPIO_MCU6_O(x)          ( ((x) & 0x1U) << 16U )
#define GPIO_MCU_4_7_GPIO_MCU6_OE_N_POS        17U
#define GPIO_MCU_4_7_GPIO_MCU6_OE_N_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU6_OE_N_MSK        ( 0x1U << 17U )
#define GPIO_MCU_4_7_GPIO_MCU6_OE_N(x)          ( ((x) & 0x1U) << 17U )
#define GPIO_MCU_4_7_GPIO_MCU6_TIE_UP_POS        18U
#define GPIO_MCU_4_7_GPIO_MCU6_TIE_UP_RST        0x1U
#define GPIO_MCU_4_7_GPIO_MCU6_TIE_UP_MSK        ( 0x1U << 18U )
#define GPIO_MCU_4_7_GPIO_MCU6_TIE_UP(x)          ( ((x) & 0x1U) << 18U )
#define GPIO_MCU_4_7_GPIO_MCU6_TIE_DN_POS        19U
#define GPIO_MCU_4_7_GPIO_MCU6_TIE_DN_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU6_TIE_DN_MSK        ( 0x1U << 19U )
#define GPIO_MCU_4_7_GPIO_MCU6_TIE_DN(x)          ( ((x) & 0x1U) << 19U )
#define GPIO_MCU_4_7_GPIO_MCU6_DRV_SEL_POS        20U
#define GPIO_MCU_4_7_GPIO_MCU6_DRV_SEL_RST        0x3U
#define GPIO_MCU_4_7_GPIO_MCU6_DRV_SEL_MSK        ( 0x3U << 20U )
#define GPIO_MCU_4_7_GPIO_MCU6_DRV_SEL(x)          ( ((x) & 0x3U) << 20U )
#define GPIO_MCU_4_7_GPIO_MCU6_ANA_EN_POS        22U
#define GPIO_MCU_4_7_GPIO_MCU6_ANA_EN_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU6_ANA_EN_MSK        ( 0x1U << 22U )
#define GPIO_MCU_4_7_GPIO_MCU6_ANA_EN(x)          ( ((x) & 0x1U) << 22U )
#define GPIO_MCU_4_7_GPIO_MCU7_O_POS        24U
#define GPIO_MCU_4_7_GPIO_MCU7_O_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU7_O_MSK        ( 0x1U << 24U )
#define GPIO_MCU_4_7_GPIO_MCU7_O(x)          ( ((x) & 0x1U) << 24U )
#define GPIO_MCU_4_7_GPIO_MCU7_OE_N_POS        25U
#define GPIO_MCU_4_7_GPIO_MCU7_OE_N_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU7_OE_N_MSK        ( 0x1U << 25U )
#define GPIO_MCU_4_7_GPIO_MCU7_OE_N(x)          ( ((x) & 0x1U) << 25U )
#define GPIO_MCU_4_7_GPIO_MCU7_TIE_UP_POS        26U
#define GPIO_MCU_4_7_GPIO_MCU7_TIE_UP_RST        0x1U
#define GPIO_MCU_4_7_GPIO_MCU7_TIE_UP_MSK        ( 0x1U << 26U )
#define GPIO_MCU_4_7_GPIO_MCU7_TIE_UP(x)          ( ((x) & 0x1U) << 26U )
#define GPIO_MCU_4_7_GPIO_MCU7_TIE_DN_POS        27U
#define GPIO_MCU_4_7_GPIO_MCU7_TIE_DN_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU7_TIE_DN_MSK        ( 0x1U << 27U )
#define GPIO_MCU_4_7_GPIO_MCU7_TIE_DN(x)          ( ((x) & 0x1U) << 27U )
#define GPIO_MCU_4_7_GPIO_MCU7_DRV_SEL_POS        28U
#define GPIO_MCU_4_7_GPIO_MCU7_DRV_SEL_RST        0x3U
#define GPIO_MCU_4_7_GPIO_MCU7_DRV_SEL_MSK        ( 0x3U << 28U )
#define GPIO_MCU_4_7_GPIO_MCU7_DRV_SEL(x)          ( ((x) & 0x3U) << 28U )
#define GPIO_MCU_4_7_GPIO_MCU7_ANA_EN_POS        30U
#define GPIO_MCU_4_7_GPIO_MCU7_ANA_EN_RST        0x0U
#define GPIO_MCU_4_7_GPIO_MCU7_ANA_EN_MSK        ( 0x1U << 30U )
#define GPIO_MCU_4_7_GPIO_MCU7_ANA_EN(x)          ( ((x) & 0x1U) << 30U )

// 0x40, gpio_mcu_8_11
#define GPIO_MCU_8_11_OFFSET        0x40U
#define GPIO_MCU_8_11_GPIO_MCU8_O_POS        0U
#define GPIO_MCU_8_11_GPIO_MCU8_O_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU8_O_MSK        ( 0x1U << 0U )
#define GPIO_MCU_8_11_GPIO_MCU8_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_MCU_8_11_GPIO_MCU8_OE_N_POS        1U
#define GPIO_MCU_8_11_GPIO_MCU8_OE_N_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU8_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_MCU_8_11_GPIO_MCU8_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_MCU_8_11_GPIO_MCU8_TIE_UP_POS        2U
#define GPIO_MCU_8_11_GPIO_MCU8_TIE_UP_RST        0x1U
#define GPIO_MCU_8_11_GPIO_MCU8_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_MCU_8_11_GPIO_MCU8_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_MCU_8_11_GPIO_MCU8_TIE_DN_POS        3U
#define GPIO_MCU_8_11_GPIO_MCU8_TIE_DN_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU8_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_MCU_8_11_GPIO_MCU8_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_MCU_8_11_GPIO_MCU8_DRV_SEL_POS        4U
#define GPIO_MCU_8_11_GPIO_MCU8_DRV_SEL_RST        0x3U
#define GPIO_MCU_8_11_GPIO_MCU8_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_MCU_8_11_GPIO_MCU8_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_MCU_8_11_GPIO_MCU8_ANA_EN_POS        6U
#define GPIO_MCU_8_11_GPIO_MCU8_ANA_EN_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU8_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_MCU_8_11_GPIO_MCU8_ANA_EN(x)          ( ((x) & 0x1U) << 6U )
#define GPIO_MCU_8_11_GPIO_MCU9_O_POS        8U
#define GPIO_MCU_8_11_GPIO_MCU9_O_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU9_O_MSK        ( 0x1U << 8U )
#define GPIO_MCU_8_11_GPIO_MCU9_O(x)          ( ((x) & 0x1U) << 8U )
#define GPIO_MCU_8_11_GPIO_MCU9_OE_N_POS        9U
#define GPIO_MCU_8_11_GPIO_MCU9_OE_N_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU9_OE_N_MSK        ( 0x1U << 9U )
#define GPIO_MCU_8_11_GPIO_MCU9_OE_N(x)          ( ((x) & 0x1U) << 9U )
#define GPIO_MCU_8_11_GPIO_MCU9_TIE_UP_POS        10U
#define GPIO_MCU_8_11_GPIO_MCU9_TIE_UP_RST        0x1U
#define GPIO_MCU_8_11_GPIO_MCU9_TIE_UP_MSK        ( 0x1U << 10U )
#define GPIO_MCU_8_11_GPIO_MCU9_TIE_UP(x)          ( ((x) & 0x1U) << 10U )
#define GPIO_MCU_8_11_GPIO_MCU9_TIE_DN_POS        11U
#define GPIO_MCU_8_11_GPIO_MCU9_TIE_DN_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU9_TIE_DN_MSK        ( 0x1U << 11U )
#define GPIO_MCU_8_11_GPIO_MCU9_TIE_DN(x)          ( ((x) & 0x1U) << 11U )
#define GPIO_MCU_8_11_GPIO_MCU9_DRV_SEL_POS        12U
#define GPIO_MCU_8_11_GPIO_MCU9_DRV_SEL_RST        0x3U
#define GPIO_MCU_8_11_GPIO_MCU9_DRV_SEL_MSK        ( 0x3U << 12U )
#define GPIO_MCU_8_11_GPIO_MCU9_DRV_SEL(x)          ( ((x) & 0x3U) << 12U )
#define GPIO_MCU_8_11_GPIO_MCU9_ANA_EN_POS        14U
#define GPIO_MCU_8_11_GPIO_MCU9_ANA_EN_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU9_ANA_EN_MSK        ( 0x1U << 14U )
#define GPIO_MCU_8_11_GPIO_MCU9_ANA_EN(x)          ( ((x) & 0x1U) << 14U )
#define GPIO_MCU_8_11_GPIO_MCU10_O_POS        16U
#define GPIO_MCU_8_11_GPIO_MCU10_O_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU10_O_MSK        ( 0x1U << 16U )
#define GPIO_MCU_8_11_GPIO_MCU10_O(x)          ( ((x) & 0x1U) << 16U )
#define GPIO_MCU_8_11_GPIO_MCU10_OE_N_POS        17U
#define GPIO_MCU_8_11_GPIO_MCU10_OE_N_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU10_OE_N_MSK        ( 0x1U << 17U )
#define GPIO_MCU_8_11_GPIO_MCU10_OE_N(x)          ( ((x) & 0x1U) << 17U )
#define GPIO_MCU_8_11_GPIO_MCU10_TIE_UP_POS        18U
#define GPIO_MCU_8_11_GPIO_MCU10_TIE_UP_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU10_TIE_UP_MSK        ( 0x1U << 18U )
#define GPIO_MCU_8_11_GPIO_MCU10_TIE_UP(x)          ( ((x) & 0x1U) << 18U )
#define GPIO_MCU_8_11_GPIO_MCU10_TIE_DN_POS        19U
#define GPIO_MCU_8_11_GPIO_MCU10_TIE_DN_RST        0x1U
#define GPIO_MCU_8_11_GPIO_MCU10_TIE_DN_MSK        ( 0x1U << 19U )
#define GPIO_MCU_8_11_GPIO_MCU10_TIE_DN(x)          ( ((x) & 0x1U) << 19U )
#define GPIO_MCU_8_11_GPIO_MCU10_DRV_SEL_POS        20U
#define GPIO_MCU_8_11_GPIO_MCU10_DRV_SEL_RST        0x3U
#define GPIO_MCU_8_11_GPIO_MCU10_DRV_SEL_MSK        ( 0x3U << 20U )
#define GPIO_MCU_8_11_GPIO_MCU10_DRV_SEL(x)          ( ((x) & 0x3U) << 20U )
#define GPIO_MCU_8_11_GPIO_MCU10_ANA_EN_POS        22U
#define GPIO_MCU_8_11_GPIO_MCU10_ANA_EN_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU10_ANA_EN_MSK        ( 0x1U << 22U )
#define GPIO_MCU_8_11_GPIO_MCU10_ANA_EN(x)          ( ((x) & 0x1U) << 22U )
#define GPIO_MCU_8_11_GPIO_MCU11_O_POS        24U
#define GPIO_MCU_8_11_GPIO_MCU11_O_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU11_O_MSK        ( 0x1U << 24U )
#define GPIO_MCU_8_11_GPIO_MCU11_O(x)          ( ((x) & 0x1U) << 24U )
#define GPIO_MCU_8_11_GPIO_MCU11_OE_N_POS        25U
#define GPIO_MCU_8_11_GPIO_MCU11_OE_N_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU11_OE_N_MSK        ( 0x1U << 25U )
#define GPIO_MCU_8_11_GPIO_MCU11_OE_N(x)          ( ((x) & 0x1U) << 25U )
#define GPIO_MCU_8_11_GPIO_MCU11_TIE_UP_POS        26U
#define GPIO_MCU_8_11_GPIO_MCU11_TIE_UP_RST        0x1U
#define GPIO_MCU_8_11_GPIO_MCU11_TIE_UP_MSK        ( 0x1U << 26U )
#define GPIO_MCU_8_11_GPIO_MCU11_TIE_UP(x)          ( ((x) & 0x1U) << 26U )
#define GPIO_MCU_8_11_GPIO_MCU11_TIE_DN_POS        27U
#define GPIO_MCU_8_11_GPIO_MCU11_TIE_DN_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU11_TIE_DN_MSK        ( 0x1U << 27U )
#define GPIO_MCU_8_11_GPIO_MCU11_TIE_DN(x)          ( ((x) & 0x1U) << 27U )
#define GPIO_MCU_8_11_GPIO_MCU11_DRV_SEL_POS        28U
#define GPIO_MCU_8_11_GPIO_MCU11_DRV_SEL_RST        0x3U
#define GPIO_MCU_8_11_GPIO_MCU11_DRV_SEL_MSK        ( 0x3U << 28U )
#define GPIO_MCU_8_11_GPIO_MCU11_DRV_SEL(x)          ( ((x) & 0x3U) << 28U )
#define GPIO_MCU_8_11_GPIO_MCU11_ANA_EN_POS        30U
#define GPIO_MCU_8_11_GPIO_MCU11_ANA_EN_RST        0x0U
#define GPIO_MCU_8_11_GPIO_MCU11_ANA_EN_MSK        ( 0x1U << 30U )
#define GPIO_MCU_8_11_GPIO_MCU11_ANA_EN(x)          ( ((x) & 0x1U) << 30U )

// 0x44, gpio_mcu_12_15
#define GPIO_MCU_12_15_OFFSET        0x44U
#define GPIO_MCU_12_15_GPIO_MCU12_O_POS        0U
#define GPIO_MCU_12_15_GPIO_MCU12_O_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU12_O_MSK        ( 0x1U << 0U )
#define GPIO_MCU_12_15_GPIO_MCU12_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_MCU_12_15_GPIO_MCU12_OE_N_POS        1U
#define GPIO_MCU_12_15_GPIO_MCU12_OE_N_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU12_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_MCU_12_15_GPIO_MCU12_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_MCU_12_15_GPIO_MCU12_TIE_UP_POS        2U
#define GPIO_MCU_12_15_GPIO_MCU12_TIE_UP_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU12_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_MCU_12_15_GPIO_MCU12_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_MCU_12_15_GPIO_MCU12_TIE_DN_POS        3U
#define GPIO_MCU_12_15_GPIO_MCU12_TIE_DN_RST        0x1U
#define GPIO_MCU_12_15_GPIO_MCU12_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_MCU_12_15_GPIO_MCU12_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_MCU_12_15_GPIO_MCU12_DRV_SEL_POS        4U
#define GPIO_MCU_12_15_GPIO_MCU12_DRV_SEL_RST        0x3U
#define GPIO_MCU_12_15_GPIO_MCU12_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_MCU_12_15_GPIO_MCU12_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_MCU_12_15_GPIO_MCU12_ANA_EN_POS        6U
#define GPIO_MCU_12_15_GPIO_MCU12_ANA_EN_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU12_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_MCU_12_15_GPIO_MCU12_ANA_EN(x)          ( ((x) & 0x1U) << 6U )
#define GPIO_MCU_12_15_GPIO_MCU13_O_POS        8U
#define GPIO_MCU_12_15_GPIO_MCU13_O_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU13_O_MSK        ( 0x1U << 8U )
#define GPIO_MCU_12_15_GPIO_MCU13_O(x)          ( ((x) & 0x1U) << 8U )
#define GPIO_MCU_12_15_GPIO_MCU13_OE_N_POS        9U
#define GPIO_MCU_12_15_GPIO_MCU13_OE_N_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU13_OE_N_MSK        ( 0x1U << 9U )
#define GPIO_MCU_12_15_GPIO_MCU13_OE_N(x)          ( ((x) & 0x1U) << 9U )
#define GPIO_MCU_12_15_GPIO_MCU13_TIE_UP_POS        10U
#define GPIO_MCU_12_15_GPIO_MCU13_TIE_UP_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU13_TIE_UP_MSK        ( 0x1U << 10U )
#define GPIO_MCU_12_15_GPIO_MCU13_TIE_UP(x)          ( ((x) & 0x1U) << 10U )
#define GPIO_MCU_12_15_GPIO_MCU13_TIE_DN_POS        11U
#define GPIO_MCU_12_15_GPIO_MCU13_TIE_DN_RST        0x1U
#define GPIO_MCU_12_15_GPIO_MCU13_TIE_DN_MSK        ( 0x1U << 11U )
#define GPIO_MCU_12_15_GPIO_MCU13_TIE_DN(x)          ( ((x) & 0x1U) << 11U )
#define GPIO_MCU_12_15_GPIO_MCU13_DRV_SEL_POS        12U
#define GPIO_MCU_12_15_GPIO_MCU13_DRV_SEL_RST        0x3U
#define GPIO_MCU_12_15_GPIO_MCU13_DRV_SEL_MSK        ( 0x3U << 12U )
#define GPIO_MCU_12_15_GPIO_MCU13_DRV_SEL(x)          ( ((x) & 0x3U) << 12U )
#define GPIO_MCU_12_15_GPIO_MCU13_ANA_EN_POS        14U
#define GPIO_MCU_12_15_GPIO_MCU13_ANA_EN_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU13_ANA_EN_MSK        ( 0x1U << 14U )
#define GPIO_MCU_12_15_GPIO_MCU13_ANA_EN(x)          ( ((x) & 0x1U) << 14U )
#define GPIO_MCU_12_15_GPIO_MCU14_O_POS        16U
#define GPIO_MCU_12_15_GPIO_MCU14_O_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU14_O_MSK        ( 0x1U << 16U )
#define GPIO_MCU_12_15_GPIO_MCU14_O(x)          ( ((x) & 0x1U) << 16U )
#define GPIO_MCU_12_15_GPIO_MCU14_OE_N_POS        17U
#define GPIO_MCU_12_15_GPIO_MCU14_OE_N_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU14_OE_N_MSK        ( 0x1U << 17U )
#define GPIO_MCU_12_15_GPIO_MCU14_OE_N(x)          ( ((x) & 0x1U) << 17U )
#define GPIO_MCU_12_15_GPIO_MCU14_TIE_UP_POS        18U
#define GPIO_MCU_12_15_GPIO_MCU14_TIE_UP_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU14_TIE_UP_MSK        ( 0x1U << 18U )
#define GPIO_MCU_12_15_GPIO_MCU14_TIE_UP(x)          ( ((x) & 0x1U) << 18U )
#define GPIO_MCU_12_15_GPIO_MCU14_TIE_DN_POS        19U
#define GPIO_MCU_12_15_GPIO_MCU14_TIE_DN_RST        0x1U
#define GPIO_MCU_12_15_GPIO_MCU14_TIE_DN_MSK        ( 0x1U << 19U )
#define GPIO_MCU_12_15_GPIO_MCU14_TIE_DN(x)          ( ((x) & 0x1U) << 19U )
#define GPIO_MCU_12_15_GPIO_MCU14_DRV_SEL_POS        20U
#define GPIO_MCU_12_15_GPIO_MCU14_DRV_SEL_RST        0x3U
#define GPIO_MCU_12_15_GPIO_MCU14_DRV_SEL_MSK        ( 0x3U << 20U )
#define GPIO_MCU_12_15_GPIO_MCU14_DRV_SEL(x)          ( ((x) & 0x3U) << 20U )
#define GPIO_MCU_12_15_GPIO_MCU14_ANA_EN_POS        22U
#define GPIO_MCU_12_15_GPIO_MCU14_ANA_EN_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU14_ANA_EN_MSK        ( 0x1U << 22U )
#define GPIO_MCU_12_15_GPIO_MCU14_ANA_EN(x)          ( ((x) & 0x1U) << 22U )
#define GPIO_MCU_12_15_GPIO_MCU15_O_POS        24U
#define GPIO_MCU_12_15_GPIO_MCU15_O_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU15_O_MSK        ( 0x1U << 24U )
#define GPIO_MCU_12_15_GPIO_MCU15_O(x)          ( ((x) & 0x1U) << 24U )
#define GPIO_MCU_12_15_GPIO_MCU15_OE_N_POS        25U
#define GPIO_MCU_12_15_GPIO_MCU15_OE_N_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU15_OE_N_MSK        ( 0x1U << 25U )
#define GPIO_MCU_12_15_GPIO_MCU15_OE_N(x)          ( ((x) & 0x1U) << 25U )
#define GPIO_MCU_12_15_GPIO_MCU15_TIE_UP_POS        26U
#define GPIO_MCU_12_15_GPIO_MCU15_TIE_UP_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU15_TIE_UP_MSK        ( 0x1U << 26U )
#define GPIO_MCU_12_15_GPIO_MCU15_TIE_UP(x)          ( ((x) & 0x1U) << 26U )
#define GPIO_MCU_12_15_GPIO_MCU15_TIE_DN_POS        27U
#define GPIO_MCU_12_15_GPIO_MCU15_TIE_DN_RST        0x1U
#define GPIO_MCU_12_15_GPIO_MCU15_TIE_DN_MSK        ( 0x1U << 27U )
#define GPIO_MCU_12_15_GPIO_MCU15_TIE_DN(x)          ( ((x) & 0x1U) << 27U )
#define GPIO_MCU_12_15_GPIO_MCU15_DRV_SEL_POS        28U
#define GPIO_MCU_12_15_GPIO_MCU15_DRV_SEL_RST        0x3U
#define GPIO_MCU_12_15_GPIO_MCU15_DRV_SEL_MSK        ( 0x3U << 28U )
#define GPIO_MCU_12_15_GPIO_MCU15_DRV_SEL(x)          ( ((x) & 0x3U) << 28U )
#define GPIO_MCU_12_15_GPIO_MCU15_ANA_EN_POS        30U
#define GPIO_MCU_12_15_GPIO_MCU15_ANA_EN_RST        0x0U
#define GPIO_MCU_12_15_GPIO_MCU15_ANA_EN_MSK        ( 0x1U << 30U )
#define GPIO_MCU_12_15_GPIO_MCU15_ANA_EN(x)          ( ((x) & 0x1U) << 30U )

// 0x48, gpio_mcu_16_19
#define GPIO_MCU_16_19_OFFSET        0x48U
#define GPIO_MCU_16_19_GPIO_MCU16_O_POS        0U
#define GPIO_MCU_16_19_GPIO_MCU16_O_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU16_O_MSK        ( 0x1U << 0U )
#define GPIO_MCU_16_19_GPIO_MCU16_O(x)          ( ((x) & 0x1U) << 0U )
#define GPIO_MCU_16_19_GPIO_MCU16_OE_N_POS        1U
#define GPIO_MCU_16_19_GPIO_MCU16_OE_N_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU16_OE_N_MSK        ( 0x1U << 1U )
#define GPIO_MCU_16_19_GPIO_MCU16_OE_N(x)          ( ((x) & 0x1U) << 1U )
#define GPIO_MCU_16_19_GPIO_MCU16_TIE_UP_POS        2U
#define GPIO_MCU_16_19_GPIO_MCU16_TIE_UP_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU16_TIE_UP_MSK        ( 0x1U << 2U )
#define GPIO_MCU_16_19_GPIO_MCU16_TIE_UP(x)          ( ((x) & 0x1U) << 2U )
#define GPIO_MCU_16_19_GPIO_MCU16_TIE_DN_POS        3U
#define GPIO_MCU_16_19_GPIO_MCU16_TIE_DN_RST        0x1U
#define GPIO_MCU_16_19_GPIO_MCU16_TIE_DN_MSK        ( 0x1U << 3U )
#define GPIO_MCU_16_19_GPIO_MCU16_TIE_DN(x)          ( ((x) & 0x1U) << 3U )
#define GPIO_MCU_16_19_GPIO_MCU16_DRV_SEL_POS        4U
#define GPIO_MCU_16_19_GPIO_MCU16_DRV_SEL_RST        0x3U
#define GPIO_MCU_16_19_GPIO_MCU16_DRV_SEL_MSK        ( 0x3U << 4U )
#define GPIO_MCU_16_19_GPIO_MCU16_DRV_SEL(x)          ( ((x) & 0x3U) << 4U )
#define GPIO_MCU_16_19_GPIO_MCU16_ANA_EN_POS        6U
#define GPIO_MCU_16_19_GPIO_MCU16_ANA_EN_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU16_ANA_EN_MSK        ( 0x1U << 6U )
#define GPIO_MCU_16_19_GPIO_MCU16_ANA_EN(x)          ( ((x) & 0x1U) << 6U )
#define GPIO_MCU_16_19_GPIO_MCU17_O_POS        8U
#define GPIO_MCU_16_19_GPIO_MCU17_O_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU17_O_MSK        ( 0x1U << 8U )
#define GPIO_MCU_16_19_GPIO_MCU17_O(x)          ( ((x) & 0x1U) << 8U )
#define GPIO_MCU_16_19_GPIO_MCU17_OE_N_POS        9U
#define GPIO_MCU_16_19_GPIO_MCU17_OE_N_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU17_OE_N_MSK        ( 0x1U << 9U )
#define GPIO_MCU_16_19_GPIO_MCU17_OE_N(x)          ( ((x) & 0x1U) << 9U )
#define GPIO_MCU_16_19_GPIO_MCU17_TIE_UP_POS        10U
#define GPIO_MCU_16_19_GPIO_MCU17_TIE_UP_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU17_TIE_UP_MSK        ( 0x1U << 10U )
#define GPIO_MCU_16_19_GPIO_MCU17_TIE_UP(x)          ( ((x) & 0x1U) << 10U )
#define GPIO_MCU_16_19_GPIO_MCU17_TIE_DN_POS        11U
#define GPIO_MCU_16_19_GPIO_MCU17_TIE_DN_RST        0x1U
#define GPIO_MCU_16_19_GPIO_MCU17_TIE_DN_MSK        ( 0x1U << 11U )
#define GPIO_MCU_16_19_GPIO_MCU17_TIE_DN(x)          ( ((x) & 0x1U) << 11U )
#define GPIO_MCU_16_19_GPIO_MCU17_DRV_SEL_POS        12U
#define GPIO_MCU_16_19_GPIO_MCU17_DRV_SEL_RST        0x3U
#define GPIO_MCU_16_19_GPIO_MCU17_DRV_SEL_MSK        ( 0x3U << 12U )
#define GPIO_MCU_16_19_GPIO_MCU17_DRV_SEL(x)          ( ((x) & 0x3U) << 12U )
#define GPIO_MCU_16_19_GPIO_MCU17_ANA_EN_POS        14U
#define GPIO_MCU_16_19_GPIO_MCU17_ANA_EN_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU17_ANA_EN_MSK        ( 0x1U << 14U )
#define GPIO_MCU_16_19_GPIO_MCU17_ANA_EN(x)          ( ((x) & 0x1U) << 14U )
#define GPIO_MCU_16_19_GPIO_MCU18_O_POS        16U
#define GPIO_MCU_16_19_GPIO_MCU18_O_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU18_O_MSK        ( 0x1U << 16U )
#define GPIO_MCU_16_19_GPIO_MCU18_O(x)          ( ((x) & 0x1U) << 16U )
#define GPIO_MCU_16_19_GPIO_MCU18_OE_N_POS        17U
#define GPIO_MCU_16_19_GPIO_MCU18_OE_N_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU18_OE_N_MSK        ( 0x1U << 17U )
#define GPIO_MCU_16_19_GPIO_MCU18_OE_N(x)          ( ((x) & 0x1U) << 17U )
#define GPIO_MCU_16_19_GPIO_MCU18_TIE_UP_POS        18U
#define GPIO_MCU_16_19_GPIO_MCU18_TIE_UP_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU18_TIE_UP_MSK        ( 0x1U << 18U )
#define GPIO_MCU_16_19_GPIO_MCU18_TIE_UP(x)          ( ((x) & 0x1U) << 18U )
#define GPIO_MCU_16_19_GPIO_MCU18_TIE_DN_POS        19U
#define GPIO_MCU_16_19_GPIO_MCU18_TIE_DN_RST        0x1U
#define GPIO_MCU_16_19_GPIO_MCU18_TIE_DN_MSK        ( 0x1U << 19U )
#define GPIO_MCU_16_19_GPIO_MCU18_TIE_DN(x)          ( ((x) & 0x1U) << 19U )
#define GPIO_MCU_16_19_GPIO_MCU18_DRV_SEL_POS        20U
#define GPIO_MCU_16_19_GPIO_MCU18_DRV_SEL_RST        0x3U
#define GPIO_MCU_16_19_GPIO_MCU18_DRV_SEL_MSK        ( 0x3U << 20U )
#define GPIO_MCU_16_19_GPIO_MCU18_DRV_SEL(x)          ( ((x) & 0x3U) << 20U )
#define GPIO_MCU_16_19_GPIO_MCU18_ANA_EN_POS        22U
#define GPIO_MCU_16_19_GPIO_MCU18_ANA_EN_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU18_ANA_EN_MSK        ( 0x1U << 22U )
#define GPIO_MCU_16_19_GPIO_MCU18_ANA_EN(x)          ( ((x) & 0x1U) << 22U )
#define GPIO_MCU_16_19_GPIO_MCU19_O_POS        24U
#define GPIO_MCU_16_19_GPIO_MCU19_O_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU19_O_MSK        ( 0x1U << 24U )
#define GPIO_MCU_16_19_GPIO_MCU19_O(x)          ( ((x) & 0x1U) << 24U )
#define GPIO_MCU_16_19_GPIO_MCU19_OE_N_POS        25U
#define GPIO_MCU_16_19_GPIO_MCU19_OE_N_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU19_OE_N_MSK        ( 0x1U << 25U )
#define GPIO_MCU_16_19_GPIO_MCU19_OE_N(x)          ( ((x) & 0x1U) << 25U )
#define GPIO_MCU_16_19_GPIO_MCU19_TIE_UP_POS        26U
#define GPIO_MCU_16_19_GPIO_MCU19_TIE_UP_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU19_TIE_UP_MSK        ( 0x1U << 26U )
#define GPIO_MCU_16_19_GPIO_MCU19_TIE_UP(x)          ( ((x) & 0x1U) << 26U )
#define GPIO_MCU_16_19_GPIO_MCU19_TIE_DN_POS        27U
#define GPIO_MCU_16_19_GPIO_MCU19_TIE_DN_RST        0x1U
#define GPIO_MCU_16_19_GPIO_MCU19_TIE_DN_MSK        ( 0x1U << 27U )
#define GPIO_MCU_16_19_GPIO_MCU19_TIE_DN(x)          ( ((x) & 0x1U) << 27U )
#define GPIO_MCU_16_19_GPIO_MCU19_DRV_SEL_POS        28U
#define GPIO_MCU_16_19_GPIO_MCU19_DRV_SEL_RST        0x3U
#define GPIO_MCU_16_19_GPIO_MCU19_DRV_SEL_MSK        ( 0x3U << 28U )
#define GPIO_MCU_16_19_GPIO_MCU19_DRV_SEL(x)          ( ((x) & 0x3U) << 28U )
#define GPIO_MCU_16_19_GPIO_MCU19_ANA_EN_POS        30U
#define GPIO_MCU_16_19_GPIO_MCU19_ANA_EN_RST        0x0U
#define GPIO_MCU_16_19_GPIO_MCU19_ANA_EN_MSK        ( 0x1U << 30U )
#define GPIO_MCU_16_19_GPIO_MCU19_ANA_EN(x)          ( ((x) & 0x1U) << 30U )

// 0x4c, gpio_status
#define GPIO_STATUS_OFFSET        0x4cU
#define GPIO_STATUS_GPIO_MCU_IN_POS        0U
#define GPIO_STATUS_GPIO_MCU_IN_RST        0x0U
#define GPIO_STATUS_GPIO_MCU_IN_MSK        ( 0xfffffU << 0U )
#define GPIO_STATUS_GPIO_MCU_IN(x)          ( ((x) & 0xfffffU) << 0U )
#define GPIO_STATUS_GPIO_AON_IN_POS        20U
#define GPIO_STATUS_GPIO_AON_IN_RST        0x0U
#define GPIO_STATUS_GPIO_AON_IN_MSK        ( 0xffU << 20U )
#define GPIO_STATUS_GPIO_AON_IN(x)          ( ((x) & 0xffU) << 20U )

// 0x50, bus_ctrl
#define BUS_CTRL_OFFSET        0x50U
#define BUS_CTRL_FFT_RAM_RDY_SEL_POS        0U
#define BUS_CTRL_FFT_RAM_RDY_SEL_RST        0x0U
#define BUS_CTRL_FFT_RAM_RDY_SEL_MSK        ( 0xfU << 0U )
#define BUS_CTRL_FFT_RAM_RDY_SEL(x)          ( ((x) & 0xfU) << 0U )

#endif
