/**
* @file main.c
* @brief Questo è un esempio di utilizzo della libreria @ref mygpio.h sul S.O GNU/Linux in esecuzione sulla board Zybo . 
* Per utilizzare la libreria vi è bisogno dell'indirizzo a cui è mappata la periferica.
* Tramite un meccanismo di mapping sarà possibile ottenere un indirizzo virtuale, appartenente allo spazio di indrizzamento del processo, per comunicare con la periferica. 
* @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
*			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
*			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
* @date 15/06/2020
*
* @details E' possibile l'utilizzo delle funzioni della libreria mygpio.h grazie a un mapping che viene fatto della pagina fisica, 
* a cui è mappata la periferica GPIO custom, a una virtuale nello spazio di indirizzamento del processo.
* Tale esempio non può fare uso delle interruzione  non essendovi alcun modulo kernel
* ad occuparsi della periferica; per tale motivo questo modo di operare è definito driver no driver.
*
* @{
*/

/***************************** Include Files *******************************/

#include "mygpio.h"
#include "utils.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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

/**
* @brief Main di un semplice programma di test per accendere i led usando una 
* periferica GPIO costum
* @details 
* Riceve come parametro di ingresso l'opzione -w e il valore in hex da scrivere 
* sul registro write dei led.
* Uso: led_noDriver -w hex 
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

	void* vrt_page_addr;
	// indirizzo virtuale del device gpio nello spazio d'indirizzamento del processo
	void* vrt_gpio_addr = configure_no_driver(descriptor,&vrt_page_addr,ADDR_LED);
	
	//Inizializzazione del device in modalità scrittura
	myGPIO* led = myGPIO_init(vrt_gpio_addr);
	myGPIO_set_mode(led, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  WRITE_MODE);

	printf("Sto per scrivere sui led il valore %08x:", value);

	myGPIO_write_mask(led, value);

	//rimozione della pagain di memoria e chiusura del file /dev/mem
	munmap(vrt_page_addr, sysconf(_SC_PAGESIZE));
	close(descriptor);

	return 0;
}


/***************************** Functions definition *******************************/

void howto(void) {
	printf("Uso:\n");
	printf("\tled_noDriver -w <hex-value> \n");
}
 
int parse_args(int argc, char**argv, uint32_t*val){
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
