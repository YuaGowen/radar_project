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

#if defined(CFG_CI)
#include "k_msg.h"
#include "ci_main.h"
#include "ci_port.h"
#include "ci_ulp.h"
#include "ci_dld.h"
#include "ci_eol.h"
#include "jiffies.h"
#include "sys_timer.h"
#endif

/* Configurable ----------------------------------------------------------------------------------*/

//#define DBG_HTOL_DIG_VCORE /* control dig vcore during power up/down for htol test. */

/* Private Variables -----------------------------------------------------------------------------*/

/* Private Functions -----------------------------------------------------------------------------*/

/* Function Implementations ----------------------------------------------------------------------*/

static void mcu_init(void)
{
	clk_mcu_set(CLK_SRC_PLL, 80000000U);
	hal_dualtimer_init();
}

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

	ulp_start_active_timer();
#endif /* !CFG_CI */
#endif /* ULP_PROG_1 */
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

	hal_radar_ulp_start();
}

/*------------------------------------------  MAIN  ----------------------------------------------*/
extern u8 volatile m_ulp_prog1_run_time;

int main(void)
{
	platform_init();

	radar_cfg_ulp_init();

#if defined(ULP_PROG_1)
#if defined(CFG_CI)
	for (;;) {
		k_msg_loop();

#if defined(CFG_EOL_TESTMOD)
		eol_app_process();
		if (1U == ci_eol_exit_confirm()) {
			logi("EOL exit...\r\n");
			break;
		}
#endif
	}
#endif /* !CFG_CI */
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

	logi("enter sleep...\r\n");
	pmu_enter_sleep();
	while (1);

	return 0;
}
