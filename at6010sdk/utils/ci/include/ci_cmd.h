/**
 *******************************************************************************
 * @file    ci_cmd.h
 * @author  Airtouch Software Team
 * @brief   Firmware Software version
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

#ifndef __CI_CMD_H__
#define __CI_CMD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_types.h"
#include "ci_main.h"

/**
 * @defgroup CI_CMD CI CMD
 * @details CI CMD parse && execution
 * @{
 */

/**
 * @defgroup CI_CMD_GROUP CI CMD GROUP
 * @{
 */

/**
 * @defgroup CI_CMD_BASE Base Command
 * @{
 */
#define CI_REG_W                       0x00
#define CI_REG_R                       0x01
#define CI_DIST_SET                    0x02
#define CI_DIST_GET                    0x03
#define CI_LOT_SET                     0x04
#define CI_LOT_GET                     0x05
#define CI_LUX_SET                     0x06
#define CI_LUX_GET                     0x07
#define CI_LIGHT_SET                   0x0A
#define CI_PWM_SET                     0x0B
#define CI_BHR_DIST_SET                0x0C
#define CI_BHR_DIST_GET                0x0D
#define CI_MICRODET_DIST_SET           0x0E
#define CI_MICRODET_DIST_GET           0x0F
#define CI_MEM_W                       0x10
#define CI_MEM_R                       0x11
#define CI_FLASH_R                     0x12
#define CI_SYS_RESET                   0x13
#define CI_FLASH_W                     0x14
#define CI_SET_BAUD                    0x19
#define CI_RADAR_GET                   0xD0
#define CI_RADAR_SET                   0xD1
#define CI_MDET_DELTA_SET              0xD2
#define CI_MDET_DELTA_GET              0xD3
/**
 * @}
 */

/**
 * @defgroup CI_CMD_EXTEND Extend Command
 * @{
 */
#define CI_CFG_SAVE_SET                0x08
#define CI_CFG_SAVE_GET                0x09
#define CI_USER_SETTING_CLEAR          0x15

#define CI_MDET_INFO                   0x30
#define CI_ALG_TYPE_QUERY              0x31
#define CI_MDET_BOUND_CFG_GET          0x32
#define CI_MDET_DEFAULT_CFG_GET        0x33

#define CI_MDET_UNDET_DELTA_SET        0x34
#define CI_MDET_SSTV_LEV_SET           0x35

#define CI_MICRO_DELTA_SET             0x36
#define CI_MICRO_UNDET_DELTA_SET       0x37
#define CI_MICRO_SSTV_SET              0x38

#define CI_BHR_DELTA_SET               0x39
#define CI_BHR_UNDET_DELTA_SET         0x3a
#define CI_BHR_SSTV_SET                0x3b

#define CI_MDET_INFO_MV_PWR            0x3c
/**
 * @}
 */

/**
 * @defgroup CI_CMD_FIRMWARE Firmware Command
 * @{
 */
/* firware download command. */
#define CI_DO_RESET                    0x20
#define CI_PREPARE_DOWN                0x21
#define CI_DO_ERASE_FLS                0x22
#define CI_DO_PROG                     0x23
#define CI_SEND_D_PACK                 0x24
#define CI_SEND_D_FINISH               0x25
#define CI_DO_VERIFY                   0x26
#define CI_GO_RUN                      0x27
/**
 * @}
 */


/**
 * @defgroup CI_CMD_MESH Mesh Command
 * @{
 */
#define CI_MESH_PROBE                  0x50
#define CI_MESH_LAMP_ONOFF_SET         0x51
#define CI_MESH_LAMP_FLSH_SWITCH       0x52
#define CI_MESH_NODE_INFO_SET          0x53
#define CI_MESH_SENSOR_ONOFF_SET       0x54
#define CI_MESH_NETWK_ONOFF_SET        0x55
#define CI_MESH_INDCD_BRTNESS_SET      0x56
#define CI_MESH_SLP_BRTNESS_SET        0x57
#define CI_MESH_LGT_DELAY_TM_SET       0x58
#define CI_MESH_NGBR_GRPNOTC_SET       0x59
#define CI_MESH_ALL_CFGDATA_SET        0x5A
/**
 * @}
 */

/**
 * @defgroup CI_CMD_HTM HTM Command
 * @{
 */
#define CI_HTM_MEASURE_START           0x70
#define CI_HTM_WAKEUP_END              0x71

/**
 * @}
 */

/**
 * @defgroup CI_DLD Download file commands
 * @brief Download file while SDK is running.
 * @details Only the following partition is allowed: new firmware, system data, user data.
 * @{
 */
#define CI_DLD_RESET                   0x80
#define CI_DLD_CONFIG                  0x81
#define CI_DLD_ERASE                   0x82
#define CI_DLD_START                   0x83 /**< Receiver starts Ymodem receive process */
#define CI_DLD_PACKING                 0x84
#define CI_DLD_FINISH                  0x85
#define CI_DLD_VERIFY                  0x86
/**
 * @}
 */

/**
 * @defgroup CI_CMD_ULP ULP Commands
 * @{
 */
#define CI_ULP_SET_ACTIVE_TIME         0x90
#define CI_ULP_EXTI_REG_READ           0x91
#define CI_ULP_EXTI_REG_WRITE          0x92
/**
 * @}
 */

/**
 * @defgroup CI_CMD_OTHERS Other Commands
 * @{
 */
/* other command */
#define CI_HOST_ACK                    0xe0
#ifdef CFG_EOL_TESTMOD
#define CI_EOL_ENTRY_TESTMOD           0xe1
#define CI_EOL_SNR_GET                 0xe2
#define CI_EOL_GPIO_TEST               0xe3
#define CI_EOL_LOCALE_FFT_DUMP         0xe5
#define CI_EOL_PHASECAL_LOCALE_SET     0xe7
#define CI_EOL_PHASECAL_RES_R          0xe8
#define CI_EOL_PHASECAL_RES_W          0xe9
#define CI_EOL_PHASECAL_APPLY_TGTGET   0xea
#define CI_EOL_IO_OUT_SET              0xeb
#define CI_EOL_PWRLVL_SET              0xec
#define CI_EOL_SELF_CHECK              0xed
#define CI_EOL_RFTX_ONOFF              0xee
#define CI_EOL_EXIT_TESTMOD            0xef
#endif
#define CI_HOST_COMPLTM_GET            0xe4
#define CI_UUID_GET                    0xe6
#define CI_VER_GET                     0xfe
/**
 * @}
 */


/**
 * @}
 */


typedef enum {
	CI_ERR_NO             = 0x00,

	CI_ERR_PARA_LEN_INVLD = 0x01,

	CI_ERR_PARA_ERR       = 0x02,

	CI_ERR_PROG_MOD_OUT   = 0x03,

	CI_ERR_ERASE          = 0x04,

	CI_ERR_FLASH_READ     = 0x05,

	CI_ERR_INVALID        = 0xff,
} ci_err_code_t;

typedef enum {
	CI_VERIF_NO           = 0x00,

	CI_VERIF_CRC16        = 0x01,

	CI_VERIF_CRC32        = 0x02,

	CI_VERIF_CKSUM        = 0x03,

	CI_VERIF_INVALID      = 0xff,
} ci_verif_mod_t;

u16  get_check_code(const ci_ctrl_frm_t *p);
void cmd_resp_para(u8 cmdi, u8 *para, u8 len);
u8   ci_cmd_proc(const ci_ctrl_frm_t *p);
u8   ci_frm_parser(ci_ctrl_frm_t *frm, u8 *buf);
u8   ci_get_resp(u8 cmd, u32 timeout);
u8   ci_resp_proc(const ci_ctrl_frm_t *p);
void ci_setting_load(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__CI_CMD_H__ */
