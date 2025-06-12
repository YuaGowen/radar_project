/**
* Copyright (c) 2023, Airtouching Intelligence Technology.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form, except as embedded into a Airtouching
* Intelligence Technology integrated circuit in a product or a software update for
* such product, must reproduce the above copyright notice, this list of
* conditions and the following disclaimer in the documentation and/or other
* materials provided with the distribution.
*
* 3. Any software provided in binary form under this license must not be reverse
* engineered, decompiled, modified and/or disassembled.
*
* THIS SOFTWARE IS PROVIDED BY AIRTOUCHING TECHNOLOGY "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL AIRTOUCHING TECHNOLOGY OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "jiffies.h"
#include "hal_dualtimer.h"
#include "osal_time.h"

/**
 * @brief CPU busy-waits delay.
 *
 * @param[in]  ms  delay time in milliseconds.
 */
void osal_mdelay(u32 ms)
{
	hal_dualtimer_delay_ms(ms);
}

/**
 * @brief CPU busy-waits delay.
 *
 * @param[in]  us  delay time in microsecond.
 */
void osal_udelay(u32 us)
{
	hal_dualtimer_delay_us(us);
}

/**
 * @brief Sleep the current task.
 *
 * @param[in]  ms  sleep time in milliseconds.
 */
void osal_msleep(u32 ms)
{
	osal_mdelay(ms);

	return ;
}

/**
 * @brief Get the number of ticks since OS start.
 *
 * @return OS kernel current tick count.
 */
u32 osal_time_get_tick(void)
{
	return jiffies();
}

/**
 * @brief Get the OS kernel tick frequency.
 *
 * @return frequency of the kernel tick in hertz, i.e. kernel ticks per second.
 */
u32 osal_time_get_tick_freq(void)
{
	return OSAL_HZ;
}

/**
 * @brief Get current time in nanoseconds.
 *
 * @return  elapsed time in nanoseconds from system starting.
 */
u64 osal_time_get_ns(void)
{
	return osal_time_get_ms() * 1000 * 1000;
}

/**
 * @brief Get current time in milliseconds.
 *
 * @return  elapsed time in milliseconds from system starting.
 */
u64 osal_time_get_ms(void)
{
	u64 ms;
	u32 tick_cnt;

	tick_cnt = osal_time_get_tick();

	ms = tick_cnt * 1000 / OSAL_HZ;

	return ms;
}

/**
 * @brief Get current time in seconds.
 *
 * @return elapsed time in seconds from system starting
 */
u32 osal_time_get_sec(void)
{
	return osal_time_get_tick() / OSAL_HZ;
}
