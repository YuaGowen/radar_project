#include "sys_data.h"
#include "spi_flash_drv.h"
#include "bootrom_func.h"
#include "chip_defs.h"
#include "stdio.h"
#include "atlog.h"

#define SYS_DATA_OFFSET (0x00ffffff & FLASH_SYS_DATA_ADDR)
#define SYS_DATA_SIZE   FLASH_SYS_DATA_SIZE

typedef struct _sys_data {
	u32 sys_data_version;
	ant_phase_item_t ant_phase;
} sys_data_t;

static const u32 m_item_size_arr[INDEX_INVALID] = {sizeof(u32), sizeof(ant_phase_item_t)};

static sys_data_t *m_sys_data_ptr;

static const sys_data_t sys_data_default = {
	.sys_data_version = 0U,
	.ant_phase.calib = 0x013a0000U,	//0x13aU,	//delta==0,k=314
	.ant_phase.check_code = 0x8c15ce0cU
};

static u8 items_verify(sys_data_index_t index, void *p)
{
	u8 ret = 2U;

	if (index >= INDEX_INVALID) {
		return ret;
	}

	switch (index) {
	case INDEX_SYS_DATA_VERSION:
		ret = (m_sys_data_ptr->sys_data_version == *(u32 *)p) ? 0U : 1U;
		break;
	case INDEX_ANT_PHASE:
		ret = (m_sys_data_ptr->ant_phase.calib == ((ant_phase_item_t *)p)->calib) ? 0U : 1U;
		break;
	default: break;
	}

	return ret;
}

void sys_data_init(u32 version)
{
	m_sys_data_ptr = (sys_data_t *)(FLASH_SYS_DATA_ADDR);
	if (m_sys_data_ptr->sys_data_version != version) {
		//g_sys_data = (void *)&sys_data_default;
	}
}

void *sys_data_item_get(sys_data_index_t index)
{
	u32 cc1, cc2;
	void *ret = NULL;

	if (index >= INDEX_INVALID) {
		return ret;
	}

	switch (index) {
	case INDEX_SYS_DATA_VERSION:
		ret = &m_sys_data_ptr->sys_data_version;
		break;
	case INDEX_ANT_PHASE:
		cc1 = m_sys_data_ptr->ant_phase.check_code;
		cc2 = bootrom_crc32((u8 *)&m_sys_data_ptr->ant_phase.calib, sizeof(ant_phase_item_t) - sizeof(u32));

		if (cc1 == cc2) {
			ret = &m_sys_data_ptr->ant_phase;
		}
		else {
			ret = (void *)&sys_data_default.ant_phase;
		}
		break;
	default:
		break;
	}

	return ret;
}

static u8 byte_diff_check(void *old_item, void *new_item, u32 byte_size)
{
	u8 ret = 0U;
	u8 byte_old, byte_new;

	for (u32 i = 0; i < byte_size; i++) {
		byte_old = *((u8 *)old_item + i);
		byte_new = *((u8 *)new_item + i);
		if (((byte_old ^ byte_new) & byte_new) > 0U) {
			ret = 1U;
			break;
		}
	}

	return ret;
}

void sys_data_item_set(sys_data_index_t index, void *new_item)
{
	u32 read_byte_cnt, write_byte_cnt;
	sys_data_t tmp;
	void *old_item = NULL;
	u8 erase_flag = 0U;

	if (index >= INDEX_INVALID) {
		return;
	}

	spi_flash_ll_read(SYS_DATA_OFFSET, sizeof(sys_data_t), &read_byte_cnt, (u8 *)&tmp);

	switch (index) {
	case INDEX_SYS_DATA_VERSION:
		old_item = &tmp.sys_data_version;
		break;
	case INDEX_ANT_PHASE:
		old_item = &tmp.ant_phase;
		break;
	default:
		break;
	}

	if (1U == byte_diff_check(old_item, new_item, m_item_size_arr[index])) {
		spi_flash_erase(SYS_DATA_OFFSET, (SYS_DATA_OFFSET + SYS_DATA_SIZE));
		erase_flag = 1U;
	}

	switch (index) {
	case INDEX_SYS_DATA_VERSION:
		tmp.sys_data_version = *(u32 *)new_item;
		break;
	case INDEX_ANT_PHASE:
		tmp.ant_phase.calib = ((ant_phase_item_t *)new_item)->calib;
		tmp.ant_phase.check_code = bootrom_crc32((u8 *)&tmp.ant_phase, sizeof(ant_phase_item_t) - sizeof(u32));
		break;
	default:
		break;
	}

	if (erase_flag == 1U) {
		spi_flash_write(SYS_DATA_OFFSET, sizeof(sys_data_t), &write_byte_cnt, (u8 *)&tmp);
	}
	else {
		switch (index) {
		case INDEX_SYS_DATA_VERSION:
			spi_flash_write(SYS_DATA_OFFSET, sizeof(u32), &write_byte_cnt, (u8 *)&tmp.sys_data_version);
			break;
		case INDEX_ANT_PHASE:
			spi_flash_write(SYS_DATA_OFFSET + sizeof(u32), sizeof(ant_phase_item_t), &write_byte_cnt, (u8 *)&tmp.ant_phase);
			break;
		default:
			break;
		}
	}
}
