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

void timeout_init(void);
bool timeout_start(uint32_t timeout_ms, bool periodic, void (*callback)(void*), void *cb_params);
void timeout_stop(void);
bool timeout_has_elapsed(void);

#endif /* TIMEOUT_H_ */
