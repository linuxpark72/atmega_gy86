/*
 *  qmc5883l.h
 *
 * Desc    : 3-Axis Digital compass IC driver.
 * Created : 2018. 12.  26
 * Author  : jeho park<linuxpark@gmail.com>
 */
#ifndef _QMC5883_
#define _QMC5883_

/*
 *   QMC5883L.h - QMC5883L library
 *   Copyright (c) 2017 e-Gizmo Mechatronix Central
 *   Rewritten by Amoree.  All right reserved.
 *   July 10,2017
 */
#define QMC5883_ADDR 0x0D//The default I2C address is 0D: 0001101


//Registers Control //0x09

#define Mode_Standby    0b00000000
#define Mode_Continuous 0b00000001

#define ODR_10Hz        0b00000000
#define ODR_50Hz        0b00000100
#define ODR_100Hz       0b00001000
#define ODR_200Hz       0b00001100

#define RNG_2G          0b00000000
#define RNG_8G          0b00010000

#define OSR_512         0b00000000
#define OSR_256         0b01000000
#define OSR_128         0b10000000
#define OSR_64          0b11000000


extern void qmc_setaddress(uint8_t addr);
extern unsigned char qmc_writereg(uint8_t Reg,uint8_t val);
extern unsigned char qmc_init();
extern unsigned char qmc_setmode(uint16_t mode,uint16_t odr,uint16_t rng,uint16_t osr);
extern unsigned char qmc_softreset();
extern unsigned char qmc_readxyz(uint16_t* x, uint16_t* y, uint16_t* z);
extern int qmc5883_test(void);

extern uint8_t qmc_address;

#endif /* _QMC5883_ */
