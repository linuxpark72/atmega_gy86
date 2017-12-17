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
	struct coeff_ms5611 ms_coe;
	uint32_t d1, d2;
	uint32_t count = 0;
	
	stdout = &OUTPUT;
	stdin = &INPUT;

	uart_init();
	i2c_init();
	sei();
	dprint("initialized...\n\r");

	_delay_ms(500);
	ret = ms5611_reset();
	if (ret) {
		dprint("Failed ms5611_reset()\n\r");
		goto exit;
	} else {
		dprint("success on ms5611_reset()\n\r");
	}

	while(1) {
		
		d1 = d2 = 0;
		ms_coe.c1 = ms_coe.c2 = ms_coe.c3 = ms_coe.c4 = ms_coe.c5 = ms_coe.c6 = 0;
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

		dprint("%ld: D1(%ld), D2(%ld), c1(%d), c2(%d) c3(%d), c4(%d), c5(%d) c6(%d)\n\r", 
			   count++, d1, d2, ms_coe.c1, ms_coe.c2, ms_coe.c3, ms_coe.c4, ms_coe.c5, ms_coe.c6);
		_delay_ms(1000);
	};

exit:
	dprint("exit...\n\r");
}
