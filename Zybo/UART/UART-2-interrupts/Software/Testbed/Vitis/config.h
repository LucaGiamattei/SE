#ifndef CONFIG_H
#define CONFIG_H

//#include "xparameters.h"

//Uart1 transmission interrupt line
#define UART1_T 62U		//XPAR_FABRIC_MYUART_0_INTERRUPT_1_INTR

//Uart1 receive interrupt line
#define UART1_R 61U

//Uart2 transmission interrupt line
#define UART2_T 64U		//XPAR_FABRIC_MYUART_1_INTERRUPT_1_INTR

//Uart1 receive interrupt line
#define UART2_R 63U

//uart1 Peripheral base address
#define UART1_BA XPAR_MYUART_0_S00_AXI_BASEADDR

//uart2 Peripheral base address
#define UART2_BA XPAR_MYUART_1_S00_AXI_BASEADDR

//GIC Peripheral ID
#define GIC_ID XPAR_PS7_SCUGIC_0_DEVICE_ID

#endif
