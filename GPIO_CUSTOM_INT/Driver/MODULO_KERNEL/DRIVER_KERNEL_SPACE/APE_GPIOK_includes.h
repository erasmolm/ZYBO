/**
  ******************************************************************************
  * @file    APE_GPIOK_defines.h
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V1.0
  * @date    08-Luglio-2017
  *	@addtogroup DRIVER
  * @{
  * @addtogroup KERNEL
  * @{
  ******************************************************************************
  */

#ifndef APE_GPIOK_INCLUDES_H
#define APE_GPIOK_INCLUDES_H

#define EN_BLOC_READ        1<<0
#define EN_NONBLOC_READ     1<<1

#define EN_BLOC_WRITE       1<<0
#define EN_NONBLOC_WRITE    1<<1

#define LED_NIBBLE_OFFSET	4

#define APE_DATA_REG		0	/*!< offset registro dato*/
#define APE_DIR_REG			4 	/*!< offset registro direzione (W)*/
#define APE_IERR_REG		8 	/*!< offset registro enable interrupt su rising edge*/
#define APE_IERF_REG		12	/*!< offset registro enable interrupt su falling edge*/
#define APE_ICRISR_REG		16	/*!< offset registro controllo interrupt (W) / stato interrupt (R)*/

#define CURRENT_POINTER_OFF 0

static int APE_GPIOK_setDIR(unsigned long mask);
static int APE_GPIOK_writeIER(unsigned long mask);
static int APE_GPIOK_clearISR(unsigned long mask);

#endif /*APE_GPIOK_INCLUDES_H*/


/**@}*/
/**@}*/
