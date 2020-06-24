/**
* @file main.c
* @brief Questo è un esempio di utilizzo della libreria @ref myuart.h
* @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
*			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
*			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
* @date 12/06/2020
*
* @details
*   In questo esempio è utilizzato il file .xsa generato dai file VHDL presenti in
*   Hardware/UART/VHDL. Si esegue una prima operazione di trasmissione da parte di UART1
*   e successivamente, dopo una breve attesa, una nuova trasmissione da parte da UART2.
*
* @{
*/


/***************************** Include Files *******************************/
#include "xparameters.h"
#include "xscugic.h"
#include "xil_printf.h"
#include "config.h"
#include "myuart.h"
#include "utils.h"

/***************************** Variables declaration *******************************/

myUART* uart1;
myUART* uart2;
XScuGic gic_inst;

/***************************** Functions prototype *******************************/
/**
 * @brief Funzione eseguita all'interno del while(1), vuota nello specifico caso
 * @{
 */
void loop (void);
/** @} */

/**
 * @brief Funzione di inizializzazione e configurazione delle periferiche
 * @{
 */
uint32_t setup (void);
/** @} */

/**
 * @brief Handler delle interruzioni di UART1
 * @{
 */
void uart1_transmit_IRQHandler(void*);
void uart1_receive_IRQHandler(void*);
/** @} */

/**
 * @brief Handler delle interruzioni di UART2
 * @{
 */
void uart2_transmit_IRQHandler(void*);
void uart2_receive_IRQHandler(void*);
/** @} */


/***************************** main *******************************/

int main(){

	setup();

	myUART_transmit_int(uart1, 'a');

	for(int i =0;i<100000000;i++);

	myUART_transmit_int(uart2, 'b');

	while(1){
		loop();
	}

	return 0;
}

/***************************** Functions definition *******************************/

void loop(){}


uint32_t setup(void){
	xil_printf("Setup \n");

	uart1 = myUART_init(UART1_BA);
	uart2 = myUART_init(UART2_BA);
	
	myUART_en_int(uart1, INT_EN);
	myUART_en_int(uart2, INT_EN);

	gic_enable(GIC_ID, &gic_inst);

	gic_register_interrupt(&gic_inst ,UART1_T, &uart1_transmit_IRQHandler);
	gic_register_interrupt(&gic_inst ,UART1_R, &uart1_receive_IRQHandler);
	gic_register_interrupt(&gic_inst ,UART2_T, &uart2_transmit_IRQHandler);
	gic_register_interrupt(&gic_inst ,UART2_R, &uart2_receive_IRQHandler);

	return 0;
}


/***************************** Interrupt Handlers *******************************/

void uart1_transmit_IRQHandler(void* data){

	xil_printf("scrive UART1\n");

	myUART_Iack_w(uart1);
}


void uart1_receive_IRQHandler(void* data){

	int valore_letto = myUART_read_DBOUT(uart1);

	xil_printf("riceve UART1: %c \r\n", valore_letto);

	myUART_Iack_r(uart1);

}

void uart2_transmit_IRQHandler(void* data){

	xil_printf("scrive UART2\n");

	myUART_Iack_w(uart2);
}


void uart2_receive_IRQHandler(void* data){

	int valore_letto = myUART_read_DBOUT(uart2);

	xil_printf("riceve UART2: %c \r\n", valore_letto);

	myUART_Iack_r(uart2);
}


/** @} */
