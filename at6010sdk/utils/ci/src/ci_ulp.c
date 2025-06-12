/**
 *******************************************************************************
 * @file    ci_ulp.c
 * @author  Airtouch Software Team
 * @brief   ULP commands
 *******************************************************************************
 * @copyright Copyright (c) 2024, Airtouching Intelligence Technology.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Airtouching
 *    Intelligence Technology integrated circuit in a product or a software update
 *    for such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
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
 ******************************************************************************/

/* Includes --------------------------------------------------------------------------------------*/

#include "ci_cmd.h"
#include "i2c_host.h"
#include "ci_ulp.h"
#include "pmu.h"

#if defined(CFG_EOL_TESTMOD)
#include "ci_eol.h"
#endif

#if defined(ULP_PROG_1)
#include "sys_timer.h"
#include "osal_timer.h"
#endif /* !ULP_PROG_1 */

#if defined(CFG_CI)

#if defined(ULP_PROG_1)

static osal_timer_t m_ulp_prog1_active_timer = { 0 }; // Period: 1 second
volatile s8  m_ulp_prog1_run_time = PARA_ULP_PROG_1_ACTIVE_TIME;
static   s8 volatile m_ulp_prog1_run_time_setting = PARA_ULP_PROG_1_ACTIVE_TIME;

/**
 *@brief
 *
 * @param arg
 */
void ulp_prog1_active_timer_cb(void* arg)
{
	m_ulp_prog1_run_time--;
	logi("PROG1 CNTD = %d\r\n", m_ulp_prog1_run_time);

	if (m_ulp_prog1_run_time <= 0) {
		osal_timer_stop(&m_ulp_prog1_active_timer);
#if defined(CFG_EOL_TESTMOD)
		ci_eol_timeout();
#endif

#if defined(CFG_EXTI_ULP)
		pmu_enter_sleep();
#endif
	}
}

/**
 * @brief
 *
 * @param to
 * @param mode see @ref refresh_mode_t
 */
void ulp_prog1_refresh_active_timer(u8 to, u8 mode)
{
	if (REFRESH_MODE_RELOAD == mode) {
		m_ulp_prog1_run_time = m_ulp_prog1_run_time_setting;
	} else if (REFRESH_MODE_FORCE == mode) {
		m_ulp_prog1_run_time_setting = to;
		m_ulp_prog1_run_time = m_ulp_prog1_run_time_setting;
	} else {
		// nothing
	}
}

#endif /* ULP_PROG_1 */


/**
 * @brief Update ULP_PROG_1 active time.
 * Positive value means active time, zero means jump to ULP_PROG_2 immediately.
 *
 * @param p
 * @param len
 * @return u8
 */
u8 ci_ulp_set_active_time(const u8* p, u8 len)
{
	u8 time = 0;
	u8 ret = 0;
	u8 ret_para = CI_ERR_NO;

	if ((NULL == p) || (len != 1)) {
		ret_para = CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		time = p[0];

		ulp_prog1_refresh_active_timer(time, REFRESH_MODE_FORCE);
	}

	cmd_resp_para(CI_ULP_SET_ACTIVE_TIME, &ret_para, 1);

	return ret;
}

/**
 * @brief ULP_PROG_1 支持 CI 命令，倒计时进入 ULP_PROG_2 程序
 */
void ulp_start_active_timer(void)
{
	osal_status_t ret = osal_timer_create(&m_ulp_prog1_active_timer, OSAL_TIMER_MODE_PERIODIC,
	                                      ulp_prog1_active_timer_cb, NULL, 1000);
	if(OSAL_OK != ret) {
		loge("Error: failed to create ULP_PROG_1 Active Timer!!! \r\n");
	}

	osal_timer_start(&m_ulp_prog1_active_timer);
}

#if defined(CFG_EXTI_ULP)

#define I2C_SLV_ADDR              0x28   /* 58xx IIC Device Address. */

/**
 * @brief Read AT5815C register via I2C.
 * usage: reg_read reg_addr
 *
 * @param p
 * @param len
 * @return u8
 */
u8 ci_ulp_exit_reg_read(const u8* p, u8 len)
{
	u8 ret = CI_ERR_NO, ret_param = CI_ERR_NO;
	u8 reg_addr = 0, reg_val = 0;
	i2c_err_t err = I2C_ERR_NONE;

	if ((NULL == p) || (1 != len)) {
		ret_param = CI_ERR_PARA_LEN_INVLD;
		ret = CI_ERR_INVALID;
	} else {
		reg_addr = p[0];
		err = i2c_host_read_byte(I2C_SLV_ADDR, reg_addr, &reg_val);
		if (err != I2C_ERR_NONE) {
			ret_param = CI_ERR_INVALID;
			ret = CI_ERR_INVALID;
		} else {
			ret_param = reg_val;
		}
	};

	cmd_resp_para(CI_ULP_EXTI_REG_READ, &ret_param, sizeof(u8));

	return ret;
}

/**
 *@brief Write AT5815C register via I2C.
 * usage: reg_write reg_addr reg_value
 *
 * @param p
 * @param len
 * @return u8
 */
u8 ci_ulp_exit_reg_write(const u8* p, u8 len)
{
	u8 ret = CI_ERR_NO, ret_param = CI_ERR_NO;
	u8 reg_addr = 0, reg_val = 0;
	i2c_err_t err = I2C_ERR_NONE;

	if ((NULL == p) || (2 != len)) {
		ret_param = CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		reg_addr = p[0];
		reg_val = p[1];

		err = i2c_host_write_byte(I2C_SLV_ADDR, reg_addr, reg_val);
		if (err != I2C_ERR_NONE) {
			ret_param = CI_ERR_INVALID;
			ret = CI_ERR_INVALID;
		} else {
			ret_param = CI_ERR_NO;
		}
	};

	cmd_resp_para(CI_ULP_EXTI_REG_WRITE, &ret_param, sizeof(u8));

	return ret;
}

#endif /* !CFG_EXTI_ULP */

#endif /* !CFG_CI */
