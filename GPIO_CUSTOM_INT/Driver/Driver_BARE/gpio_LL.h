/**
  ******************************************************************************
  * @file    gpio_LL.h
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   Questo file definisce le principali funzioni di basso livello per
  *			 l'accesso ai registri di una periferica GPIO con gestione delle
  *			 interrupt.
  ******************************************************************************
  */

#ifndef SRC_GPIO_LL_H_
#define SRC_GPIO_LL_H_

/* Includes ------------------------------------------------------------------*/
#include <inttypes.h>

/* Macro ---------------------------------------------------------------------*/
#define APE_DATA_REG		0	/*!< offset registro dato*/
#define APE_DIR_REG			4 	/*!< offset registro direzione (W)*/
#define APE_IERR_REG		8 	/*!< offset registro enable interrupt su rising edge*/
#define APE_IERF_REG		12	/*!< offset registro enable interrupt su falling edge*/
#define APE_ICRISR_REG		16	/*!< offset registro controllo interrupt (W) / stato interrupt (R)*/

/**
  * @brief selezione parte del registro per indirizzamento
  *		   a 16 bit a partire da destra
  *		    _________________ _________________
  *		   |________H________|________L________|
  *
 */
#define L	0	/*!<parte meno significativa*/
#define H	1	/*!<parte più significativa*/

/**
  * @brief selezione parte del registro per l'indirizzamento
  *		    ________ ________ ________ ________
  *		   |___HH___|___HL___|___LH___|___LL___|
  *
 */
#define LL	0	/*!<primo byte*/
#define LH	1	/*!<secondo byte*/
#define HL	2	/*!<terzo byte*/
#define HH	3	/*!<quarto byte*/


typedef enum { false, true } bool;
typedef enum {INT_RISING, INT_FALLING, INT_RIS_FALL} interrupt_mode;

/**
  * @brief firme delle funzioni
 */
void APE_writeValue32(uint32_t*,int,uint32_t);
void APE_writeValue16(uint32_t*,int,uint16_t,int);
void APE_writeValue8(uint32_t*,int,uint8_t,int);
uint32_t APE_readValue32(uint32_t*,int);
uint16_t APE_readValue16(uint32_t*,int,int);
uint8_t APE_readValue8(uint32_t*,int,int);
void APE_setBit(uint32_t*,int,bool,int);
void APE_toggleBit(uint32_t*,int,int);

#endif /* SRC_GPGPIO_LL_H_ */
