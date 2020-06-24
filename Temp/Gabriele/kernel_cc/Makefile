TARGET = myGPIOK
OBJS = myGPIOK.o 

obj-m += $(TARGET).o
KERNEL_SOURCE := /home/jiin995/Linux-on-Zynq/linux-digilent
PWD := $(shell pwd)
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

obj-m += main.o

all:
	make -C ${KERNEL_SOURCE} M=$(PWD) modules

clean:
	make -C ${KERNEL_SOURCE} M=$(PWD) clean