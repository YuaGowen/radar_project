/**
 *******************************************************************************
 * @file    radar_ext_chip.h
 * @author  Airtouch Software Team
 * @brief   External radar chip driver.
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

#ifndef __RADAR_EXT_CHIP_H__
#define __RADAR_EXT_CHIP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/

#include "at_common.h"

/* Constants -------------------------------------------------------------------------------------*/

/**
 * @defgroup AT5815C_DET_2ND_TRIGGER Detect 2nd trigger Enable/Disable
 * @{
 */
#define DET_2ND_TRIGGER_DISABLE         1
#define DET_2ND_TRIGGER_ENABLE          0
/**
 * @}
 */

/* Data Types ------------------------------------------------------------------------------------*/

/**
 * @brief 感应判断参数
 *
 */
typedef struct {
	u8 signal_det_len;     /**< 定义窗口长度，8比特长度，寄存器 0x36 默认值 0x0A                         */
	u8 power_det_win_len;  /**< 连续观察的窗口总数，8比特长度，寄存器 0x49 默认值 0x04                    */
	u8 pwer_det_win_thr;   /**< 触发需要超过阈值的窗口数，8比特长度，寄存器 0x4A 默认值 0x03               */
	u8 cnf_power_th_dr;    /**< 阈值配置方式：1 -- 寄存器来配置； 0 -- 外部PIN控制。寄存器 0x02 的 BIT3    */
	u16 signal_det_thr;    /**< 阈值设置范围: 0~1024 。寄存器 0x38 配置低8位，寄存器 0x39 配置高2位       */
} detect_para_t;


/**
 * @brief 二次触发功能开关
 */
typedef struct {
	u8 det_2nd_trigger; /**< 0 -- 开启二次触发；1 -- 关闭二次触发, see @ref AT5815C_DET_2ND_TRIGGER    */
} power_det_ctrl_t;

/* Macros ----------------------------------------------------------------------------------------*/

#define IS_DET_2ND_TRIGGER_MDOE(v)      (((v) == DET_2ND_TRIGGER_DISABLE) || ((v) == DET_2ND_TRIGGER_ENABLE))


/* Function Declarations -------------------------------------------------------------------------*/

void at5815c_init(void);

void at5815c_turn_off_ls(void);

void at5815c_2nd_trigger_cfg(u8 en);

void at5815c_cfg_freq(void);

void at5815c_cfg_lps(void);

void at5815c_cfg_distance(u8 distance);

#ifdef __cplusplus
}
#endif

#endif /* !__RADAR_EXT_CHIP_H__ */
