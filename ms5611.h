/*
 *  ms5611.h
 *
 * Desc    : The MS5611-01BA is a high resolution altimeter sensors.
 * Created : Sun Dec 10 KST 2017
 * Author  : jeho park<linuxpark@gmail.com>
 */
#ifndef _MS5611_
#define _MS5611_

/* b1110 11Cx, c: complementary value of the pin CSB */
#define MS5611_I2C_ADDR  0XEE

/* command */
  /* reset */
#define MS5611_RESET     0x1E
  /* convert D1 (ADC) */
#define MS5611_CONV_D1   0x40
  /* convert D2 (ADC) */
#define MS5611_CONV_D2   0x50

/* OSR : resolution ? */
#define MS5611_OSR_256   0x0
#define MS5611_OSR_512   0x2
#define MS5611_OSR_1024  0x4
#define MS5611_OSR_2048  0x6
#define MS5611_OSR_4096  0x8

/* ADC read */
#define MS5611_ADC_READ  0x0
/* PROM read  from 0xA0 ~ 0xAE */
#define MS5611_PROM_READ 0xA0 /* or AD2 + AD1 + AD0 + 0 */ 

struct coeff_ms5611 {
	uint16_t c1;
	uint16_t c2;
	uint16_t c3;
	uint16_t c4;
	uint16_t c5;
	uint16_t c6;
};

/**
 @brief Issues reset command 

 @param    none
 @retval   0   device accessible
 @retval   1   failed to access device
 */
extern unsigned char ms5611_init(void);

/**
 @brief get digital values. one is pressure and the other is temperature.

 @param:    
           res:  resolution (from MS5611_OSR_256 to MS5611_OSR_4096)
           d1: pressure
           d2: temperature

 @retval   0   device accessible
 @retval   1   failed to access device
 */
extern unsigned char ms5611_get_pressure(uint8_t res, int32_t *d1, int32_t *d2);

#endif /* _MS5611_ */
