/*
 *  ms5611.c
 *
 * Desc    : The MS5611-01BA is a high resolution altimeter sensors.
 * Created : Sun Dec 10 KST 2017
 * Author  : jeho park<linuxpark@gmail.com>
 */
#define F_CPU 16000000L
#include <util/delay.h>
#include <math.h>
#include "i2c.h"
#include "ms5611.h"
#include "uart.h"

/********************************************************
 @brief send command using I2C hardware interface

 @return 0: fail, 1: success;
********************************************************/
static unsigned int ms5611_i2c_send(char cmd)
{
	return i2c_send(MS5611_ADDR, cmd);
}

//********************************************************
//! @brief send reset sequence
//!
//! @return none
//********************************************************
unsigned int ms5611_reset(void)
{
	int ret;
	
	ret = ms5611_i2c_send(CMD_RESET);       // send reset sequence
	if (!ret)
		return ret;

	_delay_ms(3);                           // wait for the reset sequence timing
	return 1;
}

//********************************************************
//! @brief Read calibration coefficients
//!
//! @return coefficient
//********************************************************
uint16_t cmd_prom(int coef_num)
{
	int ret;
	uint16_t hi_coef;
	uint16_t lo_coef;
	uint16_t coef = 0;

	ms5611_i2c_send(CMD_PROM_RD + coef_num * 2);
	ret = i2c_start(MS5611_ADDR_R);
	if (ret) {
		//failed to issue start condition, possibly no device found
		i2c_stop();
	} else {
		//issuing start condition ok, device accessible
		hi_coef = i2c_readAck(); // read MSB and acknowledge
		//rC = 256 * ret;
		coef = (hi_coef << 8);
		lo_coef = i2c_readNak(); // read LSB and not acknowledge
		coef |= lo_coef;
		i2c_stop();
	}
	return coef;
}

//********************************************************
//! @brief preform adc conversion
//!
//! @return 24bit result
//********************************************************
unsigned long cmd_adc(char cmd) 
{
	unsigned int ret;
	unsigned long temp=0;

	ms5611_i2c_send(CMD_ADC_CONV+cmd);
	switch (cmd & 0x0f)
	{
		// send conversion command
		// wait necessary conversion time
		case CMD_ADC_256 : _delay_us(900); break;
		case CMD_ADC_512 : _delay_ms(3); break;
		case CMD_ADC_1024: _delay_ms(4); break;
		case CMD_ADC_2048: _delay_ms(6); break;
		case CMD_ADC_4096: _delay_ms(10); break;
	}
	ms5611_i2c_send(CMD_ADC_READ);
	ret = i2c_start(MS5611_ADDR_R); // set device address and read mode if ( ret )
	if (ret) {
		//failed to issue start condition, possibly no device found
		i2c_stop();
	} else {
		//issuing start condition ok, device accessible
		ret = i2c_readAck();       // read MSB and acknowledge
		temp = 65536 * ret;

		ret = i2c_readAck();       // read byte and acknowledge
		temp = temp + 256 * ret;
		ret = i2c_readNak();       // read LSB and not acknowledge
		temp = temp + ret;
		i2c_stop();                // send stop condition
	}
	return temp;
}

//********************************************************
//! @brief calculate the CRC code
//!
//! @return crc code
//********************************************************
unsigned char ms5611_crc4(unsigned int C[])
{
	int cnt;                            // simple counter
	unsigned int n_rem;                 // crc reminder
	unsigned int crc_read;              // original value of the crc
	unsigned char  n_bit;

	n_rem = 0x00;
	crc_read = C[7];                //save read CRC
	C[7] = (0xFF00 & (C[7])); //CRC byte is replaced by 0
	for (cnt = 0; cnt < 16; cnt++)      // operation is performed on bytes
	{
		if (cnt%2==1) 
			n_rem ^= (unsigned short) ((C[cnt>>1]) & 0x00FF);
		else 
			n_rem ^= (unsigned short) (C[cnt>>1]>>8);

		for (n_bit = 8; n_bit > 0; n_bit--)
		{
			if (n_rem & (0x8000)) {
				n_rem = (n_rem << 1) ^ 0x3000;
			} else {
				n_rem = (n_rem << 1);
			}
		}
	}

	n_rem = (0x000F & (n_rem >> 12)); // final 4-bit reminder is CRC code
	printf("-----> CRC(0x%x)\r\n", n_rem);
	C[7] = crc_read;      // restore the crc_read to its original place
	return (n_rem ^ 0x0);
}

int ms5611_test() {
#if defined(MS5611_TEST)
	//unsigned int C[8]; // calibration coefficients
	uint16_t C[8]; // calibration coefficients
	//unsigned int C[8] = {0x3132, 0x3334, 0x3536, 0x3738, 0x3940, 0x4142, 0x4344, 0x4500}; // calibration coefficients
	unsigned long D1; // ADC value of the pressure conversion
	unsigned long D2; // ADC value of the temperature conversion
	double P;               // compensated pressure value
	double T;               // compensated temperature value
	double dT; // difference between actual and measured temperature
	double OFF; // offset at actual temperature
	double SENS; // sensitivity at actual temperature
	unsigned char n_crc;  // crc value of the prom
	unsigned int ret;
	int i;

	D1 = D2 = 0;
	printf("\n%s initialized...\r\n", __FUNCTION__);
	ret = ms5611_reset();              // reset IC
	if (!ret) {
		printf("failed to ms5611_reset(): %d\r\n", ret);
		return 0;
	}

	// read coefficients
	for (i=0;i<8;i++) { 
	  C[i] = cmd_prom(i);
	}
	
	C[7] = 0xFF00 & C[7];
	n_crc = ms5611_crc4(C);  /* calculate the CRC */

	for(;;) {

		D2 = cmd_adc(CMD_ADC_D2+CMD_ADC_256);      // read D2
		D1 = cmd_adc(CMD_ADC_D1+CMD_ADC_256);      // read D1
		
		// calcualte 1st order pressure and
		// temperature (MS5607 1st order algorithm)
		dT = D2 - C[5] * pow(2, 8);
		OFF = C[2] * pow(2, 17) + dT * C[4]/pow(2,6);
		SENS = C[1] * pow(2, 16) + dT * C[3]/pow(2,7);

		T = (2000 + (dT * C[6])/pow(2,23))/100;
		P = (((D1*SENS)/pow(2,21) -OFF)/ pow(2,15))/100;

		printf("C0[%d], C1[%d], C2[%d], C3[%d], C4[%d], C5[%d], C6[%d], C7[%d]\r\n"
				">>> D1(%ld), D2(%ld), CRC(%d), dT(%5.2f), Temp(%5.2f), Press(%.f mbar)\r\n",
				C[0], C[1], C[2], C[3],	C[4], C[5], C[6], C[7], 
				D1, D2, n_crc, dT, T,  P);
		_delay_ms(1000);
	}

	printf("%s exit...\r\n", __FUNCTION__);
#endif
	return 1;
}
