/**
  ******************************************************************************
  * @file    gpio_LL.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   Questo file implementa le Interrupt Handler delle
  * 		 periferiche GPIO.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio_it.h"

/* Dichiarati nel main */
//extern b; TODO

/**
  * @brief  IRQ Handler della periferica GPIO_0
  */
void APE_IRQHandler_0(void){

	/* Legge il registro ISR */
	uint32_t state = APE_readValue32((uint32_t*)GPIO_0_BASE_ADDRESS,APE_ICRISR_REG);

	/* Chiama le callback dei bottoni */
	if((BTN0_MASK & state) == BTN0_MASK){
		APE_BTN0_Callback();
	}
	if((BTN1_MASK & state) == BTN1_MASK){
		APE_BTN1_Callback();
	}
	if((BTN2_MASK & state) == BTN2_MASK){
		APE_BTN2_Callback();
	}
	if((BTN3_MASK & state) == BTN3_MASK){
		APE_BTN3_Callback();
	}

	/* Chiama le callback degli switch*/
	if((SW0_MASK & state) == SW0_MASK){
		APE_SW0_Callback();
	}
	if((SW1_MASK & state) == SW1_MASK){
		APE_SW1_Callback();
	}
	if((SW2_MASK & state) == SW2_MASK){
		APE_SW2_Callback();
	}
	if((SW3_MASK & state) == SW3_MASK){
		APE_SW3_Callback();
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
