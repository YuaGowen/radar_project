/**
* Copyright (c) 2020, Airtouching Intelligence Technology.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form, except as embedded into a Airtouching
* Intelligence Technology integrated circuit in a product or a software update for
* such product, must reproduce the above copyright notice, this list of
* conditions and the following disclaimer in the documentation and/or other
* materials provided with the distribution.
*
* 3. Any software provided in binary form under this license must not be reverse
* engineered, decompiled, modified and/or disassembled.
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

#include <stdlib.h>
#include "at_error.h"
#include "osal.h"

/**
 * Enter critical area.
 *
 * @return  PRIMASK value.
 *
 */
uint32_t osal_enter_critical(void)
{
	// 保存当前 PRIMASK 值
	uint32_t primask = __get_PRIMASK();

	// 关闭系统全局中断（其实就是将 PRIMASK 设为 1）
	__disable_irq();

	return primask;
}

/**
 * Exit critical area.
 *
 * @param[in]  previously saved PRIMASK value.
 *
 */
void osal_exit_critical(uint32_t mask)
{
	// 恢复 PRIMASK
	__set_PRIMASK(mask);
}
