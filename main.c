/*
 * main.c 
 * Created: Sun Dec 10 KST 2017
 * Author : jeho park<linuxpark@gmail.com>
 */ 
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "i2c.h"
#include "uart.h"
#include "ms5611.h"
#include "hmc5883l.h"

int main(void) {
	stdout = &OUTPUT;

	uart_init();
	i2c_init();

#if defined(MS5611_TEST)
	ms5611_test();
#endif

#if defined(HMC5883L_TEST)
	hmc5883l_test();
#endif 

#if defined(MPU6050_TEST)
	printf("TODO :-) \n");
#endif 

	return 0;
}
