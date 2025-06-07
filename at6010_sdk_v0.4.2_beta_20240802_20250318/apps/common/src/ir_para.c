/**
 *******************************************************************************
 * @file    ir_para.c
 * @author  Airtouch Software Team
 * @brief   target configuration of the current project
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
 *    Intelligence Technology integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
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
 */

#include "cmsis_compiler.h"
#include "ir_para.h"
#include "atlog.h"

#ifdef CFG_IR

typedef struct {
	u8 mode;
	u8 intensity;
	u8 detectionRange;
	u8 standbyLevel;
	u8 activeTimer;
	u8 luxLevel;
} ir_para_t;


static ir_para_t g_ir_para = {
	.mode = IR_MODE_SC1,
	.intensity = PARA_PWM_LON_DUTY / 10u,
	.detectionRange = IR_CODE_DIST_L4,
	.standbyLevel = IR_CODE_STD_L4,
	.activeTimer = IR_CODE_KEEP_TIME1_L4,
	.luxLevel = IR_CODE_LUX_L4
};


__STATIC_INLINE void set_ir_para_mode(u8 mode)
{
	g_ir_para.mode = mode;
}

__STATIC_INLINE u8 get_ir_para_mode(void)
{
	return g_ir_para.mode;
}

__STATIC_INLINE void set_ir_para_intensity(u8 para)
{
	g_ir_para.intensity = para;
}

__STATIC_INLINE u8 get_ir_para_intensity(void)
{
	return g_ir_para.intensity;
}

__STATIC_INLINE void set_ir_para_detectionRange(u8 para)
{
	g_ir_para.detectionRange = para;
}

__STATIC_INLINE u8 get_ir_para_detectionRange(void)
{
	return g_ir_para.detectionRange;
}

__STATIC_INLINE void set_ir_para_standbyLevel(u8 para)
{
	g_ir_para.standbyLevel = para;
}

__STATIC_INLINE u8 get_ir_para_standbyLevel(void)
{
	return g_ir_para.standbyLevel;
}

__STATIC_INLINE void set_ir_para_activeTimer(u8 para)
{
	g_ir_para.activeTimer = para;
}

__STATIC_INLINE u8 get_ir_para_activeTimer(void)
{
	return g_ir_para.activeTimer;
}

__STATIC_INLINE void set_ir_para_luxLevel(u8 para)
{
	g_ir_para.luxLevel = para;
}

__STATIC_INLINE u8 get_ir_para_luxLevel(void)
{
	return g_ir_para.luxLevel;
}

void set_ir_para(ir_para_type_t para, u8 value)
{
	switch (para) {
	case IR_PARA_MODE:
		set_ir_para_mode(value);
		break;
	case IR_PARA_INTENSITY:
		set_ir_para_intensity(value);
		break;
	case IR_PARA_DETECTION_RANGE:
		set_ir_para_detectionRange(value);
		break;
	case IR_PARA_STANDBY_LEVEL:
		set_ir_para_standbyLevel(value);
		break;
	case IR_PARA_ACTIVE_TIMER:
		set_ir_para_activeTimer(value);
		break;
	case IR_PARA_LUX_LEVEL:
		set_ir_para_luxLevel(value);
		break;
	default:
		loge("[%s]para not exists.\r\n", __func__);
		break;
	}
}

u8 get_ir_para(ir_para_type_t para)
{
	u8 ret;
	switch (para) {
	case IR_PARA_MODE:
		ret = get_ir_para_mode();
		break;
	case IR_PARA_INTENSITY:
		ret = get_ir_para_intensity();
		break;
	case IR_PARA_DETECTION_RANGE:
		ret = get_ir_para_detectionRange();
		break;
	case IR_PARA_STANDBY_LEVEL:
		ret = get_ir_para_standbyLevel();
		break;
	case IR_PARA_ACTIVE_TIMER:
		ret = get_ir_para_activeTimer();
		break;
	case IR_PARA_LUX_LEVEL:
		ret = get_ir_para_luxLevel();
		break;
	default:
		ret = 0u;
		loge("[%s]para not exists.\r\n", __func__);
		break;
	}
	return ret;
}

#endif /* !CFG_IR */
