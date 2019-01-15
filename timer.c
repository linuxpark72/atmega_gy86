/*
 * file    : timer.c
 *
 * Desc    : timer lib
 * Created : 2019. 01. 15. (화) 14:24:05 KST
 * Author  : jeho park<linuxpark@gmail.com>
 */

#include <avr/io.h>
#include <avr/interrupt.h>

/* prescaler 64 -> 1/16M x 64 x 256(8bit timer) -> 1m, 24us*/
#define MILLIS_INC_PER_OVF  1
#define MICROS_INC_PER_OVF  24

volatile unsigned long timer0_millis = 0;
volatile int timer0_micros = 0;

ISR(TIMER0_OVF_vect) {
	unsigned long millis = timer0_millis;
	int micros = timer0_micros;

	millis += MILLIS_INC_PER_OVF;
	micros += MICROS_INC_PER_OVF;

	millis += (micros / 1000);
	micros = micros % 1000;

	timer0_millis = millis;
	timer0_micros = micros;
}

unsigned long timer0() {
	unsigned long millis;
	uint8_t oldsreg = SREG;

	cli();
	millis = timer0_millis;

	SREG = oldsreg;
	return millis;
}

void init_timer0() {
	TCCR0 |= (1 << CS02);
	TIMSK |= (1 << TOIE0);

	sei();
}
