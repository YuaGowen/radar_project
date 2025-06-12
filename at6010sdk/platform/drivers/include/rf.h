#ifndef __RF_H__
#define __RF_H__

#include "at_types.h"
#include "rf_intf.h"

typedef enum {
    RF_RESET_CTRL = 0x0, //reset_ctrl;    // 0x0
    RF_DEVICE_ID = 0x1, //device_id;    // 0x1
    RF_AFC_PLL_CTRL = 0x2, //afc_pll_ctrl;    // 0x2
    RF_PLL_FREQ_0 = 0x3, //afc_pll_freq_0;    // 0x3
    RF_AFC_PLL_FREQ_1 = 0x4, //afc_pll_freq_1;    // 0x4
    RF_AFC_PLL_FREQ_2 = 0x5, //afc_pll_freq_2;    // 0x5
    RF_AFC_PLL_STATUS_0 = 0x6, //afc_pll_status_0;    // 0x6
    RF_AFC_PLL_STATUS_1 = 0x7, //afc_pll_status_1;    // 0x7
    RF_PLL_SDM_CTRL = 0x8, //pll_sdm_ctrl;    // 0x8
    RF_PLL_SDM_FREQ_0 = 0x9, //pll_sdm_freq_0;    // 0x9
    RF_PLL_SDM_FREQ_1 = 0xa, //pll_sdm_freq_1;    // 0xa
    RF_PLL_SDM_FREQ_2 = 0xb, //pll_sdm_freq_2;    // 0xb
    RF_PLL_SDM_FREQ_3 = 0xc, //pll_sdm_freq_3;    // 0xc
    RF_PLL_SDM_STATUS_0 = 0xd, //pll_sdm_status_0;    // 0xd
    RF_PLL_SDM_STATUS_1 = 0xe, //pll_sdm_status_1;    // 0xe
    RF_FMCW_CTRL = 0xf, //fmcw_ctrl;    // 0xf
    RF_FMCW_START_0 = 0x10, //fmcw_start_0;    // 0x10
    RF_FMCW_START_1 = 0x11, //fmcw_start_1;    // 0x11
    RF_FMCW_START_2 = 0x12, //fmcw_start_2;    // 0x12
    RF_FMCW_START_3 = 0x13, //fmcw_start_3;    // 0x13
    RF_FMCW_STEP_0 = 0x14, //fmcw_step_0;    // 0x14
    RF_FMCW_STEP_1 = 0x15, //fmcw_step_1;    // 0x15
    RF_FMCW_NUM_0 = 0x16, //fmcw_num_0;    // 0x16
    RF_FMCW_NUM_1 = 0x17, //fmcw_num_1;    // 0x17
    RF_ADC_CTRL = 0x18, //adc_ctrl;    // 0x18
    RF_ADC_STATUS_0 = 0x19, //adc_status_0;    // 0x19
    RF_ADC_STATUS_1 = 0x1a, //adc_status_1;    // 0x1a
    RF_IRQ_CTRL = 0xab, //irq_ctrl;    // 0x1b
    RF_IRQ_STATUS = 0x1c, //irq_status;    // 0x1c
    RF_RX_RF_0 = 0x1d, //rx_rf_0;    // 0x1d
    RF_RX_RF_1 = 0x1e, //rx_rf_1;    // 0x1e
    RF_RX_RF_2 = 0x1f, //rx_rf_2;    // 0x1f
    RF_RX_FILT_0 = 0x20, //rx_filt_0;    // 0x20
    RF_RX_FILT_1 = 0x21, //rx_filt_1;    // 0x21
    RF_OSC_LDO_0 = 0x22, //osc_ldo_0;    // 0x22
    RF_OSC_LDO_1 = 0x23, //osc_ldo_1;    // 0x23
    RF_OSC_LDO_2 = 0x24, //osc_ldo_2;    // 0x24
    RF_VCO_PA_0 = 0x25, //vco_pa_0;    // 0x25
    RF_VCO_PA_1 = 0x26, //vco_pa_1;    // 0x26
    RF_VCO_PA_2 = 0x27, //vco_pa_2;    // 0x27
    RF_VCO_PA_3 = 0x28, //vco_pa_3;    // 0x28
    RF_PLL_0 = 0x29, //pll_0;    // 0x29
    RF_PLL_1 = 0x2a, //pll_1;    // 0x2a
    RF_PLL_2 = 0x2b, //pll_2;    // 0x2b
    RF_PLL_3 = 0x2c, //pll_3;    // 0x2c
    RF_TEST = 0x2d, //test;    // 0x2d
    RF_PWR_CTRL = 0x2e, //pwr_ctrl;    // 0x2e
}rf_reg_t;

void rf_domain_init(void);
u32 rf_reg_read(u32 reg);
u8 rf_reg_write(u32 reg, u32 dat);
u8 rf_reg_field_write(u32 reg, u32 field_mask, u32 field_val);

#endif

