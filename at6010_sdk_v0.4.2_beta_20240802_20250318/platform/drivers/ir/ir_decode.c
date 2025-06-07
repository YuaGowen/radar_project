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

#include "at6010.h"
#include "ir_decode.h"
#include "common.h"
#include "gpio.h"
#include "clock.h"
#include "timer.h"

#ifdef CFG_IR

#define IR_DBG_EN        0
#define IR_TIMER_SOURCE  TIMER_ID_2
//#define USE_DUAL_TIM2

//#define IR_TEST_PULL_EN
//#define DBG_MEASURE_ISR_TIME

#ifdef IR_TEST_PULL_EN
#define IR_PULL_PIN            GPIO_P7
#endif

#ifdef DBG_MEASURE_ISR_TIME
#define DBG_IR_OUTPUT_PIN      GPIO_P6
#endif

#if (IR_DBG_EN == 1)
#define IR_LOG logi
#else
#define IR_LOG(...)
#endif

#define IR_RECV_PIN            GPIO_MCU_3
#define IR_IRQ_NUM             GPIO3_IRQn

#define NEC_NBITS         (32U)
#define NEC_UNIT_TICK     m_nec_unit_tick
#define NEC_UNIT          NEC_UNIT_TICK
#define NEC_UNIT_US       (560U)                          /* us, approximately equal to 0.56ms. */
#define NEC_UNIT_QUARTER  (NEC_UNIT >> 2)
#define NEC_UNIT_HALF     (NEC_UNIT >> 1)                 /* us, half of NEC_UNIT. */
#define NEC_UNIT_3_4X     (NEC_UNIT_QUARTER + NEC_UNIT_HALF)
#define NEC_UNIT_2X       (NEC_UNIT << 1)                 /* us, NEC_UNIT*2. */
#define NEC_UNIT_3X       (NEC_UNIT_2X + NEC_UNIT)        /* us, NEC_UNIT*3. */
#define NEC_HEADER_PULSE  (NEC_UNIT << 4)                 /* 8.96ms */
#define NEC_HEADER_SPACE  (NEC_UNIT << 3)                 /* 4.48ms */
#define NEC_REPEAT_SPACE  (NEC_UNIT << 2)                 /* 2.24ms */
#define NEC_BIT_PULSE     (NEC_UNIT)                      /* 0.56ms */
#define NEC_BIT_0_SPACE   (NEC_UNIT)                      /* 0.56ms */
#define NEC_BIT_1_SPACE   (NEC_UNIT_2X + NEC_UNIT)        /* 1.68ms */
#define NEC_TRAILER_PULSE (NEC_UNIT)                      /* 0.56ms */
#define NEC_TRAILER_SPACE ((NEC_UNIT << 3) + NEC_UNIT_2X) /* even longer in reality */

#define EQ_MARGIN(d1, d2, margin) (((d1) > ((d2) - (margin))) && ((d1) < ((d2) + (margin))))
#define EQ_WITHIN(d, min, max)    (((d) > (min)) && ((d) < (max)))

#define ASSERT_BIT_PULSE(d)     EQ_WITHIN(d, NEC_UNIT_QUARTER >> 1, NEC_BIT_PULSE + NEC_UNIT)
#define ASSERT_BIT_1_SPACE(d)   EQ_MARGIN(d, NEC_BIT_1_SPACE, NEC_UNIT)
#define ASSERT_BIT_0_SPACE(d)   EQ_WITHIN(d, NEC_UNIT_QUARTER >> 1, NEC_BIT_0_SPACE + NEC_UNIT)
#define ASSERT_TRAILER_PULSE(d) EQ_WITHIN(d, NEC_UNIT_QUARTER >> 1, NEC_TRAILER_PULSE + NEC_UNIT)

typedef enum {
	STATE_INACTIVE,
	STATE_HEADER_PULSE,
	STATE_HEADER_SPACE,
	STATE_BIT_PULSE,
	STATE_BIT_SPACE,
	STATE_TRAILER_PULSE
} nec_state_t;

typedef struct {
	nec_state_t state;
	bool repeat;
	u32 count;
	u32 bits;
} nec_data_t;

static nec_data_t nec_dec_dat = {
	STATE_INACTIVE,
	0,
	0,
	0
};

static ir_nec_hdl_t m_ir_hdl;
static u32 m_nec_unit_tick;

/**
 * rc_repeat() - signals that a key is still pressed
 *
 * This routine is used by IR decoders when a repeat message which does
 * not include the necessary bits to reproduce the scancode has been
 * received.
 */
ATTR_RAM_SECTION __STATIC_INLINE void rc_repeat(void)
{
	/* send msg to other module for process. */
	IR_LOG("Repeat last key\n");
}

/**
 * rc_keydown() - generates input event for a key press
 * @scancode:   the scancode that we're seeking
 *
 * This routine is used to signal that a key has been pressed on the
 * remote control.
 */
ATTR_RAM_SECTION __STATIC_INLINE void rc_keydown(u16 scancode)
{
	if (m_ir_hdl != NULL) {
		m_ir_hdl(scancode);
	}
}

ATTR_RAM_SECTION __STATIC_INLINE void parse_rc_code(const nec_data_t *dat)
{
	u16 scancode;
	u8 address, not_address, command, not_command;

	address = ((u8)dat->bits & 0xffU);
	not_address = ((u8)(dat->bits >> 8) & 0xffU);
	command = ((u8)(dat->bits >> 16) & 0xffU);
	not_command = ((u8)(dat->bits >> 24) & 0xffU);

	if (((command ^ not_command) == 0xffU) &&
	    (((address ^ not_address) == 0xffU) || ((((u16)address << 8) | not_address) == 0xEFU))
	    && (dat->repeat == FALSE)) {
		/* Normal NEC IR code */
		scancode = (u16)((u16)address << 8) | command;
		rc_keydown(scancode);
		IR_LOG("NEC scancode 0x%04x\n", scancode);
	}
}

/**
 * ir_nec_decode() - Decode one NEC pulse or space
 * @param duration sys ticks from last edge
 */
ATTR_RAM_SECTION static void ir_nec_decode(u32 duration)
{
	nec_data_t *dat = &nec_dec_dat;

	IR_LOG("NEC decode started at state %d (%uus)\n", dat->state, duration);

	switch (dat->state) {
	case STATE_INACTIVE:
		/* NOTE: merge inactive and header_pulse in case of receiving error */
	case STATE_HEADER_PULSE:
		if (EQ_MARGIN(duration, NEC_HEADER_PULSE, NEC_UNIT_3X)) {
			dat->state = STATE_HEADER_SPACE;
			dat->count = 0;
			dat->repeat = FALSE;
		} else {
			dat->state = STATE_INACTIVE;
		}
		break;
	case STATE_HEADER_SPACE:
		if (EQ_MARGIN(duration, NEC_HEADER_SPACE, NEC_UNIT_2X)) {
			dat->state = STATE_BIT_PULSE;
		} else if (EQ_MARGIN(duration, NEC_REPEAT_SPACE, NEC_UNIT)) {
			rc_repeat();
			dat->repeat = TRUE;
			dat->state = STATE_TRAILER_PULSE;
		} else {
			dat->state = STATE_INACTIVE;
		}
		break;
	case STATE_BIT_PULSE:
		dat->state = (ASSERT_BIT_PULSE(duration)) ? STATE_BIT_SPACE : STATE_INACTIVE;
		break;
	case STATE_BIT_SPACE:
		if (ASSERT_BIT_1_SPACE(duration)) {
			dat->bits >>= 1;
			dat->bits |= (u32)1U << 31;
		} else if (ASSERT_BIT_0_SPACE(duration)) {
			dat->bits >>= 1;
			/* dat->bits |= 0U << 31; */
		} else {
			dat->state = STATE_INACTIVE;
			break;
		}
		dat->count++;
		dat->state = (dat->count == NEC_NBITS) ? STATE_TRAILER_PULSE : STATE_BIT_PULSE;
		break;

	case STATE_TRAILER_PULSE:
		if (ASSERT_TRAILER_PULSE(duration)) {
			parse_rc_code(dat);
		}
		dat->state = STATE_INACTIVE;
		break;
	default:
		dat->state = STATE_INACTIVE;
		break;
	}

	IR_LOG("NEC decode failed at count %d state %d %uus\r\n", dat->count, dat->state, duration);
}

ATTR_RAM_SECTION static void recv_intr_hdl(void)
{
	u32 tim_value;

#ifdef DBG_MEASURE_ISR_TIME
	gpio_set_value(DBG_IR_OUTPUT_PIN, 1);
#endif
	NVIC_DisableIRQ(IR_IRQ_NUM);

	/* get value and re-start timer. */
#ifdef USE_DUAL_TIM2
	tim_value = CMSDK_DUALTIMER2->TimerValue;
	CMSDK_DUALTIMER2->TimerLoad = TIMER_VALUE_MAX;
#else
	tim_value = timer_value_get(IR_TIMER_SOURCE);
	timer_value_set(IR_TIMER_SOURCE, TIMER_VALUE_MAX);
#endif
	tim_value = (TIMER_VALUE_MAX - tim_value);

	CMSDK_GPIO0->INTCLEAR = (0x1U << (u8)IR_RECV_PIN);

	ir_nec_decode(tim_value);

	NVIC_EnableIRQ(IR_IRQ_NUM);

#ifdef DBG_MEASURE_ISR_TIME
	gpio_set_value(DBG_IR_OUTPUT_PIN, 0);
#endif
}

void ir_recv_init()
{
	u32 tmp = 0;

	NEC_UNIT_TICK = NEC_UNIT_US * (clk_mcu_get() / 1000000U);

	/* io configuration. */
	gpio_set_pinmux(IR_RECV_PIN, 1);
	gpio_set_direction(IR_RECV_PIN, 0);

	/* irq configuration. */
	gpio_set_irq_type(IR_RECV_PIN, IRQ_TYPE_EDGE_BOTH);
	gpio_irq_set_hdl(IR_RECV_PIN, recv_intr_hdl);
	gpio_irq_enable(IR_RECV_PIN);

	NVIC_EnableIRQ(IR_IRQ_NUM);
	NVIC_SetPriority(IR_IRQ_NUM, 0);

#ifdef USE_DUAL_TIM2
	sys_clk_enable_dualtimer();
	CMSDK_DUALTIMER2->TimerControl = 0;
	CMSDK_DUALTIMER2->TimerIntClr = 0;
	CMSDK_DUALTIMER2->TimerLoad = 0xffffffffU;

	tmp = (0U << CMSDK_DUALTIMER_CTRL_PRESCALE_Pos) | /* prescale, 0: 1, 1: 16, 2: 256. */
	      (0U << CMSDK_DUALTIMER_CTRL_INTEN_Pos) |    /* interrupt, 0: disable, 1: enable. */
	      (0U << CMSDK_DUALTIMER_CTRL_MODE_Pos) |     /* free running mode. */
	      CMSDK_DUALTIMER_CTRL_SIZE_Msk |             /* 32-bits */
	      CMSDK_DUALTIMER_CTRL_EN_Msk;

	CMSDK_DUALTIMER2->TimerControl = tmp;
#else
	sys_clk_enable_timer2();
	timer_init(IR_TIMER_SOURCE, TIMER_SOURCE_SYSTEM);
	timer_reload_set(IR_TIMER_SOURCE, TIMER_VALUE_MAX);
	timer_irq_disable(IR_TIMER_SOURCE);
	timer_start(IR_TIMER_SOURCE);
#endif

#ifdef DBG_MEASURE_ISR_TIME
	gpio_set_pinmux(DBG_IR_OUTPUT_PIN, 1);
	gpio_set_direction(DBG_IR_OUTPUT_PIN, 1);
	gpio_set_value(DBG_IR_OUTPUT_PIN, 0);
#endif

#ifdef IR_TEST_PULL_EN
	gpio_set_pinmux(IR_PULL_PIN, 1);
	gpio_set_direction(IR_PULL_PIN, 1);

	logi("%s, loop.\r\n", __func__);
	while (1) {
		/*logi("time_dru 1: %d us\r\n", m_time_dur);
		gpio_set_value(IR_PULL_PIN, 0);
		delay_us(300);
		gpio_set_value(IR_PULL_PIN, 1);
		logi("time_dru 2: %d us\r\n", m_time_dur);*/
		delay_ms(1000);

		tmp = hal_ir_get_code();
		if (tmp != 0xff) {
			logi("key code: 0x%x\r\n", tmp);
		}
	};
#endif
}

void ir_nec_set_hdl(ir_nec_hdl_t hdl)
{
	m_ir_hdl = hdl;
}

#endif /* CFG_IR */
