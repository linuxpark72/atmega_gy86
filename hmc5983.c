/*
 *  hmc5983.c
 *
 * Desc    : 3-Axis Digital compass IC driver.
 * Created : 2018. 2.  4
 * Author  : jeho park<linuxpark@gmail.com>
 *
 */
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include "i2c.h"
#include "uart.h"
#include "hmc5983.h"
//#include "mpu6050.h"
#ifdef _PROFILE_FREG_
#include "timer.h"
#endif

uint8_t hmc5983_readreg(uint8_t reg) {
	uint8_t data;

	i2c_start(HMC5983_ADDR + I2C_WRITE);
	i2c_write(reg);

	i2c_rep_start(HMC5983_READ);
	data = i2c_readNak();
	i2c_stop();
	return data;
}

void hmc5983_writereg(uint8_t reg, uint8_t data) {

	i2c_start(HMC5983_ADDR + I2C_WRITE);
	i2c_write(reg);
	i2c_write((uint8_t) data);
	i2c_stop();
}

int hmc5983_get_headangle(int *angle, int *x, int *z, int *y) {
	double Heading;

	i2c_start(HMC5983_ADDR + I2C_WRITE);
	i2c_write(HMC5983_REG_DX_H);
	i2c_rep_start(HMC5983_READ);
	/* Read 16 bit x,y,z value (2's complement form) */
	*x = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
	*z = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
	*y = (((int)i2c_readAck()<<8) | (int)i2c_readNak());
	i2c_stop();

	Heading = atan2((double)*y, (double)*x) + Declination;
	if (Heading>2*PI)	/* Due to declination check for >360 degree */
		Heading = Heading - 2*PI;
	if (Heading<0)		/* Check for sign */
		Heading = Heading + 2*PI;
	*angle = (Heading* 180 / PI);/* Convert into angle and return */

	return 0;
}

void hmc5983_init() {
	/* #sample, output rate, measurement mode */
	hmc5983_writereg(HMC5983_REG_CONFIG_A, 0x70);
	/* set device gain */
	hmc5983_writereg(HMC5983_REG_CONFIG_B, 0xA0);
	/* set operating mode */
	hmc5983_writereg(HMC5983_REG_MODE, 0x00);
}
#if 0
int hmc5983_isready(void) {
	uint8_t data;
	
	data = hmc5983_readreg(HMC5983_REG_SR);
	return data & 0x01;
}
#endif
int hmc5983_id_check() {
	uint8_t a, b, c;
	a = b = c = 0;
	
	i2c_start(HMC5983_ADDR);
	i2c_write(HMC5983_REG_IA);
	i2c_rep_start(HMC5983_READ);
	a = i2c_readAck();
	b = i2c_readAck();
	c = i2c_readNak();
	i2c_stop();
	printf("ID: 0x48 ?= 0x%x, 0x34 ?= 0x%x 0x33 ?= 0x%x\r\n", a, b, c);

	if (a != 0x48 || b != 0x34 || c != 0x33) {
		return -1;
	}

	return 0;
}

int hmc5983_test(void) {
#ifdef HMC5983_TEST
	int x = 0;
	int z = 0;
	int y = 0;
	int angle = 0;
#ifdef _PROFILE_FREG_
	tm_t    prev, current, tmp;
	float   delay, delay_sum = 0;
	int     lcnt = 0;

	DDRB |= 0x01;
	init_timer0();
	timer0(&prev);
#endif
	printf("\r\nhmc5983 testing...\r\n");

	/* todo: delete me ? */
	//mpu6050_init();
	hmc5983_init();
	
	if (hmc5983_id_check() < 0) {
		printf("hmc5983: connection failed! \r\n");
		return -1;
	} else {
		printf("hmc5983: check done! \r\n");
	}

	_delay_ms(6);
	while(1) {
		//while(!hmc5983_isready());
		hmc5983_get_headangle(&angle, &x, &z, &y);

#ifdef _PROFILE_FREG_
		/**********************  Profiling **********************/
		PORTB = 0x0;
		timer0(&current);
		delay = timer0_cal(&current, &prev);
		delay_sum += delay;

		if (++lcnt == TIMER_LOOP_CNT) {
			/* avg: delay (ms) */
			float avg_hz = 1 / (delay_sum * TIMER_LOOP_HZ);

			/* TODO */
			printf("%4.3f(KHz) angle(%d), x(%d), y(%d), z(%d)\r\n", avg_hz, angle, x, y, z);
			/* TODO -end */
			delay_sum = 0;
			lcnt = 0;
		}
		timer0(&tmp);
		timer0_update(&tmp, &prev);
		PORTB = 0x1;
		/**********************  Profiling **********************/
#endif
		//_delay_ms(67);
	} 

	return 0;
#endif /* HMC5983_TEST */ 
}
