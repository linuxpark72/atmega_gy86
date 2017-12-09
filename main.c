/*
 * main.c 
 * Created: Sun Dec 10 KST 2017
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
