#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "myuart.h"
#include "config.h"


int open_device(int* file_descriptor, char* device_file);

int main (int argc, char** argv){

    int uart_1_file_descriptor, uart_2_file_descriptor, error;


    __uint32_t read_value = 0;
    /*inizializzo in modo tale da configurare il primo uart in modalità
        Output*/
     
     uint8_t byte_to_send = 'c';
    int pid = fork();
    
    if(pid == 0){
        
       printf("%s[UART1]%s sono il figlio \n",COL_BLUE,COL_GRAY);
        //apertura del file
        if((error = open_device(&uart_1_file_descriptor, UART_1_FILE)) != 0){
            printf("Errore mentre aprivo il device file: %s\n", UART_1_FILE);
            return -1;
            }
        read_value = 'a';

        while(1){
            if (read_value=='a'){
                int send = 1;
                while(send){
                    printf("%s[UART1]%s wait user input char \n",COL_BLUE, COL_GRAY);
                    scanf(" %c", &byte_to_send);
                    if(byte_to_send != 0){
                        printf("%s[UART1]%s send byte %c\n",COL_BLUE, COL_GRAY, byte_to_send);
                        lseek(uart_1_file_descriptor, DBIN_OFFSET, SEEK_SET);
                        write(uart_1_file_descriptor, &byte_to_send , sizeof(__uint32_t));
                        if(byte_to_send=='a') send=0; //smetti di inviare
                    }

                }
                               
            }
            //ricevo il valore
            lseek(uart_1_file_descriptor, DBOUT_OFFSET, SEEK_SET);
            read(uart_1_file_descriptor, &read_value, sizeof(__uint32_t));
            printf("%s[UART1]%s receive byte %c\n",COL_BLUE, COL_GRAY, read_value);
            //rispondo
            
        }
        
        
    }else if(pid>0){
        
        printf("%s[UART2]%s sono il figlio \n",COL_RED,COL_GRAY);
        //apertura del file
        if((error = open_device(&uart_2_file_descriptor, UART_2_FILE)) != 0){
            printf("Errore mentre aprivo il device file: %s\n", UART_2_FILE);
            return -1;
            }

        while(1){
            //ricevo il valore
            lseek(uart_2_file_descriptor, DBOUT_OFFSET, SEEK_SET);
            read(uart_2_file_descriptor, &read_value, sizeof(__uint32_t));
            printf("%s[UART2]%s receive byte %c\n",COL_RED, COL_GRAY, read_value);
            //rispondo
            if (read_value=='a'){
                int send = 1;
                while(send){
                    printf("%s[UART2]%s wait user input char \n",COL_RED, COL_GRAY);
                    scanf(" %c", &byte_to_send);
                    if(byte_to_send != 0){
                        printf("%s[UART2]%s send byte %c\n",COL_RED, COL_GRAY, byte_to_send);
                        lseek(uart_2_file_descriptor, DBIN_OFFSET, SEEK_SET);
                        write(uart_2_file_descriptor, &byte_to_send , sizeof(__uint32_t));
                        if(byte_to_send=='a') send=0; //smetti di inviare
                    }
                }
            }           
        }
    }

    close(uart_1_file_descriptor);
    close(uart_2_file_descriptor);
    return 0;
}

int open_device(int* file_descriptor, char* device_file){
    printf("Open device_file: %s \n", device_file);
    *file_descriptor = open(device_file, O_RDWR);
	if (*file_descriptor < 1) {
		perror(device_file);
		return -1;
	}
	return 0;
}