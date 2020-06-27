TARGET = myUARTK
OBJS = myUARTK_main.o myUARTK_t.o myUARTK_list.o


obj-m += $(TARGET).o
$(TARGET)-y += $(OBJS)
KERNEL_SOURCE := /home/giorgio/Documents/embeddedProject/progetti/SE/SE/Temp/Gabriele/Zybo/UART/UART-1-Interrupt/Software/Driver/kernel/linux-digilent/
PWD := $(shell pwd)
ARCH=arm
#CROSS_COMPILE=arm-linux-gnueabihf-
CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabihf-
CFLAGS_myUARTK.o := -DBUILD=$(BUILD_N)

default:
	@echo "BUILD ID : " $(BUILD_N)
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
