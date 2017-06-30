/**
  ******************************************************************************
  * @file    led.h
  * @author  Pierluigi,Alfonso,Erasmo
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   Questo file definisce le principali funzioni di gestione dei led      
  ******************************************************************************
  */
#ifndef SRC_LED_H_
#define SRC_LED_H_

/* Includes ------------------------------------------------------------------*/
#include "gpio_LL.h"
#include "defines.h"

#define LED0	(0 + LED_NIBBLE_OFFSET)
#define LED1	(1 + LED_NIBBLE_OFFSET)
#define LED2	(2 + LED_NIBBLE_OFFSET)
#define LED3	(3 + LED_NIBBLE_OFFSET)

/**
  * @brief maschere per accendere i led
*/  
#define LED0_MASK	(uint32_t)(0x1 << LED0)
#define LED1_MASK	(uint32_t)(0x1 << LED1)
#define LED2_MASK	(uint32_t)(0x1 << LED2)
#define LED3_MASK	(uint32_t)(0x1 << LED3)

/**
  * @brief maschera per abilitare banco led
*/
#define LED_ALL_MASK	(uint32_t)(0xF << LED_NIBBLE_OFFSET)

typedef struct led_t led_t;
/**
  * @brief tipo di dato astratto led_t 
  *
*/
struct led_t{

	void (*enable)(led_t* self);
	void (*disable)(led_t* self);
	uint32_t (*readStatus)(led_t* self);
	void (*setOn)(led_t* self,int pos);
	void (*setOff)(led_t* self,int pos);
	void (*toggle)(led_t* self,int pos);
	void (*setLeds)(led_t* self,uint32_t led_mask);

    uint32_t* base_addr;
};

/**
  * @brief firma della funzione di inizializzazione
 */
void LED_Init(led_t*);

#endif /* SRC_LED_H_ */
