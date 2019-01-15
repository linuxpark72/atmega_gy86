/*
 * file    : timer.h
 *
 * Desc    : 
 * Created : 2019. 01. 15. (화) 14:34:48 KST
 * Author  : jeho park<linuxpark@gmail.com>
 */

#ifndef _TIMER_
#define _TIMER_

typedef struct {
	unsigned long millis;
	int micros;
} tm_t;

extern int timer0(tm_t *t);
float timer0_cal(tm_t *c, tm_t *p);
void timer0_update(tm_t *c, tm_t *p);
extern void init_timer0();

#endif /* #define _TIMER_ */
