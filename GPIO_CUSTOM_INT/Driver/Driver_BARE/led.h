/**
  ******************************************************************************
  * @file    led.h
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   Questo file definisce le funzioni di gestione dei led
  *
  *	@addtogroup LIB_OBJECTS
  * @{
  * @addtogroup LED
  * @{
  * @brief Questo modulo fornisce un'interfaccia di alto livello per
  * 	   utilizzare i led.
  ******************************************************************************
  */
#ifndef SRC_LED_H_
#define SRC_LED_H_

/* Includes ------------------------------------------------------------------*/
#include "gpio_LL.h"
#include "defines.h"

/* Macro ---------------------------------------------------------------------*/

/**
  * @brief maschere dei led
*/
#define LED0_MASK	(uint32_t)(0x1 << LED0)	/*!< maschera del led 0*/
#define LED1_MASK	(uint32_t)(0x1 << LED1)	/*!< maschera del led 1*/
#define LED2_MASK	(uint32_t)(0x1 << LED2)	/*!< maschera del led 2*/
#define LED3_MASK	(uint32_t)(0x1 << LED3)	/*!< maschera del led 3*/
#define LED_ALL_MASK	(uint32_t)(0xF << LED_NIBBLE_OFFSET) /*!< maschera per l'intero abilitare banco led*/

/* Typedef -------------------------------------------------------------------*/

/**
  * @brief posizione dei led nei registri
*/
typedef enum{
	LED0 = 0 + LED_NIBBLE_OFFSET , /*!< posizione bit del led 0*/
	LED1 = 1 + LED_NIBBLE_OFFSET , /*!< posizione bit del led 1*/
	LED2 = 2 + LED_NIBBLE_OFFSET , /*!< posizione bit del led 2*/
	LED3 = 3 + LED_NIBBLE_OFFSET   /*!< posizione bit del led 3*/
}led_n;

/**
  * @brief tipo led_t
*/
typedef struct led_t led_t;

/**
  * @brief struttura che racchiude tutte le funzioni del tipo led_t
*/
struct led_t{

	void (*enable)(led_t* self);
	void (*disable)(led_t* self);
	uint32_t (*readStatus)(led_t* self);
	void (*setOn)(led_t* self,led_n pos);
	void (*setOff)(led_t* self,led_n pos);
	void (*toggle)(led_t* self,led_n pos);
	void (*setLeds)(led_t* self,uint32_t led_mask);
    uint32_t* base_addr;
};

/**
  * @brief firma della funzione di inizializzazione
 */
void LED_Init(led_t*);

#endif /* SRC_LED_H_ */
/**@}*/
/**@}*/