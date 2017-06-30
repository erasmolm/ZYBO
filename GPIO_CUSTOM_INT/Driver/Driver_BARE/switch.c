/**
  ******************************************************************************
  * @file    switch.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V2.0
  * @date    01-Luglio-2017
  * @brief   Questo file implementa le funzioni di gestione degli switch
  ********************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "switch.h"

/**
  * @brief  abilita tutti gli switch
  * @param 	self: puntatore alla struttura
  * @retval	None
  */
void SW_enable(switch_t* self){
	uint32_t status = APE_readValue32(self->base_addr,APE_DIR_REG);
	uint32_t mask = SW_ALL_MASK;

	mask = mask | status;

	APE_writeValue32(self->base_addr,APE_DIR_REG,mask);
}

/**
  * @brief  disabilita tutti gli switch
  * @param 	self: puntatore alla struttura
  * @retval	None
  */
void SW_disable(switch_t* self){
	uint32_t status = APE_readValue32(self->base_addr,APE_DIR_REG);
	uint32_t mask = ~SW_ALL_MASK;

	mask = mask & status;

	APE_writeValue32(self->base_addr,APE_DIR_REG,mask);
}

/**
  * @brief  legge il valore degli switch
  * @param 	self: puntatore alla struttura
  * @retval	il valore del nibble del registro dato su cui sono mappati gli switch
  */
uint32_t SW_readStatus(switch_t* self){
	uint32_t status = APE_readValue32(self->base_addr,APE_DATA_REG);
	status = (status & SW_ALL_MASK);
	return (status >> SW_NIBBLE_OFFSET);
}

/**
  * @brief  abilita le interrupt agli switch
  * @param 	self: puntatore alla struttura
  * @param 	int_mask: maschera delle interrupt
  * @param 	mode: modalità interrompente
  * 	Questo parametro può assumere i seguenti valori:
  *     	@arg INT_RISING
  *     	@arg INT_FALLING
  *     	@arg INT_RIS_FALL
  * @retval	None
  */
void SW_enableInterrupt(switch_t* self,uint32_t int_mask,interrupt_mode mode){

	uint32_t status;
	uint32_t mask = int_mask << SW_NIBBLE_OFFSET;

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
  * @brief  disabilita le interrupt agli switch
  * @param 	self: puntatore alla struttura
  * @param 	int_mask: maschera delle interrupt
  * @param 	mode: modalità interrompente
  * 	Questo parametro può assumere i seguenti valori:
  *     	@arg INT_RISING
  *     	@arg INT_FALLING
  *     	@arg INT_RIS_FALL
  * @retval	None
  */
void SW_disableInterrupt(switch_t* self,uint32_t int_mask,interrupt_mode mode){
	uint32_t status;
	uint32_t mask = int_mask << SW_NIBBLE_OFFSET;
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
  * @retval	il valore del nibble del registro ISR su cui sono mappate le interrupt degli switch
  */
uint32_t SW_readISR(switch_t* self){
	uint32_t status = APE_readValue32(self->base_addr,APE_ICRISR_REG);
	status = (status & SW_ALL_MASK);
	return (status >> SW_NIBBLE_OFFSET);
}

/**
  * @brief  setta a 0 i bit del registro ISR corrispondenti ad una maschera
  * @param 	self: puntatore alla struttura
  * @param	int_mask: maschera delle interrupt
  * @retval	None
  */
void SW_clearISR(switch_t* self,uint32_t int_mask){
	uint32_t ISR_value = APE_readValue32(self->base_addr,APE_ICRISR_REG);
	uint32_t mask = int_mask & (ISR_value >> SW_NIBBLE_OFFSET);

	mask = mask << SW_NIBBLE_OFFSET;

	APE_writeValue32(self->base_addr,APE_ICRISR_REG,mask);
}

/**
  * @brief  dichiarazione debole della callback dello switch 0
  * @Note	questa funzione deve essere ridefinita dall'utente
  * 		nel programma principale
  * @retval	None
  */
__attribute__((weak))
void APE_SW0_Callback(void){

}

/**
  * @brief  dichiarazione debole della callback dello switch 1
  * @Note	questa funzione deve essere ridefinita dall'utente
  * 		nel programma principale
  * @retval	None
  */
__attribute__((weak))
void APE_SW1_Callback(void){
	/* Lasciare vuota */
}

/**
  * @brief  dichiarazione debole della callback dello switch 2
  * @Note	questa funzione deve essere ridefinita dall'utente
  * 		nel programma principale
  * @retval	None
  */
__attribute__((weak))
void APE_SW2_Callback(void){
	/* Lasciare vuota */
}

/**
  * @brief  dichiarazione debole della callback dello switch 3
  * @Note	questa funzione deve essere ridefinita dall'utente
  * 		nel programma principale
  * @retval	None
  */
__attribute__((weak))
void APE_SW3_Callback(void){
	/* Lasciare vuota */
}

/**
  * @brief  inizializzazione della struttura
  * @param 	self: puntatore alla struttura
  * @retval	None
  */
void SW_Init(switch_t* self){
	self->base_addr = (uint32_t*)SW_BASE_ADDRESS;
	self->enable = &SW_enable;
	self->disable = &SW_disable;
	self->readStatus = &SW_readStatus;
	self->enableInterrupt = &SW_enableInterrupt;
	self->disableInterrupt = &SW_disableInterrupt;
	self->readISR = &SW_readISR;
	self->clearISR = &SW_clearISR;
}
