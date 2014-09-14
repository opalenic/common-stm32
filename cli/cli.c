/*
 * cli.c
 *
 *  Created on: 7. 9. 2014
 *      Author: ondra
 */



#include <string.h>
#include <limits.h>

#include "cli.h"
#include "comm.h"
#include "common.h"

#define MAX_CL_LENGTH 40

#define MAX_COMMANDS 10

#define MAX_ESC_SEQ_LENGTH 5

// -----------------------------------------------------------------------------

enum char_type {
	PRINTABLE,
	DELETE,
	BACKSPACE,
	ENTER,
	ESCAPE,
	UNKNOWN
};


LIST_HEAD(cli_char_list_head, character);
struct cli_char_list_head free_char_list = LIST_HEAD_INITIALIZER();
struct cli_char_list_head command_char_list = LIST_HEAD_INITIALIZER();

struct character char_storage[MAX_CL_LENGTH];

struct character *cursor_after = NULL;

// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------

struct command {
	LIST_ENTRY(command) list_el;
	const char *name;
	void (*cmd_func)(struct character *first_opt);
};

LIST_HEAD(cmd_list_head, command);
struct cmd_list_head free_cmd_list = LIST_HEAD_INITIALIZER();
struct cmd_list_head cmd_list = LIST_HEAD_INITIALIZER();

struct command command_storage[MAX_COMMANDS];

// -----------------------------------------------------------------------------


enum term_color {
	BLACK = '0',
	RED = '1',
	GREEN = '2',
	YELLOW = '3',
	BLUE = '4',
	MAGENTA = '5',
	CYAN = '6',
	WHITE = '7'
};



static struct character *char_take(void);
static void char_give(struct character *ch);
static struct command *cmd_take(void);
static void cmd_give(struct command *cmd);


static enum char_type get_char_type(uint8_t ch);
static void set_term_color(enum term_color color);

static void process_left_key(void);
static void process_right_key(void);

static void process_char_insert(uint8_t ch);
static void process_char_delete(void);
static void process_char_backspace(void);
static void process_enter_pressed(void);
static void process_escape_seq(void);
static void output_prompt(void);


static void execute_command(void);

static void help_command(struct character *first_opt);



void cli_init(void)
{
	// initialize the list of free chars
	for (uint16_t i = 0; i < MAX_CL_LENGTH; i++) {
		LIST_INSERT_HEAD(&free_char_list, &char_storage[i], list_el);
	}

	// initialize the list of free command structs
	for (uint16_t i = 0; i < MAX_COMMANDS; i++) {
		LIST_INSERT_HEAD(&free_cmd_list, &command_storage[i], list_el);
	}
}


void cli_loop(void)
{
	set_term_color(BLUE);

	while (true) {
		uint8_t new_ch = comm_read_ch_blocking();

		switch (get_char_type(new_ch)) {
		case PRINTABLE:
			process_char_insert(new_ch);

			break;
		case DELETE:
			process_char_delete();

			break;
		case BACKSPACE:
			process_char_backspace();

			break;
		case ENTER:
			process_enter_pressed();

			break;
		case ESCAPE:
			process_escape_seq();

			break;
		default:
			break;
		}

		output_prompt();
	}
}

bool cli_register_command(const char *name, void (*cmd_func)(struct character *first_opt))
{
	struct command *cmd = LIST_FIRST(&cmd_list);
	while (cmd != NULL) {
		if (strcmp(name, cmd->name) == 0) {
			return false;
		}

		cmd = LIST_NEXT(cmd, list_el);
	}

	cmd = cmd_take();

	if (cmd == NULL) {
		return false;
	}

	cmd->name = name;
	cmd->cmd_func = cmd_func;

	LIST_INSERT_HEAD(&cmd_list, cmd, list_el);

	return true;
}

bool cli_deregister_command(const char *name)
{
	struct command *cmd = LIST_FIRST(&cmd_list);

	while (cmd != NULL) {
		if (strcmp(cmd->name, name) == 0) {
			LIST_REMOVE(cmd, list_el);
			cmd_give(cmd);
			return true;
		}
	}

	return false;
}

int8_t cli_match_tok(struct character *first, const char* str)
{
	struct character *cl_char = first;

	uint8_t str_pos = 0;
	uint8_t ch = str[str_pos++];

	// Iterate until end of either string or until no match.
	while (cl_char != NULL && ch != '\0' && cl_char->ch == ch) {

		cl_char = LIST_NEXT(cl_char, list_el);
		ch = str[str_pos++];
	}

	// A successful match is when we reached the end of 'str' and are either
	// at the end of the string headed by 'first', or the next character is
	// a space (everything after a space are options).
	if (ch == '\0' && (cl_char == NULL || cl_char->ch == ' ')) {
		return 0;

	// This means, that we got to the end of 'str', but there are still
	// chars left in 'first'.
	} else if (ch == '\0') {
		return 1;
	// We didn't get to the end of 'str'.
	} else {
		return -1;
	}
}

bool cli_parse_tok_int16(struct character *first, int16_t *res)
{
//	if (first == NULL) {
//		return false;
//	}
//
//	struct character *curr = first;
//	bool is_negative = false;
//	if (first == '-') {
//		is_negative = true;
//		curr = LIST_NEXT(first, list_el);
//	}
//
//	uint16_t res;
//	cli_parse_tok_uint16(curr, res);

	return false;
}

bool cli_parse_tok_uint16(struct character *first, uint16_t *res)
{
	if (first == NULL) {
		return false;
	}

	uint32_t tmp = 0;

	struct character *curr = first;

	while (curr != NULL) {
		if (tmp > UINT16_MAX) {
			return false;
		}

		if (curr->ch < '0' || curr->ch > '9') {
			return false;
		}

		uint8_t digit = curr->ch - '0';
		tmp = tmp * 10 + digit;

		curr = LIST_NEXT(curr, list_el);
	}

	*res = tmp;
	return true;
}


struct character *cli_next_tok(struct character *first)
{
	struct character *ch = first;

	while (ch != NULL && ch->ch != ' ') {

		ch = LIST_NEXT(ch, list_el);
	}

	if (ch != NULL) {
		do {
			ch = LIST_NEXT(ch, list_el);
		} while (ch != NULL && ch->ch == ' ');
	}

	return ch;

}





// ----------------------------------------------------------
// MEMORY MANAGEMENT
// ----------------------------------------------------------
static struct character *char_take(void)
{
	struct character *ch = LIST_FIRST(&free_char_list);

	if (ch != NULL) {
		LIST_REMOVE(ch, list_el);
	}
	return ch;
}

static void char_give(struct character *ch)
{
	if (ch == NULL) {
		return;
	}

	LIST_INSERT_HEAD(&free_char_list, ch, list_el);
}

static struct command *cmd_take(void)
{
	struct command *cmd = LIST_FIRST(&free_cmd_list);

	if (cmd != NULL) {
		LIST_REMOVE(cmd, list_el);
	}
	return cmd;
}

static void cmd_give(struct command *cmd)
{
	if (cmd == NULL) {
		return;
	}

	LIST_INSERT_HEAD(&free_cmd_list, cmd, list_el);
}


// ----------------------------------------------------------
// CLI I/O
// ----------------------------------------------------------
static enum char_type get_char_type(uint8_t ch)
{
	if ((ch >= 32 && ch <= 126) || ch == '\t') {
		return PRINTABLE;
	} else if (ch == 127 || ch == '\004') {
		return DELETE;
	} else if (ch == 8) {
		return BACKSPACE;
	} else if (ch == '\n' || ch == '\r') {
		return ENTER;
	} else if (ch == '\e') {
		return ESCAPE;
	} else {
		return UNKNOWN;
	}
}

static void set_term_color(enum term_color color)
{
	comm_send_str("\e[3");
	comm_send_ch(color);
	comm_send_ch('m');
}

static void process_left_key(void)
{
	if (cursor_after != NULL) {
		cursor_after = LIST_PREV(cursor_after, &command_char_list, character, list_el);
	}
}

static void process_right_key(void)
{
	struct character *next = LIST_NEXT(cursor_after, list_el);
	if (next != NULL) {
		cursor_after = next;
	}
}

static void process_char_insert(uint8_t ch)
{
	struct character *cl_char = char_take();
	if (cl_char == NULL) {
		return;
	}

	cl_char->ch = ch;

	if (cursor_after != NULL) {
		LIST_INSERT_AFTER(cursor_after, cl_char, list_el);
	} else {
		LIST_INSERT_HEAD(&command_char_list, cl_char, list_el);
	}

	cursor_after = cl_char;

}

static void process_char_delete(void)
{
	struct character *cl_char = LIST_NEXT(cursor_after, list_el);
	if (cl_char != NULL) {
		LIST_REMOVE(cl_char, list_el);
		char_give(cl_char);
	}

}

static void process_char_backspace(void)
{
	if (cursor_after != NULL) {
		struct character *cl_char = LIST_PREV(cursor_after,
				&command_char_list, character, list_el);

		LIST_REMOVE(cursor_after, list_el);
		char_give(cursor_after);

		cursor_after = cl_char;
	}
}

static void process_enter_pressed(void)
{
	comm_send_str("\r\n");

	set_term_color(YELLOW);

	execute_command();

	set_term_color(BLUE);

	struct character *cl_char = LIST_FIRST(&command_char_list);
	while (cl_char != NULL) {
		struct character *next = LIST_NEXT(cl_char, list_el);
		LIST_REMOVE(cl_char, list_el);
		char_give(cl_char);

		cl_char = next;
	}

	cursor_after = NULL;
}

static void process_escape_seq(void)
{

	struct escape_sequence {
		const char *code;
		void (*action)(void);
		bool no_match;
	};

	struct escape_sequence sequences[] = {
		{
			.code = "[D",
			.action = process_left_key,
			.no_match = false
		},
		{
			.code = "[C",
			.action = process_right_key,
			.no_match = false
		}
	};

	uint8_t possible_matches = ARRAY_SIZE(sequences);

	uint8_t esc_chars[MAX_ESC_SEQ_LENGTH + 1];
	uint8_t esc_char_num = 0;

	while (possible_matches != 0 && esc_char_num < MAX_ESC_SEQ_LENGTH) {

		uint8_t new_ch = comm_read_ch_blocking();
		if (get_char_type(new_ch) != PRINTABLE) {
			break;
		}

		esc_chars[esc_char_num++] = new_ch;
		esc_chars[esc_char_num] = '\0';


		for (uint8_t i = 0; i < ARRAY_SIZE(sequences); i++) {

			if (sequences[i].no_match) {
				continue;
			}

			int8_t match = strcmp(esc_chars, sequences[i].code);

			if (match == 0) {
				sequences[i].action();

				return;
			} else if (match > 0) {
				sequences[i].no_match = true;
				possible_matches--;
			}
		}
	}

	for (uint8_t i = 0; i < esc_char_num; i++) {
		process_char_insert(esc_chars[i]);
	}
}

static void output_prompt(void)
{
	comm_send_str("\e[2K"); // clears current line
	comm_send_str("\r -> ");

	uint16_t total_len = 0;
	uint16_t cursor_pos = 0;
	// output the chars in command_char_list
	for (struct character *curr = LIST_FIRST(&command_char_list);
			curr != NULL;
			curr = LIST_NEXT(curr, list_el)) {
		comm_send_ch(curr->ch);

		total_len++;

		if (cursor_after == curr) {
			cursor_pos = total_len;
		}

	}

	uint16_t shift = total_len - cursor_pos;

	if (shift > 0) {
		comm_send_str("\e[");
		comm_send_num_u(shift);
		comm_send_ch('D');
	}
}


// ----------------------------------------------------------
// Command processing
// ----------------------------------------------------------
static void execute_command(void)
{
	struct character *first_char = LIST_FIRST(&command_char_list);
	struct character *first_opt = cli_next_tok(first_char);

	struct command *cmd = LIST_FIRST(&cmd_list);

	while (cmd != NULL) {

		if (cli_match_tok(first_char, cmd->name) == 0) {
			cmd->cmd_func(first_opt);
			return;
		}

		cmd = LIST_NEXT(cmd, list_el);
	}

	if (cli_match_tok(first_char, "help") == 0) {
		help_command(first_opt);
		return;
	}

	comm_send_str("Command not found.\r\n");
}


static void help_command(struct character *first_opt)
{
	comm_send_str("The currently registered commands are:\r\n");

	struct command *cmd = LIST_FIRST(&cmd_list);
	while (cmd != NULL) {

		comm_send_ch('\t');
		comm_send_str(cmd->name);
		comm_send_str("\r\n");

		cmd = LIST_NEXT(cmd, list_el);
	}

	comm_send_str("\r\n");
}
