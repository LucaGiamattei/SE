#include "utils.h"
/*TODO
* METTERE ASSERT
*
*/


//Scrive lo stesso bit nelle posizioni desiderate
uint32_t write_bit_in_pos(uint32_t* address, uint32_t pos, uint32_t bit){
    if(bit == 0){
        *(address) &= ~pos;
    }else if(bit == 1){
        *(address) |= pos;
    }else{
        //invalid bit
        return 0;
    }

    return *(address);
}

uint8_t  read_bit_in_single_pos(uint32_t* address, uint8_t pos){
    if(pos && !(pos & (pos-1))){
        return (*(address) & pos) && 1;
    }
    return 0;
}
