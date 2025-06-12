/**
 *******************************************************************************
 * @file    user_setting.h
 * @author  Airtouch Software Team
 * @brief   Firmware Software version
 *******************************************************************************
 * @copyright Copyright (c) 2023, Airtouching Intelligence Technology.
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

#ifndef __USER_SETTING_H__
#define __USER_SETTING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_types.h"

#define USER_SETTING_OFFSET                 0x3E000
#define USER_VALID_FLAG                     0xA6
#define USER_SETTING_NAME_SIZE              10
#define USER_SETTING_VAL_SIZE               24//10

typedef struct _user_setting_hdl {
	u8 valid;
	u8 reserved;
	u8 name_len;
	u8 val_len;
} user_setting_hdl_t;
typedef struct _user_setting_item {
	user_setting_hdl_t hdr;
	char name[USER_SETTING_NAME_SIZE];
	char val[USER_SETTING_VAL_SIZE];
} user_setting_item_t;

typedef enum {
	USER_SETTING_CFG_SAVE_NAME_ITEM,
	USER_SETTING_DIST_NAME_ITEM,
	USER_SETTING_LOT_NAME_ITEM_ITEM,
	USER_SETTING_RADAR_ONOFF_ITEM,
	USER_SETTING_CI_BDR_ITEM,
	USER_SETTING_DET_MOT_START_ITEM,
	USER_SETTING_DET_MOT_STOP_ITEM,
	USER_SETTING_DET_SSTV_LEV_ITEM,
#ifdef CFG_LS
	USER_SETTING_LUX_NAME_ITEM,
#endif
#ifdef CFG_PRESENCE_DET
	USER_SETTING_BHR_DIST_NAME_ITEM,
	USER_SETTING_DET_BHR_START_ITEM,
	USER_SETTING_DET_BHR_STOP_ITEM,
	USER_SETTING_DET_BHR_SSTV_LEV_ITEM,
#endif
#ifdef CFG_MICRO_DET
	USER_SETTING_MICRO_DIST_NAME_ITEM,
	USER_SETTING_DET_MICRO_START_ITEM,
	USER_SETTING_DET_MICRO_STOP_ITEM,
	USER_SETTING_DET_MICRO_SSTV_LEV_ITEM,
#endif
	USER_SETTING_ITEM_COUNT
} user_setting_items_t;

#define USER_SETTING_SIZE                   (sizeof(user_setting_item_t)*USER_SETTING_ITEM_COUNT) /* Do not exceed   0x1000(4 KB) */


typedef enum {
	USER_SETTING_STATUS_SUCCESS   = 0,
	USER_SETTING_STATUS_ERROR     = 1,
	USER_SETTING_STATUS_NEED_INIT = 2,
	USER_SETTING_STATUS_MISMATCH  = 3,
} user_setting_status_t;

user_setting_status_t user_setting_get(const char *name, char *val, int *len);
int user_setting_set(const char *name, const char *val, int len);
int user_setting_delete(char *name);
u32 user_setting_delete_all(void);
int user_setting_erase(void);
int user_setting_store(void);
int user_setting_init(void);

#ifdef __cplusplus
}
#endif

#endif /* !__USER_SETTING_H__ */
