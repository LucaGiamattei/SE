#ifndef CONFIG_H
#define CONFIG_H


#ifdef MYGPIO_UIO
#define UIO_FILE_LED "/dev/uio0"
#define UIO_FILE_BTN "/dev/uio1"
#define UIO_FILE_SWT "/dev/uio2"
#endif

#ifdef MYGPIO_NO_DRIVER
//Button Peripheral base address
#define ADDR_BTN 0x43c10000
//Switch Peripheral base address
#define ADDR_SWT 0x43c20000
//Led Peripheral base address
#define ADDR_LED 0x43c00000
#endif

#ifdef MYGPIO_BARE_METAL
//#include "xparameters.h"
//Switch interrupt line
#define SWT_IRQN XPAR_FABRIC_MYGPIO_INT_SEL_2_INTERRUPT_INTR

//Button interrupt line
#define BTN_IRQN XPAR_FABRIC_MYGPIO_INT_SEL_1_INTERRUPT_INTR

//Button Peripheral base address
#define BTN_BA XPAR_MYGPIO_INT_SEL_1_S00_AXI_BASEADDR

//Switch Peripheral base address
#define SWT_BA XPAR_MYGPIO_INT_SEL_2_S00_AXI_BASEADDR

//Led Peripheral base address
#define LED_BA XPAR_MYGPIO_INT_SEL_0_S00_AXI_BASEADDR

//GIC Peripheral ID
#define GIC_ID XPAR_PS7_SCUGIC_0_DEVICE_ID
#endif

#ifdef MYGPIO_KERNEL
#define GPIO_LED_FILE "/dev/myGPIOK0"
#define GPIO_BUTTON_FILE "/dev/myGPIOK1"
#endif


/**
 *  colors macros 
 **/
#define COL(x) "\033[" #x ";1m"
#define COL_RED COL(31)
#define COL_GREEN COL(32)
#define COL_YELLOW COL(33)
#define COL_BLUE COL(34)
#define COL_WHITE COL(37)
#define COL_GRAY "\033[0m"

#endif


