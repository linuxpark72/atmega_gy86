/*
 * file    : timer.c
 *
 * Desc    : timer lib
 * Created : 2019. 01. 15. (화) 14:24:05 KST
 * Author  : jeho park<linuxpark@gmail.com>
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
/* prescaler 64 -> 1/16M x 64 x 256(8bit timer) -> 1m, 24us*/
#define MILLIS_INC_PER_OVF  1
#define MICROS_INC_PER_OVF  24

static volatile tm_t tm = {0, 0};

ISR(TIMER0_OVF_vect) {
	unsigned long millis = tm.millis;
	int micros = tm.micros;

	millis += MILLIS_INC_PER_OVF;
	micros += MICROS_INC_PER_OVF;

	millis += (micros / 1000);
	micros = micros % 1000;

	tm.millis = millis;
	tm.micros = micros;
}

int timer0(tm_t *t) {
	uint8_t oldsreg = SREG;

	if (!t)
		return -1;

	cli();
	t->millis = tm.millis;
	t->micros = tm.micros;

	SREG = oldsreg;
	return 0;
}

float timer0_cal(tm_t *c, tm_t *p) {
	float millis;
	float micros;

	millis = c->millis - p->millis;
	micros = c->micros - p->micros;
	if (micros < 0) {
		micros += 1000;
		--millis;
	}

	return millis + micros/1000;
}

void timer0_update(tm_t *c, tm_t *p) {
	p->millis = c->millis;
	p->micros = c->micros;
}

void init_timer0() {
	TCCR0 |= (1 << CS02);
	TIMSK |= (1 << TOIE0);

	sei();
}
