
/**
 * @file config.h
 * @brief La liberia config.h definisce delle macro che potrebbero essere customizzate dall'utente \n
 *  @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
 *			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
 *			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
 * @date 15/06/2020
 * @{
 */
#ifndef CONFIG_H
#define CONFIG_H


#ifdef MYGPIO_UIO
/**
 * @name Macro UIO
 * @brief Associano le periferiche al file /def/uio specifico
//@{
 */
#define UIO_FILE_LED "/dev/uio0"
#define UIO_FILE_BTN "/dev/uio1"
#define UIO_FILE_SWT "/dev/uio2"
//@}
#endif

#ifdef MYGPIO_NO_DRIVER
/**
 * @name Macro no-driver
 * @brief Associano le periferiche al proprio indirizzo fisico
//@{
 */
//Button Peripheral base address
#define ADDR_BTN 0x43c10000
//Switch Peripheral base address
#define ADDR_SWT 0x43c20000
//Led Peripheral base address
#define ADDR_LED 0x43c00000
//@}
#endif

#ifdef MYGPIO_BARE_METAL
/**
 * @name Macro bare-metal
 * @brief Associano le periferiche al proprio indirizzo fisico e  linea di interruzione
//@{
 */
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
//@}
#endif

#ifdef MYGPIO_KERNEL
/**
 * @name Macro utilizzate nell'interfacciamento con il modulo kernel custom
 * @brief Associano le periferiche al proprio file /dev
//@{
 */
#define GPIO_LED_FILE "/dev/myGPIOK0"
#define GPIO_BUTTON_FILE "/dev/myGPIOK1"
//@}
#endif


/**
 * @name Macro Colori
 //@{
 */
#define COL(x) "\033[" #x ";1m"
#define COL_RED COL(31)
#define COL_GREEN COL(32)
#define COL_YELLOW COL(33)
#define COL_BLUE COL(34)
#define COL_WHITE COL(37)
#define COL_GRAY "\033[0m"
//@}

#endif

 /* @} */
