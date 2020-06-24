/**
* @file main.c
* @brief Questo è un esempio di utilizzo del driver uio per pilotare una
* periferica GPIO AXI Lite costum su un S.O. GNU/Linux 
* @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
*			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
*			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
* @date 15/06/2020
*
* @details
*   In questo esempio e' utilizzata la periferica generata dai file VHDL presenti in
*   Hardware/GPIO/VHDL.
* 	Nota per utilizzare correttamente la periferica è necessario generare il dtb 
* 	seguento quanto riportato nella documentazione a corredo di questi esempi, 
* 	abilitare il supporto dei driver uio e abilitare la compatibilità della periferica
* 	con il driver uio.
*	Nell'esempio si utilizzano i led in modalita' WRITE per accendere i led in base al
*	parametro che si riceve in input.
*
*
* @{
*/

/***************************** Include Files *******************************/

#include "mygpio.h"
#include "utils.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define UIO_FILE_LED "/dev/uio0"

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
* periferica GPIO costum utilizzando il driver uio
* @details Main di un semplice programma di test per accendere i led usando 
* una periferica GPIO costum utilizzando il driver uio.
* Riceve come parametro di ingresso l'opzione -w e il valore in hex da scrivere 
* sul registro write dei led.
* Uso: led_noDriver -w hex 
*/


int main (int argc, char** argv){

	uint32_t value;

	if(parse_args(argc, argv, &value) == -1){
		return -1;	
	}

	//Apertura del file che rappresenta "il dispositivo GPIO collegato ai led"
	int descriptor = open (UIO_FILE_LED, O_RDWR);
	if (descriptor < 1) {
		perror(argv[0]);
		return -1;
	}
	
	// dimensione della pagina di memoria
	uint32_t page_size = sysconf(_SC_PAGESIZE);		

	// mapping della pagina fisica, contenente il nostro device, nello spazio d'indirizzamento del processo
	void* vrt_page_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
	if (vrt_page_addr == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		return -1;
	}

	// indirizzo virtuale del device gpio nello spazio d'indirizzamento del processo
	void* vrt_gpio_addr = vrt_page_addr;	// indirizzo virtuale del device gpio
	
	//Inizializzazione del device in modalità scrittura
	myGPIO* led = myGPIO_init(vrt_gpio_addr);
	myGPIO_set_mode(led, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  WRITE_MODE);


	printf("Sto per scrivere sui led il valore %08x:", value);

	myGPIO_write_mask(led, value);

	munmap(vrt_page_addr, page_size);
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
