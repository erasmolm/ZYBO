/**
  ******************************************************************************
  * @file    helloworld.c
  * @author  Gruppo 5
  * @date    26-Giugno-2017
  * @brief	 TODO porcoddio scrivi qualcosa
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "led.h"
#include "button.h"


/* Macro ----------------------------------------------------------------------*/
#define GPIO_DEVICE_ID 		XPAR_GPIO_CUSTOM_IPCORE_0_DEVICE_ID
#define INTC_DEVICE_ID 		XPAR_SCUGIC_SINGLE_DEVICE_ID
//#define GPIO_INTERRUPT_ID 	XPS_FPGA5_INT_ID
#define GPIO_INTERRUPT_ID XPAR_FAB
/* Private variables ---------------------------------------------------------*/
static XScuGic Intc; // Interrupt Controller Driver
//static XGpioPs Gpio; //TODO IMPLEMENTARE IL NOSTRO TIPO
btn_t b;


/* Private function prototypes -----------------------------------------------*/
void setup(void);
void GpioHandler(void* CallBackRef);

int main()
{
    init_platform();

    //uint32_t status;

    int count = 0;
    BTN_Init(&b);
    b.enable(&b);

    setup();
/*
 *
 *
    status=b.readStatus(&b);
    b.enableInterrupt(&b,0xF,INT_RISING);
    status=b.readISR(&b);
    b.clearInterrupt(&b,BTN0);
    b.disableInterrupt(&b,0xF,INT_RISING);

    b.enableInterrupt(&b,0xF,INT_FALLING);
	status=b.readISR(&b);
	b.clearISR(&b,status);
	b.disableInterrupt(&b,0xF,INT_FALLING);

	b.enableInterrupt(&b,0xF,INT_RIS_FALL);
	status=b.readISR(&b);
	b.clearInterrupt(&b,BTN0);
	status=b.readISR(&b);
	b.clearInterrupt(&b,BTN0);
	b.disableInterrupt(&b,0xF,INT_RIS_FALL);

	b.disable(&b);
	status=b.readStatus(&b);
	b.enableInterrupt(&b,0xF,INT_RISING);
	status=b.readISR(&b);
	b.clearInterrupt(&b,BTN0);
	b.disableInterrupt(&b,0xF,INT_RISING);

*/


    while(1){
    	count++;
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
	XScuGic_Connect(&Intc,GPIO_INTERRUPT_ID,(Xil_ExceptionHandler)GpioHandler,&b);

	/* 5: Abilita le interrupt per il device GPIO*/
	XScuGic_Enable(&Intc, GPIO_INTERRUPT_ID);

	//6
	b.enableInterrupt(&b,0xF,INT_RISING);

	/* 7: Inizializza la exception table*/
	Xil_ExceptionInit();

	/* 8: registra l'interrupt controller handler con la exception table*/
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,&Intc);

	/* 9: abilita eccezioni non critiche*/
	Xil_ExceptionEnable();
}

void GpioHandler(void* CallBackRef){
	btn_t* btn = (btn_t*)CallBackRef;

	uint32_t status=b.readISR(btn);
	b.clearInterrupt(btn,BTN0);
}


