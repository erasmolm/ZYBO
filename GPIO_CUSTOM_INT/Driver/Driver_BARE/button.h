/**
  ******************************************************************************
  * @file    button.h
  * @author  Pierluigi,Alfonso,Erasmo
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   Questo file definisce le principali funzioni di gestione dei bottoni      
  ******************************************************************************
  */

#ifndef SRC_BUTTON_H_
#define SRC_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
#include "gpio_LL.h"
#include "defines.h"

/* Macro ---------------------------------------------------------------------*/
#define BTN0	(0 + BTN_NIBBLE_OFFSET)
#define BTN1	(1 + BTN_NIBBLE_OFFSET)
#define BTN2	(2 + BTN_NIBBLE_OFFSET)
#define BTN3	(3 + BTN_NIBBLE_OFFSET)

/**
  * @brief maschera per abilitare banco bottoni
*/
#define BTN0_MASK	(uint32_t)(0x1 << BTN0)
#define BTN1_MASK	(uint32_t)(0x1 << BTN1)
#define BTN2_MASK	(uint32_t)(0x1 << BTN2)
#define BTN3_MASK	(uint32_t)(0x1 << BTN3)
#define BTN_ALL_MASK	(uint32_t)(0xF << BTN_NIBBLE_OFFSET)

/**
  * @brief tipo di dato astratto btn_t 
  *
*/
typedef struct btn_t btn_t;

struct btn_t{

	void (*enable)(btn_t* self);
	void (*disable)(btn_t* self);
	uint32_t (*readStatus)(btn_t* self);
	void (*enableInterrupt)(btn_t*,uint32_t ,interrupt_mode);
	void (*disableInterrupt)(btn_t*,uint32_t ,interrupt_mode);
	uint32_t (*readISR)(btn_t*);
	void (*clearISR)(btn_t*,uint32_t);
	uint32_t* base_addr;
};

/**
  * @brief firma della funzione di inizializzazione
 */
void BTN_Init(btn_t*);

#endif /* SRC_BUTTON_H_ */
