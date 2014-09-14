/*
 * comm.h
 *
 *  Created on: 19. 6. 2014
 *      Author: ondra
 */

#ifndef COMM_H_
#define COMM_H_

#include "common.h"
#include <stdint.h>

//#define comm_send_num(num) _Generic((num), \
//		int8_t: comm_send_num_s, \
//		int16_t: comm_send_num_s, \
//		int32_t: comm_send_num_s, \
//		int64_t: comm_send_num_s, \
//		uint8_t: comm_send_num_u, \
//		uint16_t: comm_send_num_u, \
//		uint32_t: comm_send_num_u, \
//		uint64_t: comm_send_num_u)

void comm_init(void);

enum result comm_send_ch(uint8_t ch);
enum result comm_send_buf(uint8_t *buf, uint16_t len);
enum result comm_send_str(uint8_t *str);
enum result comm_send_num_s(int64_t num);
enum result comm_send_num_u(uint64_t num);

enum result comm_read_ch(uint8_t *ch);
uint8_t comm_read_ch_blocking(void);

void comm_read_buf(uint8_t *buf, uint16_t len);
uint16_t comm_read_new(uint8_t *buf, uint16_t len);


#endif /* COMM_H_ */
