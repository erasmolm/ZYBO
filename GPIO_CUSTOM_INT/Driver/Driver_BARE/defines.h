/**
  ******************************************************************************
  * @file    defines.h
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   In questo file sono definite le macro utilizzate dai bottoni, led
  * 		 e switch. Modificando tali macro è possibile mappare questi ultimi
  * 		 su nibble differenti e/o dichiarare l'indirizzo base di più
  * 		 periferiche GPIO differenti.
  ******************************************************************************
  */

#ifndef SRC_DEFINES_H_
#define SRC_DEFINES_H_

/* Includes ------------------------------------------------------------------*/
#include "xparameters.h"

/*
 * @brief Definisce l'indizzo base della periferica GPIO utilizzata.
 * 		  E' possibile dichiarare molteplici periferiche e mappare
 * 		  su di esse il banco led, quello dei bottoni e/o quello degli
 * 		  switch.
 */
#define GPIO_0_BASE_ADDRESS	XPAR_GPIO_CUSTOM_IPCORE_0_S00_AXI_BASEADDR	/*!< Indirizzo base periferica 0*/

/*
 * @brief Definisce a quale periferica GPIO associare l'intero banco di bottoni
 * 		  e su quale nibble controllare e leggerne i valori dai registri.
 * 		  Il nibble su cui mappare il banco è definito mediante un offset in
 * 		  bit. Il banco può essere associato ad una sola periferica e
 * 		  non è divisibile.
 */
#define BTN_BASE_ADDRESS	GPIO_0_BASE_ADDRESS	/*!< Indirizzo base */
#define BTN_NIBBLE_OFFSET	8					/*!< Spiazzamento nibble */

/*
 * @brief Definisce a quale periferica GPIO associare l'intero banco di led
 * 		  e su quale nibble controllare e leggerne i valori dai registri.
 * 		  Il nibble su cui mappare il banco è definito mediante un offset in
 * 		  bit. Il banco può essere associato ad una sola periferica e
 * 		  non è divisibile.
 */
#define LED_BASE_ADDRESS	GPIO_0_BASE_ADDRESS	/*!< Indirizzo base */
#define LED_NIBBLE_OFFSET	4					/*!< Spiazzamento nibble */

/*
 * @brief Definisce a quale periferica GPIO associare l'intero banco di switch
 * 		  e su quale nibble controllare e leggerne i valori dai registri.
 * 		  Il nibble su cui mappare il banco è definito mediante un offset in
 * 		  bit. Il banco può essere associato ad una sola periferica e
 * 		  non è divisibile.
 */
#define SW_BASE_ADDRESS		GPIO_0_BASE_ADDRESS	/*!< Indirizzo base */
#define SW_NIBBLE_OFFSET	0					/*!< Spiazzamento nibble */

#endif /* SRC_DEFINES_H_ */
