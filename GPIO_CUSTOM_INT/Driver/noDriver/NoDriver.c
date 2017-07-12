/**
  ******************************************************************************
  * @file    noDriver.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V1.0
  * @date    03-Luglio-2017
  * @brief   File del Driver UIO user-space per linux della periferica APE_GPIO.
  *
  *	@addtogroup DRIVER
  * @{
  * @addtogroup NO_DRIVER
  * @{
  * @brief   Driver user-space per linux della periferica APE_GPIO.
  * @details Il driver presenta un approccio rudimentale di gestione dell'hardware
  *			 mediante la syscall mmap, si hanno 3 modalità di esecuzione:
  * 		 - IN: generica lettura dei registri della periferica.
  * 		 - OUT: generica scrittura verso i registri della periferica.
  * 		 - TEST: il driver testa le interrupt effettuando il toggle di un led
  * 		 	 alla pressione/rilascio di un bottone e/o toggling di uno switch.
  *
  * La modalita' di esecuzione e selezionata in base agli argomenti forniti a riga
  * di comando. La modalita' di default e <b>TEST</b>, altrimenti utilizzare:
  * - i: per la modalita' <b>IN</b>.
  * - o: per la modalita' <b>OUT</b>.
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

/* Typedef -------------------------------------------------------------------*/
typedef enum {
	IN,		/*!< Modalità in lettura dalla GPIO */
	OUT,	/*!< Modalità in scrittura verso la GPIO */
	TEST	/*!< Modalità di test */
}direction;

/* Private function prototypes -----------------------------------------------*/
void usage(void);
void initScreen(void);

int main(int argc, char *argv[]){
	int c;
	int fd;
	int direction=TEST;
	unsigned gpio_addr;
	uint32_t value = 0;

	unsigned page_addr, page_offset;
	void *ptr;
	unsigned page_size=sysconf(_SC_PAGESIZE);

	initScreen();

	while((c = getopt(argc, argv, "a:io:h")) != -1) {
		switch(c) {
		case 'a':
			gpio_addr=strtoul(optarg,NULL, 0);
			break;
		case 'i':
			direction=IN;
			break;
		case 'o':
			direction=OUT;
			value=strtoul(optarg, NULL, 0);
			break;
		case 'h':
			usage();
			return 0;
		default:
			printf("Errore: %c\n", (char)c);
			usage();
			return -1;

	}

	/* Invoca la open sul mem file */
	fd = open("/dev/mem", O_RDWR);
	if (fd < 1) {
		perror(argv[0]);
		return -1;
	}

	/* Calcola l'indirizzo della pagina di memoria*/
	page_addr = (gpio_addr & (~(page_size-1)));

	/* Calcola il page offset*/
	page_offset = gpio_addr - page_addr;

	/* Effettua il mapping degli indirizzi fisici-virtuali */
	ptr = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, page_addr);

	/* Lettura generica dalla periferica */
	if (direction == IN) {
		printf("\n\n Modalità IN \n\n");

		/* Setta la direzione */
		APE_writeValue32(ptr,APE_DIR_REG,BTN_ALL_MASK|SW_ALL_MASK);

		/* Legge il valore*/
		value = APE_readValue32(ptr,page_offset);
		printf("Input: %08x\n",value);
	}

	/* Scrittura generica verso la periferica */
	if(direction == OUT) {
		printf("\n\n Modalità OUT \n\n");

		/* Setta la direzione */
		APE_writeValue32(ptr,APE_DIR_REG,0x000);

		/* Scrive il valore*/
		APE_writeValue32(ptr,APE_DATA_REG,value);
	}

	/* Modalità di test */
	if(direction == TEST) {
		printf("\n\n Modalità TEST \n\n");

		/*Dichiarazione degli handler*/
		btn_t btn_handler;
		switch_t sw_handler;
		led_t led_handler;
		uint32_t sw_status = 0x0;
		uint32_t btn_status = 0x0;

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

			/*Scrive sui led il valore degli switch in and con quello dei bottoni*/
			sw_status = sw_handler.readStatus(&sw_handler);
			btn_status = btn_handler.readStatus(&btn_handler);
			led_handler.setLeds(&led_handler,sw_status & btn_status);
		}
	}

	/*Unmap della periferica*/
	munmap(ptr, page_size);

	return 0;
}

/**
  * @brief  stampa la guida di utilizzo del driver
  * @param  None
  * @retval None
  */
void usage(){
	printf("\n\n *argv[0] -a <INDIRIZZO> -i|-o <VALUE>\n");
	printf("	-a				Indirizzo base periferica. e.g. 0x43C00000\n");
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

/**@}*/
/**@}*/
