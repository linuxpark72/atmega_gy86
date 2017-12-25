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

int main(void) {
	int32_t pressure, temperature;
	int ret;
	
	stdout = &OUTPUT;

	uart_init();
	i2c_init();
	sei();
	dprintf("initialized...\n\r");

	ret = ms5611_init();
	_delay_ms(100);
	if (ret) {
		dprintf("Failed ms5611_reset()\n\r");
		goto exit;
	} else {
		dprintf("success on ms5611_init()\n\r");
	}

	while(1) {
		
		ret = ms5611_get_pressure(MS5611_OSR_4096, &pressure, &temperature);
		if (ret) {
			dprintf("Failed ms5611_get_pressure: %d\n\r", ret);
			goto exit;
		}
		_delay_ms(1000);
	};

exit:
	dprintf("exit...\n\r");
}
