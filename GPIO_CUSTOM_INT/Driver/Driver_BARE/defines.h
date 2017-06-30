/**
  ******************************************************************************
  * @file    defines.h
  * @author  Pierluigi,Alfonso,Erasmo
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   TODO
  ******************************************************************************
  */

#ifndef SRC_DEFINES_H_
#define SRC_DEFINES_H_

#include "xparameters.h"

#define GPIO_0_BASE_ADDRESS	XPAR_GPIO_CUSTOM_IPCORE_0_S00_AXI_BASEADDR

#define BTN_BASE_ADDRESS	GPIO_0_BASE_ADDRESS
#define BTN_NIBBLE_OFFSET	8

#define LED_BASE_ADDRESS	GPIO_0_BASE_ADDRESS
#define LED_NIBBLE_OFFSET	4

#define SW_BASE_ADDRESS		GPIO_0_BASE_ADDRESS
#define SW_NIBBLE_OFFSET	0

#endif /* SRC_DEFINES_H_ */
