#include "mygpio.h"
#include "utils.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

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

int read_value = 0;

void* create_shared_memory(size_t size);

int main (int argc, char** argv){
    //--------------------Shared memory---------------
    void* shmem = create_shared_memory(1);
    memcpy(shmem, "", sizeof(0));
    int pid = fork();

    //-------------------Fork--------------------
    if(pid == 0){
        
        printf("CIAO SONO IL FIGLIO SWITCH \n");

        //---------------Switch uio e mmap------------------
        int uio_swt_descriptor = open (UIO_FILE_SWT, O_RDWR);
        uint32_t page_size = sysconf(_SC_PAGESIZE);
        
	    if (uio_swt_descriptor < 1) {
		    printf("Errore nell'aprire il descrittore: %s\n", UIO_FILE_SWT);
		    return -1;
	    }
        
        printf("file aperto con successo descrittore: %d \n", uio_swt_descriptor);

	    void* vrt_gpio_addr3 = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, uio_swt_descriptor, 0);
	    if (vrt_gpio_addr3 == MAP_FAILED) {
	    	printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
	    	return -1;
    	}
        printf("mapping indirizzo avvenuto con successo indirizzo: %08x\n", vrt_gpio_addr3);

         //-----------------Configurazione device----------------
         myGPIO* swt = myGPIO_init(vrt_gpio_addr3);
         myGPIO_set_mode(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  READ_MODE);
         myGPIO_set_irq_mode(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,INT_EDGE );
         myGPIO_set_edge(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_RE);
         printf("SWT configurati \n\tDescirttore %d\n\taddress %08x\n",uio_swt_descriptor, swt);
        
        int32_t interrupt_count = 0 ;

        while(1){
            myGPIO_en_int(swt, INT_EN);
            myGPIO_en_pins_int(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_EN);
            printf("Attesa dell'interruzione sugli SWT\n");
            myGPIO_clear_irq(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
            
            if (read(uio_swt_descriptor, &interrupt_count, sizeof(uint32_t)) != sizeof(uint32_t)) {
                printf("Read error!\n");
                return 0;
            }
        
            printf("Interrupt count: %08x\n", interrupt_count);
            
            //leggo il valore dagli switch gpio    
            read_value = myGPIO_read(swt);
            
            // place data into memory
            printf("Child read: %d\n", *((uint32_t*) shmem));
            memcpy(shmem, &read_value, sizeof(read_value));
            printf("Child wrote: %d\n", *((uint32_t*) shmem));

            
            
           
            myGPIO_en_int(swt, INT_DIS);
            myGPIO_en_pins_int(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_DIS);
            
            printf("Lettura dal registro read del SWT: %08x\n", read_value);
            myGPIO_clear_irq(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
            
            uint32_t reenable = 1;
            printf("Read IRQ SWT (tra write e read): %08x\n",  myGPIO_read_irq(swt));
            if (write(uio_swt_descriptor, (void*)&reenable, sizeof(uint32_t)) != sizeof(uint32_t)) {
                printf("Write error!\n");
                return 0;
            }

            printf("Read IRQ SWT (dopo la write): %08x\n",  myGPIO_read_irq(swt));
            
           
            sleep(1);
        }

    }else if(pid > 0) { 
            printf("CIAO SONO IL PADRE LED \n");

            //-------------------Led uio e mmap----------------------------
            int uio_led_descriptor = open (UIO_FILE_LED, O_RDWR);
	        if (uio_led_descriptor < 1) {
		        printf("Errore nell'aprire il descrittore:%s \n",UIO_FILE_LED );
		        return -1;
	        }
            printf("file aperto con successo descrittore: %d \n",uio_led_descriptor );
            uint32_t page_size = sysconf(_SC_PAGESIZE);		
            void* vrt_gpio_addr  = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, uio_led_descriptor, 0);
        	if (vrt_gpio_addr == MAP_FAILED) {
	    	    printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
	    	    return -1;
	        }
            printf("mapping indirizzo avvenuto con successo indirizzo: %08x\n", vrt_gpio_addr);

            //------------------Configurazione device------------------------
            myGPIO* led = myGPIO_init(vrt_gpio_addr);
            printf("Led configurati \n\tDescirttore %d\n\taddress %08x\n",uio_led_descriptor, led);
            myGPIO_set_mode(led, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  WRITE_MODE);
            
            close(uio_led_descriptor);
            
            while(1){
               
                
                sleep(1);
                myGPIO_write_mask(led, *((uint32_t*) shmem));


                
            }
    }
    
    uint32_t read_value = 0;

	

	//munmap(led, page_size);
   // munmap(btn, page_size);
    //munmap(swt, page_size);

	//close(uio_led_descriptor);
   // close(uio_swt_descriptor);
	//close(uio_btn_descriptor);


    return 0;
}

int configure_uio_mygpio(char *filename, void* gpio){

    int file_descriptor = open (filename, O_RDWR);
	if (file_descriptor < 1) {
		printf("Errore nell'aprire il descrittore: %s\n", filename);
		return -1;
	}
    printf("file aperto con successo descrittore: %s \n", file_descriptor);
    uint32_t page_size = sysconf(_SC_PAGESIZE);		

	gpio = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0);
	if (gpio == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		return -1;
	}

    printf("mapping indirizzo avvenuto con successo indirizzo: %08x\n", gpio);

    return file_descriptor;
}



void* create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}