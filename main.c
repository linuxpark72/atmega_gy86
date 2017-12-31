/*
 * main.c 
 * Created: Sun Dec 10 KST 2017
 * Author : jeho park<linuxpark@gmail.com>
 */ 
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include "i2c.h"
#include "uart.h"
#include "ms5611.h"

int main(void) {
	unsigned int n_prom[8]; // calibration coefficients
	double P;               // compensated pressure value
	double T;               // compensated temperature value
	unsigned char n_crc;  // crc value of the prom
	stdout = &OUTPUT;
	unsigned int ret;

	uart_init();
	i2c_init();
	sei();

	dprintf("initialized...\n\r");
	ret = ms5611_reset();              // reset IC
	if (!ret) {
		dprintf("failed to ms5611_reset(): %d\n\r", ret);
		return 0;
	}

	ret = ms5611_get_coeffs(n_prom);   // get coefficient 
	if (!ret) {
		dprintf("failed to ms5611_get_coeffs(): %d\n\r", ret);
		return 0;
	}

	n_crc = ms5611_cal_crc4(n_prom);  // calculate the CRC
	// TODO: check crc 
	dprintf("(%d)[(%d),(%d),(%d),(%d),(%d),(%d)](%d),c(%d)\n\r",
			n_prom[0], n_prom[1], n_prom[2], n_prom[3], 
			n_prom[4], n_prom[5], n_prom[6], n_prom[7], n_crc);

	for(;;) 
	{
		// calculate P, T with the coefficients
		ret = ms5611_get_pt(n_prom, &P, &T);
		if (!ret) {
			dprintf("failed to ms5611_get_pt(): %d\n\r", ret);
			continue;
		}
		dprintf("T(%5.2f), P(%5.2f)\n", T, P);
		_delay_ms(500);
	}

	dprintf("exit...\n\r");
	return 0;
}
