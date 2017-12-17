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
/* TODO : static */
unsigned char ms5611_reset(void)
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
/* TODO : static */
unsigned char ms5611_get_coeff(struct coeff_ms5611 *cc) 
{
	uint8_t i;
	uint8_t addr;
	uint16_t c;
	uint16_t *p = (uint16_t *)cc;

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
static unsigned char _ms5611_get_ds(uint8_t n, uint8_t r, uint32_t *digit) 
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
/* TODO : static */
unsigned char ms5611_get_ds(uint8_t res, uint32_t *d1, uint32_t *d2) 
{
	unsigned char ret;

	if ((ret = _ms5611_get_ds(MS5611_CONV_D1, res, d1)))
		return ret;

	if ((ret = _ms5611_get_ds(MS5611_CONV_D2, res, d2)))
		return ret;

	return 0;
}

/*************************************************************************
 ms5611_init

 Return:  0 means success, 1 means failure
*************************************************************************/
unsigned char ms5611_init(void)
{
	return ms5611_reset();
}

/*************************************************************************

Input: type is D1 or D2 (MS5611_CONV_D1 or MS5611_CONV_D2)
       res is resolution (from MS5611_OSR_256 to MS5611_OSR_4096)


Return:  0:  means success, 
         1:  _ms5611_get_coeff/i2c_write fail 
*************************************************************************/
unsigned char ms5611_get_pressure(uint8_t res, uint32_t *d1, uint32_t *d2) 
{
	/* 5611 datasheet, 8pages */
	struct coeff_ms5611 cc; 
	unsigned char ret;
#if 0
	const int32_t p_min = 10;     /*  PMIN = 10mbar  */
	const int32_t p_max = 1200;   /*  PMAX = 1200mbar */
	const int32_t t_min = -40;    /*  TMIN = -40°C  */
	const int32_t t_max = 80;     /*  TMAX = 85°C  */
#endif
	uint16_t p_sens_t1;           /*  c1, Pressure sensitivity | SENST1 */
    uint16_t p_off_t1;            /*  c2, Pressure offset | OFFT1 */
	uint16_t tcs;                 /*  c3, Temperature coefficient of pressure sensitivity | TCS */
	uint16_t tco;                 /*  c4, Temperature coefficient of pressure offset | TCO */
	uint16_t tref;                /*  c5, Reference temperature | TREF */
	uint16_t tempsens;            /*  c6, Temperature coefficient of the temperature | TEMPSENS */

	int32_t  dt;                  /*  Difference between actual and reference temperature */
	int32_t  temp;                /*  Actual temperature (-40...85°C with 0.01°C resolution) */

	int64_t  off;                 /*  Offset at actual temperature [3] */
	int64_t  sens;                /*  Sensitivity at actual temperature [4] */
	int32_t  p;                   /*  Temperature compensated pressure (10...1200mbar with 0.01mbar resolution) */

	ret =  ms5611_get_coeff(&cc);
	if (ret) {
		return ret;
	}

	p_sens_t1 = cc.c1;
	p_off_t1 = cc.c2;
	tcs = cc.c3;
	tco = cc.c4;
	tref = cc.c5;
	tempsens = cc.c6;

	ret = ms5611_get_ds(res, d1, d2);
	if (ret) {
		return ret;
	}

	/* Calculate temperature */
	dt = *d2 - tref;                  /* dT =D2-TREF =D2 -C5*28 */
	temp = 20 + dt * tempsens;        /* TEMP =20°C+dT*TEMPSENS=2000+dT *C6 /223 */

	/* Calculate temperature compensated pressure */
	off = p_off_t1 + tco * dt;        /* OFF = OFFT1 + TCO * dT = C2 * 216 + (C4 * dT ) / 27 */
	sens = p_sens_t1 + tcs * dt;      /* SENS=SENST1+TCS*dT=C1*215 +(C3*dT)/28 */
	p = *d1 * sens - off;             /* Temperature compensated pressure
										 (10...1200mbar with 0.01mbar resolution) */

	*d1 = temp;
	*d2 = p;

	/* Todo */
	/* SECOND ORDER TEMPERATURE COMPENSATION */
	return 0;
}
