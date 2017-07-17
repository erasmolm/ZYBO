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

/* Includes -------------------------------------------------------------------*/
#include <linux/cdev.h> /* necessario per la cdev_init */
#include <linux/of_irq.h>


#define EN_BLOC_READ        1<<0
#define EN_NONBLOC_READ     1<<1

#define APE_DATA_REG		0	/*!< offset registro dato*/
#define APE_DIR_REG			4 	/*!< offset registro direzione (W)*/
#define APE_IERR_REG		8 	/*!< offset registro enable interrupt su rising edge*/
#define APE_IERF_REG		12	/*!< offset registro enable interrupt su falling edge*/
#define APE_ICRISR_REG		16	/*!< offset registro controllo interrupt (W) / stato interrupt (R)*/

#define APE_INT_MASK		0xFFFFFFFF	/*!< maschera per abilitare tutte le interrupt*/

/**
  * @brief	Tipo struttura del device
  */
typedef struct {
	dev_t dev_num;					/*!< Major/minor number del device*/
	unsigned int size;				/*!< Dimensione della memoria da associare al device*/
	struct resource res;			/*!< Struttura della risorsa device rappresentata in memoria, contiene start e end*/

	struct cdev cdev;				/*!< Struttura char device interna al kernel*/
	unsigned long *base_addr;		/*!< Indirizzo base*/

	int irq_number;					/*!< Numero della linea di interrupt*/
	struct platform_device *op;		/*!< Puntatore alla struttura platform_device associata al device*/

	wait_queue_head_t read_queue;	/*!< Variabile condition per la read*/
	wait_queue_head_t poll_queue;	/*!< Variabile condition per la poll*/

	spinlock_t num_interrupts_sl;	/*!< Variabile lock contatore delle interrupt*/
	spinlock_t read_flag_sl;		/*!< Variabile lock per flag abilitazione lettura*/

	int num_interrupts;				/*!< Contatore delle interruzioni avvenute*/
	int read_flag;					/*!< Flag di abilitazione lettura*/

}APE_GPIOK_dev_t;

/**
  * @brief	Tipo struttura IER_status_t.
  * @details Struttura utilizzata per salvare lo stato dei registri IERR e IERF.
  */
typedef struct {
	unsigned int IERR_status;
	unsigned int IERF_status;
}IER_status_t;

/* Prototipi delle funzioni --------------------------------------------------*/
extern void APE_GPIOK_setDIR(APE_GPIOK_dev_t*, unsigned long mask);
extern void APE_GPIOK_writeIER(APE_GPIOK_dev_t*, unsigned long mask);
extern void APE_GPIOK_clearISR(APE_GPIOK_dev_t*, unsigned long mask);
extern void APE_GPIOK_saveInt(APE_GPIOK_dev_t*,IER_status_t*);
extern void APE_GPIOK_restoreInt(APE_GPIOK_dev_t*,IER_status_t*);

#endif /*APE_GPIOK_INCLUDES_H*/


/**@}*/
/**@}*/
