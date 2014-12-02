/*
 * timeout.c
 *
 *  Created on: 2. 12. 2014
 *      Author: ondra
 */

#include <stdbool.h>
#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#include <timeout.h>

#define TIMEOUT_TIMER TIMxy
#define TIMEOUT_TIMER_RCC RCC_TIMxy
#define TIMEOUT_TIMER_ISR timxy_isr
#define TIMEOUT_TIMER_IRQ NVIC_TIMxy_IRQ
#define TIMEOUT_TIMER_IRQ_PRIO 4
#define TICKS_PER_MS 48000

bool timeout = true;
void (*cb_fun)(void *) = NULL;
void *cb_par = NULL;

void timeout_init(void)
{
	rcc_periph_clock_enable(TIMEOUT_TIMER_RCC);

	timer_set_mode(TIM6, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_prescaler(TIMEOUT_TIMER, TICKS_PER_MS - 1);
	timer_enable_irq(TIM6, TIM_DIER_UIE);

	nvic_enable_irq(TIMEOUT_TIMER_IRQ);
	nvic_set_priority(TIMEOUT_TIMER_IRQ, TIMEOUT_TIMER_IRQ_PRIO);
}

bool timeout_start(uint32_t timeout_ms, bool periodic, void (*callback)(void*), void *cb_params)
{
	if (TIM_CR1(TIMEOUT_TIMER) & TIM_CR1_CEN) {
		return false;
	}

	timer_set_period(timeout_ms);


	if (periodic) {
		timer_continuous_mode(TIMEOUT_TIMER);
	} else {
		timer_one_shot_mode(TIMEOUT_TIMER);
	}

	timeout = false;
	cb_fun = callback;
	cb_par = cb_params;

	timer_enable_counter(TIMEOUT_TIMER);


	return true;
}

void timeout_stop(void)
{
	timer_disable_counter(TIMEOUT_TIMER);
}

bool timeout_has_elapsed(void)
{
	return timeout;
}

TIMEOUT_TIMER_ISR(void)
{
	timer_clear_flag(TIMEOUT_TIMER, TIM_SR_UIF);

	timeout = true;

	if (cb_fun != NULL) {
		cb_fun(cb_par);
	}
}
