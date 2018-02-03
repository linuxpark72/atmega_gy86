/*
 *  hmc5883l.h
 *
 * Desc    : 3-Axis Digital compass IC driver for atmega128
 * Created : 2018. 2.  4
 * Author  : jeho park<linuxpark@gmail.com>
 */
#ifndef _HMC5883L_
#define _HMC5883L_

#define HMC5883L_ADDR  0x3C
#define HMC5883L_WRITE 0x3C
#define HMC5883L_READ  0x3D

#define ADDR_W         HMC5883L_ADDR + I2C_WRITE
#define ADDR_R         HMC5883L_ADDR + I2C_READ

#define HMC5883L_ADDR_CRA     0x00
#define HMC5883L_ADDR_CRB     0x01
#define HMC5883L_ADDR_MR      0x02
#define HMC5883L_ADDR_DOXMSBR 0x03  /* Data Output X MSB Register */
#define HMC5883L_ADDR_DOXLSBR 0x04  /* Data Output X LSB Register */
#define HMC5883L_ADDR_DOZMSBR 0x05  /* Data Output Z MSB Register */
#define HMC5883L_ADDR_DOXLSBR 0x06  /* Data Output Z LSB Register */
#define HMC5883L_ADDR_DOYMSBR 0x07  /* Data Output Y MSB Register */
#define HMC5883L_ADDR_DOYLSBR 0x08  /* Data Output Y LSB Register */
#define HMC5883L_ADDR_SR      0x09  /* Status Register */
#define HMC5883L_ADDR_IRA     0x0A  /* Identification Regster A, must be 0b01001000 = 0x48 */
#define HMC5883L_ADDR_IRB     0x0B  /* Identification Regster B, must be 0b00110100 = 0x34 */
#define HMC5883L_ADDR_IRC     0x0C  /* Identification Regster C, must be 0b00110011 = 0x33 */

/* 
  CRA (Configuration Registar A
  
  [ CRA7      |  CRA6  |  CRA5  |  CRA4  |  CRA3  |  CRA2  |  CRA1  | CRA0   ]
  [Reserved(0)| MA1(0) | MA0(0) | DO2(1) | DO1(0) | DO0(0) | MS1(0) | MS0(0) ]

*/

/* 
  MA 
 
  MAx: number of samples averaged(1 ~ 8) per measurement output
  00 : 1 (default),
  01 : 2
  10 : 4
  11 : 8
*/
#define CRA_MA_SHIFT (1 << 5)
#define CRA_MA_1	0x0
#define CRA_MA_2	0x1
#define CRA_MA_4	0x2
#define CRA_MA_8	0x3

/* 
  DOxMSB: (Data Output Rate) (Hz)

*/
#define CRA_DOR_SHIFT	0x2
#define CRA_DOR_0	0x0     /* 0.75 (Hz) */
#define CRA_DOR_1P5 0x1     /* 1.5       */
#define CRA_DOR_3	0x2     /* 3         */
#define CRA_DOR_7P5	0x3     /* 7.5       */
#define CRA_DOR_15	0x4     /* 15        */
#define CRA_DOR_30	0x5     /* 30        */
#define CRA_DOR_75	0x6     /* 75        */

/*
  MM(Measurement Mode)
*/
#define CRA_MM_SHIFT  0x0
#define CRA_MM_NORMAL 0x0  /* Normal Mlode */
#define CRA_MM_PB     0x1  /* Positive Bias config for X, Y and Z axes */
#define CRA_MM_NB     0x2  /* Negative Bias config for X, Y and Z axes */
#endif /* _HMC5883L_ */
