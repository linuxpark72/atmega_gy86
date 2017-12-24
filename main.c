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
	int ret;
	int32_t pressure, temperature;
	uint32_t count = 0;
	
	stdout = &OUTPUT;
	stdin = &INPUT;

	uart_init();
	i2c_init();
	sei();
	dprint("initialized...\n\r");

	_delay_ms(500);
	ret = ms5611_init();
	if (ret) {
		dprint("Failed ms5611_reset()\n\r");
		goto exit;
	} else {
		dprint("success on ms5611_reset()\n\r");
	}

	while(1) {
		
		ret = ms5611_get_pressure(MS5611_OSR_4096, &pressure, &temperature);
		if (ret) {
			dprint("Failed ms5611_get_pressure: %d\n\r", ret);
			goto exit;
		}

		dprint("%ld: D1(%ld), D2(%ld)\n\r", count++, pressure, temperature);
		_delay_ms(1000);
	};

exit:
	dprint("exit...\n\r");
}
