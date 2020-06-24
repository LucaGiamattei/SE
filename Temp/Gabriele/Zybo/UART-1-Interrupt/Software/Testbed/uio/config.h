#ifndef CONFIG_H
#define CONFIG_H

/**
* UIO device macros
**/

#define UIO_FILE_UART1 "/dev/uio0"
#define UIO_FILE_UART2 "/dev/uio1"

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


