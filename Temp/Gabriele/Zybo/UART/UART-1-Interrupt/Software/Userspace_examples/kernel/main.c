#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "myuart.h"
#include "config.h"




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

                }
                               
            }
            myUART_read_DBOUT_bloc_k(uart_1_file_descriptor,read_value);
            printf("%s[UART1]%s receive byte %c with state %d\n",COL_BLUE, COL_GRAY, read_value[0], read_value[1]);
           
            
        }
       
        
    }else if(pid>0){
        
        printf("%s[UART2]%s sono il figlio \n",COL_RED,COL_GRAY);
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
                }
            }           
        }
        
    }

    free(read_value);
    close(uart_1_file_descriptor);
    close(uart_2_file_descriptor);

    return 0;
}
