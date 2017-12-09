/*
 *  ms5611.c
 *
 * Desc    : The MS5611-01BA is a high resolution altimeter sensors.
 * Created : Sun Dec 10 KST 2017
 * Author  : jeho park<linuxpark@gmail.com>
 */

#include "i2c.h"
#include "ms5611.h"

/*************************************************************************
 reset ms5611 

 Return:  0 means success, 1 means failure
*************************************************************************/
unsigned char 5611_reset(void) {
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
unsigned char 5611_get_cc(struct 5611_cc_tab *cc) {
	u_int8_t i;
	u_int8_t addr;
	u_int16_t c;
	u_int16_t *p = (u_int16_t *)cc;

	/* from cc1 (16bits) to cc6 */
	for (i = 1; i < 7; i++, c = 0) {
		/* Figure 11: I2C Command to read memory address= ? */
		i2c_start_wait(MS5611_I2C_ADDR + I2C_WRITE);
		addr = i << 1;
		if (i2c_write(MS5611_PROM_READ + addr)) {
			return 1;
		}
		i2c_stop();

		/* Figure 12: I2C answer from MS5611-01BA */
		i2c_start_wait(MS5611_I2C_ADDR + I2C_READ);
		c = i2c_readAck();
		c = c << 8;
		c |= i2c_readNak();
		i2c_stop();
		*p++ = c;
	}

	return 0;
}

/*************************************************************************
 get one conversed value which is 24bits 
 0xXY = X is typ and 0x40(D1) or 0x50(D2)
        Y is OSR and 0x0, 0x2 ~ 0x8

 Return:  0 means success, 1 means failure
*************************************************************************/

static unsigned char _5611_get_ds(u_int8_t n, u_int8_t r, u_int32_t *digit) {
	u_int8_t d;

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
unsigned char 5611_get_ds(u_int8_t res, u_int32_t *d1, u_int32_t *d2) {
	unsigned char ret;

	if ((ret = _5611_get_ds(MS5611_CONV_D1, res, d1))) 
		return ret;

	if ((ret = _5611_get_ds(MS5611_CONV_D2, res, d2))) 
		return ret;

	return 0;
}

#endif /* _MS5611_ */
