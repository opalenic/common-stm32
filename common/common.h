/*
 * common.h
 *
 *  Created on: 2. 9. 2014
 *      Author: ondra
 */

#ifndef COMMON_H_
#define COMMON_H_

enum result {
	OK = 0,
	ERROR = 1
};

#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x[0])))


#endif /* COMMON_H_ */
