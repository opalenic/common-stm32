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

#include <common.h>

#define TIMER_A_ISR timxy_isr
#define TIMER_B_ISR timxy_isr

#define IRQ_PRIO (0x03 << 6)
#define TICKS_PER_MS 48000


static struct timeout_timer {
	uint32_t base;
	enum rcc_periph_clken clken;
	enum rcc_periph_rst rst;
	uint32_t irq;
	void (*callback)(void *);
	void *params;
	bool periodic;
	bool running;
};

static struct timeout_timer conf[] = {
	// {
	// 	.base = TIMx,
	// 	.clken =,
	// 	.rst = ,
	// 	.irq =
	// }
}


static bool valid_timer(enum timeout_timer timer)
{
	return timer == TIMEOUT_TIMER_A || timer == TIMEOUT_TIMER_B;
}

void timeout_init(void)
{
	for (int i = 0; i < ARRAY_SIZE(conf); i++ {

		rcc_periph_clock_enable(conf[i].clken);

		timer_set_mode(conf[i].base, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
		timer_set_prescaler(conf[i].base, TICKS_PER_MS - 1);
		timer_enable_irq(conf[i].base, TIM_DIER_UIE);

		nvic_enable_irq(conf[i].irq);
		nvic_set_priority(conf[i].irq, TIMEOUT_TIMER_IRQ_PRIO);

	}
}

bool timeout_start(enum timeout_timer timer, uint32_t timeout_ms, bool periodic, void (*callback)(void*), void *params)
{
	if (!valid_timer(timer)) {
		return false;
	}

	if (conf[timer].running) {
		return false;
	}

	timer_set_period(conf[timer].base, timeout_ms);


	if (periodic) {
		timer_continuous_mode(conf[timer].base);
	} else {
		timer_one_shot_mode(conf[timer].base);
	}
	conf[timer].periodic = periodic;

	conf[timer].callback = callback;
	conf[timer].params = params;

	conf[timer].running = true;

	timer_enable_counter(conf[timer].base);

	return true;
}

void timeout_stop(enum timeout_timer timer)
{
	if (!valid_timer(timer)) {
		return;
	}

	conf[timer].running = false;

	timer_disable_counter(conf[timer].base);
}

bool timeout_timer_is_running(enum timeout_timer timer)
{
	if (!valid_timer(timer)) {
		return false;
	}

	return conf[timer].running;
}

static inline void isr_handler(enum timeout_timer timer)
{
	timer_clear_flag(conf[timer].base, TIM_SR_UIF);

	if (!conf[timer].periodic) {
		conf[timer].running = false;
	}

	if (conf[timer].callback != NULL) {
		conf[timer].callback(conf[timer].params);
	}
}

TIMER_A_ISR(void)
{
	isr_handler(TIMEOUT_TIMER_A);
}

TIMER_B_ISR(void)
{
	isr_handler(TIMEOUT_TIMER_B);
}
