/**
  ******************************************************************************
  * @file    button.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   Questo file implementa le funzioni di gestione dei bottoni
  *
  *	@addtogroup LIB_OBJECTS
  * @{
  * @addtogroup BUTTON
  * @{
  ******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "button.h"

/**
  * @brief  abilita tutti i bottoni
  * @param 	self: puntatore alla struttura
  * @retval	None
  */
void BTN_enable(btn_t* self){
	uint32_t status = APE_readValue32(self->base_addr,APE_DIR_REG);
	uint32_t mask = BTN_ALL_MASK;

	mask = mask | status;

	APE_writeValue32(self->base_addr,APE_DIR_REG,mask);
}

/**
  * @brief  disabilita tutti i bottoni
  * @param 	self: puntatore alla struttura
  * @retval	None
  */
void BTN_disable(btn_t* self){
	uint32_t status = APE_readValue32(self->base_addr,APE_DIR_REG);
	uint32_t mask = ~BTN_ALL_MASK;

	mask = mask & status;

	APE_writeValue32(self->base_addr,APE_DIR_REG,mask);
}

/**
  * @brief  legge il valore dei bottoni
  * @param 	self: puntatore alla struttura
  * @retval	il valore del nibble del registro dato su cui sono mappati i bottoni
  */
uint32_t BTN_readStatus(btn_t* self){
	uint32_t status = APE_readValue32(self->base_addr,APE_DATA_REG);
	status = (status & BTN_ALL_MASK);
	return (status >> BTN_NIBBLE_OFFSET);
}

/**
  * @brief  abilita le interrupt ai bottoni
  * @param 	self: puntatore alla struttura
  * @param 	int_mask: maschera delle interrupt
  * @param 	mode: modalità interrompente
  * 	Questo parametro può assumere i seguenti valori:
  *     	@arg INT_RISING
  *     	@arg INT_FALLING
  *     	@arg INT_RIS_FALL
  * @retval	None
  */
void BTN_enableInterrupt(btn_t* self,uint32_t int_mask,interrupt_mode mode){
	uint32_t status;
	uint32_t mask = int_mask << BTN_NIBBLE_OFFSET;

	switch (mode){
	case INT_RISING:
		status = APE_readValue32(self->base_addr,APE_IERR_REG);
		mask = mask | status;
		APE_writeValue32(self->base_addr,APE_IERR_REG,mask);
		break;
	case INT_FALLING:
		status = APE_readValue32(self->base_addr,APE_IERF_REG);
		mask = mask | status;
		APE_writeValue32(self->base_addr,APE_IERF_REG,mask);
		break;
	case INT_RIS_FALL:
		status = APE_readValue32(self->base_addr,APE_IERR_REG);
		mask = mask | status;
		APE_writeValue32(self->base_addr,APE_IERR_REG,mask);

		status = APE_readValue32(self->base_addr,APE_IERF_REG);
		mask = mask | status;
		APE_writeValue32(self->base_addr,APE_IERF_REG,mask);
		break;
	}
}

/**
  * @brief  disabilita le interrupt ai bottoni
  * @param 	self: puntatore alla struttura
  * @param 	int_mask: maschera delle interrupt
  * @param 	mode: modalità interrompente
  * 	Questo parametro può assumere i seguenti valori:
  *     	@arg INT_RISING
  *     	@arg INT_FALLING
  *     	@arg INT_RIS_FALL
  * @retval	None
  */
void BTN_disableInterrupt(btn_t* self,uint32_t int_mask,interrupt_mode mode){
	uint32_t status;
	uint32_t mask = int_mask << BTN_NIBBLE_OFFSET;
	mask = ~mask;

	switch (mode){
	case INT_RISING:
		status = APE_readValue32(self->base_addr,APE_IERR_REG);
		mask = mask & status;
		APE_writeValue32(self->base_addr,APE_IERR_REG,mask);
		break;
	case INT_FALLING:
		status = APE_readValue32(self->base_addr,APE_IERF_REG);
		mask = mask & status;
		APE_writeValue32(self->base_addr,APE_IERF_REG,mask);
		break;
	case INT_RIS_FALL:
		status = APE_readValue32(self->base_addr,APE_IERR_REG);
		mask = mask & status;
		APE_writeValue32(self->base_addr,APE_IERR_REG,mask);

		status = APE_readValue32(self->base_addr,APE_IERF_REG);
		mask = mask & status;
		APE_writeValue32(self->base_addr,APE_IERF_REG,mask);
		break;
	}
}

/**
  * @brief  legge il valore del registro ISR
  * @param 	self: puntatore alla struttura
  * @retval	il valore del nibble del registro ISR su cui sono mappate le interrupt dei bottoni
  */
uint32_t BTN_readISR(btn_t* self){
	uint32_t status = APE_readValue32(self->base_addr,APE_ICRISR_REG);
	status = (status & BTN_ALL_MASK);
	return (status >> BTN_NIBBLE_OFFSET);
}

/**
  * @brief  setta a 0 i bit del registro ISR corrispondenti ad una maschera
  * @param 	self: puntatore alla struttura
  * @param	int_mask: maschera delle interrupt
  * @retval	None
  */
void BTN_clearISR(btn_t* self,uint32_t int_mask){
	uint32_t ISR_value = APE_readValue32(self->base_addr,APE_ICRISR_REG);
	uint32_t mask = int_mask & (ISR_value >> BTN_NIBBLE_OFFSET);

	mask = mask << BTN_NIBBLE_OFFSET;

	APE_writeValue32(self->base_addr,APE_ICRISR_REG,mask);
}

/**
  * @brief  dichiarazione debole della callback del bottone 0
  * @note	questa funzione deve essere ridefinita dall'utente
  * 		nel programma principale
  * @retval	None
  */
__attribute__((weak))
void APE_BTN0_Callback(void){

}

/**
  * @brief  dichiarazione debole della callback del bottone 0
  * @note	questa funzione deve essere ridefinita dall'utente
  * 		nel programma principale
  * @retval	None
  */
__attribute__((weak))
void APE_BTN1_Callback(void){
	/* Lasciare vuota */
}

/**
  * @brief  dichiarazione debole della callback del bottone 0
  * @note	questa funzione deve essere ridefinita dall'utente
  * 		nel programma principale
  * @retval	None
  */
__attribute__((weak))
void APE_BTN2_Callback(void){
	/* Lasciare vuota */
}

/**
  * @brief  dichiarazione debole della callback del bottone 0
  * @note	questa funzione deve essere ridefinita dall'utente
  * 		nel programma principale
  * @retval	None
  */
__attribute__((weak))
void APE_BTN3_Callback(void){
	/* Lasciare vuota */
}

/**
  * @brief  inizializzazione della struttura
  * @param 	self: puntatore alla struttura
  * @retval None
  */
void BTN_Init(btn_t* self){
	self->base_addr = (uint32_t*)BTN_BASE_ADDRESS;
	self->enable = &BTN_enable;
	self->disable = &BTN_disable;
	self->readStatus = &BTN_readStatus;
	self->enableInterrupt = &BTN_enableInterrupt;
	self->disableInterrupt = &BTN_disableInterrupt;
	self->readISR = &BTN_readISR;
	self->clearISR = &BTN_clearISR;
}
/**@}*/
/**@}*/