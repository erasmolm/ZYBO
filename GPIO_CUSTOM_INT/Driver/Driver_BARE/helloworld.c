/**
  ******************************************************************************
  * @file    helloworld.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V2.0
  * @date    26-Giugno-2017
  * @brief   Questo programma testa le interrupt lanciate dai bottoni e dagli
  *          switch. Ad ogni bottone e ad ogni switch corrisponde una callback,
  *          che effettua il toggle del led alla medesima posizione.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "gpio_it.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "button.h"
#include "led.h"
#include "switch.h"

/* Macro ----------------------------------------------------------------------*/
#define GPIO_DEVICE_ID 		XPAR_GPIO_CUSTOM_IPCORE_0_DEVICE_ID /*!< ID della periferica*/
#define INTC_DEVICE_ID 		XPAR_SCUGIC_SINGLE_DEVICE_ID 		/*!< ID del GIC*/
#define GPIO_INTERRUPT_ID 	XPS_FPGA0_INT_ID /*!< ID della linea di interrupt*/

/* Private variables ---------------------------------------------------------*/
static XScuGic Intc; /*!< Handler del GIC*/
btn_t b;	/*!< Handler dei bottoni*/
led_t l;	/*!< Handler dei led*/
switch_t s;	/*!< Handler degli switch*/
switch_state state[4]; /*!< Stato degli switch*/

/* Private function prototypes -----------------------------------------------*/
void setup(void);
void loop(void);

int main()
{
    init_platform();

    /*Inizializza led, bottoni e switch*/
    setup();

    /**
     * INIZIALIZZAZIONE INTERRUPT
     */

    /* 1: Inizializza interrupt controller*/
	XScuGic_Config *IntcConfig =  XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig){
		return XST_FAILURE;
	}

	if(XST_SUCCESS != XScuGic_CfgInitialize(&Intc, IntcConfig,IntcConfig->CpuBaseAddress)){
		return XST_FAILURE;
	}

	/* 2: Setta la priorità per la sorgente IRQ*/
	XScuGic_SetPriorityTriggerType(&Intc,GPIO_INTERRUPT_ID,0xA0,0x3);

	/* 3: gpiohandler è la funzione di gestione delle interrupt */
	if(XST_SUCCESS != XScuGic_Connect(&Intc,GPIO_INTERRUPT_ID,(Xil_ExceptionHandler)APE_IRQHandler_0,NULL)){
		return XST_FAILURE;
	}

	/* 4: Abilita le interrupt per il device GPIO*/
	XScuGic_Enable(&Intc, GPIO_INTERRUPT_ID);

	/* 5: Abilita interrupt su entrambi i fronti per bottoni e switch*/
	b.enableInterrupt(&b,0xF,INT_RIS_FALL);
	s.enableInterrupt(&s,0xF,INT_RIS_FALL);

	/* 6: Inizializza la exception table*/
	Xil_ExceptionInit();

	/* 7: registra l'interrupt controller handler con la exception table*/
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,&Intc);

	/* 8: abilita eccezioni */
	Xil_ExceptionEnable();

    /*-------------------------------------------------------------------------------------------*/

    for(;;)loop();

    cleanup_platform();
    return 0;
}

/**
  * @brief  Inizializza gli handler di switch, led e bottoni
  * @param  None
  * @retval None
  */
void setup(void){

	/*Inizializza handler bottoni e setta la direzione*/
	BTN_Init(&b);
	b.enable(&b);

	/*Inizializza handler switch e setta la direzione*/
	SW_Init(&s);
	s.enable(&s);

	/*Inizializza handler led e setta la direzione*/
	LED_Init(&l);
	l.enable(&l);

	/*Spegni tutti i led*/
	l.setLeds(&l,0x0);
}

/**
  * @brief  Legge lo stato di ogni switch e lo salva nella variabile
  * 		corrispondente
  * @param  None
  * @retval None
  */
void loop(void){
	state[0] = s.readSwitch(&s,SW0);
	state[1] = s.readSwitch(&s,SW1);
	state[2] = s.readSwitch(&s,SW2);
	state[3] = s.readSwitch(&s,SW3);
}

/**
  * @brief  Callback associata al bottone 0
  * @param  None
  * @retval None
  */
void APE_BTN0_Callback(void){
	l.toggle(&l,LED0);
}

/**
  * @brief  Callback associata al bottone 1
  * @param  None
  * @retval None
  */
void APE_BTN1_Callback(void){
	l.toggle(&l,LED1);
}

/**
  * @brief  Callback associata al bottone 2
  * @param  None
  * @retval None
  */
void APE_BTN2_Callback(void){
	l.toggle(&l,LED2);
}

/**
  * @brief  Callback associata al bottone 3
  * @param  None
  * @retval None
  */
void APE_BTN3_Callback(void){
	l.toggle(&l,LED3);
}

/**
  * @brief  Callback associata allo switch 0
  * @param  None
  * @retval None
  */
void APE_SW0_Callback(void){
	l.toggle(&l,LED0);
}

/**
  * @brief  Callback associata allo switch 1
  * @param  None
  * @retval None
  */
void APE_SW1_Callback(void){
	l.toggle(&l,LED1);
}

/**
  * @brief  Callback associata allo switch 2
  * @param  None
  * @retval None
  */
void APE_SW2_Callback(void){
	l.toggle(&l,LED2);
}

/**
  * @brief  Callback associata allo switch 3
  * @param  None
  * @retval None
  */
void APE_SW3_Callback(void){
	l.toggle(&l,LED3);
}


