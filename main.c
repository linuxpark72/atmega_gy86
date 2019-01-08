/*
 * main.c 
 * Created: Sun Dec 10 KST 2017
 * Author : jeho park<linuxpark@gmail.com>
 */ 
#undef F_CPU
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "i2c.h"
#include "uart.h"
#include "mpu6050.h"

int main(void) {
	stdout = &OUTPUT;

	i2c_init();
	uart_init();

	ms5611_test();
	hmc5883l_test();
	qmc5883_test();
	mpu6050_test();

	return 0;
}
