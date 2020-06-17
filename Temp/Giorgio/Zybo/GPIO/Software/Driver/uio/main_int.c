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
/* colors macros */
#define COL(x) "\033[" #x ";1m"
#define COL_RED COL(31)
#define COL_GREEN COL(32)
#define COL_YELLOW COL(33)
#define COL_BLUE COL(34)
#define COL_WHITE COL(37)
#define COL_GRAY "\033[0m"


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
        
        printf("%s[SWT]%s sono il figlio \n",COL_BLUE,COL_GRAY);

        //---------------Switch uio e mmap------------------
        int uio_swt_descriptor = open (UIO_FILE_SWT, O_RDWR);
        uint32_t page_size = sysconf(_SC_PAGESIZE);
        
	    if (uio_swt_descriptor < 1) {
		    printf("%s[SWT]%s Errore nell'aprire il descrittore: %s \n",COL_BLUE,COL_GRAY, UIO_FILE_SWT);
		    return -1;
	    }
        
        printf("%s[SWT]%s file aperto con successo descrittore: %d \n",COL_BLUE,COL_GRAY, uio_swt_descriptor);

	    void* vrt_gpio_addr3 = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, uio_swt_descriptor, 0);
       

	    if (vrt_gpio_addr3 == MAP_FAILED) {
	    	printf("%s[SWT]%s Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n",COL_BLUE,COL_GRAY);
	    	return -1;
    	}
        printf("%s[SWT]%s mapping indirizzo avvenuto con successo indirizzo: %08x\n",COL_BLUE,COL_GRAY, vrt_gpio_addr3);

         //-----------------Configurazione device----------------
         myGPIO* swt = myGPIO_init(vrt_gpio_addr3);
         myGPIO_set_mode(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  READ_MODE);
         myGPIO_set_irq_mode(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,INT_EDGE );
         myGPIO_set_edge(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_RE);
         printf("%s[SWT]%s configurazione switch \n\tDescrittore %d\n\taddress %08x\n",COL_BLUE,COL_GRAY,uio_swt_descriptor, swt);
        
        int32_t interrupt_count = 1 ;
        int32_t reenable =1;
        myGPIO_en_int(swt, INT_EN);
        myGPIO_en_pins_int(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_EN);
        
        while(1){
            
            if (read(uio_swt_descriptor, &interrupt_count, sizeof(uint32_t)) != sizeof(uint32_t)) {
                printf("%sSWT]%s Read error!\n",COL_BLUE,COL_GRAY);
                return 0;
            }
            if (interrupt_count > 1){
                //leggo il valore dagli switch gpio    
                read_value = myGPIO_read(swt);
                // place data into memory
                printf("%s[SWT]%s read: %d\n",COL_BLUE,COL_GRAY, *((uint32_t*) shmem));
                memcpy(shmem, &read_value, sizeof(read_value));
                printf("%s[SWT]%s wrote: %d\n",COL_BLUE,COL_GRAY, *((uint32_t*) shmem));
              
            }
            //IACK
            myGPIO_clear_irq(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
           
            if (write(uio_swt_descriptor, (void*)&reenable, sizeof(uint32_t)) != sizeof(uint32_t)) {
                printf("%s[SWT]%s Write error!\n",COL_BLUE,COL_GRAY);
                return 0;
            }
            sleep(1);
        }

    }else if(pid > 0) { 
            
            printf("%s[LED]%s Sono il padre \n",COL_RED,COL_GRAY);

            //-------------------Led uio e mmap----------------------------
            int uio_led_descriptor = open (UIO_FILE_LED, O_RDWR);
	        if (uio_led_descriptor < 1) {
		        printf("%s[LED]%s Errore nell'aprire il descrittore:%s \n", COL_RED, COL_GRAY, UIO_FILE_LED );
		        return -1;
	        }
            printf("%s[LED]%s file aperto con successo descrittore: %d \n", COL_RED, COL_GRAY, uio_led_descriptor );
            uint32_t page_size = sysconf(_SC_PAGESIZE);		
            void* vrt_gpio_addr  = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, uio_led_descriptor, 0);
        	if (vrt_gpio_addr == MAP_FAILED) {
	    	    printf("%s[LED]%s Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n",COL_RED, COL_GRAY);
	    	    return -1;
	        }
            printf("%s[LED]%s mapping indirizzo avvenuto con successo indirizzo: %08x\n", COL_RED, COL_GRAY, vrt_gpio_addr);

            //------------------Configurazione device------------------------
            myGPIO* led = myGPIO_init(vrt_gpio_addr);
            printf("%s[LED]%s Led configurati \n\tDescirttore %d\n\taddress %08x\n", COL_RED, COL_GRAY, uio_led_descriptor, led);
            myGPIO_set_mode(led, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  WRITE_MODE);
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