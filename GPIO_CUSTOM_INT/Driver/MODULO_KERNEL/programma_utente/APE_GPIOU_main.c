/**
  ******************************************************************************
  * @file    APE_GPIOU_main.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V1.0
  * @date    03-Luglio-2017
  * @brief
  ******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>

//#include "button.h"
//#include "led.h"
//#include "switch.h"

/* Macro ---------------------------------------------------------------------*/


/* Typedef -------------------------------------------------------------------*/
typedef enum {
	IN,		/*!< Modalità in lettura dalla GPIO */
	OUT,	/*!< Modalità in scrittura verso la GPIO */
	//TEST	/*!< Modalità di test */
}direction;

/* Private function prototypes -----------------------------------------------*/
void usage(void);
void initScreen(void);

int main(int argc, char *argv[]){
	int c;
	int fd;
	int direction=IN;
	char *dev;
    uint32_t value;

	unsigned int info = 1;
	ssize_t nb;

	initScreen();

	while((c = getopt(argc, argv, "d:io:h")) != -1) {
		switch(c) {
		case 'd':
			dev=optarg;
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

	/* Open the device file */
	fd = open(dev, O_RDWR);
	if (fd < 1) {
		perror(argv[0]);
		printf("Invalid device file:%s.\n", dev);
		usage();
		return -1;
	}

	/* Lettura generica dalla GPIO */
	if (direction == IN) {
		printf("\n\n Modalità IN \n\n");
        fflush(stdout);

		/* Setta la direzione  TODO bei cazzi*/
		//APE_writeValue32(ptr,APE_DIR_REG,BTN_ALL_MASK|SW_ALL_MASK);

		for(;;){

			/* Attendi interrupt */
			nb = read(fd, &info, sizeof(info));

			if (nb > 0) {

				printf("Input: %u\n",info);
                fflush(stdout);
			}
		}
	}

	/* Scrittura generica verso la GPIO */
	if(direction == OUT) {
		printf("\n\n Modalità OUT, non faccio un cazzo \n\n");
	}

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
