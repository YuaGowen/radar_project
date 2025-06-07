#include "at6010.h"
#include "hal_uart.h"
#include "hal_clock.h"
#include "gpio.h"
#include "pwm.h"
#include "clock.h"

#define PWM_FREQ        (1000u)
#define SYNC_SIGNAL_PIN (gpio_pin_t) GPIO_MCU_2

static void test_intr_hdl(void)
{
	gpio_irq_clr_flag(SYNC_SIGNAL_PIN);
	pwm_hold_set(PWM_ID_2, 1U);
	pwm_hold_set(PWM_ID_3, 1U);
	pwm_hold_set(PWM_ID_2, 0U);
	pwm_hold_set(PWM_ID_3, 0U);
}

int main(void)
{
	uart_cfg_t uart_cfg;

	hal_clk_init(CLK_SYS_80M_FLASH_80M_HZ);

	uart_cfg.baud_rate = UART_BAUD_RATE_921600;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;

	(void)hal_uart_init(UART_ID_0, &uart_cfg);

	(void)printf("*** example code: pwm sync. ***\r\n");

	sys_clk_enable_gpio();
	sys_clk_enable_pwm();
	pwm_clk_divider_set(4U);

	/* Configure input sync signal port */
	gpio_set_pinmux((u32)SYNC_SIGNAL_PIN, 1U);
	gpio_set_direction((u32)SYNC_SIGNAL_PIN, 0U);

	gpio_set_irq_type(SYNC_SIGNAL_PIN, IRQ_TYPE_EDGE_FALLING);
	gpio_irq_set_hdl(SYNC_SIGNAL_PIN, test_intr_hdl);
	gpio_irq_enable(SYNC_SIGNAL_PIN);
	NVIC_EnableIRQ((IRQn_Type)((u32)GPIO0_IRQn + (u32)SYNC_SIGNAL_PIN));

	/* Configure pwm port */
	gpio_set_pinmux(((u32)GPIO_AON_2), 0U);
	gpio_set_alternate_function(((gpio_pin_t)GPIO_AON_2), GPIO_AON2_AF0_PWM2);

	gpio_set_pinmux((u32)GPIO_AON_3, 0U);
	gpio_set_alternate_function(((gpio_pin_t)GPIO_AON_3), GPIO_AON3_AF2_PWM3);

	pwm_init(PWM_ID_2, 500U, PWM_FREQ, 0U);
	pwm_init(PWM_ID_3, 500U, PWM_FREQ, 0U);

	for (;;) {}
}
