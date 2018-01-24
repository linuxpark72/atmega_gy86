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

unsigned int sm2tc(unsigned int x) {
  unsigned int m = x >> (sizeof(unsigned int) * 8 - 1);
  return (~m & x) | (((x & 0x8000) - x) & m);
}

int main(void) {
	unsigned int n_prom[8]; // calibration coefficients
	double P;               // compensated pressure value
	double T;               // compensated temperature value
	unsigned char n_crc;  // crc value of the prom
	stdout = &OUTPUT;
	unsigned int ret;

	uart_init();
	i2c_init();

	printf("\ninitialized...\n");
	ret = ms5611_reset();              // reset IC
	if (!ret) {
		printf("failed to ms5611_reset(): %d\n", ret);
		return 0;
	}

	ret = ms5611_get_coeffs(n_prom);   // get coefficient 
	if (!ret) {
		printf("failed to ms5611_get_coeffs(): %d\n", ret);
		return 0;
	}

	/* TODO: these two coefficient are always negative so i take these 2's complement */
	n_prom[1] = sm2tc(n_prom[1]);
	n_prom[2] = sm2tc(n_prom[2]);

	/* TODO: verify crc */
	n_crc = ms5611_cal_crc4(n_prom);  /* calculate the CRC */

	for(;;) {
		/*  calculate P, T with the coefficients */
		ret = ms5611_get_pt(n_prom, &P, &T);
		if (!ret) {
			printf("failed to ms5611_get_pt(): %d\n", ret);
			continue;
		}
		
		printf("- %d/%d- \n0[%d], 1[%d], 2[%d], 3[%d], 4[%d], 5[%d], 6[%d], 7[%d],"
				" CRC(0x%x)\nTemperature(%5.2f), Pressure(%7.2f mbar)\n\n",
				sizeof(int), sizeof(unsigned int),
				n_prom[0], n_prom[1], n_prom[2], n_prom[3], 
				n_prom[4], n_prom[5], n_prom[6], n_prom[7], 
				n_crc, T,  P);
		_delay_ms(1000);
	}

	printf("exit...\n");
	return 0;
}
