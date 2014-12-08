/*
 * timeout.h
 *
 *  Created on: 2. 12. 2014
 *      Author: ondra
 */

#ifndef TIMEOUT_H_
#define TIMEOUT_H_

#include <stdbool.h>
#include <stdint.h>

enum timeout_timer {
	TIMEOUT_TIMER_A = 0,
	TIMEOUT_TIMER_B,
};

void timeout_init(void);
bool timeout_start(enum timeout_timer timer, uint32_t timeout_ms, bool periodic, void (*callback)(void*), void *params);
void timeout_stop(enum timeout_timer timer);
bool timeout_timer_is_running(enum timeout_timer timer);

#endif /* TIMEOUT_H_ */
