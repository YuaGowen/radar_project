#include <stdio.h>
#include <string.h>
#include "deep_sleep_mem_main.h"
#include "clock.h"
#include "pmu.h"
#include "radar_frame.h"
#include "gpio.h"
#include "hal_clock.h"
#include "hal_uart.h"
#include "hal_dualtimer.h"
#include "hal_fft.h"
#include "hal_timer.h"
#include "hal_radar.h"
#include "common.h"
#include "radar_fmcw.h"
#include "output_config.h"
#include "memory_map.h"
#include "ulp_mem.h"
#include "at_malloc.h"
#include "at_malloc_defs.h"

#define DBG_RAMRUN_MODE           0 /* 0: boot from the SRAM1, 1: boot from a specified addr. 2:boot from Flash */
#define DGB_RAMRUN_ADDR           0x20008000 /* Set the startup address if boot from a specified address. */
#define DBG_MEASURE_INIT_TIME     0
#define DBG_DUMP_RADAR_DATA       1
#define FRAME_CAP_NUM_ONE_TIME    1
#define FRAME_INTERVAL_MS         (250)

#if (DBG_DUMP_RADAR_DATA == 1)
#define SLEEP_DELAY_TIME_MS       12
#else
#define SLEEP_DELAY_TIME_MS       3
#endif

/* retention memory structure definition. */
typedef struct deep_sleep_ret_mem {
    u32 rx_subfrm_id;
} deep_sleep_ret_mem_t;

/* Private Variables ----------------------------------------------------------------------------- */
#if DEMO_TYPE_USING == DEMO_TYPE_RAWDATA
short GsFrameBuf[NUM_CHIRP][NUM_SAMPLE_PER_CHIRP][NUM_ANT] FFT_BUF_ATTRIBUTES = {0}; /* 容纳1帧ADC数据的缓存 */
#elif DEMO_TYPE_USING == DEMO_TYPE_RANGEFFT
short GsFrameBuf[NUM_ANT][FFT_OUT_VALID_RB][NUM_CHIRP][2] FFT_BUF_ATTRIBUTES = {0}; /* 容纳1帧数据的缓存 */
#endif
static u32 *fft_output_buffer = (u32 *)&GsFrameBuf;

volatile radar_frame_hdr_t GtFrameHeader = {0}; /* 帧头 */
static radar_frame_hdr_t *frame_header =  (radar_frame_hdr_t*)&GtFrameHeader;

static volatile enum_fifo_wp_status fifo_state = 0;

static timer_id_t radar_worker_timer_id = TIMER_ID_INVALID;

#if defined(WIN_ENABLE) && defined(ULP_PROG_1)
static u16 win[] FFT_WIN_ATTRIBUTES = {
    #include "win_hann.tab"
};
#endif

/* Private Functions Declarations ---------------------------------------------------------------- */
static void fft_end_isr(void);
static u32  get_current_data_addr(void);

void app_init(void);
void app_rawdata_config(void);
void app_rangefft_config(void);
int  app_radar_init(void);
int  app_radar_start(void);
int  app_radar_result(void);
int  app_radar_deinit(void);

/* Private Functions Implementations ------------------------------------------------------------- */

static void fft_end_isr(void)
{
    fifo_state = DEV_FIFO_WP_END;
    frame_header->radar_common_cfg.rxSubFrameId++;
}

static u32 get_current_data_addr(void)
{
    u32 d_update = 0;

    critical_section_start();
    if(fifo_state != DEV_FIFO_WP_NA) {
        d_update = (u32)fft_output_buffer;
    }
    critical_section_end();

    return d_update;
}

/* Fake Deep Sleep ------------------------------------------------------------------------------- */

__STATIC_INLINE void mcu_init(void)
{
    /* System clock initialization */
    hal_clk_init(SYS_FLASH_CLK_SEL);
    hal_dualtimer_init();
    sys_clk_enable_dma();
    sys_clk_enable_gpio();

    /* Log Uart Init */
    uart_cfg_t uart_cfg = {0};
    uart_cfg.baud_rate = UART_BAUD_RATE_921600;
    uart_cfg.parity = UART_PARITY_DISABLE;
    uart_cfg.stop_bits = UART_STOP_BITS_1;
    uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
    uart_cfg.fifo_cfg.fifo_en = 1U;
    hal_uart_init(UART_LOG_PORT, &uart_cfg);

    #if DBG_MEASURE_INIT_TIME == 1
    sys_clk_enable_gpio();
    gpio_toggle_init(GPIO_AON_2);
    gpio_set_value(GPIO_AON_2, 0U);
    #endif
}

void app_init(void)
{
    u32 id = 0;
    mcu_init();

    #ifdef ULP_PROG_1
    ulp_mem_init();
    #endif

    #if DBG_MEASURE_INIT_TIME == 1
    gpio_set_value(GPIO_AON_2, 1U);
    #endif

    /* FFT: Raw ADC Configuration */
    #ifdef ULP_PROG_1
    hal_fft_init();
    #else
    hal_fft_ulp_init();

    #if DEMO_TYPE_USING == DEMO_TYPE_RAWDATA
    id = FFT_ADC_RDAT_MODE;
    #elif DEMO_TYPE_USING == DEMO_TYPE_RANGEFFT
    id = FFT_ADC_RANGE_FFT_MODE;
    #endif
    hal_fft_notify_set(id,NULL,fft_end_isr);
    #endif
    /* output data configuration */
    output_dma_config();

    frame_header_cfg((u8*)frame_header);

    #ifdef ULP_PROG_2
    radar_fmcw_burst_cfg();
    #else
    radar_fmcw_cfg();
    app_radar_init();
    #endif

    #if DBG_MEASURE_INIT_TIME == 1
    gpio_set_value(GPIO_AON_2, 0U);
    #endif
}

#ifdef ULP_PROG_1
void app_rawdata_config(void)
{
    adc_rdat_para_t fft_rdat_cfg;
    fft_rdat_cfg.enable = 1;
    fft_rdat_cfg.num_adc_ch = FFT_MONO_ADC_CH;
    #if NUM_ANT == 2
    fft_rdat_cfg.num_adc_ch = FFT_DUAL_ADC_CH;
    #endif
    fft_rdat_cfg.nframe = NUM_CHIRP;
    fft_rdat_cfg.frame_size = NUM_SAMPLE_PER_CHIRP;
    fft_rdat_cfg.out_buf = (u32)fft_output_buffer;
    fft_rdat_cfg.end_callback = fft_end_isr;
    fft_rdat_cfg.half_callback = NULL;
    fft_rdat_cfg.half_int_en = 0;
    hal_fft_set_config(FFT_IDX_CONFIG_ADC_RDAT,(void*)&fft_rdat_cfg);
}

void app_rangefft_config(void)
{
    /* Window */
    fft_win_para_t win_cfg = {
        #ifdef WIN_ENABLE
        .enable = 1,
        .waddr    = (u32)win,
        #else
        .enable = 0,
        .waddr    = (u32)NULL,
        #endif
        .fft_mode = FFT_ADC_RANGE_FFT_MODE,
    };

    hal_fft_set_config(FFT_IDX_CONFIG_ADC_WIN,(void*)&win_cfg);

    /* Range-FFT */
    adc_range_fft_para_t fft_cfg = { 0 };
    fft_cfg.enable =  1;
    #ifdef FFT_DC_CUT_ENABLE
    fft_cfg.dc_cut = 1;
    #endif
    fft_cfg.num_adc_ch = FFT_MONO_ADC_CH;
    #if FFT_NUM_ADC_CH == 2
    fft_cfg.num_adc_ch = FFT_DUAL_ADC_CH;
    #endif
    fft_cfg.shift_value = 0x4;
    fft_cfg.fft_point = FFT_SIZE;
    fft_cfg.nframe = FFT_NFRAME;
    fft_cfg.rfft_upper_rb = FFT_OUT_VALID_RB;
    fft_cfg.in_frame_size = FFT_INPUT_SIZE;
    fft_cfg.out_buf = (u32)fft_output_buffer;
    fft_cfg.half_int_en = 0;
    fft_cfg.end_callback = fft_end_isr;
    fft_cfg.half_callback = NULL;

    hal_fft_set_config(FFT_IDX_CONFIG_ADC_RANGE_FFT, (void *)&fft_cfg);
}

int app_radar_init(void)
{
    /* Variables Init */
    fifo_state = DEV_FIFO_WP_NA;

    #if DEMO_TYPE_USING == DEMO_TYPE_RAWDATA
    app_rawdata_config();
    #elif DEMO_TYPE_USING == DEMO_TYPE_RANGEFFT
    app_rangefft_config();
    #endif

    return 0;
}
#endif

int app_radar_start(void)
{
    hal_radar_start();

    return 0;
}

/**
* @brief Check Radar DSP result.
*
* @return 0 on success; -1 on failure.
*/
int app_radar_result(void)
{
    u32 rdat_dat = 0;
    s32 ret = -1;

    rdat_dat = get_current_data_addr();
    if (rdat_dat != 0) {
        /* printf("dump data...\r\n"); */
        #if (DBG_DUMP_RADAR_DATA == 1)
        output_port_send_data((u32)frame_header, sizeof(radar_frame_hdr_t));
        output_port_send_data((u32)rdat_dat, sizeof(GsFrameBuf));
        #endif
        ret = 0;
    }

    return ret;
}

/**
* @brief De-Init RF/DSP
* TODO:
* @return int
*/
int app_radar_deinit(void)
{
    hal_radar_stop();

    return 0;
}

#ifdef ULP_PROG_1
extern const int ramrun_image_len;
extern const unsigned char ramrun_image[];

u8 bootmode_check(void)
{
    u8 btm = pmu_get_bootmode();

    switch (btm) {
        case PMU_BOOTMODE_INIT: /* 首次启动 */
        case PMU_BOOTMODE_FLASH: /* 选择从Flash启动 */
        #if (DBG_RAMRUN_MODE == 0)
        memcpy((void *)SRAM_BASE_1, (void *)ramrun_image, ramrun_image_len);
        pmu_set_bootmode(PMU_BOOTMODE_SRAM1);
        #elif (DBG_RAMRUN_MODE == 1)
        memcpy((void *)DGB_RAMRUN_ADDR, (void *)ramrun_image, ramrun_image_len);
        pmu_set_bootmode(PMU_BOOTMODE_ADDR);
        pmu_set_boot_addr(DGB_RAMRUN_ADDR);
        #endif
        /* enable sram retention */
        pmu_set_retention(1);
        pmu_retention_select(PMU_RETENTION_ALL);
        break;

        case PMU_BOOTMODE_SRAM1: /* 从SRAM1启动 */
        break;

        default:
        pmu_set_bootmode(PMU_BOOTMODE_INIT);
        break;
    }
    return btm;
}
#endif

static void wakeup_timer_setup(void)
{
    pmu_set_wakeup_time(FRAME_INTERVAL_MS);
    pmu_enable_wakeup_source(PMU_WAKEUP_TIMER);
}

static int deep_sleep_ret_mem_init(void)
{
    void *ptr = NULL;

    /* ULP_MEM_TEST01_ID for radar frame header retention memory. */
    ptr = at_malloc_pool(AT_MEM_POOL_0_ID, sizeof(deep_sleep_ret_mem_t));
    if (NULL == ptr) {
        printf("Error: at_malloc_pool POOL 0 for %d bytes!!!\r\n", sizeof(deep_sleep_ret_mem_t));
        return -1;
    }
    ulp_mem_set_addr(ULP_MEM_AT_ID_1, (u32)ptr);

    return 0;
}

static int deep_sleep_ret_mem_save(void)
{
    deep_sleep_ret_mem_t *ptr = NULL;

    ptr = (deep_sleep_ret_mem_t *)ulp_mem_get_addr(ULP_MEM_AT_ID_1);
    ptr->rx_subfrm_id = frame_header->radar_common_cfg.rxSubFrameId;

    /* printf("%s, rxSubFrameId = %d.\r\n", __func__, ptr->rx_subfrm_id); */

    return 0;
}

static int deep_sleep_ret_mem_load(void)
{
    deep_sleep_ret_mem_t *ptr = NULL;

    ptr = (deep_sleep_ret_mem_t *)ulp_mem_get_addr(ULP_MEM_AT_ID_1);
    frame_header->radar_common_cfg.rxSubFrameId = ptr->rx_subfrm_id;

    /* printf("%s, rxSubFrameId = %d.\r\n", __func__, ptr->rx_subfrm_id); */

    return 0;
}

/* ------------------------------------------  MAIN  ---------------------------------------------- */

int main(void)
{
    app_init();

    #ifdef ULP_PROG_1
    printf("ULP FlashRun -- %s, %s \r\n", __DATE__, __TIME__);
    bootmode_check();
    deep_sleep_ret_mem_init();
    #endif
    #ifdef ULP_PROG_2
    printf("ULP RamRun -- %s, %s \r\n", __DATE__, __TIME__);
    deep_sleep_ret_mem_load();
    #endif

    for (u32 i = 0; i < FRAME_CAP_NUM_ONE_TIME; i++) {
        app_radar_start();
        while (app_radar_result() != 0) {}
    }

    app_radar_deinit();
    deep_sleep_ret_mem_save();
    delay_ms(SLEEP_DELAY_TIME_MS);
    wakeup_timer_setup();
    pmu_set_retention(1);
    pmu_retention_select(PMU_RETENTION_ALL);
    wait_output_dma_done(); /* 确保串口数据发送完成 */
    pmu_enter_sleep();
    for (;;) {}
}

