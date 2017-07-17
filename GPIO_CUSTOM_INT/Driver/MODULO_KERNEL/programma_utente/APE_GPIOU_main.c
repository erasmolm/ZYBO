/**
  ******************************************************************************
  * @file    APE_GPIOU_main.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V1.0
  * @date    16-Luglio-2017
  * @brief	File che implementa il modulo KERNEL_USERSPACE
  *	@addtogroup DRIVER
  * @{
  * @addtogroup KERNEL_USERSPACE
  * @{
  * @brief Programma userspace che utilizza le funzioni del modulo KERNEL.
  * @details Il programma testa semplicemente le funzioni fornite dal driver
  *			 kernel-space.
  *			 Per questa particolare implementazione l'hardware è composto da 2
  *			periferiche APE_GPIO, pertanto saranno presenti 2 file sotto /dev:
  *			- /dev/APE_GPIO_0
  *			- /dev/APE_GPIO_1
  *			Su APE_GPIO_0 sono mappati gli switch sul primo nibble e i led sul secondo.
  *			Su APE_GPIO_1 sono mappati solo i bottoni.
  *			Il programma opera in due modalità:
  *			- IN: viene effettuata una lettura interrompente sul dispositivo, in
  *				uscita verra' riportato il valore del registro specificato da OFFSET.
  *			- OUT: viene effettuata una scrittura del valore <VALUE> sul registro
  *				indicato da OFFSET.
  *			Le operazioni di lettura e scrittura sono eseguite mediante pread e pwrite.
  *			Queste funzioni permettono di specificare un offset su cui spiazzare
  *			l'operazione. L'utilizzo di queste due funzioni permette di non utilizzare
  *			la funzione lseek, non presente nel modulo kernel.
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
    uint32_t pos = 0;
	ssize_t nb;

	initScreen();

	while((c = getopt(argc, argv, "d:io:p:h")) != -1) {
		switch(c) {
		case 'd':
			dev=optarg;
			break;
		case 'i':
			direction=IN;
			break;
		case 'o':
			direction=OUT;
			value = strtoul(optarg, NULL, 0);
			break;
		case 'p':
			pos = strtoul(optarg, NULL, 0);
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

	/* Apre il device file */
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

		/* Attendi interrupt */
		nb = pread(fd, &value, sizeof(value),(off_t)pos);

		if (nb > 0) {
			printf("Input: %8x\n",value);
			fflush(stdout);
		}

	}

	/* Scrittura generica verso la GPIO */
	if(direction == OUT) {
		printf("\n\n Modalità OUT\n\n");

		nb = pwrite(fd,(const void*)&value, sizeof(uint32_t), (off_t)pos);

		printf("Valore inserito: %8x\n",value);
	}

	return 0;
}

/**
  * @brief  stampa la guida di utilizzo del driver
  * @param  None
  * @retval None
  */
void usage(){
	printf("\n\n *argv[0] -d <UIO_DEV_FILE> -i|-o <VALORE> | -p <OFFSET> \n");
	printf("	-d				Device file. e.g. /dev/APE_GPIOK_0\n");
	printf("	-i				Lettura dalla GPIO\n");
	printf("	-o <VALORE>		Scrittura verso la GPIO\n");
	printf("	-p <OFFSET>		Device file. e.g. /dev/APE_GPIOK_0\n");
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
