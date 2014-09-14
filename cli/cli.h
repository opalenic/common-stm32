/*
 * cli.h
 *
 *  Created on: 7. 9. 2014
 *      Author: ondra
 */

#ifndef CLI_H_
#define CLI_H_

#include <stdbool.h>
#include <stdint.h>
#include <sys/queue.h>


struct character {
	LIST_ENTRY(character) list_el;
	uint8_t ch;
};

void cli_init(void);
void cli_loop(void);

bool cli_register_command(const char *name, void (*cmd_func)(struct character *first_opt));
bool cli_deregister_command(const char *name);

int8_t cli_match_tok(struct character *first, const char* str);
bool cli_parse_tok_int16(struct character *first, int16_t *res);
bool cli_parse_tok_uint16(struct character *first, uint16_t *res);
struct character *cli_next_tok(struct character *first);



#endif /* CLI_H_ */
