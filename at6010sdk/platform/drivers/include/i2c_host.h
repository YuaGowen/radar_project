/**
 *******************************************************************************
 * @file    i2c_host.h
 * @author  Airtouch Software Team
 * @brief   I2C host driver
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

#ifndef __I2C_HOST_H__
#define __I2C_HOST_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_LL_DRIVER AT6010 LL Driver
 * @{
 */

/**
 * @defgroup AT6010_LL_DRIVER_I2C_HOST I2C HOST
 * @brief I2C HOST Low-Level Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"

/* Macro -------------------------------------------------------------------*/
/**
 * @defgroup I2C_HOST_DEFINE Define
 * @{
 */

#define I2C_WRITE_BUF_SIZE              32U   /**< No less than 2 */

/**
 * @}
 */
/*I2C_HOST_DEFINE*/

/* Data Types ----------------------------------------------------------------*/
/**
 * @defgroup I2C_HOST_TYPEDEF Typedef
 * @{
 */

typedef enum {
	I2C_ERR_NONE,
	I2C_ERR_ADDR_NACK,
	I2C_ERR_MEM_NACK,
	I2C_ERR_DATA_NACK,
	I2C_ERR_BUSY,
	I2C_ERR_FALSE_MODE
} i2c_err_t;

typedef enum {
	I2C_ID_0,
	I2C_ID_QTY
} i2c_id_t;

typedef enum {
	I2C_MODE_SLAVE = 0,
	I2C_MODE_MASTER = 1,
	I2C_MODE_HOST = 2
} i2c_mode_t;

typedef enum {
	I2C_IRQ_ADDR_HIT,
	I2C_IRQ_EMPTY,
	I2C_IRQ_FULL,
	I2C_IRQ_STOP
} i2c_irq_t;

typedef struct {
	i2c_mode_t mode;
	u8 addr;
	u32 sck;
} i2c_cfg_t;

typedef struct {
	void (*i2c_rx_hdl)(u8 data);
	void (*i2c_err_hdl)(i2c_err_t err);
	void (*i2c_tx_cplt_hdl)(void);
} i2c_hdl_t;

/**
 * @}
 */
/*I2C_HOST_TYPEDEF*/

/* Function ----------------------------------------------------------------*/
/**
 * @defgroup I2C_HOST_FUNC Function
 * @{
 */

/**
 * @defgroup I2C_HOST_FUNC_BASE Basic function
 * @brief Basic functions and configuration
 * @{
 */

void i2c_host_init(void);
i2c_err_t i2c_host_write_byte(u8 slv_addr, u8 mem_addr, u8 data);
i2c_err_t i2c_host_read_byte(u8 slv_addr, u8 mem_addr, u8 *pdat);

/**
 * @brief i2c host write slave memory
 *
 * @param slv_addr i2c slave address
 * @param mem_addr memory address
 * @param data data to be transmitted
 * @return i2c_err_t
 */
i2c_err_t i2c_host_write_byte_int(u8 slv_addr, u8 mem_addr, u8 data);
i2c_err_t i2c_host_read_byte_int(u8 slv_addr, u8 mem_addr);

void i2c_init(i2c_id_t id, const i2c_cfg_t *cfg);
void i2c_deinit(i2c_id_t id);

void i2c_irq_enable(i2c_irq_t irq);
void i2c_irq_disable(i2c_irq_t irq);
void i2c_set_hdl(i2c_id_t id, const i2c_hdl_t *hdl);

void i2c_dma_enable(void);
void i2c_dma_isr(void);

u8 i2c_state_get(void);

/**
 * @}
 */
/*I2C_HOST_FUNC_BASE*/

/**
 * @defgroup I2c_HOST_FUNC_POLLING I2C Polling Functions
 * @brief I2C Polling Functions
 * @{
 */
i2c_err_t i2c_master_write_wait(i2c_id_t id, u8 slv_addr, const u8 *data, u8 size);
i2c_err_t i2c_master_read_wait(i2c_id_t id, u8 slv_addr, u8 *data, u8 size);
i2c_err_t i2c_slave_write_wait(i2c_id_t id, const u8 *data, u8 size);
i2c_err_t i2c_slave_read_wait(i2c_id_t id, u8 *data, u8 size, u8 *recvd_size);

/**
 * @}
 */
/*I2c_HOST_FUNC_POLLING*/

/**
 * @defgroup I2c_HOST_FUNC_INTR I2C Interrupt Functions
 * @brief I2C Interrupt Functions
 * NOTE: SCK should not exceed 100KHz
 * @{
 */

void i2c_slave_write_int(i2c_id_t id, const u8 *pdat, u8 size);
void i2c_slave_read_int_start(i2c_id_t id);
void i2c_slave_read_int_stop(i2c_id_t id);
void i2c_isr(void);
/**
 * @}
 */
/*I2c_HOST_FUNC_INTR*/

i2c_err_t i2c_slave_write_dma(i2c_id_t id, u32 *data, u8 size);
i2c_err_t i2c_slave_read_dma(i2c_id_t id, u32 *data, u8 size);

/**
 * @}
 */
/*I2C_HOST_FUNC*/

/**
 * @}
 */
/*AT6010_LL_DRIVER_I2C_HOST*/

/**
 * @}
 */
/*AT6010_LL_DRIVER*/

#ifdef __cplusplus
}
#endif

#endif /* !__I2C_HOST_H__ */
