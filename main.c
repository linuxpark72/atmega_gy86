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
#include "ms5611.h"
#include "hmc5883l.h"

#define I2C_SCL PD0
#define I2C_SDA PD1

int main(void) {
	stdout = &OUTPUT;

	printf("Main \r\n");
	uart_init();
	i2c_init();

	ms5611_test();
	hmc5883l_test();

#if defined(MPU6050_TEST)
	printf("TODO :-) \r\n");
#endif 
	printf("exit\r\n");

	return 0;
}
