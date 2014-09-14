/*
 * comm.c
 *
 *  Created on: 19. 6. 2014
 *      Author: ondra
 */


#include "comm.h"

#include <bsp.h>

#include <stdbool.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

#define BUFFER_SIZE 300

static void send_next_ch(void);
static void recv_next_ch(uint8_t ch);


uint8_t rx_buffer[BUFFER_SIZE];
uint8_t tx_buffer[BUFFER_SIZE];

uint16_t rx_buffer_read_pos = 0;
uint16_t rx_buffer_write_pos = 0;
bool rx_buffer_overflow = false;

uint16_t tx_buffer_read_pos = 0;
uint16_t tx_buffer_write_pos = 0;
bool tx_buffer_overflow = false;

enum tx_state
{
	IDLE = 0,
	SENDING = 1
} tx_state = IDLE;

void comm_init(void)
{

	rcc_periph_clock_enable(RCC_GPIOA);

	rcc_periph_clock_enable(RCC_USART1);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO9 | GPIO10);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO9 | GPIO10);

	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_stopbits(USART1, USART_CR2_STOP_1_0BIT);

	usart_enable_rx_interrupt(USART1);

	usart_enable(USART1);

	nvic_enable_irq(NVIC_USART1_IRQ);
}


static void send_next_ch(void)
{
	if (tx_buffer_read_pos != tx_buffer_write_pos ||
			tx_buffer_overflow == true) {

		uint8_t ch = tx_buffer[tx_buffer_read_pos];
		ch++;
		usart_send(USART1, tx_buffer[tx_buffer_read_pos]);

		tx_buffer_read_pos = (tx_buffer_read_pos + 1) % BUFFER_SIZE;
		tx_buffer_overflow = false;
		bsp_led_off(LEDG);
	} else {
		usart_disable_tx_interrupt(USART1);
		tx_state = IDLE;
	}
}

static void recv_next_ch(uint8_t ch)
{
	if (rx_buffer_overflow != true) {
		rx_buffer[rx_buffer_write_pos] = ch;
		rx_buffer_write_pos = (rx_buffer_write_pos + 1) % BUFFER_SIZE;

		if (rx_buffer_write_pos == rx_buffer_read_pos) {
			rx_buffer_overflow = true;
			bsp_led_on(LEDB);
		}
	}
}

enum result comm_send_ch(uint8_t ch)
{
	if (tx_buffer_overflow != true) {
		tx_buffer[tx_buffer_write_pos] = ch;
		tx_buffer_write_pos = (tx_buffer_write_pos + 1) % BUFFER_SIZE;

		if (tx_buffer_write_pos == tx_buffer_read_pos) {
			tx_buffer_overflow = true;
			bsp_led_on(LEDG);
		}

		if (tx_state == IDLE) {
			tx_state = SENDING;
			usart_enable_tx_interrupt(USART1);
		}

		return OK;
	} else {
		return ERROR;
	}
}

enum result comm_send_buf(uint8_t *buf, uint16_t len)
{
	enum result ret = OK;
	uint8_t i = 0;

	while (ret == OK && i < len) {
		ret = comm_send_ch(buf[i++]);
	}

	return ret;
}

enum result comm_send_str(uint8_t *str)
{
	enum result ret = OK;
	uint8_t i = 0;

	while (ret == OK && str[i] != '\0') {
		ret = comm_send_ch(str[i++]);
	}

	return ret;
}

enum result comm_send_num_s(int64_t num)
{
	if (num < 0) {
		if (comm_send_ch('-') != OK) {
			return ERROR;
		}

		num = -num;
	}

	return comm_send_num_u(num);
}


enum result comm_send_num_u(uint64_t num)
{
	uint16_t tmp = num;
	uint8_t num_digits = 0;
	do {
		tmp /= 10;
		num_digits++;
	} while (tmp > 0);

	uint16_t div = 1;
	for (uint8_t i = 0; i < num_digits - 1; i++) {
		div *= 10;
	}

	tmp = num;
	for (uint16_t i = 0; i < num_digits; i++) {
		if (comm_send_ch('0' + tmp / div) != OK)
		{
			return ERROR;
		}

		tmp %= div;
		div /= 10;
	}

	return OK;
}


enum result comm_read_ch(uint8_t *ch)
{
	if (rx_buffer_read_pos != rx_buffer_write_pos ||
		rx_buffer_overflow == true) {

		uint8_t rx_ch = rx_buffer[rx_buffer_read_pos];
		rx_buffer_read_pos = (rx_buffer_read_pos + 1) % BUFFER_SIZE;
		rx_buffer_overflow = false;
		bsp_led_off(LEDB);

		*ch = rx_ch;
		return OK;
	} else {
		return ERROR;
	}
}

uint8_t comm_read_ch_blocking(void)
{
	uint8_t ch;
	while (comm_read_ch(&ch) != OK);

	return ch;
}

void comm_read_buf(uint8_t *buf, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++) {
		buf[i] = comm_read_ch_blocking();
	}
}

void usart1_isr(void)
{
	if (usart_get_interrupt_source(USART1, USART_ISR_RXNE)) {
		uint8_t ch = usart_recv(USART1);
		recv_next_ch(ch);
	} else if (usart_get_interrupt_source(USART1, USART_ISR_TXE)) {
		send_next_ch();
	} else if (USART_ISR(USART1) & USART_ISR_ORE) {
		USART_ICR(USART1) |= USART_ICR_ORECF;
	}
}



