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

extern void APE_GPIOK_setDIR(APE_GPIOK_dev_t *devp, unsigned long mask){
	iowrite32(mask, devp -> base_addr + (APE_DIR_REG/4));
}

extern void APE_GPIOK_writeIER(APE_GPIOK_dev_t *devp, unsigned long mask){
	iowrite32(mask, devp -> base_addr + (APE_IERR_REG/4));
	iowrite32(mask, devp -> base_addr + (APE_IERF_REG/4));
}

extern void APE_GPIOK_clearISR(APE_GPIOK_dev_t *devp, unsigned long mask){
	iowrite32(mask, devp -> base_addr + (APE_ICRISR_REG/4));
}

extern void APE_GPIOK_saveInt(APE_GPIOK_dev_t* devp,IER_status_t *status){
	status->IERR_status = ioread32(devp -> base_addr + (APE_IERR_REG/4));
	status->IERF_status = ioread32(devp -> base_addr + (APE_IERF_REG/4));
}

extern void APE_GPIOK_restoreInt(APE_GPIOK_dev_t* devp,IER_status_t *status){
	iowrite32(status->IERR_status,devp->base_addr + (APE_IERR_REG/4));
	iowrite32(status->IERF_status,devp->base_addr + (APE_IERF_REG/4));
}

/**@}*/
/**@}*/
