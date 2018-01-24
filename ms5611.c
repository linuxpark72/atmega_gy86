/*
 *  ms5611.c
 *
 * Desc    : The MS5611-01BA is a high resolution altimeter sensors.
 * Created : Sun Dec 10 KST 2017
 * Author  : jeho park<linuxpark@gmail.com>
 */
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
	unsigned int ret;
	
	ret = ms5611_i2c_send(CMD_RESET);       // send reset sequence
	if (!ret)
		return ret;

	_delay_ms(3);                           // wait for the reset sequence timing
	return 1;
}

//********************************************************
//! @brief preform adc conversion
//!
//! @return 24bit result
//********************************************************
static unsigned long cmd_adc(char cmd) 
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
	ret = i2c_start(ADDR_R); // set device address and read mode if ( ret )
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
//! @brief Read calibration coefficients
//!
//! @return coefficient
//********************************************************
static unsigned int cmd_prom(char coef_num)
{
	unsigned int ret;
	unsigned int rC = 0;

	ms5611_i2c_send(CMD_PROM_RD+coef_num*2);
	ret = i2c_start(ADDR_R);
	if (ret) {
		//failed to issue start condition, possibly no device found
		i2c_stop();
	} else {
		//issuing start condition ok, device accessible
		ret = i2c_readAck(); // read MSB and acknowledge
		rC = 256 * ret;
		ret = i2c_readNak(); // read LSB and not acknowledge
		rC = rC + ret;
		i2c_stop();
	}
	return rC;
}

unsigned int ms5611_get_coeffs(unsigned int C[]) {
	int i;

	for (i = 0; i < 8; i++) {
		C[i] = cmd_prom(i);    // read coefficients
		if (!C[i])
			return C[i];
	}

	return 1;
}

//********************************************************
//! @brief calculate the CRC code
//!
//! @return crc code
//********************************************************
unsigned char ms5611_cal_crc4(unsigned int n_prom[])
{
	int cnt;                            // simple counter
	unsigned int n_rem;                 // crc reminder
	unsigned int crc_read;              // original value of the crc
	unsigned char  n_bit;

	n_rem = 0x00;
	crc_read= n_prom[7];                //save read CRC
	n_prom[7] = (0xFF00 & (n_prom[7])); //CRC byte is replaced by 0
	for (cnt = 0; cnt < 16; cnt++)      // operation is performed on bytes
	{
		if (cnt%2==1) 
			n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
		else 
			n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);

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
	n_prom[7] = crc_read;      // restore the crc_read to its original place
	return (n_rem ^ 0x0);
}

unsigned int ms5611_get_pt(unsigned int C[], double *p, double *t) {
	unsigned long D1; // ADC value of the pressure conversion
	unsigned long D2; // ADC value of the temperature conversion
	double dT; // difference between actual and measured temperature
	double OFF; // offset at actual temperature
	double SENS; // sensitivity at actual temperature

	D1 = D2 = 0; 
	D2 = cmd_adc(CMD_ADC_D2+CMD_ADC_4096);      // read D2
	if (!D2)
		return 0;
	D1 = cmd_adc(CMD_ADC_D1+CMD_ADC_4096);      // read D1
	if (!D1)
		return 0;

	// calcualte 1st order pressure and
	// temperature (MS5607 1st order algorithm)
	//dT = D2 - C[5] * pow(2,8);
	dT = D2 - ((uint64_t) C[5] << 8);
	//OFF = C[2] * pow(2,17) + dT * C[4]/pow(2,6);
	OFF = (uint64_t)C[2] * pow(2,17) + dT * (uint64_t)C[4]/pow(2,6);
	//SENS = C[1] * pow(2,16) + dT * C[3]/pow(2,7);
	SENS = (uint64_t)C[1] * pow(2,16) + dT * (uint64_t)C[3]/pow(2,7);

	dprintf("-------------> dT: %7.4f\n", dT);
	//*t = (2000 + (dT*C[6])/pow(2,23))/100;
	*t = (2000 + (dT * (uint64_t)C[6])/pow(2,23))/100;
	//*p = (((D1*SENS)/pow(2,21) -OFF)/pow(2,15))/100;
	*p = (((D1*SENS)/pow(2,21) -OFF)/pow(2,15))/100;

	return 1;
}
