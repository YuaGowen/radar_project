/*
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


#ifndef _AUX_ADC_H_
#define _AUX_ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "at_common.h"
#include "gpio.h"

/* Global Consants -----------------------------------------------------------*/

#define ADC_VREF_mV                    (3300U)
#define ADC_BITS                       (12U)

/* Global Data Types ---------------------------------------------------------*/

typedef enum {
	ADC_AUX_DIR_INTERNAL,
	ADC_AUX_DIR_EXTERNAL
} adc_aux_dir_t;

typedef void (*adc_aux_hdl_t)(u16 value);

/* Global Function Declarations ----------------------------------------------*/

void adc_aux_init(adc_aux_dir_t dir);
void adc_aux_deinit(void);

void adc_aux_start_cont(gpio_pin_t pin);
void adc_aux_stop(void);

u32  adc_get_aux_value(gpio_pin_t pin);
u32  adc_get_aux_voltage(gpio_pin_t pin);

void adc_aux_irq_hdl_set(adc_aux_hdl_t hdl);
void adc_aux_isr(void);

#ifdef __cplusplus
}
#endif

#endif /* !_AUX_ADC_H_ */
