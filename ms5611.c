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
//! @brief Read calibration coefficients
//!
//! @return coefficient
//********************************************************
static unsigned int cmd_prom(char coef_num)
{
	unsigned int ret;
	unsigned int rC = 0;

	ms5611_i2c_send(CMD_PROM_RD+coef_num*2);
	ret = i2c_start(MS5611_ADDR_R);
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

//********************************************************
//! @brief calculate the CRC code
//!
//! @return crc code
//********************************************************
unsigned char ms5611_crc4(unsigned int n_prom[])
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
	printf("-----> CRC(0x%x)\r\n", n_rem);
	n_prom[7] = crc_read;      // restore the crc_read to its original place
	return (n_rem ^ 0x0);
}

int ms5611_test() {
#if defined(MS5611_TEST)
	//unsigned int n_prom[8]; // calibration coefficients
	unsigned int n_prom[8]; // calibration coefficients
	//unsigned int n_prom[] = {0x3132,0x3334,0x3536,0x3738,0x3940,0x4142,0x4344,0x4500};
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

	printf("\n%s initialized...\r\n", __FUNCTION__);
	ret = ms5611_reset();              // reset IC
	if (!ret) {
		printf("failed to ms5611_reset(): %d\r\n", ret);
		return 0;
	}
#if 0
	for (i=0;i<8;i++){ n_prom[i] = cmd_prom(i);} // read coefficients
#endif
	n_crc = ms5611_crc4(n_prom);  /* calculate the CRC */

	for(;;) {

		D2 = cmd_adc(CMD_ADC_D2+CMD_ADC_4096);      // read D2
		D1 = cmd_adc(CMD_ADC_D1+CMD_ADC_4096);      // read D1
		
		// calcualte 1st order pressure and
		// temperature (MS5607 1st order algorithm)
		dT = D2 - ((uint64_t)n_prom[5] << 8);
		OFF = n_prom[2] * pow(2,17) + dT * n_prom[4]/pow(2,6);
		SENS = n_prom[1] * pow(2,16) + dT * n_prom[3]/pow(2,7);

		T = (2000 + (dT * n_prom[6])/pow(2,23))/100;
		P = (((D1*SENS)/pow(2,21) -OFF)/pow(2,15))/100;

		//printf("n0[%x], n1[%x], n2[%x], n3[%x] n4[%x], n5[%x], n6[%x], n7[%x]\r\n"
		printf("n0[%d], n1[%d], n2[%d], n3[%d] n4[%d], n5[%d], n6[%d], n7[%d]\r\n"
				">>> CRC(%d), dT(%5.2f), Temp(%5.2f), Press(%5.2f mbar)\r\n",
				n_prom[0], n_prom[1], n_prom[2], n_prom[3], 
				n_prom[4], n_prom[5], n_prom[6], n_prom[7], 
				n_crc, dT, T,  P);
		_delay_ms(1000);
	}

	printf("%s exit...\r\n", __FUNCTION__);
#endif
	return 1;
}
