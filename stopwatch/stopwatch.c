/*
 * pwm_output.c
 *
 *  Created on: 21. 11. 2014
 *      Author: ondra
 */

#include <libopencm3/stm32/timer.h>

#include <stopwatch.h>
//#include <bsp_config.h>

#define TIMER_LOW_RCC RCC_TIM16
#define TIMER_HIGH_RCC RCC_TIM15

#define TIMER_LOW TIM16
#define TIMER_HIGH TIM15

#define TICKS_PER_US 48

#define TRIG_INPUT TIM_SMCR_TS_ITR2

void sw_init(void)
{
	rcc_periph_clock_enable(TIMER_HIGH_RCC);
	rcc_periph_clock_enable(TIMER_LOW_RCC);


	timer_set_prescaler(TIMER_LOW, TICKS_PER_US);
	timer_set_period(TIMER_LOW, 0xffff);

	timer_update_on_overflow(TIMER_LOW);
	timer_set_master_mode(TIMER_LOW, TIM_CR2_MMS_UPDATE);


	timer_slave_set_mode(TIMER_HIGH, TIM_SMCR_SMS_ECM1);
	timer_slave_set_trigger(TIMER_HIGH, TRIG_INPUT);

	timer_set_prescaler(TIMER_HIGH, 0);
	timer_set_period(TIMER_HIGH, 0xffff);

	timer_enable_counter(TIMER_HIGH);
}


void sw_start(void)
{
	timer_enable_counter(TIMER_LOW);
}


void sw_stop(void)
{
	timer_disable_counter(TIMER_LOW);
}


void sw_clear(void)
{
	timer_set_counter(TIMER_LOW, 0);
	timer_set_counter(TIMER_HIGH, 0);
}


uint32_t sw_get_time_us(void)
{
	return (timer_get_counter(TIMER_HIGH) << 16) | timer_get_counter(TIMER_LOW);
}
