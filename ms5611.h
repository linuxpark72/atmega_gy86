/*
 *  ms5611.h
 *
 * Desc    : The MS5611-01BA is a high resolution altimeter sensors.
 * Created : Sun Dec 10 KST 2017
 * Author  : jeho park<linuxpark@gmail.com>
 */
#ifndef _MS5611_
#define _MS5611_

#define TRUE            1
#define FALSE           0

#define MS5611_ADDR  0XEE
#if 0
#define ADDR_W       MS5611_ADDR + I2C_WRITE
#define ADDR_R       MS5611_ADDR + I2C_READ
#endif
#define MS5611_ADDR_W MS5611_ADDR + I2C_WRITE
#define MS5611_ADDR_R MS5611_ADDR + I2C_READ

#define CMD_RESET    0x1E
#define CMD_ADC_READ 0x00
#define CMD_ADC_CONV 0x40
#define CMD_ADC_D1   0x00
#define CMD_ADC_D2   0x10
#define CMD_ADC_256  0x00
#define CMD_ADC_512  0x02
#define CMD_ADC_1024 0x04
#define CMD_ADC_2048 0x06
#define CMD_ADC_4096 0x08
#define CMD_PROM_RD  0xA0

/**
 @brief reset ms5611, no needed delay

 @param  null

 @return  0: fail, 1: success
 */
extern unsigned int ms5611_reset(void);

/**
 @brief get 6 coefficients from ms5611 prom 

 @param n_prom : unsigned int n_prom[8] : coefficients called by ref

 @return  0: fail, 1: success;
 */
extern unsigned int ms5611_get_coeffs(unsigned int n_prom[]);

/**
 @brief calculate CRC value with the coefficient you send

 @param n_prom : unsigned int n_prom[8] : coefficients which 
                 was gotten from  ms5611_get_coefficient()

 @return  0: fail, 1: success;
 */
extern unsigned char ms5611_cal_crc4(unsigned int n_prom[]);

/**
 @brief calculate P, T with the coefficients 

 @param n_prom : unsigned int C[8] : coefficients
        p : pressure calculated 
        t : temperature calculated 

 @return  0: fail, 1: success;
 */
extern unsigned int ms5611_get_pt(unsigned int n_prom[], double *p, double *t);

extern int ms5611_test();
#endif /* _MS5611_ */
