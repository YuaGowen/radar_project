#include <string.h>
#include "pmu.h"
#include "gpio.h"
#include "hal_radar.h"
#include "hal_uart.h"
#include "common.h"
#include "at_malloc.h"
#include "at_malloc_defs.h"
#include "handle_mdet_info.h"
#include "app_hpd_ulp.h"
#include "output_config.h"
#include "target_config.h"

/* Configurable ----------------------------------------------------------------------------------*/

//#define DBG_OUTPUT_RANGEFFT_DATA  /* def -- enable; undef -- disable */

//#define DBG_DET_CNT /* count the number of triggers */

#ifdef DBG_OUTPUT_RANGEFFT_DATA
#define DBG_OUTPUT_DELAY_MS  10
#else
#define DBG_OUTPUT_DELAY_MS  0
#endif

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
	light_state_t light_state;
#ifdef DBG_DET_CNT
	u32 det_cnt; /* count every frames */
	u32 prev_fid; /* previous frame id */
	u32 det_discont_cnt; /* count discontinuous frames */
#endif
} sensor_ulp_ctx_t;

/* Private Variables -----------------------------------------------------------------------------*/

static sensor_ulp_ctx_t *m_hpd_ctx = NULL;

/* Function Implementations ----------------------------------------------------------------------*/

static void output_io_config(gpio_pin_t gpio)
{
	gpio_set_pinmux(gpio, 1);
	gpio_set_direction(gpio, 1);
}

static void sensor_ulp_ctx_init(void)
{
#ifdef ULP_PROG_1
	/* context memory malloc */
	m_hpd_ctx = (sensor_ulp_ctx_t *)at_malloc_pool(AT_MEM_POOL_0_ID, sizeof(sensor_ulp_ctx_t));
	ulp_mem_set_addr(ULP_MEM_AT_ID_4, (u32)m_hpd_ctx);

	/* context initialization */
	m_hpd_ctx->light_state = LIGHT_STATE_OFF;
#ifdef DBG_DET_CNT
	m_hpd_ctx->det_cnt = 0;
	m_hpd_ctx->prev_fid = 0xffffffff;
	m_hpd_ctx->det_discont_cnt = 0;
#endif
#endif

#ifdef ULP_PROG_2
	m_hpd_ctx = (sensor_ulp_ctx_t *)ulp_mem_get_addr(ULP_MEM_AT_ID_4);
#endif
}

static void radar_algo_init(void)
{
#ifdef ULP_PROG_1
	m_hpd_ctx->md_hdl = radar_fmcw_md_init(0, 0, 0, 0, 0);
	ulp_mem_set_addr(ULP_MEM_AT_ID_3, (u32)m_hpd_ctx->md_hdl);

	radar_fmcw_md_set_det_delta(m_hpd_ctx->md_hdl, MD_PROFILE_BASELINE, PARA_MOT_STO_DIST_MM); /* set stop det range: 1800cm */
	radar_fmcw_md_set_undet_delta(m_hpd_ctx->md_hdl, MD_PROFILE_BASELINE, PARA_MOT_STA_DIST_MM);   /* set start det range: 44cm  */

#ifdef CFG_PRESENCE_DET
	radar_fmcw_md_set_det_delta(m_hpd_ctx->md_hdl, MD_PROFILE_HIGH, PARA_BHR_STO_DIST_MM); /* set stop det range: 1800cm */
	radar_fmcw_md_set_undet_delta(m_hpd_ctx->md_hdl, MD_PROFILE_HIGH, PARA_BHR_STA_DIST_MM);   /* set start det range: 44cm  */
	radar_fmcw_active_time_keep(10); /* 10s */
#else
	radar_fmcw_md_set_det_delta(m_hpd_ctx->md_hdl, MD_PROFILE_HIGH, 0);     /* disable bhr det */
	radar_fmcw_md_set_undet_delta(m_hpd_ctx->md_hdl, MD_PROFILE_HIGH, 0);   /* disable bhr det */
	radar_fmcw_active_time_keep(2); /* 2s */
#endif

#endif /* !ULP_PROG_1 */

#ifdef ULP_PROG_2
	m_hpd_ctx->md_hdl = ulp_mem_get_addr(ULP_MEM_AT_ID_3);

#ifdef DBG_OUTPUT_RANGEFFT_DATA
	radar_fmcw_md_ulp_ca(m_hpd_ctx->md_hdl, output_port_send_data);
#endif
#endif
}

static u32 radar_algo_process(void)
{
	u32 ret = radar_fmcw_md_perform(m_hpd_ctx->md_hdl);

	return ret;
}

static void radar_algo_det_info(void)
{
	fmcw_det_info_t info = {0};

	radar_fmcw_md_det_info(m_hpd_ctx->md_hdl, &info);

	handle_mdet_info_mot(&info);
}

#ifdef DBG_DET_CNT
static void radar_det_trigger_info(void)
{
	fmcw_det_info_t info = {0};

	radar_fmcw_md_det_info(m_hpd_ctx->md_hdl, &info);
	m_hpd_ctx->det_cnt++;

	app_sensor_log_uart_init();
	if ((m_hpd_ctx->prev_fid + 1) != info.frame_idx) {
		m_hpd_ctx->det_discont_cnt++;
		logi("\r\n\ndetect_time: %d\r\n", m_hpd_ctx->det_discont_cnt);
	}
	m_hpd_ctx->prev_fid = info.frame_idx;

	logi("dcnt=%d, dist=%dcm, fid=%d\n\r", m_hpd_ctx->det_cnt, info.range_val/10, info.frame_idx);

	while (uart_get_status(UART_LOG_PORT, UART_BUSY) != 0U);
}
#endif

__STATIC_INLINE void det_info_upload(void)
{
	radar_algo_det_info();
}
static void wakeup_timer_setup(void)
{
	pmu_set_wakeup_time(PARA_FRAME_INTERVAL_MS - DBG_OUTPUT_DELAY_MS);
	pmu_enable_wakeup_source(PMU_WAKEUP_TIMER);
}

/*------------------------------------------  MAIN  ----------------------------------------------*/

void app_hpd_log_uart_init(void)
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

void app_hpd_ulp_init(void)
{
#ifdef DBG_OUTPUT_RANGEFFT_DATA
	output_dma_config();
#endif

#ifdef ULP_PROG_1
	output_io_config(RADAR_OUT_PIN);
#endif

	sensor_ulp_ctx_init();

	radar_algo_init();
}

void app_hpd_ulp_proc(void)
{
	u32 ret = 0;

	ret = radar_algo_process();

	if (ret) {
		gpio_set_value(RADAR_OUT_PIN,1);
		m_hpd_ctx->light_state = LIGHT_STATE_ON;
#ifdef DBG_DET_CNT
		radar_det_trigger_info();
#endif
	} else {
		if (m_hpd_ctx->light_state != LIGHT_STATE_OFF) {
			gpio_set_value(RADAR_OUT_PIN, 0);
			m_hpd_ctx->light_state = LIGHT_STATE_OFF;
		}
	}

#if defined(ULP_PROG_2) && !defined(DBG_OUTPUT_RANGEFFT_DATA) && !defined(DBG_DET_CNT)
	det_info_upload();
#endif /* ULP_PROG_2 */

#ifdef DBG_OUTPUT_RANGEFFT_DATA
	delay_ms(DBG_OUTPUT_DELAY_MS);
#endif

#ifdef CFG_LOG
	while (uart_get_status(UART_LOG_PORT, UART_TX_BUF_EMPTY) == 0U) {};
#endif

	wakeup_timer_setup();
	pmu_retention_select(PMU_RETENTION_ALL);
}
