/**
 *******************************************************************************
 * @file    at5815c.c
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

#include "clock.h"
#include "i2c_host.h"
#include "gpio.h"
#include "common.h"
#include "atlog.h"
#include "radar_ext_chip.h"

// #define AT5815C_DBG_EN

#ifdef AT5815C_DBG_EN
#define AT_LOG(...)           if (1) { printf(__VA_ARGS__); }
#else
#define AT_LOG(...)           if (0) { printf(__VA_ARGS__); }
#endif


#define DEV_ADDR              0x28   /* 58xx IIC Device Address. */
#define I2C_SCK               100000U



/* AT5815C IIC registers -------------------------------------------------------------------------*/

#define REG_0x4C              0x4C
#define REG_0x4B              0x4B     /**< power_det_ctrl, default 0x04 */


/* Static Functions ------------------------------------------------------------------------------*/

/**
 * hal_intr_en() - Enable interrupt.
 *
 * @return None
 */
static void hal_intr_en(void)
{

}

/**
 * hal_intr_dis() - Disable interrupt.
 *
 * @return None
 */
static void hal_intr_dis(void)
{

}

static uint8 rad_reg_write_byte(uint8 addr, uint8 dat)
{
	uint8 ret = 0;

	hal_intr_dis();
	ret = i2c_host_write_byte(DEV_ADDR, addr, dat);
	hal_intr_en();

	return ret;
}

static uint8 rad_reg_read_byte(uint8 addr, uint8 *pdat)
{
	uint8 ret = 0;

	ret = i2c_host_read_byte(DEV_ADDR, addr, pdat);

	return ret;
}

static void rad_reg_field_write(uint8 addr, uint8 field_mask, uint8 field_dat)
{
	uint8 val = field_mask;
	uint8 bit_pos = 0;
	uint8 read_dat = 0;

	while ((!(val&0x01)) && (bit_pos < 7)) {
		bit_pos++;
		val = (val >> 1);
	}

	rad_reg_read_byte(addr, &read_dat);
	read_dat = (read_dat & (~field_mask)) | (field_dat << bit_pos);

	rad_reg_write_byte(addr, read_dat);
}

static uint8 rad_reg_field_read(uint8 addr, uint8 field_mask)
{
	uint8 val = field_mask;
	uint8 bit_pos = 0;
	uint8 read_dat = 0;

	while ((!(val&0x01)) && (bit_pos < 7)) {
		bit_pos++;
		val = (val >> 1);
	}

	rad_reg_read_byte(addr, &read_dat);
	read_dat = (read_dat & field_mask) >> bit_pos;

	return read_dat;
}

/* Public Functions ------------------------------------------------------------------------------*/

/**
 * @brief After the software reset, the internal digital logic reloads all the configuration parameters,
 * and this module need to be reset after some parameter is changed.
 */
void at5815c_soft_reset(void)
{
	rad_reg_write_byte(0x00, 0x00);
	rad_reg_write_byte(0x00, 0x01);
}


/**
 * @brief 感应判断参数调整
 *
 */
void at5815c_detect_para_adjust(detect_para_t *para)
{

}


/**
 * @brief
 *
 * @param[in] en see @ref AT5815C_DET_2ND_TRIGGER
 */
void at5815c_2nd_trigger_cfg(u8 en)
{
	if (IS_DET_2ND_TRIGGER_MDOE(en)) {
		rad_reg_field_write(REG_0x4B, BIT(6), en);
	}
}

/**
 * @brief 配置 AT5815C 工作频点 5800MHz
 */
void at5815c_cfg_freq(void)
{
	rad_reg_write_byte(0x86, 0x2A);
	rad_reg_write_byte(0x82, 0x49);
	rad_reg_write_byte(0x83, 0x66);
}

/**
 * @brief 配置 AT5815C 功耗 -- 54us, 1KHz
 *
 */
void at5815c_cfg_lps(void)
{
	rad_reg_write_byte(0x03, 0xC8); // 54 us, 1KHz
}

/**
 * @brief 配置感应距离
 *
 * @param distance
 */
void at5815c_cfg_distance(u8 distance)
{
	rad_reg_write_byte(0x38, distance); // distance=9, 感应距离是10m
}

void at5815c_init(void)
{
	/* I2C init */
	gpio_cfg_t gpio_cfg;
	i2c_cfg_t i2c_cfg;

	AT_LOG("at5815c_init\n\r");

	sys_clk_enable_i2c();

	gpio_cfg.mode = GPIO_MODE_AF;
	gpio_cfg.pull = GPIO_PULL_UP;
	gpio_cfg.af = GPIO_AON3_AF1_I2C_SCL;
	(void)gpio_init(GPIO_AON_3, &gpio_cfg);

	gpio_cfg.af = GPIO_AON4_AF1_I2C_SDA;
	(void)gpio_init(GPIO_AON_4, &gpio_cfg);

	i2c_cfg.mode = I2C_MODE_MASTER;
	i2c_cfg.sck = I2C_SCK;
	i2c_init(I2C_ID_0, &i2c_cfg);
}

/**
 * @brief 关闭光敏检测
 *
 */
void at5815c_turn_off_ls(void)
{
	AT_LOG("at5815c_turn_off_ls\r\n");
	u8 data = 0;
	u8 err = I2C_ERR_NONE;

	// read first
	err = rad_reg_read_byte(REG_0x4C, &data);

	if (err != I2C_ERR_NONE) {
		AT_LOG("R: 0x6c failed!!! \r\n");
	}
	AT_LOG("light_ctrl = 0x%x , dft = 0x01\r\n", data);

	// clear
	rad_reg_field_write(REG_0x4C, 0x3, 0x02); // bit1=1, bit0=0


	// read again
	err = rad_reg_read_byte(REG_0x4C, &data);
	if (err != I2C_ERR_NONE) {
		AT_LOG("R: 0x6c failed!!! \r\n");
	}
	AT_LOG("light_ctrl = 0x%x , dft = 0x01\r\n", data);
}
