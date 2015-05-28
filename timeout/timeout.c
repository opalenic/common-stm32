/*
 * timeout.c
 *
 *  Created on: 2. 12. 2014
 *      Author: ondra
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#include <timeout.h>
#include <common.h>


#define TIMER_A_ISR tim16_isr
#define TIMER_B_ISR tim17_isr

#define IRQ_PRIO (0x02 << 6)
#define TICKS_PER_MS 48000


struct timer_conf {
	uint32_t base;
	enum rcc_periph_clken clken;
	enum rcc_periph_rst rst;
	uint32_t irq;
	void (*callback)(void *);
	void *params;
	bool periodic;
	bool running;
};

static struct timer_conf conf[] = {
	 {
	 	.base = TIM16,
	 	.clken = RCC_TIM16,
	 	.rst = RST_TIM16,
	 	.irq = NVIC_TIM16_IRQ
	 },
	 {
	 	.base = TIM17,
	 	.clken = RCC_TIM17,
	 	.rst = RST_TIM17,
	 	.irq = NVIC_TIM17_IRQ
	 }
};


static bool valid_timer(enum timeout_timer timer)
{
	return timer == TIMEOUT_TIMER_A || timer == TIMEOUT_TIMER_B;
}

void timeout_init(void)
{
	for (int i = 0; i < ARRAY_SIZE(conf); i++) {

		rcc_periph_clock_enable(conf[i].clken);

		rcc_periph_reset_pulse(conf[i].rst);

		timer_set_mode(conf[i].base, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
		timer_set_prescaler(conf[i].base, TICKS_PER_MS - 1);

		timer_update_on_overflow(conf[i].base);

		timer_enable_irq(conf[i].base, TIM_DIER_UIE);

		nvic_enable_irq(conf[i].irq);
		nvic_set_priority(conf[i].irq, IRQ_PRIO);

		timer_generate_event(conf[i].base, TIM_EGR_UG);
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

	timer_set_counter(conf[timer].base, 0);
	timer_set_period(conf[timer].base, timeout_ms);

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
		timer_disable_counter(conf[timer].base);
	}

	if (conf[timer].callback != NULL) {
		conf[timer].callback(conf[timer].params);
	}
}

void TIMER_A_ISR(void)
{
	isr_handler(TIMEOUT_TIMER_A);
}

void TIMER_B_ISR(void)
{
	isr_handler(TIMEOUT_TIMER_B);
}
