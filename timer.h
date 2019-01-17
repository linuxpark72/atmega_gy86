/*
 * file    : timer.h
 *
 * Desc    : 
 * Created : 2019. 01. 15. (화) 14:34:48 KST
 * Author  : jeho park<linuxpark@gmail.com>
 */

#ifndef _TIMER_
#define _TIMER_

/* prescaler 64 -> 1/16M x (64) x 256(8bit timer) -> 1m, 24us*/
//#define MILLIS_INC_PER_OVF  1
//#define MICROS_INC_PER_OVF  24
/* prescaler 128 -> 1/16M x (128) x 256(8bit timer) -> 2m, 48us*/
#define MILLIS_INC_PER_OVF  2
#define MICROS_INC_PER_OVF  48

#define TIMER_LOOP_CNT     1000
#define TIMER_LOOP_HZ      ((float)1/TIMER_LOOP_CNT)
typedef struct {
	unsigned long millis;
	int micros;
} tm_t;

extern int timer0(tm_t *t);
float timer0_cal(tm_t *c, tm_t *p);
void timer0_update(tm_t *c, tm_t *p);
extern void init_timer0();

#endif /* #define _TIMER_ */
