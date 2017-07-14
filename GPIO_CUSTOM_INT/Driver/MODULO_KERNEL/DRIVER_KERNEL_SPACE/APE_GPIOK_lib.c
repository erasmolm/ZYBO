/**
  ******************************************************************************
  * @file    APE_GPIOK_lib.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V1.0
  * @date    08-Luglio-2017
  *	@addtogroup DRIVER
  * @{
  * @addtogroup KERNEL
  * @{
  ******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "APE_GPIOK_includes.h"

extern int APE_GPIOK_setDIR(APE_GPIOK_dev_t *APE_GPIOK_devp, unsigned long mask){
	APE_GPIOK_dev_t *devp = APE_GPIOK_devp;
	unsigned long *addr;

	addr = devp -> base_addr + (APE_DIR_REG/4);
	iowrite32(mask, addr);

	return 1;
}

extern int APE_GPIOK_writeIER(APE_GPIOK_dev_t *APE_GPIOK_devp, unsigned long mask){
	APE_GPIOK_dev_t *devp = APE_GPIOK_devp;
	unsigned long *addr;

	addr = devp -> base_addr + (APE_IERR_REG/4);
	iowrite32(mask, addr);

	addr = devp -> base_addr + (APE_IERF_REG/4);
	iowrite32(mask, addr);

	return 1;
}

extern int APE_GPIOK_clearISR(APE_GPIOK_dev_t *APE_GPIOK_devp, unsigned long mask){
	APE_GPIOK_dev_t *devp = APE_GPIOK_devp;
	unsigned long *addr;

	addr = devp -> base_addr + (APE_ICRISR_REG/4);
	iowrite32(mask, addr);

	return 1;
}

/**@}*/
/**@}*/
