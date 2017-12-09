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
#define MS5611_I2C_ADDR  0XEC 

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

struct {
	u_int16_t c1;
	u_int16_t c2;
	u_int16_t c3;
	u_int16_t c4;
	u_int16_t c5;
	u_int16_t c6;
} 5611_cc_tab;

/**
 @brief Issues reset command 

 @param    none
 @retval   0   device accessible
 @retval   1   failed to access device
 */
extern unsigned char 5611_reset(void);

/**
 @brief read PROM and get six coff

 @param    pointer of the 5611_cc_tab structure
 @retval   0   device accessible
 @retval   1   failed to access device
 */
extern unsigned char 5611_get_coefficient(struct 5611_cc_tab *);

/**
 @brief get digital values. one is pressure and the other is temperature.

 @param    pointer of the two digit
 @retval   0   device accessible
 @retval   1   failed to access device
 */
extern unsigned char 5611_get_ds(u_int8_t res, u_int32_t *d1, u_int32_t *d2)
#endif /* _MS5611_ */

