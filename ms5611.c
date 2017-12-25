/*
 *  ms5611.c
 *
 * Desc    : The MS5611-01BA is a high resolution altimeter sensors.
 * Created : Sun Dec 10 KST 2017
 * Author  : jeho park<linuxpark@gmail.com>
 */
#include "i2c.h"
#include "ms5611.h"
#include "uart.h"

uint16_t fc[6];

/*************************************************************************
 reset ms5611 

 Return:  0 means success, 1 means failure
*************************************************************************/
static unsigned char _ms5611_reset(void)
{
	int ret;

	i2c_start_wait(MS5611_I2C_ADDR + I2C_WRITE);
	if ((ret = i2c_write(MS5611_RESET)))
		return ret;
		
	i2c_stop();

	return 0;
}

/*************************************************************************
 get six coefficients and then later calculate to get calibrated 
 two digital values (pressure and temperature).

 coefficient: 16bits so two i2c data be received. 
 one with ack, the other no ack but stop

 Return:  0 means success, 1 write fail, 2 not accessible 
*************************************************************************/
static unsigned char _ms5611_get_coeff(void) 
{
	uint8_t i;
	uint8_t addr;

	/* from cc1 (16bits) to cc6 */
	for (i = 1; i < 7; i++) {
		/* Figure 11: I2C Command to read memory address= ? */
		i2c_start_wait(MS5611_I2C_ADDR + I2C_WRITE);
		addr = i << 1;
		if (i2c_write(MS5611_PROM_READ + addr)) {
			return 1;
		}
		i2c_stop();

		/* Figure 12: I2C answer from MS5611-01BA */
		i2c_start_wait(MS5611_I2C_ADDR + I2C_READ);
		fc[i-1] = i2c_readAck();
		fc[i-1] <<= 8;
		fc[i-1] |= i2c_readNak();
		i2c_stop();
	}

	return 0;
}

/*************************************************************************
 get one conversed value which is 24bits 
 0xXY = X is typ and 0x40(D1) or 0x50(D2)
        Y is OSR and 0x0, 0x2 ~ 0x8

 Return:  0 means success, 1 means failure
*************************************************************************/
static unsigned char __ms5611_get_digit(uint8_t n, uint8_t r, uint32_t *digit) 
{
	uint8_t d;

	/* Figure 13: I2C Command to initiate a pressure conversion */
	i2c_start_wait(MS5611_I2C_ADDR + I2C_WRITE);
	if (i2c_write(n + r)) { /* typ = D1(0x40) or D2(0x50) */
		return 1;
	}
	i2c_stop();

	/* Figure 14: I2C ADC read sequence  */
	i2c_start_wait(MS5611_I2C_ADDR + I2C_WRITE);
	if (i2c_write(MS5611_ADC_READ)) {
		return 1;
	}
	i2c_stop();

	/* Figure 15: I2C answer from MS5611-01BA */
	i2c_start_wait(MS5611_I2C_ADDR + I2C_READ);
	d = i2c_readAck(); /* data 23 ~ 16 */
	*digit = d;
	d = i2c_readAck();
	*digit <<= 8;
	*digit |= d;
	d = i2c_readNak();
	*digit <<= 8;
	*digit |= d;

	i2c_stop();

	return 0;
}

/*************************************************************************
 get two digital pressure and temperature. each is 24bits 
 but these values have to be calibrated to get proper values.

Input: type is D1 or D2 (MS5611_CONV_D1 or MS5611_CONV_D2)
       res is resolution (from MS5611_OSR_256 to MS5611_OSR_4096)

Return:  0 means success, 1 means failure
*************************************************************************/
static unsigned char _ms5611_get_digits(uint8_t res, uint32_t *d1, uint32_t *d2) 
{
	unsigned char ret;

	if ((ret = __ms5611_get_digit(MS5611_CONV_D1, res, d1)))
		return ret;

	if ((ret = __ms5611_get_digit(MS5611_CONV_D2, res, d2)))
		return ret;

	return 0;
}

/* strolen from https://github.com/jarzebski/Arduino-MS5611/blob/master/MS5611.cpp */
static unsigned char 
_ms5611_read_pressure(int compensate, 
					  uint32_t *digit_press, 
					  uint32_t *digit_temp, int32_t *pressure) {
    int32_t dt = *digit_temp - (uint32_t)fc[4] * 256;

    int64_t off = (int64_t)fc[1] * 65536 + (int64_t)fc[3] * dt / 128;
    int64_t sens = (int64_t)fc[0] * 32768 + (int64_t)fc[2] * dt / 256;
	int32_t temp;
	int64_t off2;
	int64_t sens2;

    if (compensate) {
    
		temp = 2000 + ((int64_t) dt * fc[5]) / 8388608;
		off2 = 0;
		sens2 = 0;

		if (temp < 2000) {
			off2 = 5 * ((temp - 2000) * (temp - 2000)) / 2;
			sens2 = 5 * ((temp - 2000) * (temp - 2000)) / 4;
		}

		if (temp < -1500) {
			off2 = off2 + 7 * ((temp + 1500) * (temp + 1500));
			sens2 = sens2 + 11 * ((temp + 1500) * (temp + 1500)) / 2;
		}

		off = off - off2;
		sens = sens - sens2;
	}

	*pressure = (*digit_press * sens / 2097152 - off) / 32768;
	return 0;
}

static unsigned char 
_ms5611_read_temperature(int compensate, 
						 uint32_t *digit_temp, int32_t *temperature) {
	int32_t dt = *digit_temp - (uint32_t)fc[4] * 256;
	int32_t temp = 2000 + ((int64_t) dt * fc[5]) / 8388608;
	int32_t temp2 = 0;

	if (compensate) {
		if (temp < 2000) {
			temp2 = (dt * dt) / 2147483648;  /* 2147483648 <- (2 << 30) */
		}
	}

	temp = temp - temp2;
	*temperature = (int32_t) ((double)temp/100);
	return 0;
}

/*************************************************************************
 ms5611_init

 Return:  0 means success, 1 means failure
*************************************************************************/
unsigned char ms5611_init(void) {
	
	return _ms5611_reset();
}

/*************************************************************************
Input: type is D1 or D2 (MS5611_CONV_D1 or MS5611_CONV_D2)
	   d1: pressure
       d2: temperature
       res is resolution (from MS5611_OSR_256 to MS5611_OSR_4096)

Return:  0:  means success, 
         1:  _ms5611_get_coeff/i2c_write fail 
*************************************************************************/
unsigned char ms5611_get_pressure(uint8_t res, 
								  int32_t *pressure, int32_t *temperature) {
	/* 5611 datasheet, 8pages */
	unsigned char ret;
	int compensate = 1; 
	uint32_t digit_press, digit_temp;

	fc[0] = fc[1] = fc[2] = fc[3] = fc[4] = fc[5] = 0;
	dprintf("Before> 0(%d), 1(%d), 2(%d), 3(%d) 4(%d), 5(%d)\n\r",fc[0], fc[1], fc[2], fc[3], fc[4], fc[5]); 
	ret =  _ms5611_get_coeff();
	if (ret) {
		return ret;
	}
	dprintf("After> 0(%d), 1(%d), 2(%d), 3(%d) 4(%d), 5(%d)\n\r",fc[0], fc[1], fc[2], fc[3], fc[4], fc[5]); 

	ret = _ms5611_get_digits(res, &digit_press, &digit_temp);
	if (ret) {
		return ret;
	}
	dprintf("raw press(%ld), raw temp(%ld)\n\r", digit_press, digit_temp);

	ret = _ms5611_read_pressure(compensate, &digit_press, &digit_temp, pressure);
	if (ret) {
		return ret;
	}
	
	ret = _ms5611_read_temperature(compensate, &digit_temp, temperature);
	if (ret) {
		return ret;
	}
	dprintf("pressure(%ld), temperature(%ld)\n\r", *pressure, *temperature);

	return 0;
}
