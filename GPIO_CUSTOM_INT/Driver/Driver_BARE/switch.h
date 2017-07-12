/**
  ******************************************************************************
  * @file    switch.h
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V2.0
  * @date    01-Luglio-2017
  * @brief   Questo file definisce le funzioni di gestione degli switch
  *
  *	@addtogroup LIB_OBJECTS
  * @{
  * @addtogroup SWITCH
  * @{
  * @brief Questo modulo fornisce un'interfaccia di alto livello per
  * 	   utilizzare gli switch.
  ******************************************************************************
  */
#ifndef SRC_SWITCH_H_
#define SRC_SWITCH_H_

/* Includes ------------------------------------------------------------------*/
#include "gpio_LL.h"
#include "defines.h"

/* Macro ---------------------------------------------------------------------*/

/**
  * @brief maschere degli switch
*/
#define SW0_MASK	(uint32_t)(0x1 << SW0) /*!< maschera dello switch 0*/
#define SW1_MASK	(uint32_t)(0x1 << SW1) /*!< maschera dello switch 1*/
#define SW2_MASK	(uint32_t)(0x1 << SW2) /*!< maschera dello switch 2*/
#define SW3_MASK	(uint32_t)(0x1 << SW3) /*!< maschera dello switch 3*/
#define SW_ALL_MASK	(uint32_t)(0xF << SW_NIBBLE_OFFSET) /*!< maschera dell'intero banco switch*/

/* Typedef -------------------------------------------------------------------*/

/**
  * @brief posizione dei switch nei registri
*/
typedef enum{
	SW0 = 0 + SW_NIBBLE_OFFSET , /*!< posizione bit dello switch 0*/
	SW1 = 1 + SW_NIBBLE_OFFSET , /*!< posizione bit dello switch 1*/
	SW2 = 2 + SW_NIBBLE_OFFSET , /*!< posizione bit dello switch 2*/
	SW3 = 3 + SW_NIBBLE_OFFSET   /*!< posizione bit dello switch 3*/
}switch_n;

/**
  * @brief stato di uno switch
*/
typedef enum{
	SW_OFF = 0 , /*!< posizione OFF*/
	SW_ON = 1	 /*!< posizione ON */
}switch_state;

/**
  * @brief tipo switch_t
*/
typedef struct switch_t switch_t;

/**
  * @brief struttura che racchiude tutte le funzioni del tipo switch_t
*/
struct switch_t{

	void (*enable)(switch_t* self);
	void (*disable)(switch_t* self);
	uint32_t (*readStatus)(switch_t* self);
	switch_state (*readSwitch)(switch_t* self,switch_n);
	void (*enableInterrupt)(switch_t*,uint32_t ,interrupt_mode);
	void (*disableInterrupt)(switch_t*,uint32_t ,interrupt_mode);
	uint32_t (*readISR)(switch_t*);
	void (*clearISR)(switch_t*,uint32_t);
    uint32_t* base_addr;
};

/**
  * @brief firma della funzione di inizializzazione
 */
void SW_Init(switch_t*);

#endif /* SRC_SWITCH_H_ */
/**@}*/
/**@}*/