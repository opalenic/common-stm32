/*
 * stopwatch.h
 *
 *  Created on: 21. 11. 2014
 *      Author: ondra
 */

#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include <stdint.h>

void sw_init(void);

void sw_start(void);
void sw_stop(void);

void sw_clear(void);

uint32_t sw_get_time_us(void);

#endif /* STOPWATCH_H_ */
