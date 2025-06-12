/**
 * Copyright (c) 2023, Airtouching Intelligence Technology.
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

#include <string.h>
#include "user_setting.h"
#include "spi_flash_drv.h"
#include "atlog.h"

#ifdef CFG_USER_SETTING

typedef struct _user_setting {
	u8 is_inited;
	u8 load_buf[USER_SETTING_SIZE];
	u32 item_num;
} user_setting_t;

user_setting_t m_user_ctx = {
	0,
	{ 0 },
	0,
};


int user_setting_init(void)
{
	u32 r_byte;

	if (m_user_ctx.is_inited)
		return USER_SETTING_STATUS_SUCCESS;

	m_user_ctx.item_num = USER_SETTING_SIZE / sizeof(user_setting_item_t);

	/* Read settings from flash to m_user_ctx.load_buf */
	spi_flash_ll_read(USER_SETTING_OFFSET,
		USER_SETTING_SIZE, &r_byte, m_user_ctx.load_buf);

	m_user_ctx.is_inited = 1;

	return USER_SETTING_STATUS_SUCCESS;
}

user_setting_status_t user_setting_get(const char *name, char *val, int *len)
{
	u32 i;
	user_setting_item_t *setting = (user_setting_item_t *)m_user_ctx.load_buf;

	if (!m_user_ctx.is_inited)
		return USER_SETTING_STATUS_NEED_INIT;

	if ((name == NULL) || (val == NULL) || (len == NULL))
		return USER_SETTING_STATUS_ERROR;

	for (i = 0; i < m_user_ctx.item_num; i++, setting++) {
		if ((setting->hdr.valid == USER_VALID_FLAG) &&
			(strncmp(name, setting->name, USER_SETTING_NAME_SIZE) == 0)) {
			memcpy(val, setting->val, setting->hdr.val_len);
			*len = setting->hdr.val_len;
			return USER_SETTING_STATUS_SUCCESS;
		}
	}

	return USER_SETTING_STATUS_MISMATCH;
}

int user_setting_set(const char *name, const char *val, int len)
{
	u32 i, name_len, val_len;
	user_setting_item_t *setting;

	if (!m_user_ctx.is_inited)
		return USER_SETTING_STATUS_NEED_INIT;

	if ((name == NULL) || (val == NULL))
		return USER_SETTING_STATUS_ERROR;

	setting = (user_setting_item_t*)m_user_ctx.load_buf;
	name_len = strlen(name);
	val_len = len;

	if ((name_len > USER_SETTING_NAME_SIZE) ||(val_len > USER_SETTING_VAL_SIZE)) {
		return USER_SETTING_STATUS_ERROR;
	}

	/* If the specific setting is exist, just update the value */
	for (i = 0; i < m_user_ctx.item_num; i++, setting++) {
		if ((setting->hdr.valid == USER_VALID_FLAG) &&
			(strncmp(name, setting->name, USER_SETTING_NAME_SIZE) == 0)) {
			memcpy(setting->val, val, val_len);
			setting->hdr.val_len = val_len;

			return USER_SETTING_STATUS_SUCCESS;
		}
	}

	setting = (user_setting_item_t*)m_user_ctx.load_buf;
	/* If the specific setting is not exist, create a new one */
	for (i = 0; i < m_user_ctx.item_num; i++, setting++) {
		if ((setting->hdr.valid != USER_VALID_FLAG)) {
			memset(setting->name, 0,
					USER_SETTING_NAME_SIZE + USER_SETTING_VAL_SIZE);
			memcpy(setting->name, name, name_len);
			memcpy(setting->val, val, val_len);
			setting->hdr.name_len = name_len;
			setting->hdr.val_len = val_len;
			setting->hdr.valid = USER_VALID_FLAG;

			return USER_SETTING_STATUS_SUCCESS;
		}
	}

	return USER_SETTING_STATUS_ERROR;
}

int user_setting_delete(char *name)
{
	u32 i;
	user_setting_item_t *setting;

	if (!m_user_ctx.is_inited)
		return USER_SETTING_STATUS_NEED_INIT;

	if (name == NULL)
		return USER_SETTING_STATUS_ERROR;

	setting = (user_setting_item_t*)m_user_ctx.load_buf;

	for (i = 0; i < m_user_ctx.item_num; i++, setting++) {
		if ((setting->hdr.valid == USER_VALID_FLAG) &&
			(strncmp(name, setting->name, USER_SETTING_NAME_SIZE) == 0)) {
			setting->hdr.valid = 0;

			return USER_SETTING_STATUS_SUCCESS;
		}
	}

	return USER_SETTING_STATUS_ERROR;
}

u32 user_setting_delete_all(void)
{
	memset(m_user_ctx.load_buf, 0, USER_SETTING_SIZE);

	return (u32)USER_SETTING_STATUS_SUCCESS;
}

int user_setting_erase(void)
{
	u32 start, end;

	start = (USER_SETTING_OFFSET);
	end = start + USER_SETTING_SIZE;

	logi("%s, start: 0x%x, end: 0x%x\r\n", __func__, start, end);

	spi_flash_erase(start, end);

	return USER_SETTING_STATUS_SUCCESS;
}

int user_setting_store(void)
{
	spifls_err_id_t ret = SPIFLS_ERR_NO;
	u32 w_bytes = 0U;

	user_setting_erase();

	ret = spi_flash_write(USER_SETTING_OFFSET, USER_SETTING_SIZE, &w_bytes, m_user_ctx.load_buf);

	if (SPIFLS_ERR_NO != ret) {
		return USER_SETTING_STATUS_ERROR;
	}

	return USER_SETTING_STATUS_SUCCESS;
}

#endif /* !CFG_USER_SETTING */
