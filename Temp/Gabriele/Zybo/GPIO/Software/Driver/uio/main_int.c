#include "mygpio.h"
#include "utils.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define UIO_FILE_LED "/dev/uio0"
#define UIO_FILE_BTN "/dev/uio1"
#define UIO_FILE_SWT "/dev/uio2"

/**
 * @brief Stampa un messaggio che fornisce indicazioni sull'utilizzo del programma
 */
void howto(void) {
 printf("L'interruzione sul fronte di salita di SW0 accende tutti i led,\n");
 printf("mentre l'interruzione du BTN0 li spegne.\n");

 printf("\t\n");
}

int configure_uio_mygpio(char * filename, void* gpio);



int main (int argc, char** argv){
    myGPIO* led;
    myGPIO* btn;
    myGPIO* swt;
	//int uio_led_descriptor = configure_uio_mygpio(UIO_FILE_LED, (void*) led);

    
    int uio_led_descriptor = open (UIO_FILE_LED, O_RDWR);
	if (uio_led_descriptor < 1) {
		printf("Errore nell'aprire il descrittore: %s\n", UIO_FILE_LED);
		return -1;
	}
    printf("file aperto con successo descrittore: %s \n", uio_led_descriptor);
    uint32_t page_size = sysconf(_SC_PAGESIZE);		// dimensione della pagina, la prendo dal sistema

	void* vrt_gpio_addr  = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, uio_led_descriptor, 0);
	if (vrt_gpio_addr == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		return -1;
	}

    printf("mapping indirizzo avvenuto con successo indirizzo: %08x\n", vrt_gpio_addr);
    led = myGPIO_init(vrt_gpio_addr);
    printf("Led configurati \n\tDescirttore %d\n\taddress %08x\n",uio_led_descriptor, led);

    
    int uio_btn_descriptor = open (UIO_FILE_BTN, O_RDWR);
	if (uio_btn_descriptor < 1) {
		printf("Errore nell'aprire il descrittore: %s\n", UIO_FILE_BTN);
		return -1;
	}
    printf("file aperto con successo descrittore: %s \n", uio_btn_descriptor);

	vrt_gpio_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, uio_btn_descriptor, 0);
	if (btn == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		return -1;
	}

    printf("mapping indirizzo avvenuto con successo indirizzo: %08x\n", vrt_gpio_addr);

	//int uio_btn_descriptor = configure_uio_mygpio(UIO_FILE_BTN, (void*) btn);
    btn = myGPIO_init(vrt_gpio_addr);
    printf("Btn configurati \n\tDescirttore %d\n\taddress %08x\n",uio_btn_descriptor, btn);



    int uio_swt_descriptor = open (UIO_FILE_SWT, O_RDWR);
	if (uio_swt_descriptor < 1) {
		printf("Errore nell'aprire il descrittore: %s\n", UIO_FILE_BTN);
		return -1;
	}
    printf("file aperto con successo descrittore: %s \n", uio_swt_descriptor);

	vrt_gpio_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, uio_swt_descriptor, 0);
	if (swt == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		return -1;
	}

    printf("mapping indirizzo avvenuto con successo indirizzo: %08x\n", vrt_gpio_addr);

	//int uio_swt_descriptor = configure_uio_mygpio(UIO_FILE_SWT, (void*) swt);
    swt = myGPIO_init(vrt_gpio_addr);
    printf("SWT configurati \n\tDescirttore %d\n\taddress %08x\n",uio_swt_descriptor, swt);

    //Configurazione dei led
    myGPIO_set_mode(led, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  WRITE_MODE);

	//Configurazione del switch
	myGPIO_set_mode(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  READ_MODE);
   
    myGPIO_set_irq_mode(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_EDGE);
    myGPIO_set_edge(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_RE);

    //Configurazione del bottone
	myGPIO_set_mode(btn, GPIO_PIN_0,  READ_MODE);
    myGPIO_en_int(btn, INT_EN);
    myGPIO_en_pins_int(btn, GPIO_PIN_0, INT_EN);
    myGPIO_set_irq_mode(btn, GPIO_PIN_0, INT_EDGE);
    myGPIO_set_edge(btn, GPIO_PIN_0, INT_FE);

    printf("Tutti i dispositivi sono configurati correttamente \n\n");
    int pid = fork();

    if(pid == 0){
        //Processo che setta i led con gli swt
        while(1){
            myGPIO_en_int(swt, INT_EN);
            myGPIO_en_pins_int(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_EN);
            printf("Attesa dell'interruzione sugli SWT\n");
            int32_t interrupt_count = 1;

            if (read(uio_swt_descriptor, &interrupt_count, sizeof(uint32_t)) != sizeof(uint32_t)) {
                printf("Read error!\n");
                return 0;
            }
        
            printf("Interrupt count: %08x\n", uio_swt_descriptor);
                // disabilitazione interrupt (interni alla periferica)

            int read_value = myGPIO_read(swt);

            myGPIO_write_mask(led, read_value);

            myGPIO_clear_irq(swt, read_value);
            myGPIO_en_int(swt, INT_DIS);
            myGPIO_en_pins_int(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_DIS);
            
            printf("Lettura dat registro read del SWT: %08x\n", read_value);

            uint32_t reenable = 1;
                if (write(uio_swt_descriptor, (void*)&reenable, sizeof(uint32_t)) != sizeof(uint32_t)) {
                    printf("Write error!\n");
                    return 0;
                }
        }

    }else if(pid > 0) { 
        //processo che resetta i led con i btn
            while(1){
                myGPIO_en_int(btn, INT_EN);
                myGPIO_en_pins_int(btn, GPIO_PIN_0, INT_EN);
                printf("Attesa dell'interruzione sugli BTN\n");
                int32_t interrupt_count = 1;

                if (read(uio_btn_descriptor, &interrupt_count, sizeof(uint32_t)) != sizeof(uint32_t)) {
                    printf("Read error!\n");
                    return 0;
                }
            
                printf("Interrupt count BTN: %08x\n", uio_btn_descriptor);
                    // disabilitazione interrupt (interni alla periferica)

                int read_value = myGPIO_read(btn);

                myGPIO_write_mask(led, ~read_value);

                myGPIO_clear_irq(btn, read_value);
                myGPIO_en_int(btn, INT_DIS);
                myGPIO_en_pins_int(btn, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_DIS);
                
                printf("Lettura dat registro read del BTN: %08x\n", read_value);

                uint32_t reenable = 1;
                    if (write(uio_btn_descriptor, (void*)&reenable, sizeof(uint32_t)) != sizeof(uint32_t)) {
                        printf("Write error!\n");
                        return 0;
                    }
            }
    }
    uint32_t read_value = 0;

	//uint32_t page_size = sysconf(_SC_PAGESIZE);		// dimensione della pagina, la prendo dal sistema

	munmap(led, page_size);
    munmap(btn, page_size);
	munmap(swt, page_size);

	close(uio_led_descriptor);
    close(uio_swt_descriptor);
	close(uio_btn_descriptor);


    return 0;
}

int configure_uio_mygpio(char *filename, void* gpio){

    int file_descriptor = open (filename, O_RDWR);
	if (file_descriptor < 1) {
		printf("Errore nell'aprire il descrittore: %s\n", filename);
		return -1;
	}
    printf("file aperto con successo descrittore: %s \n", file_descriptor);
    uint32_t page_size = sysconf(_SC_PAGESIZE);		// dimensione della pagina, la prendo dal sistema

	gpio = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0);
	if (gpio == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		return -1;
	}

    printf("mapping indirizzo avvenuto con successo indirizzo: %08x\n", gpio);

    return file_descriptor;
}