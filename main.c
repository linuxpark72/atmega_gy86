/*
 * drone.c
 *
 * Created: 2017-05-08 오후 10:59:10
 * Author : jeho park<linuxpark@gmail.com>
 */ 
#include "drone.h"
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <math.h>  //include libm
#include "mpu6050.h"
#include "uart0.h"
#include "motor.h"
#include "rc.h"

int main(void) {
	stdout = &OUTPUT;
	stdin = &INPUT;
	double qw = 1.0f;
	double qx = 0.0f;
	double qy = 0.0f;
	double qz = 0.0f;
	double roll = 0.0f;
	double pitch = 0.0f;
	double yaw = 0.0f;

	uart0_init();
	dprint("initialized...\n\r");

	rc_init();
	motor_init();
	
	sei();

	mpu6050_init();
	_delay_ms(50);

	dprint("Initializing TOP_VAL = %d", TOP_VAL);
	motor_sync();

	/* init mpu6050 dmp processor */
	mpu6050_dmpInitialize();
	mpu6050_dmpEnable();
	_delay_ms(10);

	/* Replace with your application code */
	for(;;) {

		if(mpu6050_getQuaternionWait(&qw, &qx, &qy, &qz)) {
			mpu6050_getRollPitchYaw(qw, qx, qy, qz, &roll, &pitch, &yaw);
		}
		_delay_ms(10);

		//dprint("qw(%f) qx(%f) qy(%f) qz(%f)  roll(%f) pitch(%f) yaw(%f)",  qw, qx, qy, qz, roll, pitch, yaw);
		dprint("roll(%f) pitch(%f) yaw(%f)", (roll * 180 / 3.14159), (pitch * 180 / 3.14159), (yaw * 180 / 3.14159));
	}
}
