/**
* @file main.c
* @brief Questo è un esempio di utilizzo della libreria @ref myuart.h sul S.O. GNU/Linux in
* esecuzione sulla board Zybo tramite il driver generico UIO.
* @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
*			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
*			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
* @date 15/06/2020
*
* @details E' possibile l'utilizzo delle funzioni della libreria myuart.h sul S.O. GNU/Linux grazie a un mapping che viene fatto della pagina fisica, 
* a cui è mappata la periferica UART custom, a una virtuale nello spazio di indirizzamento del processo. \n
* Al contrario del no-driver, questa volta vi è un modulo kernel. Tale modulo, UIO, è stato generato automaticamente dal sistema operativo 
* grazie alle informazioni ricavate dal device tree. \n
* Per interagire con tale modulo bisogna interfacciarsi con il particolare file /dev/uio che si occupa della periferica.
* Tale esempio, contrariamente a quello no driver, può fare uso delle interruzione  essendovi un modulo, uio, a livello kernel,
* ad occuparsi della periferica. \n
* Una volta aperto il file /dev/uio opportuno, è possibile mappare l'indriizzo fisico della periferica allo spazio virtuale del processo.
* Questa volta, contrariamente al caso no-driver, non è nemmeno neccessario dare informazioni alla funzione mmap riguardo l'indirizzo fisico della periferica, 
* tale informazione è già contenuta nel file aperto /dev/uio. \n
* Il modulo kernel UIO implementa un meccanismo semplice per permettere a livello user_space di svolgere delle azioni una volta ricevuta un'interruzione.
* Con la system call read è possibile bloccare il processo fino all'arrivo dell'interruzione. L'ISR (implementata dal modulo UIO) sicuramente
* si occuperà di svegliare i processi in attesa su tale evento. \n
* Le interruzioni saranno disabilitate dopo la read. Per riabilitarle sarà necessario invocare la system call write. \n
*
* @{
*/


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

/**
* @brief Main di un semplice programma che guida la periferica UART a livello utente grazie al driver UIO a livello kernel.
* @details Il primo Uart invia il carattere ricevuto in ingresso dal terminale al secondo uart, il quale, una volta ricevuto il carattere,
*  lo stampa sul terminale insime allo stato della periferica. \n 
* L'uart di ricezione  rimane in attesa dell'evento "arrivo del carattere nel buffer di ricezione"  per poi leggere lo stato della periferica,
* il carattere dal registro DBOUT della periferica, ed effettuare l'Interrupt ack. \n
* Quest'ultima azione permetterà di abbassare il segnale di interruzione della periferica e di registrare come letto il dato nel buffer DBOUT 1n.
*/
int main (int argc, char** argv){
    int uart1_fd, uart2_fd;
    myUART *uart1, *uart2;
    int32_t reenable =1;

    int pid = fork();
     
    if( pid > 0 ){
        uint8_t byte_to_send = 'a';

        uart1_fd  = open(UIO_FILE_UART1,O_RDWR) ;

        void* vrt_uart = configure_uio(UIO_FILE_UART1, &uart1_fd);
        printf("%s[UART1-SENDER]%s vrt_uart: %08x\n",COL_BLUE, COL_GRAY, vrt_uart);

        if (vrt_uart == NULL) 
            return -1;

        uart1 = myUART_init(vrt_uart);
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

        void* vrt_uart = configure_uio(UIO_FILE_UART2, &uart2_fd);
        printf("%s[UART2-RECEIVER]%s vrt_uart: %08x\n",COL_RED, COL_GRAY, vrt_uart);
        if (vrt_uart == NULL) 
            return -1;

        uart2 = myUART_init(vrt_uart);
        
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
/** @} */