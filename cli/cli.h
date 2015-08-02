/**
 * @file
 *
 * simple command line interface for MourOS.
 *
 */

#ifndef MOUROS_CLI_H_
#define MOUROS_CLI_H_

#include <stdbool.h>
#include <stdint.h>

#include <mouros/buffer.h>

typedef int (*cli_cmd_main_function)(buffer_t *rx_buf, buffer_t *tx_buf, int argc, char *argv[]);

void cli_init(buffer_t *rx_buf, buffer_t *tx_buf);

void cli_loop(uint32_t sleep_ticks);

bool cli_register_command(const char *name, cli_cmd_main_function cmd_func);
bool cli_deregister_command(const char *name);


#endif /* MOUROS_CLI_H_ */
