/**
  ******************************************************************************
  * @file    gpio_LL.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   Questo file implementa le funzioni del modulo GPIO_LL.
  *
  *	@addtogroup LOW_LEVEL
  * @{
  * @addtogroup GPIO_LL
  * @{
  * @brief   Questo modulo fornisce le funzioni di basso livello per
  *			 l'accesso ai registri di una periferica GPIO con gestione delle
  *			 interrupt.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <assert.h>
#include "gpio_LL.h"

/**
  * @brief  scrive un valore di 32 bit in un registro
  * @param 	addr: indirizzo base del registro
  * @param  offset: offset sommato all'indirizzo base.
  *   Questo parametro può assumere i seguenti valori:
  *     @arg APE_DATA_REG
  *     @arg APE_DIR_REG
  *     @arg APE_IERR_REG
  *     @arg APE_IERF_REG
  *     @arg APE_ICRISR_REG
  * @param 	value: valore da scrivere nel registro
  *	@retval None
  */
void APE_writeValue32(uint32_t* addr,int offset,uint32_t value){
	assert(((uint32_t)addr)%4 == 0);
	addr[offset/4] = value;
}

/**
  * @brief  scrive un valore di 16 bit in un registro
  * @param 	addr: indirizzo base del registro
  * @param  offset: offset sommato all'indirizzo base.
  *   Questo parametro può assumere i seguenti valori:
  *     @arg APE_DATA_REG
  *     @arg APE_DIR_REG
  *     @arg APE_IERR_REG
  *     @arg APE_IERF_REG
  *     @arg APE_ICRISR_REG
  * @param 	value: valore da scrivere nel registro
  * @param  part: sottoparte del registro selezionata
  *   Questo parametro può assumere i seguenti valori:
  *     @arg L
  *     @arg H
  *	@retval None
  */
void APE_writeValue16(uint32_t* addr,int offset,uint16_t value,int part){
	assert(((uint32_t)addr)%4 == 0);
	uint32_t val_32 = (uint32_t)value<<(16*part);
	addr[offset/4] = val_32;
}

/**
  * @brief  scrive un valore di 8 bit in un registro
  * @param 	addr: indirizzo base del registro
  * @param  offset: offset sommato all'indirizzo base.
  *   Questo parametro può assumere i seguenti valori:
  *     @arg APE_DATA_REG
  *     @arg APE_DIR_REG
  *     @arg APE_IERR_REG
  *     @arg APE_IERF_REG
  *     @arg APE_ICRISR_REG
  * @param 	value: valore da scrivere nel registro
  * @param  part: sottoparte del registro selezionata.
  *   Questo parametro può assumere i seguenti valori:
  *     @arg LL
  *     @arg LH
  *     @arg HL
  *     @arg HH
  *	@retval None
  */
void APE_writeValue8(uint32_t* addr,int offset,uint8_t value,int part){
	assert(((uint32_t)addr)%4 == 0);
	uint32_t val_32 = (uint32_t)value<<(8*part);
	addr[offset/4] = val_32;
}

/**
  * @brief  legge un valore di 32 bit da un registro
  * @param 	addr: indirizzo base del registro
  * @param  offset: offset sommato all'indirizzo base.
  *   Questo parametro può assumere i seguenti valori:
  *     @arg APE_DATA_REG
  *     @arg APE_DIR_REG
  *     @arg APE_IERR_REG
  *     @arg APE_IERF_REG
  *     @arg APE_ICRISR_REG
  *	@retval Il valore a 32 bit letto dal registro
  */
uint32_t APE_readValue32(uint32_t* addr,int offset){
	assert(((uint32_t)addr)%4 == 0);
	return addr[offset/4];
}


/**
  * @brief  legge un valore di 16 bit da un registro
  * @param 	addr: indirizzo base del registro
  * @param  offset: offset sommato all'indirizzo base.
  *   Questo parametro può assumere i seguenti valori:
  *     @arg APE_DATA_REG
  *     @arg APE_DIR_REG
  *     @arg APE_IERR_REG
  *     @arg APE_IERF_REG
  *     @arg APE_ICRISR_REG
  * @param  part: sottoparte del registro selezionata.
  *    Questo parametro può assumere i seguenti valori:
  *     @arg L
  *     @arg H
  *	@retval Il valore a 16 bit letto dal registro
  */
uint16_t APE_readValue16(uint32_t* addr,int offset, int part){
	assert(((uint32_t)addr)%4 == 0);
	uint32_t val_32 = addr[offset/4];
	val_32=val_32>>(16*part);
	return (uint16_t)val_32;
}

/**
  * @brief  legge un valore di 8 bit da un registro
  * @param 	addr: indirizzo base del registro
  * @param  offset: offset sommato all'indirizzo base.
  *   Questo parametro può assumere i seguenti valori:
  *     @arg APE_DATA_REG
  *     @arg APE_DIR_REG
  *     @arg APE_IERR_REG
  *     @arg APE_IERF_REG
  *     @arg APE_ICRISR_REG
  * @param  part: sottoparte del registro selezionata
  * @param  part: sottoparte del registro selezionata.
  *   Questo parametro può assumere i seguenti valori:
  *     @arg LL
  *     @arg LH
  *     @arg HL
  *     @arg HH
  *	@retval Il valore a 8 bit letto dal registro
  */
uint8_t APE_readValue8(uint32_t* addr,int offset, int part){
	assert(((uint32_t)addr)%4 == 0);
	uint32_t val_32 = addr[offset/4];
	val_32=val_32>>(8*part);
	return (uint8_t)val_32;
}

/**
  * @brief  imposta un bit ad un determinato valore in una
  * 		particolare posizione di un registro
  * @param 	addr: indirizzo base del registro
  * @param  offset: offset sommato all'indirizzo base.
  *   Questo parametro può assumere i seguenti valori:
  *     @arg APE_DATA_REG
  *     @arg APE_DIR_REG
  *     @arg APE_IERR_REG
  *     @arg APE_IERF_REG
  *     @arg APE_ICRISR_REG
  * @param 	val: valore da impostare
  * @param 	pos: posizione in cui impostare il valore
  *	@retval None
  */
void APE_setBit(uint32_t* addr,int offset,bool val,int pos){
	uint32_t mask = 0x1 << pos;
	assert(((uint32_t)addr)%4 == 0);

	if(val){
		APE_writeValue32(addr,offset,APE_readValue32(addr,offset) | mask);
	}else{
		APE_writeValue32(addr,offset,APE_readValue32(addr,offset) & ~mask);
	}
}

/**
  * @brief  funzione di toggle per uno specifico bit
  * @param 	addr: indirizzo base del registro
  * @param  offset: offset sommato all'indirizzo base.
  *   Questo parametro può assumere i seguenti valori:
  *     @arg APE_DATA_REG
  *     @arg APE_DIR_REG
  *     @arg APE_IERR_REG
  *     @arg APE_IERF_REG
  *     @arg APE_ICRISR_REG
  * @param 	pos: posizione in cui impostare il valore
  *	@retval None
  */
void APE_toggleBit(uint32_t* addr,int offset,int pos){
	uint32_t mask = 0x1 << pos;
	assert(((uint32_t)addr)%4 == 0);

	APE_writeValue32(addr,offset,APE_readValue32(addr,offset) ^ mask);
}
/**@}*/
