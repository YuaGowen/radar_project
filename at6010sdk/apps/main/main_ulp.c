#include <string.h>
#include "clock.h"
#include "pmu.h"
#include "hal_uart.h"
#include "hal_dualtimer.h"
#include "hal_fft.h"
#include "hal_timer.h"
#include "hal_radar.h"
#include "common.h"
#include "memory_map.h"
#include "ulp_mem.h"
#include "atlog.h"
#include "at_malloc.h"
#include "at_malloc_defs.h"
#include "radar_profile_init.h"
#include "spi_flash_drv.h"
#include "handle_mdet_info.h"
#include "version.h"
#if defined(CFG_EOL_TESTMOD) && defined(CFG_CI) && defined(ULP_PROG_1)
#include "app_eol.h"
#endif
#if defined(CFG_APP_SENSOR_ULP)
#include "app_sensor_ulp.h"
#elif defined(CFG_APP_HTM_ULP)
#include "app_htm_ulp.h"
#elif defined(CFG_APP_PRESENSE_ULP)
#include "app_hpd_ulp.h"
#endif

#if defined(ULP_PROG_1) && defined(CFG_EXTI_ULP)
#include "k_msg.h"
#include "ci_main.h"
#include "jiffies.h"
#include "sys_timer.h"
#include "osal_timer.h"
#endif

#if defined(ULP_PROG_1) && defined(CFG_OTA)
#include "k_msg.h"
#include "ci_main.h"
#include "ci_port.h"
#include "jiffies.h"
#include "sys_timer.h"
#include "osal_timer.h"
#include "ymodem.h"
#include "ci_dld.h"
#include "force_jump.h"
#endif


/* Configurable ----------------------------------------------------------------------------------*/

//#define DBG_HTOL_DIG_VCORE /* control dig vcore during power up/down for htol test. */

/* Private Variables -----------------------------------------------------------------------------*/

#if defined(ULP_PROG_1) && defined(CFG_OTA)

extern ci_port_ctx_t m_ci_ctxt;

volatile u8 ota_ulpprog1_boot_flag = 0; /**< 0 -- normal boot; 1 -- boot for OTA */

#endif

/* Private Functions -----------------------------------------------------------------------------*/

/* Function Implementations ----------------------------------------------------------------------*/

static void mcu_init(void)
{
	clk_mcu_set(CLK_SRC_PLL, 80000000U);
	hal_dualtimer_init();
}

#if defined(CFG_OTA)
void force_jump_to_ulpprog1(void)
{
	force_jump((unsigned int *)0x08003000); /* app firmware address: 0x08003000 */
}
#endif

#ifdef ULP_PROG_1

extern const int ramrun_image_len;
extern const unsigned char ramrun_image[];
#ifdef PARA_RAMRUN_BOOTMODE_ADDR
#define RAMRUN_BOOTMODE_ADDR    PARA_RAMRUN_BOOTMODE_ADDR
#else
#define RAMRUN_BOOTMODE_ADDR    (0x2000c000)
#endif

u8 bootmode_check(void)
{
	u8 btm = pmu_get_bootmode();

	switch (btm) {
	case PMU_BOOTMODE_INIT:
	case PMU_BOOTMODE_FLASH:
		logi("\nflash run %d ...\r\n", btm);
		memcpy((void *)RAMRUN_BOOTMODE_ADDR, (void *)ramrun_image, ramrun_image_len);
		pmu_set_bootmode(PMU_BOOTMODE_ADDR);
		pmu_set_boot_addr(RAMRUN_BOOTMODE_ADDR);

		/* enable sram retention */
		pmu_set_retention(1);
		pmu_retention_select(PMU_RETENTION_ALL);
		break;

	case PMU_BOOTMODE_SRAM1:
		logi("\nsram run %d ...\r\n", btm);
		break;

	default:
		pmu_set_bootmode(PMU_BOOTMODE_INIT);
		break;
	}

	return btm;
}
#endif

static void platform_init(void)
{
	mcu_init();

#if defined(ULP_PROG_1) || defined(CFG_LOG)
#if defined(CFG_APP_SENSOR_ULP)
	app_sensor_log_uart_init();
#elif defined(CFG_APP_HTM_ULP)
	app_htm_log_uart_init();
#elif defined(CFG_APP_PRESENSE_ULP)
	app_hpd_log_uart_init();
#endif
	set_log_level(LOG_DEFAULT_LEVEL);
#endif

#if defined(ULP_PROG_1)
	show_version();
	ulp_mem_init();
	bootmode_check();

#if defined(CFG_CI)
	k_msg_init();
	sys_timer_init();
	sys_tick_init();
	ci_main_init();

	// TODO: 从此启动定时器
	ulp_start_active_timer();

#if defined(CFG_OTA) && defined(CFG_CUST_ANKER)
	if (pmu_get_sw_data(SW_DATA_ID_OTA_READY) == 1) {
		ota_ulpprog1_boot_flag = 1;
		/* Clear PMU SW DATA for OTA flag */
		pmu_set_sw_data(SW_DATA_ID_OTA_READY, 0);
	}

	if (ota_ulpprog1_boot_flag != 0) {
		/* Init baremetal environment */
		k_msg_init();
		sys_timer_init();
		sys_tick_init();
		ci_main_init();
	}
#endif /* !CFG_OTA */
#endif /* !CFG_CI */
#endif /* !ULP_PROG_1 */
}



static void radar_cfg_ulp_init(void)
{
#ifdef ULP_PROG_1
	radar_profile_init();
#endif

#ifdef ULP_PROG_2
	radar_profile_burst_init();
	hal_fft_ulp_init();
#endif
	radar_ulp_init();
}

/*------------------------------------------  MAIN  ----------------------------------------------*/
extern u8 volatile m_ulp_prog1_run_time;

int main(void)
{
#ifdef DBG_HTOL_DIG_VCORE
	/* set dig vcore to 0x3 for htol test. */
	pmu_set_dig_vcore(0x3);
#endif

	platform_init();

	radar_cfg_ulp_init();


#if defined(ULP_PROG_1)

#if defined(CFG_CUST_ANKER)
	extern void anker_bsp_init(void);
	anker_bsp_init();
#endif


/* HTM_ULP run EOL in FlashRun CI process */
#if defined(ULP_PROG_1) && defined(CFG_EOL_TESTMOD)
	eol_app_init();
#endif
#ifdef CFG_CI
	for (;;) {
		k_msg_loop();
#if defined(ULP_PROG_1) && defined(CFG_EOL_TESTMOD)
		eol_app_process();
#endif

		if (m_ulp_prog1_run_time == 0) {
			break;
		}
	}
#endif

	logi("ci over\r\n");

#if defined(CFG_OTA)
	if (ota_ulpprog1_boot_flag != 0) {
		/* ymodem starts here */
		logi("ymodem starts here\r\n");
		u32 recv_size = 0;

		if (m_ci_ctxt.m_dld_ctxt.m_ymodem_run == 0) {
			m_ci_ctxt.m_dld_ctxt.m_ymodem_run = 1;
			fifo_init(&m_ci_ctxt.m_dld_ctxt.m_dld_fifo, (u8 *)m_ci_ctxt.m_dld_ctxt.m_dld_buf, CI_DLD_BUF_SIZE * sizeof(u8), FIFO_WIDTH_8BITS);
			m_ci_ctxt.m_dld_ctxt.m_ymodem_ret = Ymodem_Receive(&recv_size, FLASH_DLD_PART_OFFSET, FLASH_DLD_PART_SIZE);
			m_ci_ctxt.m_dld_ctxt.m_ymodem_run = 0;
		}
	}
#endif /* !CFG_OTA */

#endif /* !ULP_PROG_1 */


#if defined(CFG_APP_SENSOR_ULP)
	app_sensor_ulp_init();
	app_sensor_ulp_proc();
#elif defined(CFG_APP_HTM_ULP)
	app_htm_ulp_init();
	app_htm_ulp_proc();
#elif defined(CFG_APP_PRESENSE_ULP)
	app_hpd_ulp_init();
	app_hpd_ulp_proc();
#endif

#ifdef ULP_PROG_1
	pmu_set_dvs(1);
#endif

#ifdef ULP_PROG_2
#if 0 /* !defined(CFG_APP_HTM_ULP) && !defined(CFG_CUST_ANKER) */
	if (0U == pmu_get_sw_data(5U)) {
		// pmu_skip_flash_pupd(1);
		// pmu_set_cache_pd();
		// spifls_host_cmd_write(SPIFLS_CMD_DP);
		pmu_set_sw_data(5U, 1U);
	}
#endif
#endif

#ifdef DBG_HTOL_DIG_VCORE
	/* set dig vcore to 0x7 beform enter sleep for htol test. */
	pmu_set_dig_vcore(0x7);
#endif

#if defined(CFG_APP_SENSOR_ULP)
#if defined(ULP_PROG_1)
#if defined(CFG_OTA)
	if (ota_ulpprog1_boot_flag) { /**< hang here to do CI process after OTA */
		for (;;) {
			k_msg_loop();
		}
	} else { /* just go to sleep if there is no OTA event */
		pmu_enter_sleep();
		while (1);
	}
#endif /* !CFG_OTA */
#endif /* !ULP_PROG_1 */
#endif /* !CFG_APP_SENSOR_ULP */

#if !defined(CFG_EXTI_ULP)
	// Goto ULP immediately in ULP_PROG_1
	pmu_enter_sleep();
	while (1);
#else
#if defined(ULP_PROG_2)
	// Goto ULP immediately in ULP_PROG_2
	pmu_enter_sleep();
	while (1);
#endif
#endif

	for (;;) {
#if defined(ULP_PROG_1) && defined(CFG_EXTI_ULP)
		k_msg_loop();
#endif
	}

	return 0;
}
