#ifndef UTILS_H
#define UTILS_H

#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"



//Scrive lo stesso bit nelle posizioni desiderate
uint32_t write_bit_in_pos(uint32_t* address, uint32_t pos, uint32_t bit);
uint8_t  read_bit_in_single_pos(uint32_t* address, uint8_t pos);



#endif
