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

int main(void) {
	unsigned int n_prom[8]; // calibration coefficients
	double P;               // compensated pressure value
	double T;               // compensated temperature value
	unsigned char n_crc;  // crc value of the prom
	stdout = &OUTPUT;
	unsigned int ret;

	uart_init();
	i2c_init();

	dprintf("\ninitialized...\n");
	ret = ms5611_reset();              // reset IC
	if (!ret) {
		dprintf("failed to ms5611_reset(): %d\n", ret);
		return 0;
	}

	ret = ms5611_get_coeffs(n_prom);   // get coefficient 
	if (!ret) {
		dprintf("failed to ms5611_get_coeffs(): %d\n", ret);
		return 0;
	}

	n_crc = ms5611_cal_crc4(n_prom);  // calculate the CRC

	for(;;) {
		// calculate P, T with the coefficients
		ret = ms5611_get_pt(n_prom, &P, &T);
		if (!ret) {
			dprintf("failed to ms5611_get_pt(): %d\n", ret);
			continue;
		}
		
		dprintf("\n0[%d], 1[%d], 2[%d], 3[%d], 4[%d], 5[%d], 6[%d], 7[%d],"
				" CRC(0x%x)\nTemperature(%d), Pressure(%d mbar)\n\n",
				n_prom[0], n_prom[1], n_prom[2], n_prom[3], 
				n_prom[4], n_prom[5], n_prom[6], n_prom[7], 
				n_crc, (signed int)T,  (signed int)P);
		_delay_ms(1000);
	}

	dprintf("exit...\n");
	return 0;
}
