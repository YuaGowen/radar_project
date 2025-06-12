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

#include "mod_test.h"
#include "at6010.h"
#include "atlog.h"
#include "aux_adc.h"
#include "clock.h"
#include "common.h"
#include "dma.h"
#include "gpio.h"
#include "hal_common.h"
#include "hal_key.h"
#include "hal_timer.h"
#include "hal_dip.h"
#include "hal_light.h"
#include "i2c_host.h"
#include "irqflags.h"
#include "k_msg.h"
#include "string.h"
#include "sys_timer.h"

#ifdef CFG_MOD_TEST

/**
 * @brief v: pass, x: fail, s: skip
 */

//#define MOD_TEST_AUX_ADC       v
//#define MOD_TEST_I2C           x
//#define MOD_TEST_GPIO          v
//#define MOD_TEST_GPIO_INTR     v
//#define MOD_TEST_PULL_DATA_I2C s
//#define MOD_TEST_LOG           v
//#define MOD_TEST_LOG_TIME      v
//#define MOD_TEST_TIMER         v
//#define MOD_TEST_SYS_TIMER     s
//#define MOD_TEST_PWM           v
//#define MOD_TEST_USER_SETTING  s
//#define MOD_TEST_ADC_KEY       s
//#define MOD_TEST_DIP           v
//#define MOD_TEST_RELAY         v

#ifdef MOD_TEST_GPIO

#define TEST_GPIO_PIN           GPIO_MCU_5

void mod_test_gpio(void)
{
	u8 tmp = 0;

	gpio_set_pinmux(TEST_GPIO_PIN, 1);
	gpio_set_direction(TEST_GPIO_PIN, 1);

	for (;;) {
		gpio_set_value(TEST_GPIO_PIN, tmp);
		tmp = !tmp;
		delay_us(1000);
	}
}
#endif


#ifdef MOD_TEST_GPIO_INTR

#define GPIO_INTR_PIN           GPIO_MCU_9

/* use another pin to pull interrupt pin. */
#define GPIO_PULL_EN
#define GPIO_PULL_IO            GPIO_MCU_0

static const IRQn_Type irq_type[] = {GPIO0_IRQn, GPIO1_IRQn, GPIO2_IRQn, GPIO3_IRQn, GPIO4_IRQn, GPIO5_IRQn,
                                     GPIO6_IRQn, GPIO_IRQn,  GPIO_IRQn,  GPIO_IRQn,  GPIO_IRQn,  GPIO_IRQn,
                                     GPIO_IRQn,  GPIO_IRQn,  GPIO_IRQn,  GPIO_IRQn,  GPIO_IRQn};

static void test_intr_hdl(void)
{
	u32 gpio_irq;
	u32 gpio_irq_rise;

	NVIC_DisableIRQ(irq_type[GPIO_INTR_PIN]);

	gpio_irq = gpio_irq_get_status(GPIO_INTR_PIN);
	gpio_irq_rise = gpio_irq_rise_get_status(GPIO_INTR_PIN);

	if (gpio_irq == 1U) {
		logi("test_intr_hdl: interrupt occurred\n");
	} else {
		logi("test_intr_hdl: interrupt not occurred\n");
	}

	if (gpio_irq_rise == 1U) {
		logi("test_intr_hdl: expected rise edge status asserted\n");
	} else {
		logi("test_intr_hdl: expected fall edge status asserted\n");
	}

	gpio_irq_clr_flag(GPIO_INTR_PIN);

	NVIC_EnableIRQ(irq_type[GPIO_INTR_PIN]);
}

void mod_test_gpio_intr(void)
{
	u8 tmp = 0U;

	logi("%s\r\n", __func__);

	/* io configuration. */
	gpio_set_pinmux(GPIO_INTR_PIN, 1);
	gpio_set_direction(GPIO_INTR_PIN, 0);

	/* irq configuration. */
	gpio_set_irq_type(GPIO_INTR_PIN, IRQ_TYPE_EDGE_BOTH);
	gpio_irq_set_hdl(GPIO_INTR_PIN, test_intr_hdl);
	gpio_irq_enable(GPIO_INTR_PIN);

	NVIC_EnableIRQ(irq_type[GPIO_INTR_PIN]);

#ifdef GPIO_PULL_EN
	gpio_set_pinmux(GPIO_PULL_IO, 1);
	gpio_direction_output(GPIO_PULL_IO, 0);
#endif

	for (;;) {
#ifdef GPIO_PULL_EN
		logi("------ pull io = %d ------\r\n", tmp);
		gpio_set_value(GPIO_PULL_IO, tmp);
#endif
		tmp ^= 1U;
		delay_ms(3000U);
	}
}
#endif

#ifdef MOD_TEST_I2C
static void mod_test_i2c(void)
{
	u8 addr = 0x28U;
	u8 reg = 0x0FU;
	u8 val_r;
	u8 val_w = 0x65U;
	i2c_cfg_t i2c_cfg;
	gpio_cfg_t gpio_cfg;

	gpio_cfg.mode = GPIO_MODE_AF;
	gpio_cfg.pull = GPIO_PULL_UP;
	gpio_cfg.af = GPIO_AON3_AF1_I2C_SCL;
	(void)gpio_init(GPIO_AON_3, &gpio_cfg);

	gpio_cfg.af = GPIO_AON4_AF1_I2C_SDA;
	(void)gpio_init(GPIO_AON_4, &gpio_cfg);

	i2c_cfg.mode = I2C_MODE_HOST;
	i2c_cfg.sck = 100000U;
	i2c_init(I2C_ID_0, &i2c_cfg);

	sys_clk_enable_i2c();

	for (;;) {
		/* write */
		val_w++;
		if (val_w > 127U) {
			val_w = 0x1U;
		}
		(void)i2c_host_write_byte(addr, reg, val_w);
		logi("reg: 0x%x, write: 0x%x\n\r", reg, val_w);

		/* read */
		(void)i2c_host_read_byte(addr, reg, &val_r);
		logi("reg: 0x%x, write: 0x%x, read: 0x%x\n\r", reg, val_w, val_r);
		if (val_r != val_w) {
			logi("-------- i2c rw error ---------\n\r");
		}
		delay_ms(2000U);
	}
}
#endif /* MOD_TEST_I2C */

#ifdef MOD_TEST_PULL_DATA_I2C

//#define MISS_CNT_EN

void mod_test_pull_data_i2c(void)
{
	u16 adc_val = 0;
#ifdef MISS_CNT_EN
	u32 miss_cnt;
#endif

	logi("mod_test_pull_data_i2c\n\r");
	at5810_init();
	at5810_soft_reset();

	logi("polling adc data...\n\r");
	for (;;) {
		adc_val = at5810_get_adc_value();
		if (adc_val != 0xffff) {
			//logi("\r\n%04x", adc_val);
			at_printf("\r\n%04x", adc_val);
		}
		//delay_ms(1);
		delay_us(100);
#ifdef MISS_CNT_EN
		miss_cnt = hwp_cache->status_miss;
		logi("%d\r\n", miss_cnt);
#endif
	}
}
#endif

#ifdef MOD_TEST_LOG

static void log_test_output(void)
{
	logv("atlog test: verbose.\r\n");
	logi("atlog test: info.\r\n");
#ifdef AIRWATCHER_RAW_SUPPORT
	logr(0xa, 1);
	logr(0xb, 0);
#endif
	logw("atlog test: warning.\r\n");
	loge("atlog test: error.\r\n");
	loga("atlog test: assert.\r\n");
}

void mod_test_log(void)
{
	for (;;) {
		at_printf("set atlog level: ------verbose\r\n");
		set_log_level(LOG_PRT_LEV_VERBOSE);
		log_test_output();
		delay_ms(2000);

		at_printf("set atlog level: ------info\r\n");
		set_log_level(LOG_PRT_LEV_INFO);
		log_test_output();
		delay_ms(2000);

		at_printf("set atlog level: ------raw\r\n");
		set_log_level(LOG_PRT_LEV_RAW);
		log_test_output();
		delay_ms(2000);

		at_printf("set atlog level: ------warning\r\n");
		set_log_level(LOG_PRT_LEV_WARN);
		log_test_output();
		delay_ms(2000);

		at_printf("set atlog level: ------error\r\n");
		set_log_level(LOG_PRT_LEV_ERR);
		log_test_output();
		delay_ms(2000);

		at_printf("set atlog level: ------assert\r\n");
		set_log_level(LOG_PRT_LEV_ASSERT);
		log_test_output();
		delay_ms(2000);
	}
}
#endif

#ifdef MOD_TEST_TIMER

#define TEST_TIM_PERIOD_MS      200
#define TEST_TIM_PERIOD_US      500

#define TIM0_TEST_GPIO          GPIO_MCU_0
#define TIM1_TEST_GPIO          GPIO_MCU_1
#define TIM2_TEST_GPIO          GPIO_MCU_2
#define TIM3_TEST_GPIO          GPIO_MCU_3

static void test_time_hdl0(void *arg)
{
	gpio_toggle(TIM0_TEST_GPIO);
}

static void test_time_hdl1(void *arg)
{
	gpio_toggle(TIM1_TEST_GPIO);
}

static void test_time_hdl2(void *arg)
{
	gpio_toggle(TIM2_TEST_GPIO);
}

static void test_time_hdl3(void *arg)
{
	gpio_toggle(TIM3_TEST_GPIO);
}

/**
 * @brief test timer period and callback, use gpio to measure timer period
 */
static void mod_test_timer(void)
{
	timer_cfg_t timer_cfg;

	gpio_set_pinmux(TIM0_TEST_GPIO, 1);
	gpio_set_direction(TIM0_TEST_GPIO, 1);
	gpio_set_pinmux(TIM1_TEST_GPIO, 1);
	gpio_set_direction(TIM1_TEST_GPIO, 1);
	gpio_set_pinmux(TIM2_TEST_GPIO, 1);
	gpio_set_direction(TIM2_TEST_GPIO, 1);
	gpio_set_pinmux(TIM3_TEST_GPIO, 1);
	gpio_set_direction(TIM3_TEST_GPIO, 1);

	timer_cfg.source = TIMER_SOURCE_SYSTEM;
	timer_cfg.irq_en = 1;
	timer_cfg.start = 1;

	/* timer 0 */
	timer_cfg.cb = test_time_hdl0;
	timer_cfg.reload = TEST_TIM_PERIOD_MS * 1000;
	hal_timer_init(TIMER_ID_0, &timer_cfg);

	/* timer 1 */
	timer_cfg.cb = test_time_hdl1;
	timer_cfg.reload = TEST_TIM_PERIOD_US;
	hal_timer_init(TIMER_ID_1, &timer_cfg);

	/* timer 2 */
	timer_cfg.cb = test_time_hdl2;
	timer_cfg.reload = TEST_TIM_PERIOD_MS * 2 * 1000;
	hal_timer_init(TIMER_ID_2, &timer_cfg);

	/* timer 3 */
	timer_cfg.cb = test_time_hdl3;
	timer_cfg.reload = TEST_TIM_PERIOD_US * 2;
	hal_timer_init(TIMER_ID_3, &timer_cfg);

	for (;;) {
		logi("timer test...\n\r");
		delay_ms(5000);
	}
}
#endif /* end of MOD_TEST_TIMER */

#ifdef MOD_TEST_SYS_TIMER

#define TEST_SYS_TIM_PERIOD_MS      100U

static sys_timer_t *m_sys_tim0 = NULL;
static sys_timer_t *m_sys_tim1 = NULL;
static sys_timer_t *m_sys_tim2 = NULL;
static sys_timer_t *m_sys_tim3 = NULL;

static void test_sys_time_hdl0(u32 arg)
{
	logi("test_sys_time_hdl0\r\n");
	gpio_toggle(GPIO_MCU_0);
}

static void test_sys_time_hdl1(u32 arg)
{
	logi("test_sys_time_hdl1\r\n");
	gpio_toggle(GPIO_MCU_1);
}

static void test_sys_time_hdl2(u32 arg)
{
	logi("test_sys_time_hdl2\r\n");
	gpio_toggle(GPIO_MCU_2);
}

static void test_sys_time_hdl3(u32 arg)
{
	logi("test_sys_time_hdl3\r\n");
	gpio_toggle(GPIO_MCU_3);
}

static void mod_test_sys_timer(void)
{
	logi("mod_test_sys_timer\n\r");

	gpio_set_pinmux(GPIO_MCU_0, 1);
	gpio_set_direction(GPIO_MCU_0, 1);
	gpio_set_pinmux(GPIO_MCU_1, 1);
	gpio_set_direction(GPIO_MCU_1, 1);
	gpio_set_pinmux(GPIO_MCU_2, 1);
	gpio_set_direction(GPIO_MCU_2, 1);
	gpio_set_pinmux(GPIO_MCU_3, 1);
	gpio_set_direction(GPIO_MCU_3, 1);

	/* timer 0 */
	m_sys_tim0 = create_timer("sys tim0", TIMER_OPT_PERIODIC, test_sys_time_hdl0, 0);
	start_timer(m_sys_tim0, TEST_SYS_TIM_PERIOD_MS);

	/* timer 1 */
	m_sys_tim1 = create_timer("sys tim1", TIMER_OPT_PERIODIC, test_sys_time_hdl1, 0);
	start_timer(m_sys_tim1, TEST_SYS_TIM_PERIOD_MS * 2);

	/* timer 2 */
	m_sys_tim2 = create_timer("sys tim2", TIMER_OPT_PERIODIC, test_sys_time_hdl2, 0);
	start_timer(m_sys_tim2, TEST_SYS_TIM_PERIOD_MS * 3);

	/* timer 3 */
	m_sys_tim3 = create_timer("sys tim3", TIMER_OPT_PERIODIC, test_sys_time_hdl3, 0);
	start_timer(m_sys_tim3, TEST_SYS_TIM_PERIOD_MS * 4);

	for (;;) {
		//logi("sys timer test...\n\r");
#ifdef SUPPORT_SYS_TIMER_MSG
		k_msg_loop();
#else
		sys_timer_workque();
#endif
		//delay_ms(5000);
	}
}
#endif /* end of MOD_TEST_SYS_TIMER */

#ifdef MOD_TEST_LOG_TIME
static void mod_test_log_time(void)
{
	u16 tmp = 0x1000;

	logi("%s\n\r", __func__);

	gpio_set_pinmux(GPIO_MCU_0, 1);
	gpio_set_direction(GPIO_MCU_0, 1);
	gpio_set_pinmux(GPIO_MCU_1, 1);
	gpio_set_direction(GPIO_MCU_1, 1);
	gpio_set_pinmux(GPIO_MCU_2, 1);
	gpio_set_direction(GPIO_MCU_2, 1);
	gpio_set_pinmux(GPIO_MCU_3, 1);
	gpio_set_direction(GPIO_MCU_3, 1);

	for (;;) {
		gpio_set_value(GPIO_MCU_0, 1);
		at_printf("abcd\n\r");
		gpio_set_value(GPIO_MCU_0, 0);

		gpio_set_value(GPIO_MCU_1, 1);
		logi("abcd\n\r");
		gpio_set_value(GPIO_MCU_1, 0);

		gpio_set_value(GPIO_MCU_2, 1);
		at_printf("%4x\n\r", tmp++);
		gpio_set_value(GPIO_MCU_2, 0);

		gpio_set_value(GPIO_MCU_3, 1);
		logi("%4x\n\r", tmp++);
		gpio_set_value(GPIO_MCU_3, 0);
	}
}
#endif /* MOD_TEST_LOG_TIME */

#ifdef MOD_TEST_PWM
#include "pwm.h"

void mod_test_pwm(void)
{
	u16 duty = 0U;
	gpio_cfg_t gpio_cfg;

	logi("%s\r\n", __func__);

	sys_clk_enable_pwm();
	pwm_clk_divider_set(9U);

	gpio_cfg.mode = GPIO_MODE_AF;
	gpio_cfg.pull = GPIO_PULL_NONE;
	gpio_cfg.af = GPIO_AON0_AF0_PWM0;
	(void)gpio_init(GPIO_AON_0, &gpio_cfg);

	gpio_cfg.af = GPIO_AON1_AF0_PWM1;
	(void)gpio_init(GPIO_AON_1, &gpio_cfg);

	gpio_cfg.af = GPIO_AON2_AF0_PWM2;
	(void)gpio_init(GPIO_AON_2, &gpio_cfg);

	gpio_cfg.af = GPIO_AON3_AF2_PWM3;
	(void)gpio_init(GPIO_AON_3, &gpio_cfg);

	gpio_cfg.af = GPIO_AON4_AF2_PWM4;
	(void)gpio_init(GPIO_AON_4, &gpio_cfg);

	pwm_init(PWM_ID_0, 0U, 1000U, 0U);
	pwm_init(PWM_ID_1, 100U, 2000U, 0U);
	pwm_init(PWM_ID_2, 200U, 3000U, 1U);
	pwm_init(PWM_ID_3, 300U, 4000U, 1U);
	pwm_init(PWM_ID_4, 400U, 5000U, 0U);

	for (;;) {
		duty = (duty >= 1000U) ? 0U : (duty + 100U);
		pwm_duty_set(PWM_ID_0, duty);
		delay_ms(2000U);
	}
}
#endif

#ifdef MOD_TEST_AUX_ADC

#define TEST_AUX_ADC_P1           GPIO_MCU_2
#define TEST_AUX_ADC_P2           GPIO_MCU_3

static void mod_test_aux_adc(void)
{
	u32 adc_val1;
	u32 adc_val2;
	gpio_cfg_t gpio_cfg;

	logi("%s\r\n", __func__);

	adc_aux_init(ADC_AUX_DIR_EXTERNAL);
	gpio_cfg.mode = GPIO_MODE_ANALOG;
	gpio_cfg.pull = GPIO_PULL_NONE;
	(void)gpio_init(TEST_AUX_ADC_P1, &gpio_cfg);
	(void)gpio_init(TEST_AUX_ADC_P2, &gpio_cfg);

	for (;;) {
		adc_val1 = adc_get_aux_voltage(TEST_AUX_ADC_P1);
		adc_val2 = adc_get_aux_voltage(TEST_AUX_ADC_P2);
		logi("adc val1 = %d mV, val2 = %d mV\r\n", adc_val1, adc_val2);
		delay_ms(1000U);
	}
}
#endif


#ifdef MOD_TEST_USER_SETTING
#include "user_setting.h"

#define FLS_TEST_ADDR_OFFSET        0x10000
#define FLS_TEST_SIZE               0x100

void mod_test_user_setting(void)
{
	char name[10];
	int age = 0, len, update = 0;

	logi("%s\r\n", __func__);

	//user_setting_erase(); /* for test: erase every time. */

	user_setting_init();

	if (USER_SETTING_STATUS_SUCCESS != user_setting_get("name", name, &len)){
		user_setting_set("name","steven",10);
		update = 1;
		logi("%s, set name: steven\r\n", __func__);
	} else {
		logi("%s, get name: %s\r\n", __func__, name);
	}

	if (USER_SETTING_STATUS_SUCCESS != user_setting_get("age", (char *)&age, &len)){
		age = 20;
		user_setting_set("age",(char*)&age,4);
		update = 1;
		logi("%s, set age: %d\r\n", __func__, age);
	} else {
		logi("%s, get age: %d\r\n", __func__, age);
	}

	logi("%s, entry test loop.\r\n", __func__);
	for (;;) {
		if (update) {
			update = 0;
			user_setting_store();
			logi("%s, user setting store!!!\r\n", __func__);
		}
		delay_us(1000);
	}
}
#endif

#ifdef MOD_TEST_ADC_KEY
static void key_msg_hdl(u8 num, u8 event)
{
	logi("key msg: num %d event %d\n\r", num, event);
}

static void key_cb(u8 num, u8 event)
{
	logi("key cb : num %d event %d\n\r", num, event);
}

void mod_test_adc_key(void)
{
	logi("%s\n\r", __func__);

	u8 pressed;
#ifdef KEY_EVENT_MSG_POST_EN
	adc_key_reg_msg_hdl(key_msg_hdl);
#endif

#ifdef KEY_EVENT_CALLBACK_EN
	adc_key_reg_cb_hdl(key_cb);
#endif

	for (;;) {
		k_msg_loop();
		pressed = hal_key_get();
		if (pressed != 0xFFu) {
			logi("pressed %d\n\r", pressed);
		}
	}
}
#endif

#ifdef MOD_TEST_DIP
void mod_test_dip(void)
{
	u8 val = 0;

	while (1) {
		logi("---------------- hal_dip_test ---------------\r\n");
		logi("hwp_gpio, dat=0x%x\r\n", ((CMSDK_GPIO_TypeDef *)CMSDK_GPIO0_BASE)->DATA);
		val = hal_dip_get_data(DIP_SETTING_SEN_DIST);
		logi("DIP_SETTING_SEN_DIST, dat=%d\r\n", (uint16)val);

		val = hal_dip_get_data(DIP_SETTING_LIGHT_TIME1);
		logi("DIP_SETTING_LIGHT_TIME1, dat=%d\r\n", (uint16)val);

		val = hal_dip_get_data(DIP_SETTING_LUX);
		logi("DIP_SETTING_LUX, dat=%d\r\n", (uint16)val);

		delay_ms(2000);
	}
}
#endif /* MOD_TEST_DIP */

#ifdef MOD_TEST_RELAY
#define TEST_GPIO_PIN           GPIO_MCU_5
void mod_test_relay(void)
{
    u8 gpio_value =0u;

	gpio_set_pinmux(TEST_GPIO_PIN, 1);
	gpio_set_direction(TEST_GPIO_PIN, 1);

	while (1) {
        logi("%s\r\n", __func__);

        gpio_value = 1U - gpio_value;
        gpio_set_value(TEST_GPIO_PIN, gpio_value);  /* Compare the phase difference to determine whether it(CFG_RELAY_ZERO_PROTECTION) is effective */
        hal_relay_set_delay(gpio_value);
		delay_ms(20);
    }
}
#endif /* MOD_TEST_RELAY */

void mod_test_main_proc(void)
{
#ifdef MOD_TEST_GPIO
	mod_test_gpio();
#endif

#ifdef MOD_TEST_GPIO_INTR
	mod_test_gpio_intr();
#endif

#ifdef MOD_TEST_I2C
	mod_test_i2c();
#endif

#ifdef MOD_TEST_EXTI
	mod_test_exti();
#endif

#ifdef MOD_TEST_PULL_DATA_I2C
	mod_test_pull_data_i2c();
#endif

#ifdef MOD_TEST_LOG
	mod_test_log();
#endif

#ifdef MOD_TEST_TIMER
	mod_test_timer();
#endif

#ifdef MOD_TEST_SYS_TIMER
	mod_test_sys_timer();
#endif

#ifdef MOD_TEST_LOG_TIME
	mod_test_log_time();
#endif

#ifdef MOD_TEST_PWM
	mod_test_pwm();
#endif

#ifdef MOD_TEST_AUX_ADC
	mod_test_aux_adc();
#endif

#ifdef MOD_TEST_USER_SETTING
	mod_test_user_setting();
#endif

#ifdef MOD_TEST_ADC_KEY
	mod_test_adc_key();
#endif

#ifdef MOD_TEST_DIP
	mod_test_dip();
#endif

#ifdef MOD_TEST_RELAY
	mod_test_relay();
#endif
}

void test_task_entry(void *arg)
{
	UNUSED(arg);

	mod_test_main_proc();
	for (;;) {}
}

#endif /* CFG_MOD_TEST */
