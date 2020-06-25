#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "mygpio.h"
#include "config.h"

int open_device(int* file_descriptor, char* device_file);

int main(){

    int led_file_descriptor, button_file_descriptor, error;

    __uint32_t* write_value = malloc(sizeof(uint32_t));
    /*inizializzo in modo tale da configurare i led in modalità
        Output*/
    *write_value = 0xf;  

    if((error = open_device(&led_file_descriptor, GPIO_LED_FILE)) != 0){
        printf("Errore mentre aprivo il device file: %s\n", GPIO_LED_FILE);
        return -1;
    }

    if((error = open_device(&button_file_descriptor, GPIO_BUTTON_FILE)) != 0){
        printf("Errore mentre aprivo il device file: %s\n", GPIO_BUTTON_FILE);
        return -1;
    }

    /*Configurazione dei led*/
    lseek(led_file_descriptor, MODE_OFFSET, SEEK_SET);
    write(led_file_descriptor, write_value , sizeof(__uint32_t));
    /*Visto che i led sono di solo output posizioniamo l'offset in modo tale da
        accedere al registro write*/
    lseek(led_file_descriptor, WRITE_OFFSET, SEEK_SET);

    /* Visto che i bottoni sono di solo input posizioniamo l'offset in modo tale da
        accedere al registro read */
    lseek(button_file_descriptor, READ_OFFSET, SEEK_SET);

    *write_value = 0;
    while(1){
        printf("Aspetto l'interrupt dalla pressione e rilascio dei bottoni (debauncing)\n");
        
        read(button_file_descriptor, write_value, sizeof(__uint32_t));

        printf("Letto dai bottoni: %08x\n", *write_value);

        write(led_file_descriptor, write_value , sizeof(__uint32_t));
    }

    free(write_value);
    close(led_file_descriptor);
    close(button_file_descriptor);
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