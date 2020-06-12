#include "mygpio.h"
#include "utils.h"

myGPIO* myGPIO_init(uint32_t * peripheral_address){
    return (myGPIO*) peripheral_address;
}

//Setta i bit di mode per i pin
void myGPIO_set_mode_mask(myGPIO * mygpio, uint32_t mode_mask){
    mygpio->MODE = mode_mask;
}

//return registro di mode settato
uint32_t myGPIO_set_mode(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t mode){
    return write_bit_in_pos(&mygpio->MODE, GPIO_pins, mode);
}

uint32_t myGPIO_en_int(myGPIO * mygpio, uint8_t int_en){
    return write_bit_in_pos(&mygpio->GIES, GIES_IE, int_en);
}

uint32_t myGPIO_en_pins_int(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t int_en){
    return write_bit_in_pos(&mygpio->PIE, GPIO_pins, int_en);
}

uint32_t myGPIO_set_irq_mode(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t int_mode){
    return write_bit_in_pos(&mygpio->IRQ_MODE, GPIO_pins, int_mode);
}

uint32_t myGPIO_set_edge(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t int_egde){
    return write_bit_in_pos(&mygpio->IRQ_EDGE, GPIO_pins, int_egde);
}

uint32_t myGPIO_write(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t level){
    return write_bit_in_pos(&mygpio->WRITE, GPIO_pins, level);
}

//Scrive i valori sul GPIO
void myGPIO_write_mask(myGPIO * mygpio, uint32_t levels){
    mygpio->WRITE = levels;
}

//Legge il valore di un singolo pin
uint8_t myGPIO_read_pin(myGPIO * mygpio, uint32_t GPIO_pin){
    return read_bit_in_single_pos(&mygpio->READ, GPIO_pin);
}

//Legge tutto il GPIO
uint32_t myGPIO_read(myGPIO * mygpio){
    return mygpio->READ;
}

//
uint32_t myGPIO_clear_irq(myGPIO * mygpio, uint32_t GPIO_pins){
    return write_bit_in_pos(&mygpio->IACK, GPIO_pins, 1);
}

uint32_t myGPIO_read_irq(myGPIO * mygpio){
    return mygpio->IRQ;
}

uint32_t myGPIO_read_pin_irq_status(myGPIO * mygpio, uint32_t GPIO_pin){
    return read_bit_in_single_pos(&mygpio->IRQ, GPIO_pin);
}