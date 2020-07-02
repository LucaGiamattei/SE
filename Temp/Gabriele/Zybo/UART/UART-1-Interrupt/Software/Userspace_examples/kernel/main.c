/**
* @file main.c
* @brief Questo è un esempio di utilizzo della libreria @ref myuart.h sul S.O GNU/Linux in esecuzione sulla board Zybo. \n La libreria è stata estesa affinchè la comunicazione con la periferica possa avvenire tramite il modulo kernel uart. 
* @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
*			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
*			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
* @date 15/06/2020
*
* 
*
* @{
*/



#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "myuart.h"
#include "config.h"



/**
* @brief Main di un semplice programma di test dell'interfaccia seriale uart tramite la libreria myuart.h estesa per rendere possibile l'utilizzo del modulo kernel uart. 
* @details Il programma instanzia due processi. Ogni processo guida una periferica uart. \n
* Ogni uart può fungere sia da receiver che da transmitter. Inizialmente l'uart1 funge da transmitter e l'uart2 da receiver; i ruoli possono
* essere scambiati inviando il carattere 'a'.
*/
int main (int argc, char** argv){

    int uart_1_file_descriptor, uart_2_file_descriptor, error;
    uint32_t *read_value = (uint32_t *)malloc(2 * sizeof (uint32_t));
    uint8_t byte_to_send = 'c';
    int pid = fork();
    
    if(pid == 0){
       
       printf("%s[UART1]%s sono il figlio \n",COL_BLUE,COL_GRAY);
        if((error = open_device(&uart_1_file_descriptor, UART_1_FILE)) != 0){
            printf("Errore mentre aprivo il device file: %s\n", UART_1_FILE);
            return -1;
            }
        read_value[0] = 'a';

        while(1){
            if (read_value[0]=='a'){
                int send = 1;
                while(send){
                    printf("%s[UART1]%s wait user input char \n",COL_BLUE, COL_GRAY);
                    scanf(" %c", &byte_to_send);
                    if(byte_to_send != 0){
                        printf("%s[UART1]%s send byte %c\n",COL_BLUE, COL_GRAY, byte_to_send);
                        myUART_transmit_k(uart_1_file_descriptor, byte_to_send);
                        if(byte_to_send=='a') send=0; 
                    }
                    usleep(300000);
                }
                               
            }
            myUART_read_DBOUT_bloc_k(uart_1_file_descriptor,read_value);
            printf("%s[UART1]%s receive byte %c with state %d\n",COL_BLUE, COL_GRAY, read_value[0], read_value[1]);
           
            
        }
       
        
    }else if(pid>0){
        
        printf("%s[UART2]%s sono il padre \n",COL_RED,COL_GRAY);
        if((error = open_device(&uart_2_file_descriptor, UART_2_FILE)) != 0){
            printf("Errore mentre aprivo il device file: %s\n", UART_2_FILE);
            return -1;
            }

        while(1){
            myUART_read_DBOUT_bloc_k(uart_2_file_descriptor,read_value);
            printf("%s[UART2]%s receive byte %c with state %d\n",COL_RED, COL_GRAY, read_value[0],read_value[1]);
            if (read_value[0]=='a'){
                int send = 1;
                while(send){
                    printf("%s[UART2]%s wait user input char \n",COL_RED, COL_GRAY);
                    scanf(" %c", &byte_to_send);
                    if(byte_to_send != 0){
                        printf("%s[UART2]%s send byte %c\n",COL_RED, COL_GRAY, byte_to_send);
                        myUART_transmit_k(uart_2_file_descriptor, byte_to_send);
                        if(byte_to_send=='a') send=0; 
                    }
                    usleep(300000);
                }
            }           
        }
        
    }

    free(read_value);
    close(uart_1_file_descriptor);
    close(uart_2_file_descriptor);

    return 0;
}
/** @} */
