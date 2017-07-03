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
#define GPIO_DEVICE_ID 		XPAR_GPIO_CUSTOM_IPCORE_0_DEVICE_ID
#define INTC_DEVICE_ID 		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define GPIO_INTERRUPT_ID 	XPS_FPGA0_INT_ID

/* Private variables ---------------------------------------------------------*/
static XScuGic Intc; // Interrupt Controller Driver
btn_t b;
led_t l;
switch_t s;

/* Private function prototypes -----------------------------------------------*/
void setup(void);

int main()
{
    init_platform();
    switch_state state[4];

    BTN_Init(&b);
    b.enable(&b);

    SW_Init(&s);
    s.enable(&s);

    LED_Init(&l);
    l.enable(&l);
    l.setLeds(&l,0x0);

    setup();

    while(1){
    	state[0] = s.readSwitch(&s,SW0);
    	state[1] = s.readSwitch(&s,SW1);
    	state[2] = s.readSwitch(&s,SW2);
    	state[3] = s.readSwitch(&s,SW3);
    }

    cleanup_platform();
    return 0;
}

/**
  * @brief  funzione di inizializzazione TODO completare
  * @param  None
  * @retval None
  */
void setup(void){
	//0
    XScuGic_Config *IntcConfig;

	/* 1: */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);

	/* 2: Inizializza interrupt controller driver*/
	XScuGic_CfgInitialize(&Intc, IntcConfig,IntcConfig->CpuBaseAddress);

	/* 3: */
	XScuGic_SetPriorityTriggerType(&Intc,GPIO_INTERRUPT_ID,0xA0,0x3);

	//4 gpiohandler è la funzione di gestione delle interrupt
	//&b è il riferimento all'istanza (o il baseaddress) della periferica interrompente
	XScuGic_Connect(&Intc,GPIO_INTERRUPT_ID,(Xil_ExceptionHandler)APE_IRQHandler_0,NULL);

	/* 5: Abilita le interrupt per il device GPIO*/
	XScuGic_Enable(&Intc, GPIO_INTERRUPT_ID);

	/* 6: Abilita interrupt su entrambi i fronti per bottoni e switch*/
	b.enableInterrupt(&b,0xF,INT_RIS_FALL);
	s.enableInterrupt(&s,0xF,INT_RIS_FALL);

	/* 7: Inizializza la exception table*/
	Xil_ExceptionInit();

	/* 8: registra l'interrupt controller handler con la exception table*/
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,&Intc);

	/* 9: abilita eccezioni non critiche*/
	Xil_ExceptionEnable();
}

void APE_BTN0_Callback(void){
	l.toggle(&l,LED0);
}

void APE_BTN1_Callback(void){
	l.toggle(&l,LED1);
}

void APE_BTN2_Callback(void){
	l.toggle(&l,LED2);
}

void APE_BTN3_Callback(void){
	l.toggle(&l,LED3);
}

void APE_SW0_Callback(void){
	l.toggle(&l,LED0);
}

void APE_SW1_Callback(void){
	l.toggle(&l,LED1);
}

void APE_SW2_Callback(void){
	l.toggle(&l,LED2);
}

void APE_SW3_Callback(void){
	l.toggle(&l,LED3);
}


