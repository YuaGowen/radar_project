#include <stdio.h>
#include <string.h>
#include "ulp_range_user_fft_main.h"
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

#define DBG_RAMRUN_MODE           0 /* 0: boot from the SRAM1, 1: boot from a specified addr. */
#define DGB_RAMRUN_ADDR           0x20008000 /* Set the startup address if boot from a specified address. */

#define DBG_MEASURE_INIT_TIME     0
#define DBG_DUMP_RADAR_DATA       1
#define FRAME_CAP_NUM_ONE_TIME    1
#define FRAME_INTERVAL_MS         (125)

#define DBG_USER_FFT_ENABLE       1 /* 0: range-fft, 1: range-fft & user-fft. */

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
volatile short GsFrameBuf[FFT_ADC_CH_NUM][RANGE_FFT_OUT_RB][NUM_CHIRP][2] FFT_BUF_ATTRIBUTES = {0}; /* 容纳1帧数据的缓存 */
static u32 *range_fft_out_buf_ptr = (u32 *)&GsFrameBuf;
static volatile enum_fifo_wp_status fifo_state = 0;

volatile radar_frame_hdr_t GtFrameHeader = {0}; /* 帧头 */
static radar_frame_hdr_t *frame_header =  (radar_frame_hdr_t*)&GtFrameHeader;

#if (RANGE_FFT_WIN_EN == 1) && defined(ULP_PROG_1)
static u16 win[] FFT_WIN_ATTRIBUTES = {
    #include "win_hann.tab"
};
#endif

#if (DBG_USER_FFT_ENABLE == 1)
#define USER_FFT_IN_SIZE        (USER_FFT_SIZE*4*USER_FFT_NFRAME*FFT_ADC_CH_NUM)
#define USER_FFT_OUT_SIZE       (USER_FFT_OUT_POINT*4*USER_FFT_NFRAME*FFT_ADC_CH_NUM)

static char user_fft_buf_in[USER_FFT_IN_SIZE] FFT_BUF_ATTRIBUTES = {0};
static s32 user_fft_buf_out[USER_FFT_OUT_SIZE>>2] FFT_BUF_ATTRIBUTES = {0}; /* data type is u32. */
static volatile s32 user_fft_done = 0;
#endif

/* Private Functions Implementations ------------------------------------------------------------- */

static void wakeup_timer_setup(void)
{
    pmu_set_wakeup_time(FRAME_INTERVAL_MS);
    pmu_enable_wakeup_source(PMU_WAKEUP_TIMER);
}

static int ulp_ret_mem_init(void)
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

static int ulp_mem_save(void)
{
    deep_sleep_ret_mem_t *ptr = NULL;

    ptr = (deep_sleep_ret_mem_t *)ulp_mem_get_addr(ULP_MEM_AT_ID_1);
    ptr->rx_subfrm_id = frame_header->radar_common_cfg.rxSubFrameId;

    /* printf("%s, rxSubFrameId = %d.\r\n", __func__, ptr->rx_subfrm_id); */

    return 0;
}

static int ulp_ret_mem_load(void)
{
    deep_sleep_ret_mem_t *ptr = NULL;

    ptr = (deep_sleep_ret_mem_t *)ulp_mem_get_addr(ULP_MEM_AT_ID_1);
    frame_header->radar_common_cfg.rxSubFrameId = ptr->rx_subfrm_id;

    /* printf("%s, rxSubFrameId = %d.\r\n", __func__, ptr->rx_subfrm_id); */

    return 0;
}

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
        d_update = (u32)range_fft_out_buf_ptr;
    }
    critical_section_end();

    return d_update;
}

__STATIC_INLINE void mcu_init(void)
{
    uart_cfg_t uart_cfg = {0};

    /* System clock initialization */
    hal_clk_init(SYS_FLASH_CLK_SEL);
    hal_dualtimer_init();

    sys_clk_enable_dma();

    /* Log Uart Init */
    uart_cfg.baud_rate = UART_BAUD_RATE_921600;
    uart_cfg.parity = UART_PARITY_DISABLE;
    uart_cfg.stop_bits = UART_STOP_BITS_1;
    uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
    uart_cfg.fifo_cfg.fifo_en = 1U;
    hal_uart_init(UART_ID_0, &uart_cfg);

    #if DBG_MEASURE_INIT_TIME == 1
    sys_clk_enable_gpio();
    gpio_toggle_init(GPIO_AON_2);
    gpio_set_value(GPIO_AON_2, 0U);
    #endif
}

#if (DBG_USER_FFT_ENABLE == 1)
static void usr_fft_isr(void)
{
    user_fft_done = 1;
}

static __INLINE void wait_user_fft_done(void)
{
    while (user_fft_done == 0) {}

    user_fft_done = 0;
}

static void app_userfft_config(void)
{
    fft_win_para_t win_cfg = {0};
    user_fft_para_t fft_cfg = {
        .data_type = COMPLEX_DATA,
        .is_power_out = USER_FFT_OUT_POWER,
        .shift_value = RANGE_FFT_SHIFT_BIT,
        .fft_point = USER_FFT_SIZE,
        .nframe = USER_FFT_NFRAME,
        .in_frame_size = USER_FFT_SIZE,
        .out_frame_size = USER_FFT_OUT_POINT,
        .in_buf = (u32)user_fft_buf_in,
        .out_buf = (u32)user_fft_buf_out,
        .end_callback = usr_fft_isr
    };

    /* User-FFT window configuration. */
    #if (USER_FFT_WIN_EN == 1)
    win_cfg.enable = 1;
    win_cfg.fft_mode = FFT_USER_FFT_MODE;
    win_cfg.waddr = (u32)user_fft_win;
    hal_fft_set_config(FFT_IDX_CONFIG_USER_WIN, (void *)&win_cfg);
    #endif

    /* User-FFT configuration. */
    hal_fft_set_config(FFT_IDX_CONFIG_USER_FFT, (void*)&fft_cfg);
}
#endif

#ifdef ULP_PROG_1
void app_rangefft_config(void)
{
    /* Window */
    fft_win_para_t win_cfg = {
        #if (RANGE_FFT_WIN_EN == 1)
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
    fft_cfg.enable = 1;
    #if (RANGE_FFT_REMOVE_DC == 1)
    fft_cfg.dc_cut = 1;
    #endif
    fft_cfg.num_adc_ch = FFT_MONO_ADC_CH;
    #if (FFT_ADC_CH_NUM == 2)
    fft_cfg.num_adc_ch = FFT_DUAL_ADC_CH;
    #endif
    fft_cfg.shift_value = 0x4;
    fft_cfg.fft_point = RANGE_FFT_SIZE;
    fft_cfg.nframe = RANGE_FFT_NFRAME;
    fft_cfg.rfft_upper_rb = RANGE_FFT_OUT_RB;
    fft_cfg.in_frame_size = RANGE_FFT_INPUT_SIZE;
    fft_cfg.out_buf = (u32)range_fft_out_buf_ptr;
    fft_cfg.half_int_en = 0;
    fft_cfg.end_callback = fft_end_isr;
    fft_cfg.half_callback = NULL;

    hal_fft_set_config(FFT_IDX_CONFIG_ADC_RANGE_FFT, (void *)&fft_cfg);
}

int app_radar_init(void)
{
    /* Variables Init */
    fifo_state = DEV_FIFO_WP_NA;

    app_rangefft_config(); /* Range-FFT configuration. */
    #if (DBG_USER_FFT_ENABLE == 1)
    app_userfft_config(); /* User-FFT configuration. */
    #endif

    return 0;
}
#endif

void app_init(void)
{
    mcu_init();

    #ifdef ULP_PROG_1
    ulp_mem_init();
    #endif

    #if DBG_MEASURE_INIT_TIME == 1
    gpio_set_value(GPIO_AON_2, 1U);
    #endif

    #ifdef ULP_PROG_1
    hal_fft_init();
    #else
    hal_fft_ulp_init();
    hal_fft_notify_set(FFT_ADC_RANGE_FFT_MODE, NULL, fft_end_isr);
    #if (DBG_USER_FFT_ENABLE == 1)
    hal_fft_notify_set(FFT_USER_FFT_MODE, NULL, usr_fft_isr);
    #endif
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
        #if (DBG_USER_FFT_ENABLE == 1)
        app_userfft_config();
        #endif

        #if (DBG_USER_FFT_ENABLE == 1)
        memcpy((void *)user_fft_buf_in, (void *)rdat_dat, USER_FFT_IN_SIZE);
        fft_process_trigger(FFT_USER_FFT_MODE);
        wait_user_fft_done();

        /* printf("user-fft in/out size = %d/%d.\r\n", USER_FFT_IN_SIZE, USER_FFT_OUT_SIZE); */

        #if (DBG_DUMP_RADAR_DATA == 1)
        output_port_send_data((u32)frame_header, sizeof(radar_frame_hdr_t));
        output_port_send_data((u32)user_fft_buf_out, USER_FFT_OUT_SIZE);
        #endif

        #else /* #if (DBG_USER_FFT_ENABLE == 1) */

        #if (DBG_DUMP_RADAR_DATA == 1)
        output_port_send_data((u32)frame_header, HEADER_DAT_SIZE);
        output_port_send_data((u32)rdat_dat, RANGE_FFT_OUT_SIZE);
        #endif
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
        case PMU_BOOTMODE_INIT:
        case PMU_BOOTMODE_FLASH:
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

        case PMU_BOOTMODE_SRAM1:
        break;

        default:
        pmu_set_bootmode(PMU_BOOTMODE_INIT);
        break;
    }
    return btm;
}
#endif

/* ------------------------------------------  MAIN  ---------------------------------------------- */

int main(void)
{
    app_init();

    #ifdef ULP_PROG_1
    printf("ULP FlashRun -- %s, %s \r\n", __DATE__, __TIME__);
    bootmode_check();
    ulp_ret_mem_init();
    #endif

    #ifdef ULP_PROG_2
    printf("ULP RamRun -- %s, %s \r\n", __DATE__, __TIME__);
    ulp_ret_mem_load();
    #endif

    #ifdef ULP_PROG_1 /* for debug. */
    printf("frame_head: 0x%x, head size: %d, chirp_num/size: %d/%d, adc_ch:%d, fft_out_rb:%d, fft_out_vb:%d\r\n",
    ((frame_header->radar_common_cfg.flag << 24)
    |(frame_header->radar_common_cfg.version<< 16)
    | (frame_header->radar_common_cfg.syncBound)),
    sizeof(radar_frame_hdr_t), NUM_CHIRP, NUM_SAMPLE_PER_CHIRP, FFT_ADC_CH_NUM, RANGE_FFT_OUT_RB, USER_FFT_OUT_POINT);
    #endif

    for (u32 i = 0; i < FRAME_CAP_NUM_ONE_TIME; i++) {
        app_radar_start();
        while (app_radar_result() != 0) {}
    }

    app_radar_deinit();

    ulp_mem_save();

    delay_ms(SLEEP_DELAY_TIME_MS);

    wakeup_timer_setup();

    pmu_set_retention(1);
    pmu_retention_select(PMU_RETENTION_ALL);

    pmu_enter_sleep();
    for (;;) {}
}

