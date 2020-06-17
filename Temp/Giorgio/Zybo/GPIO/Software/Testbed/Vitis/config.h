#ifndef CONFIG_H
#define CONFIG_H

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


