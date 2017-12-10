/*
 * main.c 
 * Created: Sun Dec 10 KST 2017
 * Author : jeho park<linuxpark@gmail.com>
 */ 
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"
#include "ms5611.h"

#undef F_CPU
#define F_CPU 16000000UL

int main(void) {
	int ret;
	struct coeff_ms5611 ms_coe;
	uint32_t d1, d2;

	stdout = &OUTPUT;
	stdin = &INPUT;

	uart_init();
	sei();
	dprint("initialized...\n\r");

	_delay_ms(100);
	ret = ms5611_reset();
	if (ret) {
		dprint("Failed ms5611_reset()\n\r");
		goto exit;
	}

	while(1){

		ret = ms5611_get_coeff(&ms_coe);
		if (ret) {
			dprint("Failed ms5611_get_coeff(): %d\n\r", ret);
			goto exit;
		}

		ret = ms5611_get_ds(MS5611_OSR_4096, &d1, &d2);
		if (ret) {
			dprint("Failed ms5611_get_coeff(): %d\n\r", ret);
			goto exit;
		}

		dprint("d1(%d) d2(%d)\n\r", d1, d2);
		_delay_ms(500);
	};

exit:
	dprint("exit...\n\r");
}
