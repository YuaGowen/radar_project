#ifndef __PWM_H__
#define __PWM_H__

#include "at_types.h"

#define PWM_MAX_DUTY (1000U)

typedef enum {
	PWM_ID_0,
	PWM_ID_1,
	PWM_ID_2,
	PWM_ID_3,
	PWM_ID_4,
	PWM_ID_QTY
} pwm_id_t;

typedef struct {
	u32 invert : 1;
	u32 duty   : 13;
	u32 period : 14;
} pwm_cfg_t;

/**
 * @brief Set PWM clock divider, as
 * pwm_clk = sys_clk / (pwm_divider + 1)
 * pwm_divider decides the maximum and minimum frequency can be reached as
 * pwm_freq = pwm_clk / (con_period + 1), where con_period in range of [0,8191]
 * To get more accurate frequency and duty, pwm_clk should be set as large as possible.
 * To get exact frequency and duty for designated value without approximating,
 * the divider should be set to make the above calculation divisible.
 * @param divider: set as per the frequency needed
 */
void pwm_clk_divider_set(u8 divider);
u8 pwm_clk_divider_get(void);

void pwm_init(pwm_id_t id, u16 duty, u32 freq, u8 invert);
void pwm_reinit(void);

void pwm_enable(pwm_id_t id);
void pwm_disable(pwm_id_t id);
void pwm_hold_set(pwm_id_t id, u8 en);
void pwm_invert_set(pwm_id_t id, u8 invt);
void pwm_duty_set(pwm_id_t id, u16 duty);

#endif
