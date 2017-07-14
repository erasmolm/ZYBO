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
#include <inttypes.h>

/* Typedef -------------------------------------------------------------------*/
typedef enum {
	IN,		/*!< Modalità in lettura dalla GPIO */
	OUT,	/*!< Modalità in scrittura verso la GPIO */
}direction;

/* Private function prototypes -----------------------------------------------*/
void usage(void);
void initScreen(void);

int main(int argc, char *argv[]){
	int c;
	int fd;
	int direction=IN;
	char *dev;
    uint32_t value = 0;
	ssize_t nb;

	initScreen();

	while((c = getopt(argc, argv, "d:ioh")) != -1) {
		switch(c) {
		case 'd':
			dev=optarg;
			break;
		case 'i':
			direction=IN;
			break;
		case 'o':
			direction=OUT;
			break;
		case 'h':
			usage();
			return 0;
		default:
			printf("Argomenti non validi: %c\n", (char)c);
			usage();
			return -1;
		}

	}

	/* Open the device file */
	fd = open(dev, O_RDWR);
	if (fd < 1) {
		perror(argv[0]);
		printf("Device file non valido:%s.\n", dev);
		usage();
		return -1;
	}

	/* Lettura generica dalla GPIO */
	if (direction == IN) {
		printf("\n\n Modalità IN \n\n");
        fflush(stdout);

		for(;;){

			/* Attendi interrupt */
			nb = read(fd, &value, sizeof(value));

			if (nb > 0) {

				printf("Input: %8x\n",value);
                fflush(stdout);
			}
		}
	}

	/* Scrittura generica verso la GPIO */
	if(direction == OUT) {
		printf("\n\n Modalità OUT\n\n");
		fflush(stdout);

		printf("Inserire un valore: ");
		while(scanf(" %8x",&value) == 1){
			nb = write(fd, &value, sizeof(value));

			if (nb > 0) {
				printf("Valore inserito: %8x\n",value);
			}
			printf("\nInserire un valore: ");
		}
	}

	return 0;
}

/**
  * @brief  stampa la guida di utilizzo del driver
  * @param  None
  * @retval None
  */
void usage(){
	printf("\n\n *argv[0] -d <UIO_DEV_FILE> -i|-o \n");
	printf("	-d				Device file. e.g. /dev/APE_GPIOK\n");
	printf("	-i				Lettura dalla GPIO\n");
	printf("	-o 				Scrittura verso la GPIO\n");
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
