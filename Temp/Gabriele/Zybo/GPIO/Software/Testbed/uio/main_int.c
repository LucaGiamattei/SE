#include "mygpio.h"
#include "utils.h"
#include "config.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>

int read_value = 0;

void* configure_uio_mygpio(char* filename, int* file_descriptor);
void* create_shared_memory(size_t size);
void cofigure_btn_swt(myGPIO* btn, int uio_btn_descriptor );
void cofigure_led(myGPIO* led, int uio_btn_descriptor );
int32_t wait_interrupt(int uio_descriptor, int32_t* file_descriptor);
int32_t reenable_interrupt(int uio_descriptor, int32_t* reenable);

/* lo switch accende il led e i bottoni li spengono*/

int main (int argc, char** argv){
    //--------------------Shared memory---------------
    void* shmem = create_shared_memory(1);
    memcpy(shmem, "", sizeof(0));
    int pid = fork();

    if(pid == 0){
        /** Processo che aspetta l'interruzione da parte dei bottoni 
        * per poter spegnere i led
        */

        int32_t interrupt_count = 1 ;
        int32_t reenable = 1;
        int32_t write_value = 0;

        printf("%s[BTN]%s sono il figlio \n",COL_YELLOW,COL_GRAY);

        //---------------Button uio e mmap------------------

        int uio_btn_descriptor = open(UIO_FILE_BTN, O_RDWR);
        void* vrt_gpio = configure_uio_mygpio(UIO_FILE_BTN, &uio_btn_descriptor);
        printf("%s[BTN]%s vrt_gpio %08x\n",COL_YELLOW,COL_GRAY, vrt_gpio);
        printf("%s[BTN]%s uio_btn_descriptor %d\n",COL_YELLOW,COL_GRAY, uio_btn_descriptor);

        if (vrt_gpio == NULL) 
            return -1;

        //-----------------Configurazione device----------------
        myGPIO* btn = myGPIO_init(vrt_gpio);
        cofigure_btn_swt(btn, uio_btn_descriptor);

        printf("%s[BTN]%s myGPIO %08x\n",COL_YELLOW,COL_GRAY, btn);

        while(1){
            printf("%s[BTN]%s aspetto interrupt\n",COL_YELLOW,COL_GRAY);
            wait_interrupt(uio_btn_descriptor, &interrupt_count);

            if (interrupt_count > 1){
                //leggo il valore dai bottoni gpio    
                read_value =  (myGPIO_read(btn));
                printf("%s[BTN]%s read: %d\n",COL_YELLOW,COL_GRAY, *((uint32_t*) shmem));
                
                // place data into memory
                write_value = ~read_value &  *((uint32_t*) shmem);
                memcpy(shmem, &write_value, sizeof(read_value));

                printf("%s[BTN]%s wrote: %d\n",COL_YELLOW,COL_GRAY, *((uint32_t*) shmem));
              
            }
            //IACK
            myGPIO_clear_irq(btn, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
           
            reenable_interrupt(uio_btn_descriptor, &reenable);
        }
    }
    else if (pid>0) {

        sleep(1);

        int pid = fork();
        if(pid == 0){

            /**
            * Processo che aspetta l'interrupt da parte degli switch per accendere i led
            */
        
            printf("%s[SWT]%s sono il figlio \n",COL_BLUE,COL_GRAY);

            //---------------Switch uio e mmap------------------

            // int uio_swt_descriptor = open (UIO_FILE_SWT, O_RDWR);
            int uio_swt_descriptor  = open(UIO_FILE_SWT,O_RDWR) ;

            void* vrt_gpio = configure_uio_mygpio(UIO_FILE_SWT, &uio_swt_descriptor);
            printf("%s[SWT]%s vrt_gpio %08x\n",COL_BLUE,COL_GRAY, vrt_gpio);

            if (vrt_gpio == NULL) 
                return -1;

            //-----------------Configurazione device----------------
            myGPIO* swt = myGPIO_init(vrt_gpio);
            cofigure_btn_swt(swt, uio_swt_descriptor);
            printf("%s[SWT]%s myGPIO %08x\n",COL_BLUE,COL_GRAY, swt);

            int32_t interrupt_count = 1 ;
            int32_t reenable =1;

            while(1){
                printf("%s[SWT]%s aspetto interrupt\n",COL_BLUE,COL_GRAY);

                wait_interrupt(uio_swt_descriptor, &interrupt_count);

                if (interrupt_count > 1){
                    //leggo il valore dagli switch gpio    
                    read_value = myGPIO_read(swt);
                    printf("%s[SWT]%s read: %d\n",COL_BLUE,COL_GRAY, *((uint32_t*) shmem));
                    
                    // place data into memory
                    memcpy(shmem, &read_value, sizeof(read_value));
                    printf("%s[SWT]%s wrote: %d\n",COL_BLUE,COL_GRAY, *((uint32_t*) shmem));
                    
                }
                //IACK
                myGPIO_clear_irq(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
                
                reenable_interrupt(uio_swt_descriptor, &reenable);

            }

        }else if(pid > 0) { 
            sleep(2);
            printf("%s[LED]%s Sono il padre \n",COL_RED,COL_GRAY);

            //-------------------Led uio e mmap----------------------------

            //int uio_led_descriptor = open (UIO_FILE_LED, O_RDWR);
            int uio_led_descriptor = open(UIO_FILE_LED,O_RDWR);
            void* vrt_gpio = configure_uio_mygpio(UIO_FILE_SWT, &uio_led_descriptor);
            printf("%s[LED]%s vrtgpio %08x\n",COL_RED,COL_GRAY, vrt_gpio);

            if (vrt_gpio == NULL) 
                return -1;
                
            //------------------Configurazione device------------------------
            myGPIO* led = myGPIO_init(vrt_gpio);
            printf("%s[LED]%s myGPIO %08x\n",COL_RED,COL_GRAY, led);

            printf("%s[LED]%s Led configurati \n\tDescirttore %d\n\taddress %08x\n", COL_RED, COL_GRAY, uio_led_descriptor, led);
            myGPIO_set_mode(led, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  WRITE_MODE);
            myGPIO_write_mask(led, 0xf);
            while(1){
                 myGPIO_write_mask(led, *((uint32_t*) shmem));     
            }
        }
    }

	//munmap(led, page_size);
    //munmap(btn, page_size);
    //munmap(swt, page_size);

	//close(uio_led_descriptor);
    //close(uio_swt_descriptor);
	//close(uio_btn_descriptor);


    return 0;
}

/**
 * @brief Stampa un messaggio che fornisce indicazioni sull'utilizzo del programma
 */
void howto(void) {
    printf("L'interruzione sul fronte di salita di SW0 accende tutti i led,\n");
    printf("mentre l'interruzione du BTN0 li spegne.\n");

    printf("\t\n");
}


void* configure_uio_mygpio(char* filename, int* file_descriptor){
   
    void* vrt_gpio = NULL;
    //Open UIO device
    //*file_descriptor = open (filename, O_RDWR);
	if (*file_descriptor < 1) {
		printf("Errore nell'aprire il descrittore  del file %s\n", filename);
		//return NULL;
	}

    printf("File aperto con successo descrittore: %d \n", *file_descriptor);

    uint32_t page_size = sysconf(_SC_PAGESIZE);		

	vrt_gpio = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, *file_descriptor, 0);
	if (vrt_gpio == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		//return NULL;
	}

    printf("Mapping indirizzo avvenuto con successo indirizzo: %08x\n", vrt_gpio);
    
    return vrt_gpio;
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

void cofigure_btn_swt(myGPIO* btn, int uio_btn_descriptor ){
    myGPIO_set_mode(btn, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  READ_MODE);
    myGPIO_set_irq_mode(btn, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,INT_EDGE );
    myGPIO_set_edge(btn, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_RE);
    printf("%s[BTN]%s configurazione switch \n\tDescrittore %d\n\taddress %08x\n",COL_YELLOW,COL_GRAY,uio_btn_descriptor, btn);
    myGPIO_en_int(btn, INT_EN);
    myGPIO_en_pins_int(btn, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_EN);
}

int32_t wait_interrupt(int uio_descriptor, int32_t *interrupt_count){
    printf("Aspetto interrupt");
     if (read(uio_descriptor, interrupt_count, sizeof(uint32_t)) != sizeof(uint32_t)) {
        printf("%sSWT]%s Read error!\n",COL_BLUE,COL_GRAY);
        return -1;
    }
                
}

int32_t reenable_interrupt(int uio_descriptor, int32_t *reenable){   
    if (write(uio_descriptor, (void*)reenable, sizeof(uint32_t)) != sizeof(uint32_t)) {
        printf("%s[SWT]%s Write error!\n",COL_BLUE,COL_GRAY);
        return -2;
    }
}