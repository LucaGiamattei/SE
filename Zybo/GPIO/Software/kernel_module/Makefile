TARGET = myGPIOK
OBJS = myGPIOK_main.o myGPIOK_t.o myGPIOK_list.o


obj-m += $(TARGET).o
$(TARGET)-y += $(OBJS)
KERNEL_SOURCE := /home/jiin995/Linux-on-Zynq/linux-digilent/
PWD := $(shell pwd)
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
CFLAGS_myGPIOK.o := -DBUILD=$(BUILD_N)

default:
	@echo "BUILD ID : " $(BUILD_N)
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
