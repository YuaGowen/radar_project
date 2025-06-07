/**
 *******************************************************************************
 * @file    target_config.h
 * @author  Airtouch Software Team
 * @brief   target configuration of the current project
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
 ******************************************************************************/

#ifndef __TARGET_CONFIG_H__
#define __TARGET_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup AT6010_APP_LAMPS_OS Application: Lamps with OS
 * @details A lamp demo.
 * @{
 */

/**
 * @brief Chip Selection
 */
#define CHIP_AT6010_U01


/**
 * @brief Application Selection
 */
#define CFG_APP_SENSOR_ULP


/**
 * @defgroup APP_FUNC_CONFIG Application Function Configuration
 * @details Uncomment the following macros to enable the functions.
 * @{
 */

//#define CFG_OS
#define CFG_LOG
//#define CFG_LS
//#define CFG_CI
//#define CFG_DIP_SWITCH
//#define CFG_LED_PWM
//#define CFG_IR
//#define CFG_SWITCH_TM /* power on/off test mode. */
//#define CFG_PRESENCE_DET /* Presence detection. */
//#define CFG_ADC_KEY /* Keysense. */
//#define CFG_AUX_ADC_KEY
//#define CFG_PWRON_SELF_CHECK
//#define CFG_RELAY_ZERO_PROTECTION
//#define CFG_HIGH_ALTITUDE
//#define CFG_PWR_FREQ_DET
//#define CFG_SWITCH_ONOFF_MODE
//#define CFG_WDT_SUPPORT
//#define CFG_AUX_LIGHT_EN
//#define CFG_DIAGNOSE_EN /* Enable self-diagnostic function. */
//#define CFG_BT

/**
 * @}
 */


/**
 * @defgroup APP_BOARD_CONFIG Application Board Configuration
 * @{
 */

#define SYS_FLASH_CLK_SEL     CLK_SYS_80M_FLASH_80M_HZ

/**
 * @defgroup APP_BOARD_CI_CFG Application Board CI Configuration
 * @{
 */
#define UART_CI_PORT          UART_ID_1
#define UART_CI_BAUDRATE      UART_BAUD_RATE_921600
#define UART0_PINMUX_SEL_X    UART0_PINMUX_SEL_0
/**
 * @}
 */

/**
 * @defgroup APP_BOARD_LOG_CFG Application Board Log Configuration
 * @{
 */
#define UART_LOG_PORT         UART_ID_1
#define UART_LOG_BAUDRATE     UART_BAUD_RATE_921600
#define UART0_PINMUX_SEL_X    UART0_PINMUX_SEL_0
/**
 * @}
 */

/**
 * @defgroup APP_BOARD_DUMP_CFG Dump Rawdata Log Configuration
 * @{
 */
#define UART_DUMP_PORT        UART_ID_0
#define UART_DUMP_BAUDRATE    5000000
#define UART1_PINMUX_SEL_X    UART1_PINMUX_SEL_3
/**
 * @}
 */

/**
 * @defgroup APP_BOARD_LIGHT_CFG Application Board Light Configuration
 * @{
 */
#define RADAR_OUT_PIN         GPIO_AON_2
#define PRIMARY_LIGHT_TYPE    LIGHT_TYPE_IO
/**
 * @}
 */

/**
 * @}
 */


/**
 * Parameter configuration
 */

/**
 * @defgroup APP_COMPONENT_PARAMETERS_CFG Application Component Parameters Configurations
 * @{
 */

/**
 * @defgroup APP_LIGHT_PARAMETERS_CFG Light Parameters for lamp control
 * @{
 */
#define PARA_LIGHT_ON_TIME_MS                   2000   /**< Lighting duration. */
#define PARA_LIGHT_DIM_TIME_MS                  2000   /**< Light dim duration ONLY enable when PARA_PWM_DIM_OFF */
#define PARA_SELF_CHECK_TIME_MS                 10000  /**< The time of self check after power on. */
#define PARA_LIGHT_IO_INVERT                    0      /**< Light I/O output invert, 1:enable, 0:disable. */
/**
 * @}
 */

/**
 * @defgroup APP_RADAR_PARAMETERS_CFG Radar Parameters
 * @{
 */
#define PARA_MOT_DIST_LV                        7      /**< The distance level for motion detection. */
#define PARA_BHR_DIST_LV                        0      /**< The distance level for breathing&heart-rate detection. */
#define PARA_PWR_NS_LV                          PWR_AC_NS_MID    /**< specifed power supply type and noise suppress level. */
#define PARA_SKIP_NUM_LIGHT_ONOFF               200              /**< Skip number when light on/off,only for light ctrl app. */
/**
 * @}
 */

/**
 * @defgroup APP_LIGHT_SENSORS_PARAMETERS_CFG Light Sensor Parameters
 * @{
 */
#define PARA_LS_THRESHOLD               600     /* The threshold of Light sensor. */
#define PARA_LS_TH_HYSTERESIS           10      /* Light sensor threshold hysteresis offset from dark to bright */
#define PARA_LS_VOL_CON_INVERSE         0       /* Voltage converter, 0: light up, voltage drop. 1: inverse. */
#define PARA_LS_SET_PIN_EN              0       /* Light sensor set pin function, 1: enable, 0: disable */
#define PARA_LS_SET_PIN_NUM             GPIO_P9 /* The pin is setted when light sensor work */
#define PARA_LS_DETECT_INVALID          0       /* Radar detection is not affected by the light sensor */
#define PARA_LS_TYPE_UVL                0       /* Is ultraviolet light sensor */
/**
 * @}
 */

/**
 * @defgroup APP_PWM_PARAMETERS_CFG PWM Parameters
 * @{
 */
#define PARA_PWM_FREQ                   1000   /* The output frequency of the PWM. */
#define PARA_PWM_LON_DUTY               800    /* PWM duty cycle when light on. */
#define PARA_PWM_LDIM_DUTY              400    /* PWM duty cycle when light dim, ONLY enable when PARA_PWM_DIM_OFF == 1 */
#define PARA_PWM_LOFF_DUTY              200    /* PWM duty cycle when light off. */
#define PARA_PWM_INVERT                 0      /* The PWM output invert, 1: enable, 0: disable */
#define PARA_PWM_OFF_DAYTIME            0      /* Turn off light when in the daytime mode. */
#define PARA_PWM_LUMACYCLE              LED_LUMACYCLE_LIGHT_NONE /**< Gradual change mode of PWM duty cycle */
#define PARA_PWM_DIM_OFF                0      /**< Add secondary luminance for light, 1: enable 2: disable */
#define PARA_PWM_PHASE_SYNC             0      /**< PWM phase sync 1: enable, 0: disable */
#define PARA_PWM_SYNC_EDGE              0      /* PWM channel sync edge 0: falling 1: rising */
/**
 * @}
 */


/**
 * @}
 */


/**
 * @defgroup APP_MISC_CFG Miscellaneous Configurations
 * @{
 */

#if defined(CFG_SWITCH_TM) || defined(CFG_CI)
#define CFG_USER_SETTING
#endif

#ifdef CFG_WDT_SUPPORT
#define WDT_TIME_MS                     (1000U)
#endif

#ifdef CFG_DIAGNOSE_EN
#define CFG_GPIO_TOGGLE

#define GPIO_DIAGNOSE_PIN               GPIO_P4  /* connect to external LED. */
#define DIAGNOSE_ADC_DELTA              (2000u)  /* The valid difference values of ADC */
#endif

#if defined(CFG_BHR_DET_IND) && !defined(CFG_AUX_LIGHT_EN)
#define CFG_AUX_LIGHT_EN
#endif

#ifdef CFG_AUX_LIGHT_EN
#define LIGHT_AUX_IO_PIN                GPIO_P4
#define AUX_LIGHT_TYPE                  LIGHT_TYPE_PWM
#define AUX_LIGHT_TIME_MS               (200)
#endif

#define PARA_ALG_HEAP_SIZE              (0)       /**< Heap size for radar algorithm */

#define PARA_OS_HEAP_SIZE               (1024*13) /* Heap size for os tasks */

#if (defined CFG_ATMESH || defined CFG_SIGMESH || defined CFG_BT)
#define PARA_BT_HEAP_SIZE               (1024*11)  /* Heap size for bluetooth functions */
#else
#define PARA_BT_HEAP_SIZE               (0) /**< Heap size for bluetooth functions */
#endif

#define CFG_HEAP_SIZE (PARA_OS_HEAP_SIZE + PARA_ALG_HEAP_SIZE + PARA_BT_HEAP_SIZE) /**< Total heap size */

#if defined(CFG_RELAY_ZERO_PROTECTION) || defined(PARA_PWM_PHASE_SYNC)
#define POWER_SAMPLE_PIN                GPIO_AON_1       /**< Select input pin for power voltage divider circuit */

#ifdef CFG_RELAY_ZERO_PROTECTION
#define PARA_RELAY_TIMER                TIMER_ID_3
#define RELAY_ON_DELAY_US               (4300u)
#define RELAY_OFF_DELAY_US              (8600u)
#endif
#endif

#ifdef CFG_CI
#define PARA_CI_PORT                    0 /* 0: ci use uart, 1: ci use i2c */
#define PARA_CI_I2C_ADDR                0x39U
#endif

//#define RAD_DET_ONE_SHOT    /* the detection event is triggered only once during the light-on period. */

/**
 * @}
 */

/**
 * @defgroup APP_DEBUG_CFG Application Debug Configuration
 * @{
 */

#define LOG_DEFAULT_LEVEL                       LOG_PRT_LEV_INFO
//#define CFG_IF_PRINT_TM /* output if-data for the motion detection. */
//#define CFG_PRINT_BHR_DATA /* output if-data for the breath detection. */
//#define CFG_PRINT_MICRO_DET_DATA /* output if-data for the micro detection. */
//#define AIRWATCHER_RAW_SUPPORT
//#define CFG_GPIO_TOGGLE
//#define CFG_CM_BACKTRACE
#define GPIO_TOGGLE_PIN                         GPIO_P2
//#define CFG_MOD_TEST
//#define CFG_BHR_DET_IND /* Indicate breath&heartrate detection by aux light */
#define CFG_RADAR_SIMU
//#define ATCMD_SUPPORT
#define CFG_CLI
//#define CFG_RADAR_SHELL
//#define CFG_LIGHT_SHELL
//#define CFG_BT_SHELL

/* Breath detection indicator configuration. */
#define BHR_DET_IND_PERIOD
#define BHR_DET_IND_PERIOD_TIME         (10)  /* in second */

//#define CFG_RADAR_STAGE_SUPPORT             /* support radar-stage */

/**
 * @}
 */

/**
 * Other Configuration 2.
 */
#include "cfg_dip_switch.h"
#include "cfg_ir.h"
#include "cfg_paras.h"

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__TARGET_CONFIG_H__ */
