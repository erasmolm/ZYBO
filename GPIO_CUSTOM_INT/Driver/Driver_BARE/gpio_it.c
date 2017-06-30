/**
  ******************************************************************************
  * @file    gpio_LL.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   Questo file implementa le Interrupt Handler relative alle
  * 		 periferiche GPIO.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio_it.h"

/* Dichiarati nel main */
extern b;

/**
  * @brief  IRQ Handler della periferica GPIO_0
  */
void APE_IRQHandler_0(void){

	/* Legge il registro ISR */
	uint32_t state = APE_readValue32((uint32_t*)GPIO_0_BASE_ADDRESS,APE_ICRISR_REG);

	/* Inserire qui il codice utente */
	if((BTN0_MASK & state) == BTN0_MASK){
		APE_BTN0_Callback();
	}
	if(state & BTN1_MASK == BTN1_MASK){
		APE_BTN1_Callback();
	}
	if(state & BTN2_MASK == BTN2_MASK){
		APE_BTN2_Callback();
	}
	if(state & BTN3_MASK == BTN3_MASK){
		APE_BTN3_Callback();
	}

	/* ----------------------------- */

	/* Azzera il registro ISR */
	APE_writeValue32((uint32_t*)GPIO_0_BASE_ADDRESS,APE_ICRISR_REG,0xFFFFFFFF);
}

/**
  * @brief  IRQ Handler della periferica GPIO_X
  * @Note	Sostituire X con il numero della GPIO
  */
/*
void APE_IRQHandler_X(void){

	//Legge il registro ISR
	APE_readValue32();

	//Inserire qui il codice utente

	//-----------------------------

	// Azzera il registro ISR
	APE_writeValue32();
}
*/
