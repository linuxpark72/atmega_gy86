/*
 *  hmc5883l.c
 *
 * Desc    : 3-Axis Digital compass IC driver.
 * Created : 2018. 2.  4
 * Author  : jeho park<linuxpark@gmail.com>
 *
 */
#include <util/delay.h>
#include "i2c.h"
#include "uart.h"
#include "hmc5883l.h"

static unsigned int hmc5883l_i2c_send(unsigned char cmd) {
    return i2c_send(HMC5883L_ADDR, cmd);
}

static unsigned int hmc5883l_setup_reg(unsigned char regaddr, unsigned char val) {
	unsigned int ret;

	ret = hmc5883l_i2c_send(regaddr);
	if (!ret) {
		/* fail */
		return 0;
	}
	
	ret = i2c_write(val);
	i2c_stop();
	return ret;
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
 * success: 0b11  [
 */
static unsigned char hmc5883l_read_sr() {
	unsigned int ret;
	unsigned char status;

	ret = hmc5883l_i2c_send(HMC5883L_ADDR_SR);
	if (!ret) {
		/* fail */
		return 0;
	} 
	
	/* success */
	ret = i2c_start(HMC5883L_READ);
	if (ret) {
		i2c_stop();
		return 0;
	} else {
		status = i2c_readNak();
		i2c_stop();
	}

	return status;
}

/* 
 * fail: 0
 * success: status, ready
 */
static unsigned char hmc5883l_wait_data() {
	unsigned char status;

	do {
		status = hmc5883l_read_sr();
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

/* continuous mode */
int hmc5883l_test(void) {
#if defined(HMC5883L_TEST)
	int ret;
#if defined(HMC5883L_CC_MODE)
	int16_t x, z, y;

	printf("\n%s initialized...\n", __FUNCTION__);
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

	do {
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
#else
	/* single mode */

#endif  /* HMC5883L_CC_MODE */

#endif /* HMC5883L_TEST */
	return 1;
}
