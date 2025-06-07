/**
 *******************************************************************************
 * @file    ci_cmd.c
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

#ifdef CORTEX_M0PLUS
#include "CMSDK_CM0plus.h"
#include "core_cm0plus.h"
#endif

#include "chip_defs.h"
#include "jiffies.h"
#include "ci_main.h"
#include "ci_cmd.h"
#include "ci_dld.h"
#include "ci_port.h"
#include "spi_flash_drv.h"
#include "pmu.h"
#include "mmdefs.h"
#include "crc.h"
#include "string.h"
#include "common.h"
#include "atlog.h"
#include "radar.h"
#include "radar_mgr.h"
#include "mdet_fmcw.h"
#include "hal_ls.h"
#include "hal_light.h"
#include "user_setting.h"
#ifdef CFG_CI_MESH
#include "cimesh_api.h"
#endif

#ifdef CFG_APP_LIGHT
#include "app_light.h"
#endif

#include "version.h"

#ifdef CFG_APP_HTM
#include "app_htm.h"
#endif
#ifdef CFG_EOL_TESTMOD
#include "radar.h"
#include "sys_data.h"
#include "gpio.h"
#include "osal_time.h"
#include "ci_eol.h"
#endif

#if defined(CFG_EXTI_ULP)
#include "ci_ulp.h"
#endif

#ifdef CFG_CI

/**
 * @defgroup CI_CFG_PARAMETER_NAME CI Configuration Parameter Name
 * @{
 */
#define CI_CFG_SAVE_NAME            "save_flag"
#define CI_DIST_NAME                "dist_lv"
#define CI_LOT_NAME                 "lot_s"
#define CI_RADAR_ONOFF_NAME         "rdr_onoff"
#define CI_BDR_NAME                 "ci_bdr"

#define CI_DET_MOT_START            "mot_sta"
#define CI_DET_MOT_STOP             "mot_sto"
#define CI_DET_SSTV_LEV             "mot_sl"

#ifdef CFG_LS
#define CI_LUX_NAME                 "lux"
#endif

#ifdef CFG_PRESENCE_DET
#define CI_BHR_DIST_NAME            "bhr_dist"
#define CI_DET_BHR_START            "bhr_sta"
#define CI_DET_BHR_STOP             "bhr_sto"
#define CI_DET_BHR_SSTV_LEV         "bhr_sl"
#endif
/**
 * @}
 */

/**
 * @defgroup CI_CMD_PRIVATE_CONSTANTS CI CMD Private Constants
 * @{
 */
//#define CI_DBG_EN
#ifdef CFG_APP_HTM
#define RADARD_STAGE_MAX_RB         50
#else
#define RADARD_STAGE_MAX_RB         70
#endif

#ifdef CFG_RADAR_STAGE_SUPPORT
typedef struct _stage_pwr_t {
    u32 boundary;
    u32 pwr_count;
    u32 rb_pwr[2][RADARD_STAGE_MAX_RB];
    u32 check_code;
}stage_pwr_t;
#endif

#define FLASH_ERASE_UNIT            (4096u)
#define CI_PARA_SIZE                (30u)
#define CI_SETTING_FLAG_MAGIC       (0xC8u)

/**
 * Initial start address for programming, the program address will
 * be set by the function of prepare_download.
 */
#define PROG_SRAM_START_ADDR        0x20004000

#define MEM_READ_U32(addr)          (*(volatile u32*)(addr))
/**
 * @}
 */

typedef u8 (*func_ci_cmd_hdl) (const u8 *para, u8 len);

typedef struct {
	u8 cmdi;
	func_ci_cmd_hdl xhdl;
} ci_cmd_tbl_t;

typedef enum {
	CI_PROG_SRAM = 0,
	CI_PROG_FLASH = 1
} ci_prog_mod_t;

typedef struct {
	u16 det_mot_min;
	u16 det_mot_max;
	u16 det_micro_min;
	u16 det_micro_max;
	u16 det_bhr_min;
	u16 det_bhr_max;
	u16 det_swp_bw;
} mdet_bound_cfg;

typedef struct {
	u16 mot_det_sta;
	u16 mot_det_sto;
	u8  mot_det_sl;
	u8  reserved0;

	u16 micro_det_sta;
	u16 micro_det_sto;
	u8  micro_det_sl;
	u8  reserved1;

	u16 bhr_det_sta;
	u16 bhr_det_sto;
	u8  bhr_det_sl;
	u8  reserved2;
} mdet_default_cfg;


#ifdef CFG_USER_SETTING
typedef struct {
	u8  save_flag;           /**< A flag indicates the settings have been saved to flash */
	u8  dist_lv;             /**< The distance level for motion detection. */
	u16 lot_s;               /**< Lighting duration, unit: S */
	u8 rdr_onoff;
	u32 ci_bdr;

#ifdef CFG_LS
	u16 lux;                 /**< The threshold of Light Sensor, unit: LUX */
#endif

	u16 mot_det_sta;
	u16 mot_det_sto;
	u8  mot_det_sl;

#ifdef CFG_PRESENCE_DET
	u8  bhr_dist_lv;         /**< The distance level for breathing & heart-rate detection */
	u16 bhr_det_sta;
	u16 bhr_det_sto;
	u8  bhr_det_sl;
#endif
} ci_setting_cfg_t;

#define RAD_DET_MOT_STA             0
#define RAD_DET_MOT_STO             650
#define RAD_DET_MOT_SL              2

#define RAD_DET_MICRO_STA           0
#define RAD_DET_MICRO_STO           600
#define RAD_DET_MICRO_SL            2

#define RAD_DET_BHR_STA             0
#define RAD_DET_BHR_STO             600
#define RAD_DET_BHR_SL              1

static ci_setting_cfg_t m_setting_cfg = {
	.save_flag = 0,
	.dist_lv = PARA_MOT_DIST_LV,
	.lot_s = PARA_LIGHT_ON_TIME_MS/1000,
	.rdr_onoff = 1u,
	.ci_bdr = UART_CI_BAUDRATE,

#ifdef CFG_LS
	.lux = PARA_LS_THRESHOLD,
#endif

	.mot_det_sta = RAD_DET_MOT_STA,
	.mot_det_sto = RAD_DET_MOT_STO,
	.mot_det_sl  = RAD_DET_MOT_SL,

#ifdef CFG_PRESENCE_DET
	.bhr_dist_lv = PARA_BHR_DIST_LV,
	.bhr_det_sta = RAD_DET_BHR_STA,
	.bhr_det_sto = RAD_DET_BHR_STO,
	.bhr_det_sl  = RAD_DET_BHR_SL,
#endif
};

typedef struct {
	const char *name;
	u8 val_len;
	u16 offset;
} setting_cfg_item_t;

static const setting_cfg_item_t m_setting_cfg_items[] = {
    {CI_CFG_SAVE_NAME,    1u, offsetof(ci_setting_cfg_t, save_flag)  },
    {CI_DIST_NAME,        1u, offsetof(ci_setting_cfg_t, dist_lv)    },
    {CI_LOT_NAME,         2u, offsetof(ci_setting_cfg_t, lot_s)      },

    {CI_RADAR_ONOFF_NAME, 1u, offsetof(ci_setting_cfg_t, rdr_onoff)  },
    {CI_BDR_NAME,         4u, offsetof(ci_setting_cfg_t, ci_bdr)     },

#ifdef CFG_LS
    {CI_LUX_NAME,         2u, offsetof(ci_setting_cfg_t, lux)        },
#endif

    {CI_DET_MOT_START,    2u, offsetof(ci_setting_cfg_t, mot_det_sta)},
    {CI_DET_MOT_STOP,     2u, offsetof(ci_setting_cfg_t, mot_det_sto)},
    {CI_DET_SSTV_LEV,     1u, offsetof(ci_setting_cfg_t, mot_det_sl) },

#ifdef CFG_PRESENCE_DET
    {CI_BHR_DIST_NAME,    1u, offsetof(ci_setting_cfg_t, bhr_dist_lv)},
    {CI_DET_BHR_START,    2u, offsetof(ci_setting_cfg_t, bhr_det_sta)},
    {CI_DET_BHR_STOP,     2u, offsetof(ci_setting_cfg_t, bhr_det_sto)},
    {CI_DET_BHR_SSTV_LEV, 1u, offsetof(ci_setting_cfg_t, bhr_det_sl) },
#endif
};
#endif /* !CFG_USER_SETTING */
static inline bool check_reg_addr(u32 addr)
{
	return ((addr >= MM_REG_START) && (addr <= MM_REG_END));
}
u16 get_check_code(const ci_ctrl_frm_t *p)
{
	u16 cc = 0;
	int i = 0;

	cc = p->head + (p->cmd_grp<<5 | p->cmd) + p->para_len;
	for (i = 0; i < p->para_len; i++) {
		cc += p->para[i];
	}

	return cc;
}

static void ci_write_frm(ci_ctrl_frm_t *p)
{
	u8 buf[128] = { 0 };

	u8 idx = 0;
	int i = 0;

	buf[idx++] = p->head;
	buf[idx++] = p->cmd_grp << 5 | p->cmd;
	buf[idx++] = p->para_len;
	for (i = 0; i < p->para_len; i++) {
		buf[idx++] = p->para[i];
	}

	p->check_code = get_check_code(p);

	/* check code low byte first */
	buf[idx++] = (u8)p->check_code;
	buf[idx++] = (u8)(p->check_code >> 8);

	ci_send_bytes(buf, idx);
}

void cmd_resp_para(u8 cmdi, u8 *para, u8 len)
{
	ci_ctrl_frm_t sfrm = { 0 };

	sfrm.head = 0x59;
	sfrm.cmd = cmdi & 0x1f;
	sfrm.cmd_grp = (cmdi >> 5) & 0x07;
	sfrm.para_len = len;
	if (len != 0) {
		sfrm.para = para;
	}

	ci_write_frm(&sfrm);
}

static u8 ci_set_baud(const u8 *p, u8 len)
{
	u32 baudrate = 0;
	u32 val = 0;

	if ((p == 0) || (len < 4)) {
		val = CI_ERR_PARA_LEN_INVLD;
		cmd_resp_para(CI_SET_BAUD, (u8 *)&val, 1);
		return 0xff;
	}

	baudrate = p[3] << 24 |p[2] << 16 | p[1] << 8 |p[0];

	cmd_resp_para(CI_SET_BAUD, (u8 *)&val, 1);

#ifdef CFG_USER_SETTING
	m_setting_cfg.ci_bdr = baudrate;
#endif

	uart_baudrate_reset(UART_CI_PORT, baudrate);

	return 0;
}


static u8 ci_reg_w(const u8 *p, u8 len)
{
	u32 reg = 0;
	u32 val = 0;
	u8 ret_para = CI_ERR_NO;

	if ((p == 0) || (len < 8)) {
		val = CI_ERR_PARA_LEN_INVLD;
		cmd_resp_para(CI_REG_W, (u8 *)&val, 1);
		return 0xff;
	}

	/* little-endian. */
	reg = p[3] << 24 |p[2] << 16 | p[1] << 8 |p[0];
	val = p[7] << 24 |p[6] << 16 | p[5] << 8 |p[4];

#ifdef CI_DBG_EN
	printf("\r\nci_reg_w: 0x%x, 0x%x", reg, val);
#endif

	if (check_reg_addr(reg)) {
		*((volatile u32 *)reg) = val;
	}

	cmd_resp_para(CI_REG_W, &ret_para, 1);

	return 0;
}

static u8 ci_reg_r(const u8 *p, u8 len)
{
	u32 reg = 0;
	u32 val = 0;

	if ((p == 0) || (len < 4)) {
		val = CI_ERR_PARA_LEN_INVLD;
		cmd_resp_para(CI_REG_R, (u8 *)&val, 1);
		return 0xff;
	}

	reg = p[3] << 24 |p[2] << 16 | p[1] << 8 |p[0];

#ifdef CI_DBG_EN
	printf("\r\nci_reg_r: 0x%x", reg);
#endif

	if (check_reg_addr(reg)) {
		val = *((volatile u32 *)reg);
	}

	cmd_resp_para(CI_REG_R, (u8 *)&val, 4);

	return 0;
}

static u8 ci_mem_w(const u8 *p, u8 len)
{
	u32 addr = 0;
	u32 val = 0;
	u8 ret_para = CI_ERR_NO;

	if ((p == 0) || (len < 8)) {
		val = CI_ERR_PARA_LEN_INVLD;
		cmd_resp_para(CI_MEM_W, (u8 *)&val, 1);
		return 0xff;
	}

	addr = p[3] << 24 |p[2] << 16 | p[1] << 8 |p[0];
	val = p[7] << 24 |p[6] << 16 | p[5] << 8 |p[4];

#ifdef CI_DBG_EN
	printf("\r\nci_mem_w: 0x%x, 0x%x", addr, val);
#endif

	*((u32 *)addr) = val;

	cmd_resp_para(CI_MEM_W, &ret_para, 1);

	return 0;
}

static u8 ci_mem_r(const u8 *p, u8 len)
{
	u32 addr = 0;
	u32 val = 0;

	if ((p == 0) || (len < 4)) {
		val = CI_ERR_PARA_LEN_INVLD;
		cmd_resp_para(CI_MEM_R, (u8 *)&val, 1);
		return 0xff;
	}

	addr = p[3] << 24 |p[2] << 16 | p[1] << 8 |p[0];

#ifdef CI_DBG_EN
	printf("\r\nci_mem_r: 0x%x", addr);
#endif

	val = MEM_READ_U32(addr);
	cmd_resp_para(CI_MEM_R, (u8 *)&val, 4);

	return 0;
}

static u8 ci_flash_w(const u8 *p, u8 len)
{
	u32 addr = 0;
	u32 pack_size = len - 4;
	u32 val;
	const u8 *pbuf = p + 4;
	u8 ret_para = CI_ERR_NO;

	if ((p == 0) || (len < 8)) {
		val = CI_ERR_PARA_LEN_INVLD;
		cmd_resp_para(CI_FLASH_W, (u8 *)&val, 1);
		return 0xff;
	}

	addr = p[3] << 24 |p[2] << 16 | p[1] << 8 |p[0];

#ifdef CI_DBG_EN
	printf("\r\nci_flash_w: 0x%x, 0x%x", addr, pack_size);
#endif

	spi_flash_write(addr, pack_size, &val, pbuf);

	cmd_resp_para(CI_FLASH_W, &ret_para, 1);

	return 0;
}

static u8 ci_sys_reset(const u8 *p, u8 len)
{
	u8 mod = 0;
	u8 val = CI_ERR_NO;

	if ((p == 0) || (len == 0)) {
		val = CI_ERR_PARA_LEN_INVLD;
		cmd_resp_para(CI_SYS_RESET, &val, 1);
		return 0xff;
	}

	mod = p[0];

#ifdef CI_DBG_EN
	printf("\r\nci_sys_reset, 0x%x", mod);
#endif

	if (mod > 0) {
		cmd_resp_para(CI_SYS_RESET, &val, 1);
		delay_ms(1);

		/* CI executes only in FlashRun, which ensures that boot device must be set to Flash */
		pmu_set_bootmode(PMU_BOOTMODE_FLASH);

		NVIC_SystemReset();
	} else {
		val = CI_ERR_PARA_ERR;
		cmd_resp_para(CI_SYS_RESET, &val, 1);
	}

	return 0;
}

static u8 do_erase_flash(const u8 *p, u8 len)
{
	u32 start_sec = 0x0;
	u32 sec_num = 0;
	u8 val = CI_ERR_NO;

	if ((p == 0) || (len < 4)) {
		val = CI_ERR_PARA_LEN_INVLD;
		cmd_resp_para(CI_DO_ERASE_FLS, &val, 1);
		return 0xff;
	}

	start_sec = (p[1] << 8) |p[0];
	sec_num = (p[3] << 8) | p[2];

#ifdef CI_DBG_EN
	printf("\r\ndo_erase_flash, 0x%x", part);
#endif

	if ((sec_num > 0) && (sec_num < 0xffff)) {
		/* convert to addr. */
		start_sec *= FLASH_ERASE_UNIT;
		sec_num *= FLASH_ERASE_UNIT;
		sec_num += start_sec;

		spi_flash_erase(start_sec, sec_num);
	} else if (sec_num == 0xffff) {
		spi_flash_erase(0x0, FLASH_MAX_SIZE);
	}

	cmd_resp_para(CI_DO_ERASE_FLS, &val, 1);

	return 0;
}

/**
 * @brief Set the distance level for motion detection.
 *
 * @param p
 * @param len
 * @return u8
 */
static u8 ci_dist_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;

	if ((p == NULL) || (len < 1u) || (p[0] >= RAD_DIST_LV_MAX)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		radar_mgr_mdet_user_cfg_set(p[0]);
#ifdef CFG_USER_SETTING
		m_setting_cfg.dist_lv = p[0];
#endif
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

	cmd_resp_para(CI_DIST_SET, &ret_para, 1u);
	return ret;
}

static u8 ci_dist_get(const u8 *p, u8 len)
{
	u8 ret_para;

	(void)*p;
	(void)len;
	ret_para = radar_mgr_mdet_user_cfg_get();
	cmd_resp_para(CI_DIST_GET, &ret_para, 1u);

	return 0u;
}

static u8 ci_mdet_info_log_rsp(const u8 *p, u8 len)
{
	u8 size = 0;
	fmcw_det_info_t det_info = {0};

	radar_mgr_mdet_info_get(&det_info);
	size = sizeof(fmcw_det_info_t);
	cmd_resp_para(CI_MDET_INFO, (u8*)&det_info, size);

	return 0u;
}



static u8 ci_uuid_get(const u8 *p, u8 len)
{
	u8 ret = CI_ERR_NO;
	u8 uuid[8];

	if ((NULL == p) || (0 != len)) {
		return CI_ERR_PARA_LEN_INVLD;
	}
	radar_drv_get_uuid(uuid);
	cmd_resp_para(CI_UUID_GET, (u8 *)uuid, sizeof(uuid));
	return ret;
}

static u8 ci_compltm_get(const u8 *p, u8 len)
{
	u8 rsp_data[32];

	if ((NULL == p) || (0u != len))
		return CI_ERR_PARA_LEN_INVLD;
	memset(rsp_data,0,sizeof(rsp_data));
	sprintf((char *)rsp_data, "%s %s", __DATE__, __TIME__);
	cmd_resp_para(CI_HOST_COMPLTM_GET, rsp_data, sizeof(rsp_data));
	return CI_ERR_NO;
}

#ifdef CFG_RADAR_STAGE_SUPPORT
static u8 ci_mdet_info_pwr_rsp(const u8 *p, u8 len)
{
    static stage_pwr_t pwr_rsp;

    pwr_rsp.boundary = 0x011c7fff;
    pwr_rsp.pwr_count = RADARD_STAGE_MAX_RB;
    pwr_rsp.check_code = 0;
    radar_mgr_mdet_pwr_get((u32 *)pwr_rsp.rb_pwr);
    for (u32 i = 0; i < RADARD_STAGE_MAX_RB; i++) {
        pwr_rsp.check_code += pwr_rsp.rb_pwr[0][i];
        pwr_rsp.check_code += pwr_rsp.rb_pwr[1][i];
    }
    ci_send_bytes((u8*)&pwr_rsp,sizeof(stage_pwr_t));

    return 0u;
}
#endif

static u8 ci_alg_type_query_rsp(const u8 *p, u8 len)
{
	u8 type = 2;

	cmd_resp_para(CI_ALG_TYPE_QUERY, (u8 *)&type, 1);
	return 0;
}


static u8 ci_mdet_get_bound_value(const u8 *p, u8 len)
{
#ifdef CFG_APP_LIGHT
	mdet_bound_cfg cfg = {0};
	cfg.det_mot_min = RAD_DET_DIST_START;
	cfg.det_mot_max = RAD_DET_MOT_MAX;

	cfg.det_micro_min = RAD_DET_DIST_START;
	cfg.det_micro_max = RAD_DET_MICRO_MAX;

	cfg.det_bhr_min = RAD_DET_DIST_START;
	cfg.det_bhr_max = RAD_DET_BHR_MAX;
	cfg.det_swp_bw = MD_SWEEP_BW_1GHZ;
	cmd_resp_para(CI_MDET_BOUND_CFG_GET, (u8 *)&cfg, sizeof(mdet_bound_cfg));
#endif
	return 0;
}

static u8 ci_mdet_get_default_value(const u8 *p, u8 len)
{
	mdet_default_cfg cfg = {0};

	cfg.micro_det_sl = 0xff;// set micro to invalid value
	cfg.bhr_det_sl = 0xff;// set bhr to invalid value

	cfg.mot_det_sta = radar_mgr_mdet_undet_delta_get();
	cfg.mot_det_sto = radar_mgr_mdet_delta_get();
	cfg.mot_det_sl  = radar_mgr_mdet_sen_lev_get();

#if defined(CFG_USER_SETTING)
	m_setting_cfg.mot_det_sta = cfg.mot_det_sta;
	m_setting_cfg.mot_det_sto = cfg.mot_det_sto;
	m_setting_cfg.mot_det_sl  = cfg.mot_det_sl;

#ifdef CFG_PRESENCE_DET
	m_setting_cfg.bhr_det_sta = cfg.bhr_det_sta = radar_mgr_bhr_undet_delta_get() ;
	m_setting_cfg.bhr_det_sto = cfg.bhr_det_sto = radar_mgr_bhr_delta_get();
	m_setting_cfg.bhr_det_sl  = cfg.bhr_det_sl  = radar_mgr_bhr_sen_lev_get();
#endif

#endif /* !CFG_USER_SETTING */

	cmd_resp_para(CI_MDET_DEFAULT_CFG_GET, (u8 *)&cfg, sizeof(mdet_default_cfg));
	return 0u;
}


static u8 ci_mdet_delta_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;
	u16 val;

	if ((p == NULL) || (len != 2u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		val = ((u16)p[1] << 8) | p[0];
		radar_mgr_mdet_delta_set(val);
#ifdef CFG_USER_SETTING
		m_setting_cfg.mot_det_sto = val;
#endif
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

	cmd_resp_para(CI_MDET_DELTA_SET, &ret_para, 1u);

	return ret;
}

static u8 ci_mdet_undet_delta_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;
	u16 val = 0;

	if ((p == NULL) || (len != 2u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		val = ((u16)p[1] << 8) | p[0];
		radar_mgr_mdet_undet_delta_set(val);
#ifdef CFG_USER_SETTING
		m_setting_cfg.mot_det_sta = val;
#endif
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

	cmd_resp_para(CI_MDET_UNDET_DELTA_SET, &ret_para, 1u);
	return ret;
}


static u8 hci_mdet_gain_lev_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;

	if ((p == NULL) || (len != 1u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		radar_mgr_mdet_sen_lev_set(p[0]);
#ifdef CFG_USER_SETTING
		m_setting_cfg.mot_det_sl = p[0];
#endif
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

	cmd_resp_para(CI_MDET_SSTV_LEV_SET, &ret_para, 1u);
	return ret;
}



#ifdef CFG_PRESENCE_DET
static u8 ci_bhr_undet_delta_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;
	u16 val;

	if ((p == NULL) || (len < 1u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		val = ((u16)p[1] << 8) | p[0];
		radar_mgr_bhr_undet_delta_set(val);
#ifdef CFG_USER_SETTING
		m_setting_cfg.bhr_det_sta = val;
#endif
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

	cmd_resp_para(CI_BHR_UNDET_DELTA_SET, &ret_para, 1u);
	return ret;
}

static u8 ci_bhr_delta_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;

	if ((p == NULL) || (len < 1u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		u16 val = ((u16)p[1] << 8) | p[0];
		radar_mgr_bhr_delta_set(val);
#ifdef CFG_USER_SETTING
		m_setting_cfg.bhr_det_sto = val;
#endif
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

	cmd_resp_para(CI_BHR_DELTA_SET, &ret_para, 1u);
	return ret;
}

static u8 ci_bhr_gain_lev_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;

	if ((p == NULL) || (len != 1u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		radar_mgr_bhr_sen_lev_set(p[0]);
#ifdef CFG_USER_SETTING
		m_setting_cfg.bhr_det_sl = p[0];
#endif
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

	cmd_resp_para(CI_BHR_SSTV_SET, &ret_para, 1u);
	return ret;
}


static u8 ci_bhr_dist_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;

	if ((p == NULL) || (len < 1u) || (p[0] >= RAD_BHR_DIST_LV_MAX)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		radar_mgr_bhr_user_cfg_set(p[0]);
#ifdef CFG_USER_SETTING
		m_setting_cfg.bhr_dist_lv = p[0];
#endif
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

	cmd_resp_para(CI_BHR_DIST_SET, &ret_para, 1u);
	return ret;
}

static u8 ci_bhr_dist_get(const u8 *p, u8 len)
{
	u8 ret_para;

	(void)*p;
	(void)len;
	ret_para = radar_mgr_bhr_user_cfg_get();
	cmd_resp_para(CI_BHR_DIST_GET, &ret_para, 1u);

	return 0u;
}
#endif /* !CFG_PRESENCE_DET */


static u8 ci_mdet_delta_get(const u8 *p, u8 len)
{
	u16 val;
	u8 buf[2];

	(void)*p;
	(void)len;
	val = radar_mgr_mdet_delta_get();
	buf[0] = (u8)(val & 0x00FFu);
	buf[1] = (u8)((val & 0xFF00u) >> 8);

	cmd_resp_para(CI_MDET_DELTA_GET, buf, 2u);

	return 0u;
}

/**
 * @brief Set lighting duration, unit: S
 *
 * @param p
 * @param len
 * @return u8
 */
static u8 ci_lot_set(const u8 *p, u8 len)
{
	u8 ret = 0;
#ifdef CFG_APP_LIGHT
	u8 ret_para;
	u16 lot;

	if ((p == NULL) || (len != 2u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		lot = ((u16)p[1] << 8) | p[0];
		lot = (lot == 0u) ? 1u : lot;
#ifdef CFG_APP_LIGHT
		app_light_set_lot((u32)lot * 1000u);
#endif
#ifdef CFG_USER_SETTING
		m_setting_cfg.lot_s = lot;
#endif
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

	cmd_resp_para(CI_LOT_SET, &ret_para, 1u);
#endif /* !CFG_APP_LIGHT */
	return ret;
}

static u8 ci_lot_get(const u8 *p, u8 len)
{
#ifdef CFG_APP_LIGHT
	u32 lot;
	u8 buf[2];

	(void)*p;
	(void)len;
	lot = app_light_get_lot();
	lot /= 1000u;
	buf[0] = (u8)(lot & 0x00FFu);
	buf[1] = (u8)(lot >> 8);

	cmd_resp_para(CI_LOT_GET, buf, 2u);
#endif
	return 0u;
}

#ifdef CFG_LS

/**
 * @brief Set the threshold of Light Sensor.
 *
 * @param p
 * @param len
 * @return u8
 */
static u8 ci_lux_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;
	u16 lux;

	if ((p == NULL) || (len != 2u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		lux = ((u16)p[1] << 8) | p[0];
		hal_ls_set_threshold(lux);
#ifdef CFG_USER_SETTING
		m_setting_cfg.lux = lux;
#endif
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

	cmd_resp_para(CI_LUX_SET, &ret_para, 1u);

	return ret;
}

/**
 * @brief Get the threshold of Light Sensor.
 *
 * @param p
 * @param len
 * @return u8
 */
static u8 ci_lux_get(const u8 *p, u8 len)
{
	u16 lux;
	u8 buf[2];

	(void)*p;
	(void)len;
	lux = hal_ls_get_threshold();
	buf[0] = (u8)(lux & 0x00FFu);
	buf[1] = (u8)((lux & 0xFF00u) >> 8);

	cmd_resp_para(CI_LUX_GET, buf, 2u);

	return 0u;
}
#endif

static u8 ci_light_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;

	if ((p == NULL) || (len != 1u) || (p[0] > 1u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		hal_light_set(LIGHT_PRIMARY, p[0]);
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

	cmd_resp_para(CI_LIGHT_SET, &ret_para, 1u);
	return ret;
}

#ifdef CFG_LED_PWM
static u8 ci_pwm_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;
	u16 pwm;

	if ((p == NULL) || (len != 2u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		pwm = ((u16)p[1] << 8) | p[0];
		pwm = (pwm > 1000u) ? 1000u : pwm;
		hal_pwm_duty_set(LIGHT_PRIMARY, pwm);
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

	cmd_resp_para(CI_PWM_SET, &ret_para, 1u);

	return ret;
}
#endif

static u8 ci_radar_get(const u8 *p, u8 len)
{
	u8 ret_para;

	(void)*p;
	(void)len;
	ret_para = radar_mgr_get_pwr_status();
	cmd_resp_para(CI_RADAR_GET, &ret_para, 1u);

	return 0u;
}

static u8 ci_radar_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;

	if ((p == NULL) || (len != 1u) || (p[0] > 1u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		radar_mgr_rf_pwr_set(p[0]);
		if (p[0] == 0) {
			hal_light_set(LIGHT_PRIMARY, p[0]);
		}
		ret_para = (u8)CI_ERR_NO;
		ret = 0u;
	}

#ifdef CFG_USER_SETTING
	m_setting_cfg.rdr_onoff = p[0];
#endif

	cmd_resp_para(CI_RADAR_SET, &ret_para, 1u);

	return ret;
}

#ifdef CFG_USER_SETTING
static u8 ci_setting_get(ci_setting_cfg_t *cfg)
{
	u8  buf[CI_PARA_SIZE] = { 0u }, ret = 0u;
	u32 cnt = sizeof(m_setting_cfg_items) / sizeof(setting_cfg_item_t);
	s32 len = 0u;

	for (u8 i = 0u; i < cnt; i ++) {
		ret = user_setting_get(m_setting_cfg_items[i].name, (char *)buf, &len);
		if ((ret != USER_SETTING_STATUS_SUCCESS) || (len != m_setting_cfg_items[i].val_len)) {
			ret = (i + 1u);
			break;
		}
		memcpy((u8 *)((u32)cfg + m_setting_cfg_items[i].offset), &buf, len);
	}

	return ret;
}

__STATIC_INLINE u8 ci_setting_check(const ci_setting_cfg_t *cfg_cur, const ci_setting_cfg_t *cfg_new)
{
	return ((cfg_cur->save_flag   == cfg_new->save_flag) &&
			(cfg_cur->dist_lv     == cfg_new->dist_lv) &&
			(cfg_cur->lot_s       == cfg_new->lot_s) &&
			(cfg_cur->rdr_onoff   == cfg_new->rdr_onoff) &&
			(cfg_cur->ci_bdr      == cfg_new->ci_bdr) &&

#ifdef CFG_LS
			(cfg_cur->lux == cfg_new->lux) &&
#endif

			(cfg_cur->mot_det_sta == cfg_new->mot_det_sta) &&
			(cfg_cur->mot_det_sto == cfg_new->mot_det_sto) &&
			(cfg_cur->mot_det_sl  == cfg_new->mot_det_sl)  &&

#ifdef CFG_PRESENCE_DET
			(cfg_cur->bhr_dist_lv == cfg_new->bhr_dist_lv) &&
			(cfg_cur->bhr_det_sta == cfg_new->bhr_det_sta) &&
			(cfg_cur->bhr_det_sto == cfg_new->bhr_det_sto) &&
			(cfg_cur->bhr_det_sl  == cfg_new->bhr_det_sl)  &&
#endif
			(1)) ? 0u : 1u;
}

static u8 ci_setting_set(const ci_setting_cfg_t *cfg)
{
	u8 ret = 0u;
	u32 cnt = sizeof(m_setting_cfg_items) / sizeof(setting_cfg_item_t);

	for (u8 i = 0u; i < cnt; i ++) {
		ret = user_setting_set(m_setting_cfg_items[i].name, (char *)((u32)cfg + m_setting_cfg_items[i].offset), m_setting_cfg_items[i].val_len);
		if (ret != USER_SETTING_STATUS_SUCCESS) {
			ret = (i + 1u);
			break;
		}
	}

	return ret;
}

void ci_setting_load(void)
{
	u8 buf[1];
	s32 len;

	user_setting_init();
	if ((s32)USER_SETTING_STATUS_SUCCESS == user_setting_get(CI_CFG_SAVE_NAME, (char*)buf, &len)) {
		if (buf[0] == CI_SETTING_FLAG_MAGIC) {
			if (0u == ci_setting_get(&m_setting_cfg)) {
				//logi("get save_flag=%d, dist_lv=%d, lot_s=%d, lux=%d\r\n", m_setting_cfg.save_flag, m_setting_cfg.dist_lv, m_setting_cfg.lot_s, m_setting_cfg.lux);
				radar_mgr_mdet_delta_set(m_setting_cfg.mot_det_sto);
				radar_mgr_mdet_undet_delta_set(m_setting_cfg.mot_det_sta);
				radar_mgr_mdet_sen_lev_set(m_setting_cfg.mot_det_sl);
				//logi("mdet:%d,%d,%d\n\r", m_setting_cfg.mot_det_sto, m_setting_cfg.mot_det_sta, m_setting_cfg.mot_det_sl);

				m_setting_cfg.lot_s = (0u == m_setting_cfg.lot_s) ? 1u : m_setting_cfg.lot_s;
				radar_mgr_rf_pwr_set(m_setting_cfg.rdr_onoff);
				uart_baudrate_reset(UART_CI_PORT, m_setting_cfg.ci_bdr);
#ifdef CFG_APP_LIGHT
				app_light_set_lot((u32)m_setting_cfg.lot_s * 1000u);
#endif

#ifdef CFG_LS
				hal_ls_set_threshold(m_setting_cfg.lux);
#endif

#ifdef CFG_PRESENCE_DET
				radar_mgr_bhr_delta_set(m_setting_cfg.bhr_det_sto);
				radar_mgr_bhr_undet_delta_set(m_setting_cfg.bhr_det_sta);
				radar_mgr_bhr_sen_lev_set(m_setting_cfg.bhr_det_sl);
				//logi("bhr:%d,%d,%d\n\r",m_setting_cfg.bhr_det_sto,m_setting_cfg.bhr_det_sta,m_setting_cfg.bhr_dist_lv);
#endif
			} else {
				loge("ci check magic code error!\r\n");
			}
		}
	} else {
		loge("ci setting read error!\r\n");
	}
}

/**
 * @brief
 *
 */
static s8 ci_setting_save(void)
{
	ci_setting_cfg_t cfg_cur = {0u};
	u8 ci_set_ret;

	ci_set_ret = ci_setting_get(&cfg_cur);
	if ((0u == ci_set_ret) && (0u == ci_setting_check(&cfg_cur, &m_setting_cfg))) {
		logi("ci setting not changed, ret=%d\r\n", ci_set_ret);
		return 0;
	}
	ci_set_ret = ci_setting_set(&m_setting_cfg);
	if (0u == ci_set_ret) {
		int store_ret = 0;
		store_ret = user_setting_store();
		if(0 == store_ret) {
#ifdef CFG_LS
			logi("set save_flag=%d, dist_lv=%d, lot_s=%d, lux=%d\r\n", m_setting_cfg.save_flag, m_setting_cfg.dist_lv, m_setting_cfg.lot_s, m_setting_cfg.lux);
#else
			logi("set save_flag=%d, dist_lv=%d, lot_s=%d\r\n", m_setting_cfg.save_flag, m_setting_cfg.dist_lv, m_setting_cfg.lot_s);
#endif
			return 0;
		}
		loge("ci setting save error.\r\n");
		return -1;
	}
	loge("ci setting save error.\r\n");
	return -1;
}

static u8 ci_cfg_save_set(const u8 *p, u8 len)
{
	u8 ret, ret_para;

	if ((p == NULL) || (len != 1u) || (p[0] > 1u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		m_setting_cfg.save_flag = (1u == p[0]) ? CI_SETTING_FLAG_MAGIC : 0u;
		ret = ci_setting_save();
		if(0 == ret) {
			ret_para = (u8)CI_ERR_NO;
			ret = 0x0;
		}
		else {
			ret_para = (u8)CI_ERR_INVALID;
			ret = 0xFFu;
		}
	}
	cmd_resp_para(CI_CFG_SAVE_SET, &ret_para, 1u);
	return ret;
}

/**
 * @brief Check if settings have been saved to flash.
 * CI result: 1 -- saved to flash; 0 -- not saved to flash.
 * @param p
 * @param len
 * @return u8
 */
static u8 ci_cfg_save_get(const u8 *p, u8 len)
{
	u8 ret_para = 0;

	(void)*p;
	(void)len;

	ret_para = (CI_SETTING_FLAG_MAGIC == m_setting_cfg.save_flag) ? 1u : 0u;
	cmd_resp_para(CI_CFG_SAVE_GET, &ret_para, 1u);

	return 0u;
}

static u8 ci_user_setting_clear(const u8 *p, u8 len)
{
	u8 ret, ret_para;

	if ((p == NULL) || (len != 1u)) {
		ret_para = (u8)CI_ERR_PARA_LEN_INVLD;
		ret = 0xFFu;
	} else {
		switch (p[0]) {
		case 0u:
			(void)user_setting_delete_all();
			ret_para = (u8)CI_ERR_NO;
			ret = 0u;
			break;
		case 1u:
			(void)user_setting_erase();
			ret_para = (u8)CI_ERR_NO;
			ret = 0u;
			break;
		case 2u:
			(void)user_setting_delete_all();
			(void)user_setting_erase();
			ret_para = (u8)CI_ERR_NO;
			ret = 0u;
			break;
		default:
			ret_para = (u8)CI_ERR_PARA_ERR;
			ret = 0xFFu;
			break;
		}
	}
	cmd_resp_para(CI_USER_SETTING_CLEAR, &ret_para, 1u);

	return ret;
}
#endif /* CFG_USER_SETTING */

#ifdef CFG_APP_HTM
extern void app_htm_wakeup_end(void *arg);
static u8 ci_htm_measure_start(const u8 *p, u8 len)
{
	logi("ci_htm_measure_start\r\n");

	app_htm_det_start();

	return 0;
}

static u8 ci_htm_wakeup_end(const u8 *p, u8 len)
{
	logi("ci_htm_wakeup_end\r\n");

	app_htm_wakeup_end(NULL);

	return 0;
}

#endif

static u8 ci_version_query_rsp(const u8 *p, u8 len)
{
	ci_ver_struct ver_rsp = {0};
	u8 ver_len = 0;

	ver_rsp.sw_ver_major = SW_VER_MAJOR;
	ver_rsp.sw_ver_minor = SW_VER_MINOR;
	ver_rsp.sw_ver_revision = SW_VER_REVISON;

#if defined(CUST_VER_MAJOR) && defined(CUST_VER_MINOR)
	ver_rsp.cust_ver_major = CUST_VER_MAJOR;
	ver_rsp.cust_ver_minor = CUST_VER_MINOR;
#endif

#if defined(HW_VER_MAJOR) && defined(HW_VER_MINOR)
	ver_rsp.hw_ver_major = HW_VER_MAJOR;
	ver_rsp.hw_ver_minor = HW_VER_MINOR;
#endif

	ver_len = sizeof(ci_ver_struct);
	cmd_resp_para(CI_VER_GET, (u8 *)&ver_rsp, (u8)ver_len);

	return 0u;
}

#if defined(CFG_CUST_ANKER) || defined(CFG_APP_HTM_ULP) || defined(CFG_APP_IPC_COM) || defined(CFG_APP_PRESENSE_ULP)
	/* Anker ULP_PROG_1 has a small set of commands */
	/* HTM_ULP ULP_PROG_1 has a small set of commands */
static const ci_cmd_tbl_t m_ci_cmd_tbl[] = {
	/* common command. */
	{CI_REG_W,     ci_reg_w},
	{CI_REG_R,     ci_reg_r},
	{CI_MEM_W,     ci_mem_w},
	{CI_MEM_R,     ci_mem_r},
	{CI_SYS_RESET, ci_sys_reset},
	{CI_FLASH_W,   ci_flash_w},
	{CI_SET_BAUD,  ci_set_baud},

#ifdef CFG_OTA
	/* Download file commands */
	{CI_DLD_START,  ci_dld_start},
#endif

#ifdef CFG_EOL_TESTMOD
	{ CI_EOL_ENTRY_TESTMOD, ci_eol_test_mode_entry },
	{ CI_EOL_SNR_GET, ci_eol_test_snr_get },
	{ CI_EOL_GPIO_TEST, ci_eol_test_mode_gpio_test },
	{ CI_EOL_LOCALE_FFT_DUMP, ci_eol_test_mode_locale_fft_get },
	{ CI_EOL_PHASECAL_LOCALE_SET, ci_eol_test_mode_antphase_calib },
	{ CI_EOL_PHASECAL_RES_R, ci_eol_test_mode_calres_read },
	{ CI_EOL_PHASECAL_RES_W, ci_eol_test_mode_calres_write },
	{ CI_EOL_PHASECAL_APPLY_TGTGET, ci_eol_test_mode_calres_apply_tgtget },
	{ CI_EOL_IO_OUT_SET, ci_eol_test_mode_io_out_set },
	{ CI_EOL_PWRLVL_SET, ci_eol_test_mode_pwrlvl_set },
	{ CI_EOL_SELF_CHECK, ci_eol_test_mode_slfchk },
	{ CI_EOL_RFTX_ONOFF, ci_eol_test_mode_rftx_onoff },
	{ CI_EOL_EXIT_TESTMOD, ci_eol_test_mode_exit },
#endif
	{ CI_HOST_COMPLTM_GET, ci_compltm_get },
	{ CI_UUID_GET, ci_uuid_get },
	{ CI_VER_GET, ci_version_query_rsp },
	{ 0xff, NULL },
};

#else /* normal ci command tables */

static const ci_cmd_tbl_t m_ci_cmd_tbl[] = {
	/* common command. */
	{ CI_REG_W, ci_reg_w },
	{ CI_REG_R, ci_reg_r },
	{ CI_MEM_W, ci_mem_w },
	{ CI_MEM_R, ci_mem_r },
	{ CI_SYS_RESET, ci_sys_reset },
	{ CI_FLASH_W, ci_flash_w },
	{ CI_SET_BAUD, ci_set_baud },

	/* basic command for setting */
	{ CI_DIST_SET, ci_dist_set },
	{ CI_DIST_GET, ci_dist_get },
	{ CI_LOT_SET, ci_lot_set },
	{ CI_LOT_GET, ci_lot_get },
	{ CI_MDET_DELTA_SET, ci_mdet_delta_set },
	{ CI_MDET_DELTA_GET, ci_mdet_delta_get},

#ifdef CFG_OTA
	/* Download file commands */
	{CI_DLD_START,  ci_dld_start},
#endif

#ifdef CFG_LS
	{ CI_LUX_SET, ci_lux_set },
	{ CI_LUX_GET, ci_lux_get },
#endif

	{ CI_LIGHT_SET, ci_light_set },

#ifdef CFG_LED_PWM
	{ CI_PWM_SET, ci_pwm_set },
#endif

#ifdef CFG_PRESENCE_DET
	{ CI_BHR_DIST_SET, ci_bhr_dist_set },
	{ CI_BHR_DIST_GET, ci_bhr_dist_get },
#endif
	{ CI_RADAR_GET, ci_radar_get },
	{ CI_RADAR_SET, ci_radar_set },

	/* extend command. */
#ifdef CFG_USER_SETTING
	{ CI_CFG_SAVE_SET, ci_cfg_save_set },
	{ CI_CFG_SAVE_GET, ci_cfg_save_get },
	{ CI_USER_SETTING_CLEAR, ci_user_setting_clear },
#endif /* !CFG_USER_SETTING */

	{ CI_DO_ERASE_FLS, do_erase_flash },
	{ CI_ALG_TYPE_QUERY, ci_alg_type_query_rsp},
	{CI_MDET_INFO,ci_mdet_info_log_rsp},
	{ CI_MDET_BOUND_CFG_GET, ci_mdet_get_bound_value},
	{ CI_MDET_DEFAULT_CFG_GET, ci_mdet_get_default_value},
	//{ CI_MDET_DELTA_SET, ci_mdet_delta_set},
	{ CI_MDET_UNDET_DELTA_SET, ci_mdet_undet_delta_set},
	{ CI_MDET_SSTV_LEV_SET, hci_mdet_gain_lev_set},
#ifdef CFG_PRESENCE_DET
	{ CI_BHR_DELTA_SET, ci_bhr_delta_set},
	{ CI_BHR_UNDET_DELTA_SET, ci_bhr_undet_delta_set},
	{ CI_BHR_SSTV_SET, ci_bhr_gain_lev_set},
#endif

#ifdef CFG_CI_MESH
	{ CI_MESH_PROBE, mesh_node_probe },
	{ CI_MESH_LAMP_ONOFF_SET, mesh_node_lamp_set },
	{ CI_MESH_LAMP_FLSH_SWITCH, mesh_node_lamp_flash_set },
	{ CI_MESH_NODE_INFO_SET, mesh_node_info_set },
	{ CI_MESH_SENSOR_ONOFF_SET, mesh_node_sensor_onoff_set },
	{ CI_MESH_NETWK_ONOFF_SET, mesh_network_onoff_set },
	{ CI_MESH_INDCD_BRTNESS_SET, mesh_node_induced_brightness_set },
	{ CI_MESH_SLP_BRTNESS_SET, mesh_node_sleep_brightness_set },
	{ CI_MESH_LGT_DELAY_TM_SET, mesh_node_light_delay_time_set },
	{ CI_MESH_NGBR_GRPNOTC_SET, mesh_node_nghbr_grp_notice_set },
	{ CI_MESH_ALL_CFGDATA_SET, mesh_node_all_cfgdata_set },
#endif

#ifdef CFG_APP_HTM
	{CI_HTM_MEASURE_START, ci_htm_measure_start},
	{CI_HTM_WAKEUP_END, ci_htm_wakeup_end},
#endif

#ifdef CFG_EXTI_ULP
	{CI_ULP_SET_ACTIVE_TIME, ci_ulp_set_active_time},
	{CI_ULP_EXTI_REG_READ,   ci_ulp_exit_reg_read},
	{CI_ULP_EXTI_REG_WRITE,  ci_ulp_exit_reg_write},
#endif

#ifdef CFG_EOL_TESTMOD
	{ CI_EOL_ENTRY_TESTMOD, ci_eol_test_mode_entry },
	{ CI_EOL_SNR_GET, ci_eol_test_snr_get },
	{ CI_EOL_GPIO_TEST, ci_eol_test_mode_gpio_test },
	{ CI_EOL_LOCALE_FFT_DUMP, ci_eol_test_mode_locale_fft_get },
	{ CI_EOL_PHASECAL_LOCALE_SET, ci_eol_test_mode_antphase_calib },
	{ CI_EOL_PHASECAL_RES_R, ci_eol_test_mode_calres_read },
	{ CI_EOL_PHASECAL_RES_W, ci_eol_test_mode_calres_write },
	{ CI_EOL_PHASECAL_APPLY_TGTGET, ci_eol_test_mode_calres_apply_tgtget },
	{ CI_EOL_IO_OUT_SET, ci_eol_test_mode_io_out_set },
	{ CI_EOL_PWRLVL_SET, ci_eol_test_mode_pwrlvl_set },
	{ CI_EOL_SELF_CHECK, ci_eol_test_mode_slfchk },
	{ CI_EOL_RFTX_ONOFF, ci_eol_test_mode_rftx_onoff },
	{ CI_EOL_EXIT_TESTMOD, ci_eol_test_mode_exit },
#endif
	{ CI_UUID_GET, ci_uuid_get },
	{ CI_HOST_COMPLTM_GET, ci_compltm_get },
#ifdef CFG_RADAR_STAGE_SUPPORT
	{ CI_MDET_INFO_MV_PWR, ci_mdet_info_pwr_rsp},
#endif

	{ CI_VER_GET, ci_version_query_rsp },

	{ 0xff, NULL },
};
#endif /* defined(CFG_CUST_ANKER) || defined(CFG_APP_HTM_ULP) */

#define CMD_TBL_SIZE		(sizeof(m_ci_cmd_tbl) / sizeof(ci_cmd_tbl_t))


u8 ci_cmd_proc(const ci_ctrl_frm_t *p)
{
#if 1
	u8 i = 0;
	u8 cmdi = (p->cmd_grp << 5) |p->cmd;

	while (m_ci_cmd_tbl[i].cmdi != 0xffu) {
		if (m_ci_cmd_tbl[i].cmdi == cmdi) {
			if (m_ci_cmd_tbl[i].xhdl != NULL) {
				if (0u != (m_ci_cmd_tbl[i].xhdl)(p->para, p->para_len)) {
					logw("[%s]ci cmd proc failed.\r\n", __func__);
				}
			}
		}
		i++;
	}
#else
	switch (p->cmd_grp) {
	case 0x0:
		ci_common_cmd_proc(p);
		break;

	case 0x01:
		ci_download_cmd_proc(p);
		break;

	default:
		break;
	}
#endif

	return 0;
}

u8 ci_frm_parser(ci_ctrl_frm_t *frm, u8 *buf)
{
	u8 ret = 0;
	u16 cc = 0;

	frm->head = buf[0];
	frm->cmd = buf[1]&0x1f;
	frm->cmd_grp = buf[1] >> 5;
	frm->para_len = buf[2];
	frm->para = &buf[3];
	frm->check_code = (buf[3+frm->para_len+1] << 8) | buf[3+frm->para_len];

	cc = get_check_code(frm);
	if (cc != frm->check_code) {
		ret = 1;
	}

	return ret;
}

u8 ci_get_resp(u8 cmd, u32 timeout)
{
	u8 ret = 0;

	while (timeout--) {
		if (ci_port_data_is_ready() == 1) {
			ci_ctrl_frm_t ci_frm;
			ret = ci_frm_parser(&ci_frm, ci_port_get_buf());
			if ((ci_frm.cmd_grp << 5 | ci_frm.cmd) == cmd) {
				if (ci_frm.para_len != 0) {
					ret = ci_frm.para[0];
					break;
				}
			}
			ci_port_reset();
		}

		/*if (m_resp_frm.head != 0) {
			if ((m_resp_frm.cmd_grp << 5 | m_resp_frm.cmd) == cmd) {
				if (m_resp_frm.para_len != 0)
					ret = m_resp_frm.para[0];
			}

			m_resp_frm.head = 0;
		}*/
		//Delay(1);

		delay_ms(1);
	}

	return ret;
}

#define RESP_PARA_MAX			1 /* fixed size: 1 bytes */

static u8 para_buf[RESP_PARA_MAX] = { 0 };
static ci_ctrl_frm_t m_resp_frm = {
	0, 0, 0, 0, para_buf, 0
};

u8 ci_resp_proc(const ci_ctrl_frm_t *p)
{
	u8 i = 0;

	m_resp_frm.head = p->head;
	m_resp_frm.cmd = p->cmd;
	m_resp_frm.cmd_grp = p->cmd_grp;
	m_resp_frm.para_len = p->para_len;

	for (i = 0; i < p->para_len && i < RESP_PARA_MAX; i++) {
		m_resp_frm.para[i] = p->para[i];
	}

	m_resp_frm.check_code = p->check_code;

	/* printf("ci resp: 0x%x, 0x%x, 0x%x, 0x%x\r\n",
		p->head, p->cmd_grp<<5 | p->cmd, p->para_len, p->para[0]); */

	return 0;
}

#endif /* !CFG_CI */
