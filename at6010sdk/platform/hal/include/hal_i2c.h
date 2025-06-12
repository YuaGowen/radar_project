/**
 *******************************************************************************
 * @file    hal_i2c.h
 * @author  Airtouch Software Team
 * @brief   I2C HAL Driver
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

#ifndef __HAL_I2C_H__
#define __HAL_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_I2C I2C
 * @brief I2C HAL Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"
#include "i2c_host.h"

/* Data Types ----------------------------------------------------------------*/

/**
 * @brief
 *
 */
typedef struct {
	u8  addr;
	u32 sck;
	i2c_mode_t mode;
} i2c_cfg_t;

typedef struct {
	void (*i2c_rx_hdl)(u8 data);
	void (*i2c_err_hdl)(i2c_err_t err);
	void (*i2c_tx_cplt_hdl)(void);
} i2c_cb_t;

/* Function Declarations -----------------------------------------------------*/

void hal_i2c_host_init(void);
void hal_i2c_init(i2c_id_t id, const i2c_cfg_t *cfg);

/**
 * @brief i2c host write slave memory
 *
 * @param slv_addr i2c slave address
 * @param mem_addr memory address
 * @param data data to be transmitted
 * @return i2c_err_t
 */
i2c_err_t hal_i2c_host_write_byte_int(u8 slv_addr, u8 mem_addr, u8 data);
i2c_err_t hal_i2c_host_read_byte_int(u8 slv_addr, u8 mem_addr);

void hal_i2c_set_callback(i2c_id_t id, const i2c_cb_t *cb);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_I2C_H__ */
