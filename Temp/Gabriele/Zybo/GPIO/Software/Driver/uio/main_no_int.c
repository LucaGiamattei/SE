#include "mygpio.h"
#include "utils.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define UIO_FILE_LED "/dev/uio0"
#define UIO_FILE_SWT ""
#define UIO_FILE_BTN ""

/**
 * @brief Stampa un messaggio che fornisce indicazioni sull'utilizzo del programma
 */
void howto(void) {
 printf("Uso:\n");
 printf("noDriver -a gpio_phisycal_address -w|m <hex-value> -r\n");
 printf("\t-m <hex-value>: scrive nel registro \"mode\"\n");
 printf("\t-w <hex-value>: scrive nel registro \"write\"\n");
 printf("\t-r: legge il valore del registro \"read\"\n");
 printf("I parametri possono anche essere usati assieme.\n");
}



int main (int argc, char** argv){
	int descriptor = open (UIO_FILE_LED, O_RDWR);
	if (descriptor < 1) {
		perror(argv[0]);
		return -1;
	}
	
	uint32_t page_size = sysconf(_SC_PAGESIZE);		// dimensione della pagina, la prendo dal sistema

	void* vrt_page_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
	if (vrt_page_addr == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		return -1;
	}
	void* vrt_gpio_addr = vrt_page_addr;	// indirizzo virtuale del device gpio
	
	//myGPIO_write_mask(vrt_gpio_addr, *((uint32_t*)argv[0]));
	myGPIO* led = myGPIO_init(vrt_gpio_addr);
	myGPIO_set_mode(led, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  WRITE_MODE);
	char* p;
	int value = strtol(argv[1],&p,10);
	printf(" sto per scriver sul pin %d",value );
	myGPIO_write_mask(led, value);

	munmap(vrt_page_addr, page_size);
	close(descriptor);

return 0;
}
