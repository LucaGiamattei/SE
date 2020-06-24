#include "myuart.h"
#include "utils.h"
#include "config.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>

void* configure_uio_myuart(char* filename, int* file_descriptor);
int32_t wait_interrupt(int uio_descriptor, int32_t* file_descriptor);
int32_t reenable_interrupt(int uio_descriptor, int32_t* reenable);



int main (int argc, char** argv){
    int uart1_fd, uart2_fd;
    myUART *uart1, *uart2;
    int32_t reenable =1;

    int pid = fork();
     
    if( pid > 0 ){
        uint8_t byte_to_send;

        uart1_fd  = open(UIO_FILE_UART1,O_RDWR) ;

        void* vrt_gpio = configure_uio_myuart(UIO_FILE_UART1, &uart1_fd);
        printf("%s[UART1-SENDER]%s vrt_gpio: %08x\n",COL_BLUE, COL_GRAY, vrt_gpio);

        if (vrt_gpio == NULL) 
            return -1;

        uart1 = myUART_init(vrt_gpio);
        printf("%s[UART1-SENDER]%s myUART pointer: %08x\n",COL_BLUE, COL_GRAY, uart1);
            
        while(1){
            printf("%s[UART1-SENDER]%s wait user input char %08x\n",COL_BLUE, COL_GRAY, uart1);
            scanf("%c", byte_to_send);

            myUART_transmit(uart1, byte_to_send);
        }

    }else{
        char received_byte;
        uint32_t status_reg;
        int interrupt_count;
        int reenable = 1;

        uart2_fd  = open(UIO_FILE_UART2,O_RDWR) ;

        void* vrt_gpio = configure_uio_myuart(UIO_FILE_UART2, &uart2_fd);
        printf("%s[UART2-RECEIVER]%s vrt_gpio: %08x\n",COL_RED, COL_GRAY, vrt_gpio);

        if (vrt_gpio == NULL) 
            return -1;

        uart2 = myUART_init(vrt_gpio);
        printf("%s[UART2-RECEIVER]%s myUART pointer: %08x\n",COL_RED, COL_GRAY, uart2);
            
        while(1){
            printf("%s[UART2-RECEIVER]%s wait interrupt %08x\n",COL_RED, COL_GRAY, uart2);
            wait_interrupt(uart2_fd, &interrupt_count);
            
            received_byte = myUART_read(uart2, &status_reg);
            printf("%s[UART2-RECEIVER]%s received: %c\n",COL_RED, COL_GRAY, received_byte);

            reenable_interrupt(uart2_fd, &reenable);
        }

    }

    uint32_t pagesize = sysconf(_SC_PAGESIZE);
    munmap(uart1, pagesize);
    munmap(uart1, pagesize);

    close(uart1_fd);
    close(uart2_fd);


    return 0;
}


void howto(void) {
    printf("L'interruzione sul fronte di salita di SW0 accende tutti i led,\n");
    printf("mentre l'interruzione du BTN0 li spegne.\n");

    printf("\t\n");
}


void* configure_uio_myuart(char* filename, int* file_descriptor){
   
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


int32_t wait_interrupt(int uio_descriptor, int32_t *interrupt_count){
    printf("Wait interrupt");
     if (read(uio_descriptor, interrupt_count, sizeof(uint32_t)) != sizeof(uint32_t)) {
        printf("Read error!\n");
        return -1;
    }
                
}

int32_t reenable_interrupt(int uio_descriptor, int32_t *reenable){   
    if (write(uio_descriptor, (void*)reenable, sizeof(uint32_t)) != sizeof(uint32_t)) {
        printf("Write error!\n");
        return -2;
    }
}