/**
  ******************************************************************************
  * @file    noDriver.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V1.0
  * @date    02-Luglio-2017
  * @brief   
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "gpio_LL.h"

#define IN 0
#define OUT 1

void usage(void);

int main(int argc, char *argv[]){
	int c;
	int fd;
	int direction=IN;
	unsigned gpio_addr = 0;
	uint32_t value = 0;
	
	unsigned page_addr, page_offset;
	void *ptr;
	unsigned page_size=sysconf(_SC_PAGESIZE);

	printf("GPIO access through /dev/mem. %u\n", page_size);

	/* Parse command line arguements */
	while((c = getopt(argc, argv, "g:io:h")) != -1) {
		switch(c) {
		case 'g':
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
			printf("invalid option: %c\n", (char)c);
			usage();
			return -1;
		}
		
	}

	if (gpio_addr == 0) {
		printf("GPIO physical address is required.\n");
		usage();
		return -1;
	}
	
	/* Open /dev/mem file */
	fd = open ("/dev/mem", O_RDWR);
	if (fd < 1) {
		perror(argv[0]);
		return -1;
	}

	/* mmap the device into memory */
	/* The and operation clears lowest bits that expresses the a page section*/
	/* In a proper linux driver, you typically find the macro #define PAGE_MASK   (~(PAGE_SIZE-1))*/
	page_addr = (gpio_addr & (~(page_size-1)));

	/* we need the page offset in order to access the device*/
	page_offset = gpio_addr - page_addr;

	/* mmap call: physical address --> virtual one */
	ptr = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, page_addr);

	if (direction == IN) {
	/* Read value from the device register */
		value = APE_readValue32(ptr,page_offset);
		printf("gpio dev-mem test: input: %08x\n",value);
	} else {
	/* Write value to the device register */
		printf("Going to write onto %08x the value %08x\n", gpio_addr, value );
		APE_writeValue32(ptr,page_offset,value);
	}
	
	/* let's unmap */
	munmap(ptr, page_size);

	return 0;
}


void usage(){
	printf("*argv[0] -g <GPIO_ADDRESS> -i|-o <VALUE>\n");
	printf("    -g <GPIO_ADDR>   GPIO physical address\n");
	printf("    -i               Input from GPIO\n");
	printf("    -o <VALUE>       Output to GPIO\n");
	return;
}


