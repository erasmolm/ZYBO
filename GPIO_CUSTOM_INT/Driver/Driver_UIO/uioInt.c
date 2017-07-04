/**
  ******************************************************************************
  * @file    uioInt.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V1.0
  * @date    03-Luglio-2017
  * @brief   Driver UIO user-space per linux della periferica GPIO custom.
  * 		 Il driver ha 3 modalità:
  * 		 IN: generica lettura dei registri della periferica con utilizzo
  * 		 	 delle interrupt.
  * 		 OUT: generica scrittura verso i registri della periferica.
  * 		 TEST: il driver testa le interrupt effettuando il toggle di un led
  * 		 	   alla pressione/rilascio di un bottone e/o toggling di uno switch.
  ******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "button.h"
#include "led.h"
#include "switch.h"

/* Macro ---------------------------------------------------------------------*/
#define GPIO_MAP_SIZE 0x10000	/*!< spazio di indirizzamento del device */

/* Typedef -------------------------------------------------------------------*/
typedef enum {
	IN,		/*!< Modalità in lettura dalla GPIO */
	OUT,	/*!< Modalità in scrittura verso la GPIO */
	TEST	/*!< Modalità di test */
}direction;

/* Private function prototypes -----------------------------------------------*/
void usage(void);
void initScreen(void);
void APE_IRQHandler_0(void* ptr);

int main(int argc, char *argv[]){
	int c;
	int fd;
	int direction=TEST;
	char *uiod;
	uint32_t value = 0;

	void *ptr;

	uint32_t info = 1;
	ssize_t nb;

	initScreen();

	while((c = getopt(argc, argv, "d:io:h")) != -1) {
		switch(c) {
		case 'd':
			uiod=optarg;
			break;
		case 'i':
			direction=IN;
			break;
		case 'o':
			direction=OUT;
			value = strtol(optarg,NULL,16);
			break;
		case 'h':
			usage();
			return 0;
		default:
			printf("invalid option: %c\n", (char)c);
			usage();
			return -1;
		}

	}

	/* Open the UIO device file */
	fd = open(uiod, O_RDWR);
	if (fd < 1) {
		perror(argv[0]);
		printf("Invalid UIO device file:%s.\n", uiod);
		usage();
		return -1;
	}

	/* mmap the UIO device */
	ptr = mmap(NULL, GPIO_MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	/* Lettura generica dalla GPIO */
	if (direction == IN) {
		printf("\n\n Modalità IN \n\n");

		/* Setta la direzione */
		APE_writeValue32(ptr,APE_DIR_REG,BTN_ALL_MASK|SW_ALL_MASK);

		for(;;){

			/* Abilita le Interrupt */
			APE_writeValue32(ptr,APE_IERR_REG,BTN_ALL_MASK|SW_ALL_MASK);

			/* Wait for interrupt */
			nb = read(fd, &info, sizeof(info));
			if (nb == sizeof(info)) {

				/* Disabilita le interrupt */
				APE_writeValue32(ptr,APE_IERR_REG,0x0);

				/* Serve le interrupt */
				value = APE_readValue32(ptr,APE_DATA_REG);
				printf("Input: %08x\n",value);

				/* Marca le interrupt come servite */
				APE_writeValue32(ptr,APE_ICRISR_REG,BTN_ALL_MASK|SW_ALL_MASK);
			}

			nb = write(fd, &info, sizeof(info));
			if (nb < sizeof(info)) {
				perror("write");
				close(fd);
				exit(EXIT_FAILURE);
			}	
		}
	}

	/* Scrittura generica verso la GPIO */
	if(direction == OUT) {
		printf("\n\n Modalità OUT \n\n");
		APE_writeValue32(ptr,APE_DIR_REG,0x000);
		APE_writeValue32(ptr,APE_DATA_REG,value);
	}

	/* Modalità di test */
	if(direction == TEST) {
		printf("\n\n Modalità TEST \n\n");

		/*Dichiarazione degli handler*/
		btn_t btn_handler;
		switch_t sw_handler;
		led_t led_handler;

		/*Inizializzazione degli handler*/
		BTN_Init(&btn_handler);
		SW_Init(&sw_handler);
		LED_Init(&led_handler);

		/*Ridefinizione dei base address*/
		btn_handler.base_addr = ptr;
		sw_handler.base_addr = ptr;
		led_handler.base_addr = ptr;

		/*Abilitazione dei moduli*/
		btn_handler.enable(&btn_handler);
		sw_handler.enable(&sw_handler);
		led_handler.enable(&led_handler);

		/*Spegni tutti i led*/
		led_handler.setLeds(&led_handler,LED_ALL_MASK);
		APE_writeValue32(ptr,APE_DATA_REG,0x0);

		for(;;){

			/*Abilita interrupt su entrambi i fronti*/
			sw_handler.enableInterrupt(&sw_handler,0xF,INT_RIS_FALL);
			btn_handler.enableInterrupt(&btn_handler,0xF,INT_RIS_FALL);
			value = APE_readValue32(ptr,APE_IERR_REG);
			printf("\n\n");

			/* Wait for interrupt */
			nb = read(fd, &info, sizeof(info));
			if (nb == sizeof(info)) {
					/* Disabilita le interrupt */
					sw_handler.disableInterrupt(&sw_handler,0xF,INT_RIS_FALL);
					btn_handler.disableInterrupt(&btn_handler,0xF,INT_RIS_FALL);

				/* Serve le interrupt */
				APE_IRQHandler_0(ptr);
				printf("Switch: %08x  ",sw_handler.readStatus(&sw_handler));
				printf("Bottoni: %08x  ",btn_handler.readStatus(&btn_handler));
				printf("Led: %08x  ",led_handler.readStatus(&led_handler));

				/* Marca le interrupt come servite */
				//APE_writeValue32(ptr,APE_ICRISR_REG,BTN_ALL_MASK|SW_ALL_MASK);
			}

			nb = write(fd, &info, sizeof(info));
			if (nb < sizeof(info)) {
				perror("write");
				close(fd);
				exit(EXIT_FAILURE);
			}
		}
	}

	/*Unmap della periferica*/
	munmap(ptr, GPIO_MAP_SIZE);

	return 0;
}

/**
  * @brief  stampa la guida di utilizzo del driver
  * @param  None
  * @retval None
  */
void usage(){
	printf("\n\n *argv[0] -d <UIO_DEV_FILE> -t|-i|-o <VALUE>\n");
	printf("	-d				UIO device file. e.g. /dev/uio0\n");
	printf("	-i				Lettura dalla GPIO\n");
	printf("	-o <VALUE>		Scrittura verso la GPIO\n");
	return;
}

/**
  * @brief  stampa la schermata di presentazione
  * @param  None
  * @retval None
  */
void initScreen(void){
	printf("\n\n\n");
	printf("  	 █████╗ ██████╗ ███████╗\n");
	printf(" 	██╔══██╗██╔══██╗██╔════╝\n");
	printf(" 	███████║██████╔╝█████╗  \n");
	printf(" 	██╔══██║██╔═══╝ ██╔══╝  \n");
	printf(" 	██║  ██║██║     ███████╗\n");
	printf(" 	╚═╝  ╚═╝╚═╝     ╚══════╝\n\n\n");
}

/**
  * @brief  IRQ Handler della periferica GPIO_0
  */
void APE_IRQHandler_0(void* ptr){

	/* Legge il registro ISR */
	uint32_t state = APE_readValue32((uint32_t*)ptr,APE_ICRISR_REG);

	/* Azzera il registro ISR */
	APE_writeValue32((uint32_t*)ptr,APE_ICRISR_REG,0xFFFFFFFF);

	if((BTN0_MASK & state) == BTN0_MASK){
		APE_toggleBit((uint32_t*)ptr,APE_DATA_REG,LED0);
	}
	if((BTN1_MASK & state) == BTN1_MASK){
		APE_toggleBit((uint32_t*)ptr,APE_DATA_REG,LED1);
	}
	if((BTN2_MASK & state) == BTN2_MASK){
		APE_toggleBit((uint32_t*)ptr,APE_DATA_REG,LED2);
	}
	if((BTN3_MASK & state) == BTN3_MASK){
		APE_toggleBit((uint32_t*)ptr,APE_DATA_REG,LED3);
	}

	if((SW0_MASK & state) == SW0_MASK){
		APE_toggleBit((uint32_t*)ptr,APE_DATA_REG,LED0);
	}
	if((SW1_MASK & state) == SW1_MASK){
		APE_toggleBit((uint32_t*)ptr,APE_DATA_REG,LED1);
	}
	if((SW2_MASK & state) == SW2_MASK){
		APE_toggleBit((uint32_t*)ptr,APE_DATA_REG,LED2);
	}
	if((SW3_MASK & state) == SW3_MASK){
		APE_toggleBit((uint32_t*)ptr,APE_DATA_REG,LED3);
	}

	/* ----------------------------- */

}

