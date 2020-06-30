
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

#ifdef MYUART_KERNEL
/**
 * @name Macro utilizzate nell'interfacciamento con il modulo kernel custom
 * @brief Associano le periferiche al proprio file /dev
//@{
 */
#define UART_1_FILE "/dev/myUARTK0"
#define UART_2_FILE "/dev/myUARTK1"
//@}
#endif

#ifdef MYUART_NO_DRIVER
/**
 * @name Macro no-driver
 * @brief Associano le periferiche al proprio indirizzo fisico
//@{
 */
#define UART1_ADDR 0x43c00000
#define UART2_ADDR 0x43c10000
//@}
#endif

#ifdef MYUART_UIO
/**
 * @name Macro UIO
 * @brief Associano le periferiche al file /def/uio specifico
//@{
 */
#define UIO_FILE_UART1 "/dev/uio0"
#define UIO_FILE_UART2 "/dev/uio1"
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

