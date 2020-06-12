
#ifndef MYGPIO_H
#define MYGPIO_H

#define GPIO_PIN_0 (uint32_t) 1 << 0
#define GPIO_PIN_1 (uint32_t) 1 << 1
#define GPIO_PIN_2 (uint32_t) 1 << 2
#define GPIO_PIN_3 (uint32_t) 1 << 3
#define GPIO_PIN_4 (uint32_t) 1 << 4
#define GPIO_PIN_5 (uint32_t) 1 << 5
#define GPIO_PIN_6 (uint32_t) 1 << 6
#define GPIO_PIN_7 (uint32_t) 1 << 7

#define GIES_IE     0
#define GIES_IS     1

#define READ_MODE   0
#define WRITE_MODE  1

#define LOW         0
#define HIGH        1

#define INT_EN      1
#define INT_DIS     0

#define INT_LEVEL   0
#define INT_EDGE    1

#define INT_RE      1
#define INT_FE      0

#define MODE_OFFSET     0x00
#define WRITE_OFFSET    0x04
#define READ_OFFSET     0x08
#define GIES_OFFSET     0x0C
#define PIE_OFFSET      0x10
#define IRQ_OFFSET      0x14
#define IACK_OFFSET     0x18
#define IRQ_MODE_OFFSET 0x1C
#define IRQ_EDGE_OFFSET 0x1F

#include "stdlib.h"

typedef struct{
    uint32_t MODE; 
    uint32_t WRITE;
    uint32_t READ;
    uint32_t GIES;
    uint32_t PIE;
    uint32_t IRQ;
    uint32_t IACK;
    uint32_t IRQ_MODE;
    uint32_t IRQ_EDGE;
} myGPIO;

myGPIO* myGPIO_init(uint32_t *);

//Setta lo stesso valore di MODE per i pin in ingresso
uint32_t myGPIO_set_mode(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t mode);

//Setta la maschera per il registro MODE
void myGPIO_set_mode_mask(myGPIO * mygpio, uint32_t mode_mask);

//Abilita il GIES
uint32_t myGPIO_en_int(myGPIO * mygpio, uint8_t int_en);
//setta lo stesso valore di PIE per i pin in ingresso
uint32_t myGPIO_en_pins_int(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t int_en);
//setta lo stesso valore di IRQ_MODE per i pin in ingresso
uint32_t myGPIO_set_irq_mode(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t int_mode);
//setta lo stesso valore IRQ_EDGE per i pin in ingresso
uint32_t myGPIO_set_edge(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t int_egde);

//Scrive lo stesso valore di level su tutti i pin in ingresso
uint32_t myGPIO_write(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t level);

//Scrive i valori sul GPIO
void myGPIO_write_mask(myGPIO * mygpio, uint32_t levels);

//Legge il valore di un singolo pin
uint8_t myGPIO_read_pin(myGPIO * mygpio, uint32_t GPIO_pin);

//Legge tutto il GPIO
uint32_t myGPIO_read(myGPIO * mygpio);

//Clear Interrupt
uint32_t myGPIO_clear_irq(myGPIO * mygpio, uint32_t GPIO_pins);

//Ritorna il vettore dell'interruzione
uint32_t myGPIO_read_irq(myGPIO * mygpio);
uint32_t myGPIO_read_pin_irq_status(myGPIO * mygpio, uint32_t GPIO_pin);


#endif