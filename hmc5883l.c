/*
 *  hmc5883l.c
 *
 * Desc    : 3-Axis Digital compass IC driver.
 * Created : 2018. 2.  4
 * Author  : jeho park<linuxpark@gmail.com>
 *
 */
#define F_CPU 16000000L
#include <util/delay.h>
#include "i2c.h"
#include "uart.h"
#include "hmc5883l.h"
#include <math.h>

static unsigned int hmc5883l_i2c_send(unsigned char cmd) {
    return i2c_send(HMC5883L_ADDR, cmd);
}

/* success: 1
 * fail: 0
 */
static unsigned int hmc5883l_setup_reg(unsigned char reg, unsigned char val) {
	unsigned int ret;

	ret = hmc5883l_i2c_send(reg);
	if (!ret) {
		/* fail */
		return 0;
	}
	
	ret = i2c_write(val);
	if (ret) {
		return 0;
	}
	i2c_stop();

	return 1;
}

/*          R e g i s t e r  A          */

/* # of sample per measurement output */
static unsigned int hmc5883l_setup_cra(unsigned char ma, 
									   unsigned char dor,
									   unsigned char mm) {
	unsigned char val = 0;

	val |= ((ma << CRA_MA_SHIFT) | (dor << CRA_DOR_SHIFT) | (mm << CRA_MM_SHIFT));
	return hmc5883l_setup_reg(HMC5883L_ADDR_CRA, val);
}

/*        R e g i s t e r  B          */
static unsigned int hmc5883l_setup_crb(unsigned char gn) {
	unsigned char val = 0;

	val |= (gn << CRB_GN_SHIFT);
	return hmc5883l_setup_reg(HMC5883L_ADDR_CRB, val);
}

/*      M o d e  R e g i s t e r      */
static unsigned int hmc5883l_setup_mr(unsigned char md) {
	unsigned char val = 0;

	val |= (md << MR_MD_SHIFT);
	return hmc5883l_setup_reg(HMC5883L_ADDR_MR, val);
}

/*
 * fail: 0
 * success: 0b11
 */

unsigned char hmc5883l_read(unsigned char reg) {
   unsigned char val = 0;

   i2c_start(HMC5883L_WRITE);
   i2c_write(reg);
   i2c_rep_start(HMC5883L_READ);
   val = i2c_readNak();
   i2c_stop();
   return (val);
}


static int hmc5883l_write(unsigned char reg, unsigned char value) {

	i2c_start(HMC5883L_WRITE);
	i2c_write(reg);
	i2c_write(value);
	i2c_stop();
	return 0;
}

static int hmc5883l_verify() {
	unsigned char v;
#if 0	
	if ((v = hmc5883l_read(HMC5883L_ADDR_IRA)) != IRA) {
		dprintf("failed to read IRA(%c, %d)\n", v, v);
		//return -1;
	}
	if ((v = hmc5883l_read(HMC5883L_ADDR_IRB)) != IRB) {
		dprintf("failed to read IRB(%c)\n", v);
		//return -2;
	}
	if ((v = hmc5883l_read(HMC5883L_ADDR_IRC)) != IRC) {
		dprintf("failed to read IRC(%c)\n", v);
		//return -3;
	}

	return 1;
#else
	i2c_start(0x3C);
	i2c_write(HMC5883L_ADDR_IRA);
	i2c_rep_start(HMC5883L_READ);
	v = i2c_readNak();
	i2c_stop();
	dprintf("--->%c\n", v);
	if ((int)v != IRA) {
		return -1;
	}

	dprintf("--->%c\n", v);
	return 1;

#endif
}

static int hmc5883l_init() {
	int ret;

	ret = hmc5883l_write(0x0, 0x70); /* 8 avg, 15Hz, normal op */
	if (ret)
		return ret;
	dprintf("%d ");
	ret = hmc5883l_write(0x1, 0xA0); /* G=5 */
	if (ret)
		return ret;
	dprintf("%d ");
	ret = hmc5883l_write(0x2, 0x00); /* 8 avg, 15Hz, normal op */
	if (ret)
		return ret;
	dprintf("%d ");

	return ret;
}

/* 
 * fail: 0
 * success: status, ready
 */
static unsigned char hmc5883l_wait_data() {
	unsigned char status;

	do {
		status = hmc5883l_read(HMC5883L_ADDR_SR);
		if (!status) {
			return 0;
		}

		if (status & (1 << SR_LOCK_SHIFT))  {
			/* now, locked */
			_delay_us(250);
			continue;
		}

		if (!(status & (1 << SR_RDY_SHIFT)))  {
			/* ready */
			_delay_us(250);
			continue;
		}
		
		/* now, no locked and ready to be read */
		break;

	} while (1);

	return status;
}

void Magneto_init()		/* Magneto initialize function */
{
	dprintf("\n");
	i2c_start(0x3C);	/* start and write SLA+W */
	i2c_write(0x00);	/* Write memory location address */
	i2c_write(0x70);	/* Configure register A as 8-average, 15 Hz default, normal measurement */
	i2c_write(0xA0);	/* Configure register B for gain */
	i2c_write(0x00);	/* Configure continuous measurement mode in mode register */
	i2c_stop();		/* Stop i2c */
	dprintf("\n");
}

#define PI	3.14159265359	 /* Define Pi value */
#define Declination	-0.00669 /* Define declination of location from where measurement going to be done. 
								e.g. here we have added declination from location Pune city, 
								India. we can get it from http://www.magnetic-declination.com */
int Magneto_GetHeading() {
	int x, y, z;
	double Heading;

	dprintf("\n");
	//i2c_start_wait(0x3C);	/* start and wait for acknowledgment */
	i2c_start(0x3C);	/* start and wait for acknowledgment */
	i2c_write(0x03);	/* Write memory location address */
	i2c_rep_start(0x3D);/* Generate repeat start condition with SLA+R */
	/* Read 16 bit x,y,z value (2's complement form) */
	x = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
	z = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
	y = (((int)i2c_readAck()<<8) | (int)i2c_readNak());
	i2c_stop();		/* Stop i2c */
	Heading = atan2((double)y,(double)x) + Declination;
	if (Heading>2*PI)	/* Due to declination check for >360 degree */
		Heading = Heading - 2*PI;
	if (Heading<0)		/* Check for sign */
		Heading = Heading + 2*PI;
	
	//return (Heading* 180 / PI);/* Convert into angle and return */
	dprintf("---> %3.0f \n", Heading* 180 / PI);/* Convert into angle and return */
	return 1;
}

/* continuous mode */
int hmc5883l_test(void) {
#if defined(HMC5883L_TEST)
	int ret;
	int16_t x, z, y;
#if 1

	dprintf("");
    //hmc5883l_init(); 
	Magneto_init();
	dprintf("");
	if (hmc5883l_verify() < 0) {
		return -1;
	}
	dprintf("");
	return 1;

	while(1) {
		Magneto_GetHeading();
	}

	return 1;
#endif

#if 0
	printf("\n%s initialized...\n", __FUNCTION__);
	ret = hmc5883l_init();
	if (ret) {
		printf("failed to init(%d)\n", ret);
		return ret;
	} else {
		printf("success to init\n");
	}

#else
	/* continuous-measurement mode */
	ret = hmc5883l_setup_cra(CRA_MA_8, CRA_DOR_15, CRA_MM_NORMAL);
	if (!ret) {
		printf("error hmc5883l_setup_cra\n");
		return 0;
	}

	ret = hmc5883l_setup_crb(CRB_GN_5);
	if (!ret) {
		printf("error hmc5883l_setup_crb\n");
		return 0;
	}

	ret = hmc5883l_setup_mr(MR_MD_CONTINUOUS);
	if (!ret) {
		printf("error hmc5883l_setup_mr\n");
		return 0;
	}
#endif	
	do {
		printf("wait data ...\n");
		/* 1. wait about 6ms or check SR */ 
		hmc5883l_wait_data(); 

		/* 2. 0x3D 0x06 : send cmd: which means "i will read 6 bytes" */
		ret = i2c_start(HMC5883L_READ);
		if ( ret ) {
			//failed to issue start condition, possibly no device found */
			i2c_stop();
			printf("failed to set device addr %d\n", __LINE__);
			return 0;
		} else {
			// issuing start condition ok, device accessible
			// now pointer is point to 0x03 which is address of X 
			ret = i2c_write(0x06); /* read 6 bytes */
			i2c_stop();
		}

		/* 2. read 6 bytes */
		ret = i2c_start(HMC5883L_READ);
		if (ret) {
			/* fail */
			i2c_stop();
			printf("Failed to start i2c (%d)\n", __LINE__);
			return 0;
		} else {
			x = z = y = 0;
			/* x, z, y, the size of each is 16bits */
			x = i2c_readAck();
			x <<= 8;
			x |= i2c_readAck();

			z = i2c_readAck();
			z <<= 8;
			z |= i2c_readAck();

			y = i2c_readAck();
			y <<= 8;
			y |= i2c_readNak();
			i2c_stop();
		}

		/* convert three 16-bit 2's compliment 
		https://www.programiz.com/c-programming/bitwise-operators#complement
		*/
		x = (~x + 1) * -1;
		z = (~z + 1) * -1;
		y = (~y + 1) * -1;
		printf("x(%d), y(%d), z(%d)\n", x, y, z);

		hmc5883l_i2c_send(0x3);
	} while (1);

	return 1;

#endif /* HMC5883L_TEST */
	return 1;
}
