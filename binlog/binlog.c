/*
 * binlog.c
 *
 *  Created on: 7. 4. 2015
 *      Author: ondra
 */

#include <binlog.h>

#include <stddef.h>

#define START_OF_TEXT 2

static bool (*send)(uint8_t ch) = NULL;
static void (*err)() = NULL;

void binlog_init(bool (*send_func)(uint8_t ch), void (*err_func)())
{
	send = send_func;
	err = err_func;
}

void binlog_log(uint8_t log_code, uint8_t *arg_buf, uint8_t arg_buf_len)
{
	if (send == NULL || err == NULL) {
		return;
	}

	if (!send(START_OF_TEXT)) {
		err();
		return;
	}

	if (!send(log_code)) {
		err();
		return;
	}

	if (arg_buf_len > 0) {
		if (!send(arg_buf_len)) {
			err();
			return;
		}

		for (uint8_t i = 0; i < arg_buf_len; i++) {
			if (!send(arg_buf[i])) {
				err();
				return;
			}
		}
	}
}
