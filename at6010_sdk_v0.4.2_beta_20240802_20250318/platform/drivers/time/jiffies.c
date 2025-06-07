
#include "jiffies.h"
#include "CMSDK_CM0plus.h"
#include "core_cm0plus.h"
#include "gpio.h"
#include "sys_timer.h"
#ifdef CFG_OS
#include "osal.h"
#endif

static u32 __jiffies = 0;

__STATIC_INLINE void inc_jiffies(void)
{
	__jiffies++;
}

u32 jiffies(void)
{
#ifdef CFG_OS
	return osal_time_get_tick();
#else
	return __jiffies;
#endif
}

void sys_tick_handler(void)
{
	/* Increase jiffies   */
	inc_jiffies();
	sys_timer_period();
}

void sys_tick_init(void)
{
	/* Config systick to enable interrupt */
	SysTick_Config((SYS_CLK_FREQ/1000000u) * SYS_TICK_TIMES);

	/* Init jiffies variable */
	__jiffies = 0;
}
