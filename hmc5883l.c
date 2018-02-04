/*
 *  hmc5883l.c
 *
 * Desc    : 
 * Created : 
 * Author  : jeho park<linuxpark@gmail.com>
 */
#include "i2c.h"
#include "uart.h"
#include "hmc5883l.h"

static unsigned int hmc5883l_i2c_send(char cmd) {
    return i2c_send(HMC5883L_ADDR, cmd);
}

static int hmc5883l_setup_reg(unsigned char v) {
}

/*          R e g i s t e r  A          */

/* # of sample per measurement output */
static int hmc5883l_setup_cra(unsigned char ma, 
							  unsigned char dor,
							  unsigned char ms) {
	int ret = 1;

	/* TODO */
	hmc5883l_i2c_send(HMC5883L_ADDR_CRA);


	return ret;
}

/*        R e g i s t e r  B          */
static int hmc5883l_setup_crb(unsigned char gn) {
	int ret = 1;
	
	/* TODO */

	return ret;
}

/*          
 *   M o d e  R e g i s t e r   
 *
 *   continuous mode
 *   single mode
 *   idle mode 
 */
static int hmc5883l_setup_md(unsigned char md) {
	int ret = 1;
	
	/* TODO */

	return ret;
}

int hmc5883l_test(void) {
	int ret = 1;

	printf("\n%s initialized...\n", __FUNCTION__);
	/* continuous-measurement mode */

	
	
	/* single-measurement mode */


	printf("%s exit...\n", __FUNCTION__);
	return ret;
}
