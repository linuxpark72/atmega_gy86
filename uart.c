/*
 *  uart.c
 * 
 *  Created: 2017. 12.  7
 *  Author : jeho park<linuxpark@gmail.com>
 */
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdio.h>
#include "uart.h"

#define BAUD_RATE      9600
#define BAUD_PRESCALE (16000000UL/16/BAUD_RATE-1)

FILE OUTPUT = FDEV_SETUP_STREAM((void *)uart_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, (void *)uart_receive, _FDEV_SETUP_READ);

void uart_transmit(char byte, FILE *stream) {
#if 0
	while(!(UCSR0A & (1 << UDRE0)) );
	UDR0 = data;
#else
	if (byte == '\n') {
		uart_transmit('\r',stream);
	}
	loop_until_bit_is_set(UCSR0A,UDRE0);
	UDR0 = byte;

#endif
}

char uart_receive(FILE *stream) {
#if 0
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
#else
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;

#endif
}

void uart_init(void) {
#if 0
	UBRR0H = 0x00;
	/* UBRR0L = 207; 9600 */
	UBRR0L = 16;    /* 115200 */

	UCSR0A |= _BV(U2X0);     /* 2배속 모드 */
	/* 비동기, 8비트 데이터, 패리티 없음, 1비트 정지 비트 모드 */
	UCSR0C |= 0x06;

	UCSR0B |= _BV(RXEN0);
	UCSR0B |= _BV(TXEN0);
#else
	UBRR0H = (BAUD_PRESCALE >> 8);
	UBRR0L = BAUD_PRESCALE;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
#endif
}
