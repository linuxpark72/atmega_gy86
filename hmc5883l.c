/*
 *  hmc5883l.c
 *
 * Desc    : 3-Axis Digital compass IC driver.
 * Created : 2018. 2.  4
 * Author  : jeho park<linuxpark@gmail.com>
 *
 */
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

static unsigned int hmc5883l_read_sr(void) {
	unsigned int ret;

	ret = hmc5883l_i2c_send(HMC5883L_READ);
	/* TODO */
}

int hmc5883l_test(void) {
	unsigned int ret;

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

	
	/* single-measurement mode */


	printf("%s exit...\n", __FUNCTION__);
	return ret;
}
