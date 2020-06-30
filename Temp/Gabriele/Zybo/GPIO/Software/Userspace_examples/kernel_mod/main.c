/**
* @file main.c
* @brief Questo è un esempio di utilizzo della libreria @ref mygpio.h sul S.O GNU/Linux in esecuzione sulla board Zybo. \n
* La libreria è stata estesa affinchè la comunicazione con la periferica possa avvenire tramite il modulo kernel gpio. 
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

#include "utils.h"
#include "mygpio.h"
#include "config.h"


/**
* @brief Main di un semplice programma di test dell'interfaccia seriale gpio tramite la libreria mygpio.h estesa per rendere possibile l'utilizzo del modulo kernel gpio. 
* @details Il programma consiste nell'aspettare l'evento di pressione dei bottoni per leggere il valore dalla periferica button GPIO e
*  mostrare tale valore sui led
*/
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


/** @} */