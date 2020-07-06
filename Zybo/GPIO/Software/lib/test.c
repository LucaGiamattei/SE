#include "utils.h"
#include "mygpio.h"

#include <assert.h>
#include <stdio.h>

int main(void){

    uint32_t *base_add = malloc(sizeof(uint32_t) * 9);
    *(base_add) = 0;
    //Test write_bit_in_pos scrivo 1 in posizione GPIO_PIN_0 e GPIO_PIN_4
    // il contenuto di base_add deve essere uguale a 0x11 
    write_bit_in_pos(base_add, GPIO_PIN_0|GPIO_PIN_4, 1);
    if(*(base_add) == 0x11 ){
        printf(" test superato: write_bit_in_pos(base_add, GPIO_PIN_0|GPIO_PIN_4,1)\n");
    }else{
        printf(" test fallito: write_bit_in_pos(base_add, GPIO_PIN_0|GPIO_PIN_4,1)\n");
    }

    *(base_add) = 0xFF;
    //Test write_bit_in_pos scrivo 1 in posizione GPIO_PIN_0 e GPIO_PIN_4
    // il contenuto di base_add deve essere uguale a 0x11 
    write_bit_in_pos(base_add, GPIO_PIN_7|GPIO_PIN_2, 0);
    if(*(base_add) == 0x7b ){
        printf(" test superato: write_bit_in_pos(base_add, GPIO_PIN_7|GPIO_PIN_2, 0)\n");
    }else{
        printf(" test fallito: write_bit_in_pos(base_add, GPIO_PIN_7|GPIO_PIN_2, 0)\n");
        printf(" %04x \n",*(base_add));
    }

    *(base_add) = 0xAB;
    //Test readbit leggo il quinto bit 
    // dovrebbe essere uguale a 0 
    uint8_t read = read_bit_in_single_pos(base_add, GPIO_PIN_4);
    if(read == 0 ){
        printf(" test superato: read_bit_in_single_pos(base_add, GPIO_PIN_4)\n");
    }else{
        printf(" test fallito: read_bit_in_single_pos(base_add, GPIO_PIN_4)\n");
        printf(" %04x \n",read);
    }

    //Test readbit leggo il quinto bit 
    // dovrebbe essere uguale a 0 
    read = read_bit_in_single_pos(base_add, GPIO_PIN_1);
    if(read == 1 ){
        printf(" test superato: read_bit_in_single_pos(base_add, GPIO_PIN_1)\n");
    }else{
        printf(" test fallito: read_bit_in_single_pos(base_add, GPIO_PIN_1)\n");
        printf(" %04x \n",read);
    }


    uint32_t *peripheral = malloc(sizeof(myGPIO));
    myGPIO *gpio = myGPIO_init(peripheral);
    gpio->MODE = 0x00;
    //Test write_bit_in_pos scrivo 1 in posizione GPIO_PIN_0 e GPIO_PIN_4
    // il contenuto di base_add deve essere uguale a 0x11 
    myGPIO_set_mode(gpio, GPIO_PIN_4|GPIO_PIN_5, 1);
    if(gpio->MODE== 0x30 ){
        printf(" test superato: myGPIO_set_mode(gpio, GPIO_PIN_4|GPIO_PIN_5, 1);\n");
    }else{
        printf(" test fallito: myGPIO_set_mode(gpio, GPIO_PIN_4|GPIO_PIN_5, 1);\n");
        printf(" %04x \n",gpio->MODE);

    }

    gpio->MODE = 0xFF;
    //Test write_bit_in_pos scrivo 1 in posizione GPIO_PIN_0 e GPIO_PIN_4
    // il contenuto di base_add deve essere uguale a 0x11 
    myGPIO_set_mode(gpio, GPIO_PIN_7|GPIO_PIN_3, 0);
    if(gpio->MODE == 0x77 ){
        printf(" test superato: myGPIO_set_mode(gpio, GPIO_PIN_7|GPIO_PIN_3, 0);\n");
    }else{
        printf(" test fallito: myGPIO_set_mode(gpio, GPIO_PIN_7|GPIO_PIN_3, 0);\n");
        printf(" %04x \n",gpio->MODE);
    }
    free(gpio);
    
    return 0;
}