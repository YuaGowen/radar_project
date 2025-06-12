#include <string.h>
#include "clock.h"
#include "pmu.h"
#include "gpio.h"
#include "hal_radar.h"
#include "hal_uart.h"
#include "common.h"
#include "at_malloc.h"
#include "at_malloc_defs.h"
#include "handle_mdet_info.h"
#include "app_sensor_ulp.h"
#include "output_config.h"
#include "target_config.h"
#include "atlog.h"
#if defined(CFG_OTA) && defined(ULP_PROG_2)
#include "force_jump.h"
#endif
#include "radar_ext_chip.h"

/* Configurable ----------------------------------------------------------------------------------*/

//#define DBG_OUTPUT_RANGEFFT_DATA  /**< Print Range-FFT data in log (comment this macro to disable this feature) */

//#define DBG_DET_CNT /* count the number of triggers */

#define RADAR_OUT_EN

#ifdef DBG_OUTPUT_RANGEFFT_DATA
#define DBG_OUTPUT_DELAY_MS     10
#else
#define DBG_OUTPUT_DELAY_MS     0
#endif

/**
 * @defgroup EXTI_ULP_WAKEUP_PINS
 * @brief 外部唤醒管脚配置，以及一次外部唤醒
 *
 */
#define WAKEUP_GPIO_PIN         (1 << 1)    /* GPIO_AON_1 是 AT6010 唤醒管脚。 AT5815C OUT 管脚与此连接 */
#define WAKEUP_GPIO_LAST_MS     (1000 * 10) /* AT6010 由 GPIO 唤醒一次持续工作时间 */
/**
 * @}
 */

/* Data Types ------------------------------------------------------------------------------------*/

typedef enum {
	WAKEUP_TYPE_LIGHT = (1 << 0), /**< 1 */
	WAKEUP_TYPE_KEY   = (1 << 1), /**< 2 */
	WAKEUP_TYPE_TIMER = (1 << 2), /**< 4 */
	WAKEUP_TYPE_GPIO  = (1 << 3)  /**< 8 */
} wakeup_type_t;

typedef struct {
	/* see @ref wakeup_type_t */
	u8  wk_type;

	/* 由定时器唤醒多次，总共持续时间 */
	u32 wk_worktime_ms;

	/* 从最近一次 GPIO 唤醒到现在的时间 */
	u32 wk_elapsed_ms;
} ulp_wake_ctx_t;

/**
 * @brief Current light state
 */
typedef enum {
	LIGHT_STATE_OFF,
	LIGHT_STATE_BLINK,
	LIGHT_STATE_ON,
} light_state_t;

/**
 * @brief app sensor ulp context
 */
typedef struct {
	void *md_hdl;

	/* 外部唤醒上下文 */
	ulp_wake_ctx_t *ulp_wake;

#ifdef RADAR_OUT_EN
	light_state_t light_state;
#endif

#ifdef DBG_DET_CNT
	u32 det_cnt; /* count every frames */
	u32 prev_fid; /* previous frame id */
	u32 det_discont_cnt; /* count discontinuous frames */
#endif

#ifdef CFG_OTA
	u32 ota_ready_to_dld;    /**< 0 -- not ready; 1 -- read */
	u32 ota_pin_trigger_cnt; /**< OTA pin trigger count, for time measurement */
#endif
} sensor_ulp_ctx_t;

/* Private Variables -----------------------------------------------------------------------------*/

static sensor_ulp_ctx_t *m_sensor_ctx = NULL;

/* Private Functions -------------*/


static void wakeup_timer_setup(void)
{
	pmu_set_wakeup_time(PARA_FRAME_INTERVAL_MS - DBG_OUTPUT_DELAY_MS);
	pmu_enable_wakeup_source(PMU_WAKEUP_TIMER);
}

static void wakeup_timer_cleanup(void)
{
	pmu_disable_wakeup_source(PMU_WAKEUP_TIMER);
}

/**
 * @brief mask for GPIO_AON_x, this can be ORed with each bit value.
 * Enable wakeup by setting the corresponding bit to 1.
 * GPIO_AON_0 -- bit0
 * GPIO_AON_1 -- bit1
 * GPIO_AON_2 -- bit2
 * GPIO_AON_3 -- bit3
 * GPIO_AON_4 -- bit4
 * GPIO_AON_5 -- bit5
 * GPIO_AON_6 -- bit6
 *
 * @param mask
 */
static void wakeup_gpio_setup(u16 mask)
{
	u8 i = 0U;
	gpio_cfg_t gpio_cfg = { 0 };

	pmu_disable_gpio_wakeup(0xFFU);

	pmu_enable_gpio_wakeup(mask); // TODO: 移动到下面去

	gpio_cfg.mode = GPIO_MODE_INPUT;
	gpio_cfg.pull = GPIO_PULL_DOWN;
	while (mask > 0U) {
		if ((mask & 1U) > 0U) {
			(void)gpio_init((gpio_pin_t)(i + GPIO_AON_0), &gpio_cfg);
		}
		mask >>= 1;
		i++;
	}
}

static void wakeup_gpio_cleanup(void)
{
	pmu_disable_gpio_wakeup(0xFF);
}

/**
 * @brief RamRun 首先关闭 GPIO 唤醒
 *
 */
static void wakeup_method_pre_config(void)
{
	wakeup_gpio_cleanup();
}

/**
 * @brief
 *
 * @param det_ok 没有检测到人，就启用外部唤醒；检测到人，
 */
static void wakeup_method_post_config(u32 det_ok)
{
#if defined(ULP_PROG_1)
	wakeup_timer_cleanup();
	wakeup_gpio_setup(WAKEUP_GPIO_PIN);
#endif

#if defined(ULP_PROG_2)
	// logi("TP=0x%0x, WT=%u, EMS=%u\r\n", m_sensor_ctx->ulp_wake->wk_type, m_sensor_ctx->ulp_wake->wk_worktime_ms, m_sensor_ctx->ulp_wake->wk_elapsed_ms);
	if ((m_sensor_ctx->ulp_wake->wk_type & WAKEUP_TYPE_GPIO)) {
		logi("WK <-- IO\r\n");
		m_sensor_ctx->ulp_wake->wk_type &= ~WAKEUP_TYPE_GPIO;
		wakeup_gpio_cleanup();

		m_sensor_ctx->ulp_wake->wk_worktime_ms = WAKEUP_GPIO_LAST_MS;
		m_sensor_ctx->ulp_wake->wk_elapsed_ms = 0;
		m_sensor_ctx->ulp_wake->wk_type |= WAKEUP_TYPE_TIMER;
		wakeup_timer_setup();
	} else if (m_sensor_ctx->ulp_wake->wk_type & WAKEUP_TYPE_TIMER) {
		// logi("WK <-- TMR\r\n");
#if 1
	// NOTE: 新的代码，如果没有检测到运动物体，则直接关闭TIMER唤醒，使能GPIO唤醒
		if (!det_ok) {
			// 关闭 TIMER 唤醒
			wakeup_timer_cleanup();

			// 使能 GPIO 唤醒
			m_sensor_ctx->ulp_wake->wk_type |= WAKEUP_TYPE_GPIO;
			wakeup_gpio_setup(WAKEUP_GPIO_PIN);
		}
#else
	// NOTE: 这里老的代码统计 TIMER 唤醒时间，超过10秒钟则关闭TIMER唤醒，使能GPIO唤醒
		m_sensor_ctx->ulp_wake->wk_elapsed_ms += PARA_FRAME_INTERVAL_MS;
		if (m_sensor_ctx->ulp_wake->wk_elapsed_ms >= m_sensor_ctx->ulp_wake->wk_worktime_ms) {
			m_sensor_ctx->ulp_wake->wk_type &= ~WAKEUP_TYPE_TIMER;
			m_sensor_ctx->ulp_wake->wk_worktime_ms = 0;
			m_sensor_ctx->ulp_wake->wk_elapsed_ms = 0;
			wakeup_timer_cleanup();

			m_sensor_ctx->ulp_wake->wk_type |= WAKEUP_TYPE_GPIO;
			wakeup_gpio_setup(WAKEUP_GPIO_PIN);
		}
#endif
	} else {
		// TODO: other wakeup method
	}
#endif /* !ULP_PROG_2 */

	pmu_retention_select(PMU_RETENTION_ALL);
}


/* Function Implementations ----------------------------------------------------------------------*/

static void radar_ulp_stop(void)
{
	hal_radar_stop();
}

static void radar_ulp_start(void)
{
	hal_radar_start();
}

static void output_io_config(gpio_pin_t gpio)
{
	gpio_set_pinmux(gpio, 1);
	gpio_set_direction(gpio, GPIO_DIR_OUTPUT);
}

static void sensor_ulp_ctx_init(void)
{
#if defined(ULP_PROG_1)
	/* context memory malloc */
	m_sensor_ctx = (sensor_ulp_ctx_t *)at_malloc_pool(AT_MEM_POOL_0_ID, sizeof(sensor_ulp_ctx_t));
	if (NULL == m_sensor_ctx) {
		loge("Failed to malloc %d bytes for m_sensor_ctx \r\n", sizeof(sensor_ulp_ctx_t));
		while (1);
	}
	memset((void *)m_sensor_ctx, 0, sizeof(sensor_ulp_ctx_t));
	ulp_mem_set_addr(ULP_MEM_AT_ID_4, (u32)m_sensor_ctx);

	/* 双鉴唤醒专用的结构体初始化 */
	m_sensor_ctx->ulp_wake = (ulp_wake_ctx_t *)at_malloc_pool(AT_MEM_POOL_0_ID, sizeof(ulp_wake_ctx_t));
	if (NULL == m_sensor_ctx->ulp_wake) {
		loge("Failed to malloc %d bytes for ulp_wake \r\n", sizeof(ulp_wake_ctx_t));
	}

	m_sensor_ctx->ulp_wake->wk_type = WAKEUP_TYPE_GPIO;
	m_sensor_ctx->ulp_wake->wk_worktime_ms = WAKEUP_GPIO_LAST_MS;
	m_sensor_ctx->ulp_wake->wk_elapsed_ms = 0;

#ifdef RADAR_OUT_EN
	/* context initialization */
	m_sensor_ctx->light_state = LIGHT_STATE_OFF;
#endif

#ifdef DBG_DET_CNT
	m_sensor_ctx->det_cnt = 0;
	m_sensor_ctx->prev_fid = 0xffffffff;
	m_sensor_ctx->det_discont_cnt = 0;
#endif

#ifdef CFG_OTA
	m_sensor_ctx->ota_ready_to_dld = 0;
	m_sensor_ctx->ota_pin_trigger_cnt = 0;
#endif

#endif /* !ULP_PROG_1 */

#ifdef ULP_PROG_2
	m_sensor_ctx = (sensor_ulp_ctx_t *)ulp_mem_get_addr(ULP_MEM_AT_ID_4);
#endif
}

static void radar_algo_init(void)
{
#if defined(ULP_PROG_1)
	m_sensor_ctx->md_hdl = radar_fmcw_md_init(0, 0, 0, 0, 0);
	// ulp_mem_set_addr(ULP_MEM_AT_ID_3, (u32)m_sensor_ctx->md_hdl);

	radar_fmcw_md_set_det_delta(m_sensor_ctx->md_hdl, MD_PROFILE_BASELINE, PARA_MOT_STO_DIST_MM); /* set stop det range: 1800cm */
	radar_fmcw_md_set_undet_delta(m_sensor_ctx->md_hdl, MD_PROFILE_BASELINE, PARA_MOT_STA_DIST_MM);   /* set start det range: 44cm  */

#ifdef CFG_PRESENCE_DET
	radar_fmcw_md_set_det_delta(m_sensor_ctx->md_hdl, MD_PROFILE_HIGH, PARA_BHR_STO_DIST_MM); /* set stop det range: 1800cm */
	radar_fmcw_md_set_undet_delta(m_sensor_ctx->md_hdl, MD_PROFILE_HIGH, PARA_BHR_STA_DIST_MM);   /* set start det range: 44cm  */
	// radar_fmcw_active_time_keep(10); /* 10s */
#else
	radar_fmcw_md_set_det_delta(m_sensor_ctx->md_hdl, MD_PROFILE_HIGH, 0);     /* disable bhr det */
	radar_fmcw_md_set_undet_delta(m_sensor_ctx->md_hdl, MD_PROFILE_HIGH, 0);   /* disable bhr det */
	// radar_fmcw_active_time_keep(2); /* 2s */
#endif

#ifdef PHASE_SHIFT_180DEGREE_EN
	radar_fmcw_rx1_phase_180_degree_shift();
#endif
#endif /* !ULP_PROG_1 */

#if defined(ULP_PROG_2)
#ifdef DBG_OUTPUT_RANGEFFT_DATA
	radar_fmcw_md_ulp_ca(m_sensor_ctx->md_hdl, output_port_send_data);
#endif
#endif /* !ULP_PROG_2 */
}

static u32 radar_algo_process(void)
{
	u32 ret = radar_fmcw_md_perform(m_sensor_ctx->md_hdl);

	return ret;
}

static void radar_algo_det_info(void)
{
	fmcw_det_info_t info = {0};
	void *pc_buffer = NULL;
	u8 pc_buf_size = 68;  /* point cloud buffer size */

	radar_fmcw_md_det_info(m_sensor_ctx->md_hdl, &info);

	handle_mdet_info_mot(&info);

#ifdef CFG_RADAR_STAGE_SUPPORT
#ifndef CFG_PRESENCE_DET
	pc_buffer = radar_fmcw_md_ulp_pc_buf_get(m_sensor_ctx->md_hdl);
	handle_mdet_pc_data(pc_buffer,pc_buf_size);
#endif
#endif
}

#ifdef DBG_DET_CNT
static void radar_det_trigger_info(void)
{
	fmcw_det_info_t info = {0};

	radar_fmcw_md_det_info(m_sensor_ctx->md_hdl, &info);
	m_sensor_ctx->det_cnt++;

	app_sensor_log_uart_init();
	if ((m_sensor_ctx->prev_fid + 1) != info.frame_idx) {
		m_sensor_ctx->det_discont_cnt++;
		logi("\r\n\ndetect_time: %d\r\n", m_sensor_ctx->det_discont_cnt);
	}
	m_sensor_ctx->prev_fid = info.frame_idx;

	logi("dcnt=%d, dist=%dcm, fid=%d\n\r", m_sensor_ctx->det_cnt, info.range_val/10, info.frame_idx);

	while (uart_get_status(UART_LOG_PORT, UART_BUSY) != 0U);
}
#endif

__STATIC_INLINE void det_info_upload(u32 det_val)
{
	radar_algo_det_info();

#ifdef DBG_DET_CNT
	radar_det_trigger_info();
#endif
}


void app_sensor_log_uart_init(void)
{
	static u8 init_flag = 0;

	if (init_flag == 0) {
		uart_cfg_t uart_cfg = {0};

		uart_cfg.baud_rate = UART_LOG_BAUDRATE;
		uart_cfg.parity = UART_PARITY_DISABLE;
		uart_cfg.stop_bits = UART_STOP_BITS_1;
		uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
		uart_cfg.fifo_cfg.fifo_en = 1U;
		hal_uart_init(UART_LOG_PORT, &uart_cfg);
		init_flag = 1;
	}
}

void app_sensor_ulp_init(void)
{
#ifdef DBG_OUTPUT_RANGEFFT_DATA
	output_dma_config();
#endif

#ifdef ULP_PROG_1
	output_io_config(RADAR_OUT_PIN);
#endif /* !ULP_PROG_1 */

	sensor_ulp_ctx_init();

#if defined(ULP_PROG_1)

	wakeup_method_pre_config();

	at5815c_init();
	at5815c_turn_off_ls();
	at5815c_2nd_trigger_cfg(DET_2ND_TRIGGER_DISABLE);
	at5815c_cfg_freq();
	at5815c_cfg_lps();
	at5815c_cfg_distance(9);
#endif

	radar_algo_init();
}

void app_sensor_ulp_proc(void)
{
	u32 det_val = 0;

	det_val = radar_algo_process();

#ifdef RADAR_OUT_EN
	if (det_val) {
		gpio_toggle(RADAR_OUT_PIN);
		m_sensor_ctx->light_state = LIGHT_STATE_BLINK;
	} else {
		if (m_sensor_ctx->light_state != LIGHT_STATE_OFF) {
			gpio_set_value(RADAR_OUT_PIN, 0);
			m_sensor_ctx->light_state = LIGHT_STATE_OFF;
		}
	}
#endif

#if defined(ULP_PROG_2) && !defined(DBG_OUTPUT_RANGEFFT_DATA)
	det_info_upload(det_val);
#endif /* ULP_PROG_2 */

	radar_ulp_stop();

	wakeup_method_post_config(det_val);

#ifdef DBG_OUTPUT_RANGEFFT_DATA
	delay_ms(DBG_OUTPUT_DELAY_MS);
#endif
}
