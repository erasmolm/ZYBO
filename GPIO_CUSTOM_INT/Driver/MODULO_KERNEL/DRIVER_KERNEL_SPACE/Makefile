TARGET = APE_GPIOK
OBJS = APE_GPIOK_main.o

obj-m += $(TARGET).o
$(TARGET)-y += $(OBJS)

KERNEL_SOURCE := /home/erasmo/Scrivania/SD_440_funzionante/linux-digilent/
PWD := $(shell pwd)

ARCH=arm
CROSS_COMPILE=arm-xilinx-linux-gnueabi-

default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
