/*
 * binlog.h
 *
 *  Created on: 7. 4. 2015
 *      Author: ondra
 */

#ifndef BINLOG_H_
#define BINLOG_H_

#include <stdint.h>
#include <stdbool.h>

void binlog_init(bool (*send_func)(uint8_t ch), void (*err_func)());
void binlog_log(uint8_t log_code, uint8_t *arg_buf, uint8_t arg_buf_len);

#endif /* BINLOG_H_ */
