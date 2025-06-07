/**
 *******************************************************************************
 * @file    hal_def.h
 * @author  Airtouch Software Team
 * @brief   common data types and macros for HAL module
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

#ifndef __HAL_DEF_H__
#define __HAL_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup AT6010_HAL_DRIVER AT6010 HAL Driver
 * @{
 */

/**
 * @defgroup AT6010_HAL_DEF HAL defines
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported Types ------------------------------------------------------------*/

/**
 * @addtogroup HAL_Exported_Types HAL Exported Types
 * @{
 */

/**
 * @brief HAL Status Enums Definitions
 */
typedef enum {
	HAL_OK                                = 0x00U,
	HAL_ERROR                             = 0x01U,
	HAL_BUSY                              = 0x02U,
	HAL_TIMEOUT                           = 0x03U,
} hal_status_t;

/**
 * @brief HAL Lock Enum Definitions
 */
typedef enum {
	HAL_UNLOCKED                          = 0x00U,
	HAL_LOCKED                            = 0x01U,
} hal_lock_t;

typedef enum {
	RESET = 0U,                           /*!< Reset state = 0 */
	SET = !RESET                          /*!< Set state = 1 */
} flag_state_t, it_state_t;


// HAL_Exported_Types
/**
 * @}
 */

/* Exported Constants --------------------------------------------------------*/
/**
 * @addtogroup HAL_Exported_Constants HAL Exported Constants
 * @{
 */
#define HAL_MAX_DELAY                     0xFFFFFFFFU

// HAL_Exported_Constants
/**
 * @}
 */

/* Exported Macros -----------------------------------------------------------*/
/**
 * @addtogroup HAL_Exported_Macors HAL Export Macros
 * @{
 */
#define HAL_IS_BIT_SET(REG, BIT)         (((REG) & (BIT)) == (BIT))
#define HAL_IS_BIT_CLR(REG, BIT)         (((REG) & (BIT)) == 0U)

#define __HAL_LINKDMA(__HDL__, __DMA_FIELD__, __DMA_HDL__)                     \
                    do {                                                       \
                        (__HDL__)->__DMA_FIELD__ = &(__DMA_HDL__);             \
                        (__DMA_HDL__).parent = (__HDL__);                      \
                    } while(0U)

/** @brief Reset the Handle's State field.
  * @param __HDL__ specifies the Peripheral Handle.
  * @note  This macro can be used for the following purpose:
  *          - When the Handle is declared as local variable; before passing it as parameter
  *            to HAL_PPP_Init() for the first time, it is mandatory to use this macro
  *            to set to 0 the Handle's "State" field.
  *            Otherwise, "State" field may have any random value and the first time the function
  *            HAL_PPP_Init() is called, the low level hardware initialization will be missed
  *            (i.e. HAL_PPP_MspInit() will not be executed).
  *          - When there is a need to reconfigure the low level hardware: instead of calling
  *            HAL_PPP_DeInit() then HAL_PPP_Init(), user can make a call to this macro then HAL_PPP_Init().
  *            In this later function, when the Handle's "State" field is set to 0, it will execute the function
  *            HAL_PPP_MspInit() which will reconfigure the low level hardware.
  * @retval None
  */
#define __HAL_RESET_HANDLE_STATE(__HDL__) ((__HDL__)->state = 0U)

#define USE_RTOS         (0U) //TODO may be deleted
#if (USE_RTOS == 1U)
  /* Reserved for future use */
#error "USE_RTOS should be 0 in the current HAL release"
#else
#define __HAL_LOCK(__HDL__)                                                    \
                    do {                                                       \
                        if((__HDL__)->lock == HAL_LOCKED) {                    \
                            return HAL_BUSY;                                   \
                        } else {                                               \
                            (__HDL__)->lock = HAL_LOCKED;                      \
                        }                                                      \
                    } while (0U)

#define __HAL_UNLOCK(__HDL__)                                                  \
                    do {                                                       \
                        (__HDL__)->lock = HAL_UNLOCKED;                        \
                    } while (0U)
#endif /* USE_RTOS */

// HAL_Exported_Macors
/**
 * @}
 */

// AT6010_HAL_DEF
/**
 * @}
 */

// AT6010_HAL_DRIVER
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* !__HAL_DEF_H__ */
