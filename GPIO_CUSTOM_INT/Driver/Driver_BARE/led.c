/**
  ******************************************************************************
  * @file    led.c
  * @author  Pierluigi,Alfonso,Erasmo
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   Questo file implementa le principali funzioni di gestione dei led
  ******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "led.h"

/**
  * @brief  abilita tutti i led
  * @param 	self: puntatore alla struttura
  */
void LED_enable(led_t* self){
	uint32_t status = APE_readValue32(self->base_addr,APE_DIR_REG);
	uint32_t mask = ~LED_ALL_MASK;

	mask = mask & status;

	APE_writeValue32(self->base_addr,APE_DIR_REG,mask);
}

/**
  * @brief  disabilita tutti i led
  * @param 	self: puntatore alla struttura
  */
void LED_disable(led_t* self){
	uint32_t status = APE_readValue32(self->base_addr,APE_DIR_REG);
	uint32_t mask = LED_ALL_MASK;

	mask = mask | status;

	APE_writeValue32(self->base_addr,APE_DIR_REG,mask);
}

/**
  * @brief  legge il valore dei led
  * @param 	self: puntatore alla struttura
  */
uint32_t LED_readStatus(led_t* self){
	uint32_t status = APE_readValue32(self->base_addr,APE_DATA_REG);
	status = (status & LED_ALL_MASK);
	return (status >> LED_NIBBLE_OFFSET);
}

/**
  * @brief  accende un led in una data posizione
  * @param 	self: puntatore alla struttura
  * @param 	led: numero del led da accendere
  * 	Questo parametro può assumere i seguenti valori:
  *     	@arg LED0
  *     	@arg LED1
  *     	@arg LED2
  *     	@arg LED3
  */
void LED_setOn(led_t* self,int led){
	APE_setBit(self->base_addr,APE_DATA_REG,1,led);
}

/**
  * @brief  spegne un led in una data posizione
  * @param 	self: puntatore alla struttura
  * @param 	led: numero del led da spegnere
  * 	Questo parametro può assumere i seguenti valori:
  *     	@arg LED0
  *     	@arg LED1
  *     	@arg LED2
  *     	@arg LED3
  */
void LED_setOff(led_t* self,int led){
	APE_setBit(self->base_addr,APE_DATA_REG,0,led);
}

/**
  * @brief  fa il toggle di un led in una data posizione
  * @param 	self: puntatore alla struttura
  * @param 	led: numero del led
  * 	Questo parametro può assumere i seguenti valori:
  *     	@arg LED0
  *     	@arg LED1
  *     	@arg LED2
  *     	@arg LED3
  */
void LED_toggle(led_t* self,int led){
	APE_toggleBit(self->base_addr,APE_DATA_REG,led);
}

/**
  * @brief  accende i led in base ad una maschera
  * @param 	self: puntatore alla struttura
  * @param 	mask: maschera dei led da accendere 
  */
void LED_setLeds(led_t* self,uint32_t mask){
	APE_writeValue32(self->base_addr,APE_DATA_REG,(mask << LED_NIBBLE_OFFSET));
}

/**
  * @brief  inizializzazione della struttura
  * @param 	self: puntatore alla struttura
  */
void LED_Init(led_t* self){
	self->base_addr = (uint32_t*)LED_BASE_ADDRESS;
	self->enable = &LED_enable;
	self->disable = &LED_disable;
	self->readStatus = &LED_readStatus;
	self->setOn = &LED_setOn;
	self->setOff = &LED_setOff;
	self->toggle = &LED_toggle;
	self->setLeds = &LED_setLeds;
}
