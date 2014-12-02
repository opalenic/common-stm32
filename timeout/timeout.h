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

bool timeout_start(uint32_t timeout_ms)
bool timeout_elapsed(void);

#endif /* TIMEOUT_H_ */
