/*
 *  uart.c
 * 
 *  Created: 2017. 12.  7
 *  Author : jeho park<linuxpark@gmail.com>
 */
#include <avr/io.h>
#include <stdio.h>

static void uart_transmit(char data);
static unsigned char uart_receive(void);

FILE OUTPUT = FDEV_SETUP_STREAM(uart_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, uart_receive, _FDEV_SETUP_READ);

static void uart_transmit(char data) {
        while(!(UCSR0A & (1 << UDRE0)) );
        UDR0 = data;
}

static unsigned char uart_receive(void) {
        while(!(UCSR0A & (1<<RXC0)));
        return UDR0;
}

void uart_init(void) {
        UBRR0H = 0x00;
        /* UBRR0L = 207; 9600 */
        UBRR0L = 16;    /* 115200 */

        UCSR0A |= _BV(U2X0);     /* 2배속 모드 */
        /* 비동기, 8비트 데이터, 패리티 없음, 1비트 정지 비트 모드 */
        UCSR0C |= 0x06;

        UCSR0B |= _BV(RXEN0);
        UCSR0B |= _BV(TXEN0);
}
