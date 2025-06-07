/**
 *******************************************************************************
 * @file    hal_clock.h
 * @author  Airtouch Software Team
 * @brief   Clock HAL Driver
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

#ifndef __HAL_CLOCK_H__
#define __HAL_CLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_CLOCK CLOCK
 * @brief CLOCK HAL Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"

typedef enum {
	CLK_SYS_160M_FLASH_160M_HZ = 0, /**< high performance mode    */
	CLK_SYS_160M_FLASH_80M_HZ = 1,  /**< high performance mode    */
	CLK_SYS_80M_FLASH_80M_HZ = 2,   /**< normal mode              */
	CLK_SYS_80M_FLASH_40M_HZ = 3,   /**< normal mode              */
	CLK_SYS_40M_FLASH_40M_HZ = 4,   /**< normal mode              */
	CLK_SYS_40M_FLASH_20M_HZ = 5,   /**< normal mode              */
} clk_setup_t;

typedef enum {
	SYS_CLK_40MHZ = 40000000u,
	SYS_CLK_80MHZ = 80000000u,
	SYS_CLK_160MHZ = 160000000u,
} sys_clk_t;

typedef enum {
	SPI_FLASH_CLK_20MHZ = 20000000u,
	SPI_FLASH_CLK_40MHZ = 40000000u,
	SPI_FLASH_CLK_80MHZ = 80000000u,
	SPI_FLASH_CLK_160MHZ = 160000000u,

} flash_clk_t;

typedef struct {
	u32 clk_system;
	u32 clk_spi_flash;
} clock_config;

void hal_clk_init(clk_setup_t clk_id);
s32  hal_setup_clk(clk_setup_t clk_id);
u32  hal_get_spi_flash_clk(void);
u32  hal_get_system_clk(void);
u8   hal_get_xtal_type(void);
void hal_setup_adc_clk(void);

u32 hal_ms_to_ticks(u32 ms);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_CLOCK_H__ */
