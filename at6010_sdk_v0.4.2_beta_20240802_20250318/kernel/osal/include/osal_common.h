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

#ifndef __OSAL_COMMON_H__
#define __OSAL_COMMON_H__
#include "atlog.h"

#ifdef CORTEX_M0PLUS
#include "CMSDK_CM0plus.h"
#include "core_cm0plus.h"
#endif

/**
 * OSAL configuration. (it may to be moved to target_config file.)
 */

/* Select specified OS for the OSAL.  */

/**
 * @defgroup OSAL_SELECT OSAL Selection
 * @{
 */
#define OSAL_NOOS               1
#define OSAL_FREERTOS           2

#ifndef OSAL_SELECTION
#define OSAL_SELECTION          OSAL_FREERTOS
#endif
/**
 * @}
 */

/* the number of user-defined tasks */
#define OSAL_USER_TASK_NUM      4

/* default msg queue length. */
#define OSAL_MSG_QUEUE_LEN      10


#if OSAL_SELECTION == OSAL_FREERTOS

#include "FreeRTOS.h"
#include "task.h"

#define OSAL_WAIT_FOREVER        portMAX_DELAY
#define OSAL_WAIT_NONE           0
#define OSAL_IDLE_PRIORITY       tskIDLE_PRIORITY
#define OSAL_MAX_PRIORITY        configMAX_PRIORITIES
#define OSAL_HZ                  configTICK_RATE_HZ /* OS ticks per second */

#else /* OSAL_SELECTION == OSAL_NOOS */

#define OSAL_WAIT_FOREVER        0xFFFFFFFFUL
#define OSAL_WAIT_NONE           0
#define OSAL_IDLE_PRIORITY       0U
#define OSAL_MAX_PRIORITY        7
#define OSAL_HZ                  1000UL /* OS ticks per second */

#endif

#define OSAL_MS2TICK(ms)         ((u32)((ms) * OSAL_HZ + 999)/1000) /* plus 999 for round up */

#define OSAL_LOG(fmt, arg...)    \
	do {                          \
		printf(fmt, ##arg);  \
	} while (0)

#define OSAL_ERR(fmt, arg...) \
	do {                                    \
		printf( "[OSAL_ERR] %s():%d, "fmt, __func__, __LINE__, ##arg);  \
	} while (0)

#define OSAL_ASSERT_ERR(cmd, fmt, arg...) \
	do { \
		if (OSAL_OK != cmd) { \
			OSAL_ERR(fmt, ##arg); \
		} \
	} while (0)

/**
 * CTF(Create Task Flag) definition.
 */
#define OASL_CTF_NONE               (0x0)
#define OASL_CTF_MSG_QUEUE          (0x1 << 0) /* bit0: new msg queue when create task. */

/* bit1~bit4: msg queue length: (x+1)*default_len. */
#define OASL_CTF_MSG_LEN_MUL_POS    1
#define OASL_CTF_MSG_LEN_MUL_MASK   (0xf << 1)
#define OASL_CTF_MSG_LEN_MUL(x)     ((x & 0xf) << 1)


typedef enum {
    OSAL_OK          = 0,
    OSAL_ERROR       = -1,
    OSAL_NOMEM       = -2,
    OSAL_INVALID     = -3,
    OSAL_TIMEOUT     = -4,
    OSAL_NOTALLOWED  = -5,
} osal_status_t;

/**
 * @brief Thread priority definition
 */
typedef enum {
    OSAL_PRIORITY_IDLE            = OSAL_IDLE_PRIORITY,
    OSAL_PRIORITY_LOW             = 1,
    OSAL_PRIORITY_BELOW_NORMAL    = 2,
    OSAL_PRIORITY_NORMAL          = 3,
    OSAL_PRIORITY_ABOVE_NORMAL    = 4,
    OSAL_PRIORITY_HIGH            = 5,
    OSAL_PRIORITY_REAL_TIME       = OSAL_MAX_PRIORITY
} osal_priority_t;

typedef struct {
	u32 id;

	/**
	 * specify the msg para to send msg, it can be assigned an
	 * address value to point to a buffer.
	 */
	u32 para;
} osal_msg_t;

typedef enum {
	// NOTE: 在这里改了 MOD_XXX 序号，为了和 MSG_ID_XXX 兼容
	//       MOD_XXX    排在前面，占据前10个
	//       MSG_ID_XXX 排在后面，占据后10个；
	MOD_APP = 0, /* mod id for application task */
	MOD_SYS,      /* mod id for system task */
	MOD_RAD,      /* mod id for radar task */

#if OSAL_SELECTION != OSAL_FREERTOS
	MOD_IDLE,     /* mod id for idle task */
#endif

#if (defined(CFG_ATMESH) || defined(CFG_SIGMESH) || defined(CFG_BT))
	MOD_WCN,	  /* mod id for wireless task */
#endif
	MOD_INIT,         /* mod id for fork task */

	/* The following mod IDs are used for user-defined tasks. */
	MOD_USER_AUTO,
	MOD_USER_START = MOD_USER_AUTO,
	MOD_USER_END = MOD_USER_START + OSAL_USER_TASK_NUM,

	MOD_ID_MAX
} osal_mod_id_t;

typedef void (*osal_task_func_t)(void *arg);
typedef void *osal_hdl_t;            /* osal common handle. */
typedef osal_hdl_t osal_task_hdl_t;  /* osal task handle. */
typedef osal_hdl_t osal_queue_hdl_t; /* osal queue handle. */

__STATIC_INLINE void osal_assert_create(const char *name, osal_status_t cmd)
{
	OSAL_ASSERT_ERR(cmd, "Create %s failed!\r\n", name);
}

#endif /* __OSAL_COMMON_H__ */
