/**
  ******************************************************************************
  * @file    gpio_LL.h
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   Questo file definisce le funzioni del modulo GPIO_LL.
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
  *		   a 16 bit a partire da destra.
  *	<table>
  * <tr><th>H</th><th>L</th></tr>
  * <tr><td>31-16</td><td>15-0</td></tr>
  * </table>
 */
#define L	0	/*!<parte meno significativa*/
#define H	1	/*!<parte più significativa*/

/**
  * @brief selezione parte del registro per l'indirizzamento
  *			a 8 bit a partire da destra.
  *	<table>
  * <tr><th>HH</th><th>HL</th><th>LH</th><th>LL</th></tr>
  * <tr><td>31-24</td><td>23-16</td><td>15-8</td><td>7-0</td></tr>
  * </table>
  *
 */
#define LL	0	/*!<primo byte*/
#define LH	1	/*!<secondo byte*/
#define HL	2	/*!<terzo byte*/
#define HH	3	/*!<quarto byte*/

/* Typedef -------------------------------------------------------------------*/

/**
  * @brief tipo booleano
*/
typedef enum { false, true } bool;

/**
  * @brief modalità di interrupt
*/
typedef enum {
	INT_RISING	, 	/*!< modalità interrompente sul fronte di salita*/
	INT_FALLING	,	/*!< modalità interrompente sul fronte di discesa*/
	INT_RIS_FALL	/*!< modalità interrompente su entrambi i fronti*/
} interrupt_mode;

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
/**@}*/
