/*
 * cli.c
 *
 *  Created on: 7. 9. 2014
 *      Author: ondra
 */


#include <stddef.h> // NULL
#include <string.h> // strcmp, strtok
#include <stdio.h>  // sniprintf

#include <libopencm3/cm3/assert.h> // For cm3_assert()
#include <mouros/common.h>         // ARRAY_SIZE()
#include <mouros/tasks.h>          // os_task_sleep() & os_task_yield()

#include "cli.h" // Public function declarations.



#define MAX_CMD_LINE_LENGTH 100
#define MAX_CMDS 30
#define MAX_CMD_ARGS 10
#define MAX_ESC_SEQ_LENGTH 5

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

enum char_type {
	PRINTABLE,
	DELETE,
	BACKSPACE,
	ENTER,
	ESCAPE,
	UNKNOWN
};

struct command {
	const char *name;
	cli_cmd_main_function cmd_func;
};


static char cmd_line[MAX_CMD_LINE_LENGTH + 1]; // Extra space for terminating '\0'
static uint8_t cmd_line_len = 0;
static uint8_t cursor_pos = 0;


static struct command commands[MAX_CMDS];
static uint8_t num_commands = 0;


static buffer_t *rx = NULL;
static buffer_t *tx = NULL;


static enum char_type get_char_type(char ch);
static void set_term_color(enum term_color color);

static void process_left_key(void);
static void process_right_key(void);

static void process_char_insert(char ch);
static void process_char_delete(void);
static void process_char_backspace(void);
static void process_enter_pressed(void);
static void process_escape_seq(void);

static void output_prompt(void);

static int execute_command(void);

static void help_command(void);



void cli_init(buffer_t *rx_buf, buffer_t *tx_buf)
{
	rx = rx_buf;
	tx = tx_buf;
}


void cli_loop(uint32_t sleep_ticks)
{
	cm3_assert(rx != NULL && tx != NULL);

	set_term_color(BLUE);

	output_prompt();
	while (true) {
		char ch;
		if (!os_buffer_read_ch(rx, (uint8_t *) &ch)) {
			os_task_sleep(sleep_ticks);
			continue;
		}

		switch (get_char_type(ch)) {
		case PRINTABLE:
			process_char_insert(ch);

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

bool cli_register_command(const char *name, cli_cmd_main_function cmd_func)
{
	if (num_commands >= MAX_CMDS) {
		return false;
	}

	commands[num_commands].name = name;
	commands[num_commands].cmd_func = cmd_func;

	num_commands++;

	return true;
}

bool cli_deregister_command(const char *name)
{
	for (uint8_t i = 0; i < num_commands; i++) {
		if (strcmp(commands[i].name, name) == 0) {
			for (uint8_t j = i; j < num_commands - 1; j++) {
				commands[j] = commands[j + 1];
			}

			num_commands--;

			return true;
		}
	}

	return false;
}


// ----------------------------------------------------------
// CLI I/O
// ----------------------------------------------------------
static enum char_type get_char_type(char ch)
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
	os_buffer_write_str(tx, "\e[3");
	os_buffer_write_byte(tx, color);
	os_buffer_write_byte(tx, 'm');
}

static void process_left_key(void)
{
	if (cursor_pos == 0) {
		return;
	}

	cursor_pos--;
}

static void process_right_key(void)
{
	if (cursor_pos >= cmd_line_len) {
		return;
	}

	cursor_pos++;
}

static void process_char_insert(char ch)
{
	if (cmd_line_len >= MAX_CMD_LINE_LENGTH) {
		return;
	}

	for (uint8_t i = cursor_pos; i < cmd_line_len; i++) {
		cmd_line[i + 1] = cmd_line[i];
	}

	cmd_line[cursor_pos] = ch;

	cursor_pos++;
	cmd_line_len++;
}

static void process_char_delete(void)
{
	if (cursor_pos == cmd_line_len) {
		return;
	}

	for (uint8_t i = cursor_pos; i < cmd_line_len - 1; i++) {
		cmd_line[i] = cmd_line[i + 1];
	}

	cmd_line_len--;
}

static void process_char_backspace(void)
{
	if (cursor_pos == 0) {
		return;
	}

	for (uint8_t i = cursor_pos; i < cmd_line_len; i++) {
		cmd_line[i - 1] = cmd_line[i];
	}

	cursor_pos--;
	cmd_line_len--;
}

static void process_enter_pressed(void)
{
	os_buffer_write_str(tx, "\r\n");

	set_term_color(YELLOW);

	execute_command();

	set_term_color(BLUE);

	cmd_line_len = 0;
	cursor_pos = 0;
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

	char esc_chars[MAX_ESC_SEQ_LENGTH + 1];
	uint8_t esc_char_num = 0;

	while (possible_matches != 0 && esc_char_num < MAX_ESC_SEQ_LENGTH) {

		char new_ch;
		while (!os_buffer_read_ch(rx, (uint8_t *) &new_ch)) {
			os_task_yield();
		}

		if (get_char_type(new_ch) != PRINTABLE) {
			break;
		}

		esc_chars[esc_char_num++] = new_ch;
		esc_chars[esc_char_num] = '\0';


		for (uint8_t i = 0; i < ARRAY_SIZE(sequences); i++) {
			if (sequences[i].no_match) {
				continue;
			}

			if (strcmp(esc_chars, sequences[i].code) == 0) {
				sequences[i].action();
				return;
			}

			if (sequences[i].code[esc_char_num] == '\0') {
				sequences[i].no_match = true;
			}
		}
	}

	for (uint8_t i = 0; i < esc_char_num; i++) {
		process_char_insert(esc_chars[i]);
	}
}

static void output_prompt(void)
{
	os_buffer_write_str(tx, "\e[2K"); // clears the current line
	os_buffer_write_str(tx, "\r -> ");


	os_buffer_write(tx, (uint8_t *) cmd_line, cmd_line_len);

	int shift = cmd_line_len - cursor_pos;

	if (shift > 0) {
		char shift_buf[12];
		int buf_len = sniprintf(shift_buf, 12, "\e[%dD", shift);

		if (buf_len > 0 && buf_len < 12) {
			os_buffer_write(tx, (uint8_t *) shift_buf, (uint32_t) buf_len);
		}
	}
}


static int execute_command(void)
{
	int argc = 1;
	char *argv[MAX_CMD_ARGS];
	char *saveptr;

	cmd_line[cmd_line_len] = '\0';

	argv[0] = strtok_r(cmd_line, " \t", &saveptr);

	if (argv[0] != NULL) {
		for (uint8_t i = 0; i < num_commands; i++) {
			if (strcmp(argv[0], commands[i].name) == 0) {

				char *token = strtok_r(NULL, " \t", &saveptr);

				while(token != NULL && argc != MAX_CMD_ARGS) {
					argv[argc++] = token;
					token = strtok_r(NULL, " \t", &saveptr);
				}

				return commands[i].cmd_func(rx, tx, argc, argv);
			}
		}

		if (strcmp(argv[0], "help") == 0) {
			help_command();
			return -1;
		}
	}

	os_buffer_write_str(tx, "Command not found.\r\n");

	return -1;
}


static void help_command(void)
{
	os_buffer_write_str(tx, "The currently registered commands are:\r\n");

	for (uint8_t i = 0; i < num_commands; i++) {

		os_buffer_write_byte(tx, '\t');
		os_buffer_write_str(tx, commands[i].name);
		os_buffer_write_str(tx, "\r\n");
	}

	os_buffer_write_str(tx, "\r\n");
}
