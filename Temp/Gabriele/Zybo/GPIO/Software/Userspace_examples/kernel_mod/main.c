#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "utils.h"
#include "mygpio.h"
#include "config.h"


int main(){

    int led_file_descriptor, button_file_descriptor, error;

    uint32_t write_value = 0; 

    if((error = open_device(&led_file_descriptor, GPIO_LED_FILE)) != 0){
        printf("Errore mentre aprivo il device file: %s\n", GPIO_LED_FILE);
        return -1;
    }

    if((error = open_device(&button_file_descriptor, GPIO_BUTTON_FILE)) != 0){
        printf("Errore mentre aprivo il device file: %s\n", GPIO_BUTTON_FILE);
        return -1;
    }

    /*Configurazione dei led*/
    myGPIO_set_mode_k(led_file_descriptor, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  WRITE_MODE);
    myGPIO_set_mode_k(button_file_descriptor, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  READ_MODE);

    while(1){
        printf("Aspetto l'interrupt dalla pressione e rilascio dei bottoni (debauncing)\n");
        
        write_value = myGPIO_read_bloc_k(button_file_descriptor);

        printf("Letto dai bottoni: %08x\n", write_value);

        myGPIO_write_mask_k(led_file_descriptor, write_value);
    }

    close(led_file_descriptor);
    close(button_file_descriptor);
    return 0;
}
