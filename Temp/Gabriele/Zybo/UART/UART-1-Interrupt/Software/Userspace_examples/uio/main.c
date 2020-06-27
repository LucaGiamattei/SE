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

int main (int argc, char** argv){
    int uart1_fd, uart2_fd;
    myUART *uart1, *uart2;
    int32_t reenable =1;

    int pid = fork();
     
    if( pid > 0 ){
        uint8_t byte_to_send = 'a';

        uart1_fd  = open(UIO_FILE_UART1,O_RDWR) ;

        void* vrt_gpio = configure_uio(UIO_FILE_UART1, &uart1_fd);
        printf("%s[UART1-SENDER]%s vrt_gpio: %08x\n",COL_BLUE, COL_GRAY, vrt_gpio);

        if (vrt_gpio == NULL) 
            return -1;

        uart1 = myUART_init(vrt_gpio);
        printf("%s[UART1-SENDER]%s myUART pointer: %08x\n",COL_BLUE, COL_GRAY, uart1);
            
        while(1){
            printf("%s[UART1-SENDER]%s wait user input char \n",COL_BLUE, COL_GRAY);
            scanf(" %c", &byte_to_send);
            if(byte_to_send != 0){
                printf("%s[UART1-SENDER]%s send byte %c\n",COL_BLUE, COL_GRAY, byte_to_send);
                myUART_transmit(uart1, byte_to_send);
            }
        }

    }else if(pid == 0){
        char received_byte;
        uint32_t status_reg;
        int interrupt_count;
        int reenable = 1;

        uart2_fd  = open(UIO_FILE_UART2,O_RDWR) ;

        void* vrt_gpio = configure_uio(UIO_FILE_UART2, &uart2_fd);
        printf("%s[UART2-RECEIVER]%s vrt_gpio: %08x\n",COL_RED, COL_GRAY, vrt_gpio);
        if (vrt_gpio == NULL) 
            return -1;

        uart2 = myUART_init(vrt_gpio);
        
        myUART_en_int_rx(uart2, INT_EN);
        myUART_Iack_r(uart2);
        printf("%s[UART2-RECEIVER]%s myUART pointer: %08x\n",COL_RED, COL_GRAY, uart2);
            
        while(1){
            //sleep(1);
            printf("%s[UART2-RECEIVER]%s wait interrupt\n",COL_RED, COL_GRAY);
            wait_interrupt(uart2_fd, &interrupt_count);
            
            status_reg = myUART_read_status(uart2);
            received_byte = myUART_read_DBOUT(uart2);
            myUART_Iack_r(uart2);

            printf("%s[UART2-RECEIVER]%s received: %c\n",COL_RED, COL_GRAY, received_byte);
            printf("%s[UART2-RECEIVER]%s status_reg: %08x\n",COL_RED, COL_GRAY, status_reg);

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
