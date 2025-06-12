/**
 *******************************************************************************
 * @file    gpio.h
 * @author  Airtouch Software Team
 * @brief   GPIO Low-Level Driver
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

#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AT6010_LL_DRIVER AT6010 LL Driver
 * @{
 */

/**
 * @defgroup GPIO_LL GPIO
 * @brief GPIO Low-Level Driver
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "at_types.h"

/* Constants -----------------------------------------------------------------*/

#define NUM_OF_GPIO                     (17U)

/* Data Types ----------------------------------------------------------------*/

typedef enum {
	GPIO_MCU_0, /* 0 */
	GPIO_MCU_1, /* 1 */
	GPIO_MCU_2, /* 2 */
	GPIO_MCU_3, /* 3 */
	GPIO_MCU_4, /* 4 */
	GPIO_MCU_5, /* 5 */
	GPIO_MCU_6, /* 6 */
	GPIO_MCU_7, /* 7 */
	GPIO_MCU_8, /* 8 */
	GPIO_MCU_9, /* 9 */

	GPIO_AON_0, /* 10 */
	GPIO_AON_1, /* 11 */
	GPIO_AON_2, /* 12 */
	GPIO_AON_3, /* 13 */
	GPIO_AON_4, /* 14 */
	GPIO_AON_5, /* 15 */
	GPIO_AON_6  /* 16 */
} gpio_pin_t;

typedef enum {
	GPIO_MODE_INPUT,
	GPIO_MODE_OUTPUT,
	GPIO_MODE_AF,
	GPIO_MODE_ANALOG
} gpio_mode_t;

typedef enum {
	GPIO_DIR_INPUT  = 0,
	GPIO_DIR_OUTPUT = 1
} gpio_dir_t;

typedef enum {
	GPIO_PULL_NONE,
	GPIO_PULL_UP,
	GPIO_PULL_DOWN
} gpio_pull_t;

typedef enum {
	/*lint --e{849} suppress same enumerator value */
	GPIO_MCU0_AF0_SPI_CLK   = 0,
	GPIO_MCU0_AF1_UART2_TXD = 1,
	GPIO_MCU0_AF3_PWM0      = 3,

	GPIO_MCU1_AF0_SPI_CSN   = 0,
	GPIO_MCU1_AF1_UART2_RXD = 1,
	GPIO_MCU1_AF3_PWM1      = 3,

	GPIO_MCU2_AF0_SPI_IO0   = 0,
	GPIO_MCU2_AF1_UART1_CTS = 1,
	GPIO_MCU2_AF2_UART0_TXD = 2,
	GPIO_MCU2_AF3_UART1_TXD = 3,

	GPIO_MCU3_AF0_SPI_IO1   = 0,
	GPIO_MCU3_AF1_UART1_RTS = 1,
	GPIO_MCU3_AF2_UART0_RXD = 2,
	GPIO_MCU3_AF3_UART1_RXD = 3,

	GPIO_MCU4_AF0_SPI_IO2   = 0,
	GPIO_MCU4_AF1_PWM3      = 1,
	GPIO_MCU4_AF3_UART0_TXD = 3,

	GPIO_MCU5_AF0_SPI_IO3   = 0,
	GPIO_MCU5_AF1_PWM4      = 1,
	GPIO_MCU5_AF3_UART0_RXD = 3,

	GPIO_MCU6_AF0_UART0_TXD = 0,
	GPIO_MCU6_AF2_PWM0      = 2,
	GPIO_MCU6_AF3_I2C_SCL   = 3,

	GPIO_MCU7_AF0_UART0_RXD = 0,
	GPIO_MCU7_AF2_PWM1      = 2,
	GPIO_MCU7_AF3_I2C_SDA   = 3,

	GPIO_MCU8_AF0_UART1_TXD = 0,
	GPIO_MCU8_AF1_SPI_CLK   = 1,
	GPIO_MCU8_AF2_I2C_SCL   = 2,

	GPIO_MCU9_AF0_UART1_RXD = 0,
	GPIO_MCU9_AF1_SPI_CSN   = 1,
	GPIO_MCU9_AF2_I2C_SDA   = 2,

	GPIO_AON0_AF0_PWM0      = 0,
	GPIO_AON0_AF1_SPI_IO0   = 1,
	GPIO_AON0_AF2_UART1_CTS = 2,
	GPIO_AON0_AF3_UART2_TXD = 3,

	GPIO_AON1_AF0_PWM1      = 0,
	GPIO_AON1_AF1_SPI_IO1   = 1,
	GPIO_AON1_AF2_UART1_RTS = 2,
	GPIO_AON1_AF3_UART2_RXD = 3,

	GPIO_AON2_AF0_PWM2      = 0,

	GPIO_AON3_AF1_I2C_SCL   = 1,
	GPIO_AON3_AF2_PWM3      = 2,
	GPIO_AON3_AF3_UART1_CTS = 3,

	GPIO_AON4_AF1_I2C_SDA   = 1,
	GPIO_AON4_AF2_PWM4      = 2,
	GPIO_AON4_AF3_UART1_RTS = 3,

	GPIO_AON5_AF0_SWC       = 0,
	GPIO_AON5_AF1_SPI_IO2   = 1,
	GPIO_AON5_AF2_UART2_TXD = 2,
	GPIO_AON5_AF3_PWM3      = 3,

	GPIO_AON6_AF0_SWD       = 0,
	GPIO_AON6_AF1_SPI_IO3   = 1,
	GPIO_AON6_AF2_UART2_RXD = 2,
	GPIO_AON6_AF3_PWM4      = 3,
} gpio_af_t;

typedef enum {
	GPIO_LEVEL_3V3 = 0,
	GPIO_LEVEL_5V  = 1
} gpio_level_t;

/**
 * gpio interrupt trigger type.
 *
 */
typedef enum {
	IRQ_TYPE_NONE         = 0, /**< invalid trigger type                      */
	IRQ_TYPE_EDGE_RISING  = 1, /**< Rising edge trigger                       */
	IRQ_TYPE_EDGE_FALLING = 2, /**< Falling edge trigger                      */
	IRQ_TYPE_EDGE_BOTH    = 3, /**< Rising and Falling edge trigger           */
	IRQ_TYPE_LEVEL_HIGH   = 4, /**< High level trigger                        */
	IRQ_TYPE_LEVEL_LOW    = 5  /**< Low level trigger                         */
} irq_type_t;

/**
 * @brief GPIO callback function prototype.
 */
typedef void (*gpio_hdl_t)(void);

typedef struct {
	gpio_mode_t mode;
	gpio_pull_t pull;
	gpio_af_t   af;  /**< Need to be set only when mode == GPIO_MODE_AF       */
} gpio_cfg_t;

typedef enum {
	GPIO_OK,
	GPIO_ERROR,
	GPIO_BUSY,
	GPIO_TIMEOUT
} gpio_status_t;

/* Function Declarations -----------------------------------------------------*/

/**
 * get corresponding GPIO's direction
 * @param gpio see @ref gpio_pin_t
 * @return     see @ref gpio_dir_t
 */
u8 gpio_get_direction(u32 gpio);

/**
 * set corresponding GPIO's direction
 * @param gpio see @ref gpio_pin_t
 * @param dir  see @ref gpio_dir_t
 */
void gpio_set_direction(u32 gpio, u8 dir);

/**
 * set corresponding GPIO's direction to input
 * @param gpio see @ref gpio_pin_t
 * @return 0 if OK
 */
int gpio_direction_input(u32 gpio);

/**
 * set corresponding GPIO's value and set direction to output
 * @param gpio see @ref gpio_pin_t
 * @param val GPIO value
 * @return 0 if OK
 */
int gpio_direction_output(u32 gpio, u32 val);

/**
 * get corresponding GPIO's value
 * @param gpio see @ref gpio_pin_t
 * @return GPIO value, can be 0 or 1
 */
u8 gpio_get_value(u32 gpio);

/**
 * set corresponding GPIO's value
 * @param gpio see @ref gpio_pin_t
 * @param val This parameter can be 0 or 1
 */
void gpio_set_value(u32 gpio, u8 val);

/**
 * @brief Pin multiplex mode.
 *
 * @param gpio @ref gpio_pin_t
 * @param mod This parameter can be one the following values:
 *        @arg 1 -- normal GPIO, used as input or output
 *        @arg 0 -- used as alternate function or analog
 */
void gpio_set_pinmux(u32 gpio, u32 mod);

/**
 * @brief Set alternate function of GPIO
 *
 * @param gpio see @ref gpio_pin_t
 * @param af   see @ref gpio_af_t
 */
void gpio_set_alternate_function(gpio_pin_t gpio, gpio_af_t af);

/**
 * @brief Enable/Disable Analog mode and connect to the Aux ADC.
 * @note  Only one GPIO can be connected to the Aux ADC at the same time.
 * @param gpio @ref gpio_pin_t
 * @param en This parameter can be one of the following values:
 *        @arg 1 -- enable
 *        @arg 0 -- disable
 */
void gpio_set_analog_en(u32 gpio, u32 en);

/**
 * @brief Enable/disable GPIO reading input level.
 *
 * @param gpio @ref gpio_pin_t
 * @param en This parameter can be one of the following values:
 *        @arg 1 -- enable
 *        @arg 0 -- disable
 */
void gpio_set_rd_en(u32 gpio, u32 en);

/**
 * @brief Enable/Disable GPIO pull-up.
 *
 * @param gpio @ref gpio_pin_t
 * @param en This parameter can be one of the following values:
 *        @arg 1 -- enable
 *        @arg 0 -- disable
 */
void gpio_set_pull_up(u32 gpio, u32 en);

/**
 * @brief Enable/Disable GPIO pull-down.
 *
 * @param gpio @ref gpio_pin_t
 * @param en This parameter can be one of the following values:
 *        @arg 1 -- enable
 *        @arg 0 -- disable
 */
void gpio_set_pull_down(u32 gpio, u32 en);

/**
 * @brief Set the pin output level.
 * @note  Only GPIO_AON_2 output can be set to 5V.
 * @param level see @ref gpio_level_t
 * @return None
 */
void gpio_set_output_level(gpio_level_t level);

/**
 * @}
 */

/**
 * @}
 */


/**
 * @addtogroup AT6010_HAL_DRIVER
 * @{
 */

/**
 * @defgroup HAL_GPIO GPIO
 * @brief GPIO HAL Driver
 * @{
 */

/**
 * @brief GPIO initialization.
 *
 * @param gpio
 * @param cfg
 * @return gpio_status_t
 */
gpio_status_t gpio_init(gpio_pin_t gpio, const gpio_cfg_t *cfg);

/**
 * @brief Set the GPIO to output mode
 *
 * @param gpio @ref gpio_pin_t
 */
void gpio_toggle_init(u32 gpio);

/**
 * @brief Toggles the GPIO output.
 *
 * @param gpio @ref gpio_pin_t
 */
void gpio_toggle(u32 gpio);

/**
 * @brief Set GPIO interrupt trigger type.
 *
 * @param gpio @ref gpio_pin_t
 * @param type @ref irq_type_t
 */
void gpio_set_irq_type(gpio_pin_t gpio, irq_type_t type);

/**
 * @brief Set interrupt callback function.
 *
 * @param gpio @ref gpio_pin_t
 * @param hdl call back function
 */
void gpio_irq_set_hdl(gpio_pin_t gpio, gpio_hdl_t hdl);

/**
 * @brief This function should be called in GPIOx_Handler().
 * @note  gpio_irq_set_hdl() must be called before.
 * @param gpio @ref gpio_pin_t
 */
void gpio_irq_hdl(u32 gpio);

/**
 * @brief This function should be called in GPIO_Handler() or GPIOx_Handler().
 * @note  gpio_irq_set_hdl() must be called before.
 */
void gpio_isr(void);

void gpio_irq_enable(gpio_pin_t gpio);

void gpio_irq_disable(gpio_pin_t gpio);

/**
 * @brief Clear the interrupt flag.
 *
 * @param gpio
 */
void gpio_irq_clr_flag(gpio_pin_t gpio);

/**
 * @brief Get the interrupt status.
 * @param gpio
 * @return state of bit: 0 or 1.
 */
u32  gpio_irq_get_status(gpio_pin_t gpio);

/**
 * @brief Get the rising edge interrupt status.
 *
 * @param gpio
 * @return state of bit: 0 or 1.
 */
u32  gpio_irq_rise_get_status(gpio_pin_t gpio);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__GPIO_H__ */
