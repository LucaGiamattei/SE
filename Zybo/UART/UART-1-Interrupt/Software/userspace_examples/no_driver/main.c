/**
* @file main.c
* @brief Questo è un esempio di utilizzo della libreria @ref myuart.h sul S.O GNU/Linux in esecuzione sulla board Zybo . 
* Per utilizzare la libreria vi è bisogno dell'indirizzo a cui è mappata la periferica.
* Tramite un meccanismo di mapping sarà possibile ottenere un indirizzo virtuale, appartenente allo spazio di indrizzamento del processo, per comunicare con la periferica. 
* @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
*			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
*			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
* @date 15/06/2020
*
* @details E' possibile l'utilizzo delle funzioni della libreria myuart.h grazie a un mapping che viene fatto della pagina fisica, 
* a cui è mappata la periferica UART custom, a una virtuale nello spazio di indirizzamento del processo.
* Tale esempio non può fare uso delle interruzione  non essendovi alcun modulo kernel
* ad occuparsi della periferica; per tale motivo questo modo di operare è definito driver no driver.
*
* @{
*/

/***************************** Include Files *******************************/

#include "myuart.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "stdint.h"
#include "config.h"



/***************************** Functions prototype *******************************/

/**
 * @brief Stampa un messaggio che fornisce indicazioni sull'utilizzo del programma
 */
void howto(void);


/**
 * @brief Effettua il parsing degli argomenti
 *
 * @details Il parsing viene effettuato usando la funzione getopt().
 * @code
 * #include <unistd.h>
 * int getopt(int argc, char * const argv[], const char *optstring);
 * @endcode
 * Per maggiori informazioni: https://linux.die.net/man/3/getopt .
 *
 ** <h4>Parametri riconosciuti</h4>
 *  I parametri riconosciuti sono:
 *  - 'w' : operazione di scrittura, seguito dal valore che si intende scrivere, in esadecimale; 
 */

int parse_args(int argc, char**argv, uint32_t	*val);

/***************************** MAIN *********************************************/


myUART* uart1;
myUART* uart2;

/**
* @brief Main di un semplice programma di test dell'interfaccia seriale uart.
* @details Il primo Uart invia il carattere ricevuto in ingresso dal terminale
* e il secondo uart stampa sul terminale il carattere ricevuto (ricezione con polling) sull'interfaccia seriale di ricezione. \n
* Riceve come parametro di ingresso l'opzione -w e il valore in hex da inviare al secondo uart. 
* Uso: uart_noDriver -w hex 
*/
int main(int argc, char** argv){

	uint32_t value;

	if(parse_args(argc, argv, &value) == -1){
		return -1;	
	}

	//Apertura del file che rappresenta "la memoria fisica"
	int descriptor = open ("/dev/mem", O_RDWR);
	if (descriptor < 1) {
		perror(argv[0]);
		return -1;
	}
	
	void* vrt_page_addr_uart1;
	void* vrt_page_addr_uart2;
	
    //UART1: ottengo l'indrizzo della pagina virtuale e l'indirizzo virtuale base della periferica
	//tenendo conto dell'offset
    void* vrt_uart1_addr = configure_no_driver(descriptor,&vrt_page_addr_uart1,UART1_ADDR);
	//UART2: ottengo l'indrizzo della pagina virtuale e l'indirizzo virtuale base della periferica
	//tenendo conto dell'offset
    void* vrt_uart2_addr = configure_no_driver(descriptor,&vrt_page_addr_uart2,UART2_ADDR);
	//Inizializzazione dell'uart1
	uart1 = myUART_init((uint32_t*)vrt_uart1_addr);
	//Inizializzazione dell'uart2
    uart2 = myUART_init((uint32_t*)vrt_uart2_addr);
    //invio del carattere sull'uart1
    myUART_transmit(uart1, value);
    printf("%s[UART1]%s Sto per scrivere il valore: %08x\n",COL_GREEN,COL_GRAY, value);
    //lettura del carattere dall'uart2 con polling sul registro di stato
    uint32_t state = 0;
    uint8_t read_value = myUART_read(uart2, & state);
    printf("%s[UART2]%s ho letto il valore: %08x\n",COL_YELLOW,COL_GRAY, read_value);
	//rimozione della pagain di memoria e chiusura del file /dev/mem
	munmap(vrt_uart1_addr, sysconf(_SC_PAGESIZE));
    munmap(vrt_uart2_addr, sysconf(_SC_PAGESIZE));
	close(descriptor);

	return 0;
}

/***************************** Functions definition *******************************/

void howto(void) {
	printf("Uso:\n");
	printf("\tled_noDriver -w <hex-value> \n");
}
 
int parse_args(int argc, char**argv, uint32_t	*val){
	int par;
    if(argc > 2){
        while((par = getopt(argc, argv, "w:")) != -1) {
            switch (par) {
                case 'w' :
                    *val = strtoul(optarg, NULL, 0);
                    break;
                default :
                    printf("%c: Parametro Sconosciuto.\n", par);
                    howto();
                    return -1;
            }
        }
    }else{
        howto();
    }
	return 0;
}

/** @} */
