/**
  ******************************************************************************
  * @file    APE_GPIOK_lib.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V1.0
  * @date    13-Luglio-2017
  * @brief	File di libreria per il modulo KERNEL.
  *	@addtogroup DRIVER
  * @{
  * @addtogroup KERNEL
  * @{
  ******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "APE_GPIOK_includes.h"

/**
  * @brief	Scrive sul registro DIR il valore di una maschera.
  *	@param	devp puntatore alla struttura del device.
  *	@param	mask maschera da scrivere sul registro.
  *	@retval	None
  */
extern void APE_GPIOK_setDIR(APE_GPIOK_dev_t *devp, unsigned long mask){
	iowrite32(mask, devp -> base_addr + (APE_DIR_REG/4));
}

/**
  * @brief	Scrive sui registri IERR e IERF il valore di una maschera.
  *	@param	devp puntatore alla struttura del device.
  *	@param	mask maschera da scrivere sui registri.
  *	@retval	None
  */
extern void APE_GPIOK_writeIER(APE_GPIOK_dev_t *devp, unsigned long mask){
	iowrite32(mask, devp -> base_addr + (APE_IERR_REG/4));
	iowrite32(mask, devp -> base_addr + (APE_IERF_REG/4));
}

/**
  * @brief	Azzera le interrupt specificate da mask.
  *	@param	devp puntatore alla struttura del device.
  *	@param	mask maschera delle interrupt da cancellare.
  *	@retval	None
  */
extern void APE_GPIOK_clearISR(APE_GPIOK_dev_t *devp, unsigned long mask){
	iowrite32(mask, devp -> base_addr + (APE_ICRISR_REG/4));
}

/**
  * @brief	Salva lo stato dei registri IERR e IERF sull'apposita struttura dati.
  *	@param	devp puntatore alla struttura del device.
  *	@param	status puntatore alla struttura che mantiene le informazioni sui due registri.
  *	@retval	None
  */
extern void APE_GPIOK_saveInt(APE_GPIOK_dev_t* devp,IER_status_t *status){
	status->IERR_status = ioread32(devp -> base_addr + (APE_IERR_REG/4));
	status->IERF_status = ioread32(devp -> base_addr + (APE_IERF_REG/4));
}

/**
  * @brief	Ripristina i valori precedentemente salvati con APE_GPIOK_saveInt.
  *	@param	devp puntatore alla struttura del device.
  *	@param	status puntatore alla struttura che mantiene le informazioni sui due registri.
  *	@retval	None
  */
extern void APE_GPIOK_restoreInt(APE_GPIOK_dev_t* devp,IER_status_t *status){
	iowrite32(status->IERR_status,devp->base_addr + (APE_IERR_REG/4));
	iowrite32(status->IERF_status,devp->base_addr + (APE_IERF_REG/4));
}

/**@}*/
/**@}*/
