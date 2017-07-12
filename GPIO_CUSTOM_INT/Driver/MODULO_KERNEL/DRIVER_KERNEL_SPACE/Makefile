obj-m := APE_GPIOK_main.o

KERNEL_SOURCE := /home/erasmo/Scrivania/SD_440_funzionante/linux-digilent/
PWD := $(shell pwd)
ARCH=arm
CROSS_COMPILE=arm-xilinx-linux-gnueabi-

default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
