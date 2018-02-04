/*
 *  hmc5883l.h
 *
 * Desc    : 3-Axis Digital compass IC driver.
 * Created : 2018. 2.  4
 * Author  : jeho park<linuxpark@gmail.com>
 */
#ifndef _HMC5883L_
#define _HMC5883L_

#define HMC5883L_ADDR  0x3C

#define HMC5883L_WRITE 0x3C
#define HMC5883L_READ  0x3D

/*
 * Register Address (8bits)
 */
#define HMC5883L_ADDR_CRA     0x00  /* Configuration Register A */
#define HMC5883L_ADDR_CRB     0x01  /* Configuration Register B */
#define HMC5883L_ADDR_MR      0x02  /* Mode Register  */
#define HMC5883L_ADDR_DOXMSBR 0x03  /* Data Output X MSB Register */
#define HMC5883L_ADDR_DOXLSBR 0x04  /* Data Output X LSB Register */
#define HMC5883L_ADDR_DOZMSBR 0x05  /* Data Output Z MSB Register */
#define HMC5883L_ADDR_DOZLSBR 0x06  /* Data Output Z LSB Register */
#define HMC5883L_ADDR_DOYMSBR 0x07  /* Data Output Y MSB Register */
#define HMC5883L_ADDR_DOYLSBR 0x08  /* Data Output Y LSB Register */
#define HMC5883L_ADDR_SR      0x09  /* Status Register */
#define HMC5883L_ADDR_IRA     0x0A  /* Identification Regster A, 0b01001000 = 0x48 */
#define HMC5883L_ADDR_IRB     0x0B  /* Identification Regster B, 0b00110100 = 0x34 */
#define HMC5883L_ADDR_IRC     0x0C  /* Identification Regster C, 0b00110011 = 0x33 */

/********************************************************************************** 
  CRA (Configuration Registar A)
  
           [ CRA7  |  CRA6  |  CRA5  |  CRA4  |  CRA3  |  CRA2  |  CRA1  | CRA0   ]
  default  [Resv(0)| MA1(0) | MA0(0) | DO2(1) | DO1(0) | DO0(0) | MS1(0) | MS0(0) ]

            *Resv: Reserved
 ***********************************************************************************/
/* 
  MAx: number of samples averaged(1 ~ 8) per measurement output
  MA1 MA0
  0    0 : # of sample 1 (default) per output
  0    1 :             2
  1    0 :             4 ...
*/
#define CRA_MA_SHIFT  5
#define CRA_MA_1	0x0     /* default, 1 samples averaged */
#define CRA_MA_2	0x1     /* 2 samples */
#define CRA_MA_4	0x2     /* 4 */
#define CRA_MA_8	0x3     /* 8 */
#define CRA_MA_DEFAULT CRA_MA_1
/* 
  DOxMSB: (Data Output Rate) (Hz)
  DO2  DO1  DO0
    0    0    0:  0.75(Hz)
	0    0    1:  1.5(Hz) ...
  ...
*/
#define CRA_DOR_SHIFT	2
#define CRA_DOR_0	0x0     /* 0.75 (Hz) */
#define CRA_DOR_1P5 0x1     /* 1.5       */
#define CRA_DOR_3	0x2     /* 3         */
#define CRA_DOR_7P5	0x3     /* 7.5       */
#define CRA_DOR_15	0x4     /* default, 15  */
#define CRA_DOR_30	0x5     /* 30        */
#define CRA_DOR_75	0x6     /* 75        */
#define CRA_DOR_DEFAULT CRA_DOR_15
/*
  MM(Measurement Mode)
  MS1  MS0
    0    0: Normal mode ...
*/
#define CRA_MM_SHIFT    0
#define CRA_MM_NORMAL 0x0  /* Normal Mlode */
#define CRA_MM_PB     0x1  /* Positive Bias config for X, Y and Z axes */
#define CRA_MM_NB     0x2  /* Negative Bias config for X, Y and Z axes */

/********************************************************************************** 
  CRB (Configuration Registar B)
         [ CRB7   |  CRB6  |  CRB5  |  CRB4  |  CRB3  |  CRB2  |  CRB1  | CRB0  ]
 default [ GN2(0) | GN1(0) | GN0(1) |   (0)  |   (0)  |   (0)  |  (0)   | (0)   ]
 ***********************************************************************************/
/* 
  GN (Gain Configuration Bit)
  GN2  GN1  GN0
    0    0    0:  +/-0.88Ga |  1370Gain (LSb/Gauss) | 0.73Digial Resolution(mG/LSb) 
    0    0    1:   +/-1.3Ga |  1090Gain (LSb/Gauss) | 0.92Digial Resolution(mG/LSb) 
	...
	                                 LSb/Gause -> mG/LSb x 1000
*/
#define CRB_GN_SHIFT   5
#define CRB_GN_0     0x0        /* +/- 0.88 Ga */
#define CRB_GN_1     0x1        /* +/- 1.3 Ga */
#define CRB_GN_2     0x2        /* +/- 1.9 Ga */
#define CRB_GN_3     0x3        /* +/- 2.5 Ga */
#define CRB_GN_4     0x4        /* +/- 4.0 Ga */
#define CRB_GN_5     0x5        /* +/- 4.7 Ga */
#define CRB_GN_6     0x6        /* +/- 5.6 Ga */
#define CRB_GN_7     0x7        /* +/- 8.1 Ga */
#define CRB_GN_DEFAULT CRB_GN_1 

/********************************************************************************** 
  
  MR (Mode Register)

          [ MR7   |  MR6  |  MR5  |  MR4  |  MR3 |  MR2  |  MR1   |  MR0   ]
  default [ HS(0) |  (0)  |  (0)  |  (0)  |  (0) |  (0)  | MD1(0) | MD0(1) ]

 ***********************************************************************************/
#define MR_MD_SHIFT          0
#define MR_MD_CONTINUOUS   0x0   /* Continuous Measurement Mode */
#define MR_MD_SINGLE       0x1   /* Single Measurement Mode */
#define MR_MD_IDLE         0x2   /* Idle Mode */
#define MR_MD_IDLE2        0x3   /* Idle Mode */ 
#define MR_MD_DEFAULT      MR_MD_SINGLE

/********************************************************************************** 
  DXRAnB (Data Output X Registers A and B)
  
  [ DXRA7 | DXRA6  | DXRA5 | DXRA4 | DXRA3 | DXRA2 | DXRA1 | DXRA0 ]
  [  (0)  |  (0)   |  (0)  |  (0)  |  (0)  |  (0)  |  (0)  |  (0)  ]
  [ DXRB7 | DXRB6  | DXRB5 | DXRB4 | DXRB3 | DXRB2 | DXRB1 | DXRB0 ]
  [  (0)  |  (0)   |  (0)  |  (0)  |  (0)  |  (0)  |  (0)  |  (0)  ]

  DYRAnB (Data Output Y Registers A and B)
  
  [ DYRA7 | DYRA6  | DYRA5 | DYRA4 | DYRA3 | DYRA2 | DYRA1 | DYRA0 ]
  [ DYRB7 | DYRB6  | DYRB5 | DYRB4 | DYRB3 | DYRB2 | DYRB1 | DYRB0 ]

  DZRAnB (Data Output Z Registers A and B)
  
  [ DZRA7 | DZRA6  | DZRA5 | DZRA4 | DZRA3 | DZRA2 | DZRA1 | DZRA0 ]
  [ DZRB7 | DZRB6  | DZRB5 | DZRB4 | DZRB3 | DZRB2 | DZRB1 | DZRB0 ]

 *********************************************************************************/

/********************************************************************************** 
  
  SR (Status Register)

          [ SR7  |  SR6  |  SR5  |  SR4  |  SR3 |  SR2  |  SR1   |  SR0   ]
  default [  (0) |  (0)  |  (0)  |  (0)  |  (0) |  (0)  |LOCK(0) | RDY(0) ]

 ***********************************************************************************/
#define SR_LOCK_SHIFT 1
#define SR_RDY_SHIFT  0

/********************************************************************************** 
  Identification Register A

          [IRA7|IRA6|IRA5|IRA4|IRA3|IRA2|IRA1|IRA0]
  default [(0) |(1) |(0) |(0) |(1) |(0) |(0) |(0) ]

 ***********************************************************************************/
#define IRA 0x48

/********************************************************************************** 
  Identification Register B

          [IRA7|IRA6|IRA5|IRA4|IRA3|IRA2|IRA1|IRA0]
  default [(0) |(0) |(1) |(1) |(0) |(1) |(0) |(0) ]

 ***********************************************************************************/
#define IRB 0x34

/********************************************************************************** 
  Identification Register C

          [IRA7|IRA6|IRA5|IRA4|IRA3|IRA2|IRA1|IRA0]
  default [(0) |(0) |(1) |(1) |(0) |(0) |(1) |(1) ]

 ***********************************************************************************/
#define IRC 0x33

extern int hmc5883l_test(void);

#endif /* _HMC5883L_ */
