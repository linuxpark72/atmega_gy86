/*
 * Created: 2017년 12월  7일 목요일 01시 18분 11초 KST
 * Author : jeho park<linuxpark@gmail.com>
 */ 
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"

int main(void) {
	stdout = &OUTPUT;
	stdin = &INPUT;

	uart_init();
	dprint("initialized...\n\r");

	sei();

}
