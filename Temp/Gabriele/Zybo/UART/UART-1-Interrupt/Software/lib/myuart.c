
/***************************** Include Files *********************************/
#include "myuart.h"


/***************************** Function Implementation *******************************/
#if defined MYUART_NO_DRIVER || defined MYUART_BARE_METAL ||defined MYUART_UIO
myUART* myUART_init(uint32_t *peripheral_address){
    return (myUART*) peripheral_address;
}

uint32_t myUART_en_int_rx(myUART * myuart, uint8_t int_en){
    return write_bit_in_pos(&myuart->CONTROL_REG, CTR_IERX, int_en);
}

uint32_t myUART_en_int_tx(myUART * myuart, uint8_t int_en){
    return write_bit_in_pos(&myuart->CONTROL_REG, CTR_IETX, int_en);
}

void myUART_transmit_int(myUART * myuart, uint8_t transmit_data){
    myuart->DBIN = transmit_data;
    write_bit_in_pos(&myuart->CONTROL_REG, CTR_WR, HIGH);
    write_bit_in_pos(&myuart->CONTROL_REG, CTR_WR, LOW);
}

//ATT: Si dovrebbe gestire il TIMEOUT con un timer
void myUART_transmit(myUART * myuart, uint8_t transmit_data){
    myuart->DBIN = transmit_data;
    write_bit_in_pos(&myuart->CONTROL_REG, CTR_WR, HIGH);
    write_bit_in_pos(&myuart->CONTROL_REG, CTR_WR, LOW);
    while(!read_bit_in_single_pos(&myuart->STATUS_REG, ST_TBE)){}
}

//ATT: Si dovrebbe gestire il TIMEOUT con un timer
uint8_t myUART_read(myUART * myuart,uint32_t* status_reg){
    while(!read_bit_in_single_pos(&myuart->STATUS_REG, ST_RDA)){}
    
    *(status_reg) = myuart->STATUS_REG;
    uint8_t read_value = myuart->DBOUT;
    
    myUART_Iack_r(myuart);
    
    return read_value;
}

uint8_t myUART_read_DBOUT(myUART * myuart){
	return myuart->DBOUT;
}

uint8_t myUART_read_status_bit(myUART * myuart, uint32_t pos){
    return read_bit_in_single_pos(&myuart->STATUS_REG, pos);
}

uint32_t myUART_read_status(myUART * myuart){
    return myuart->STATUS_REG;
}

void myUART_Iack_r(myUART * myuart){
    write_bit_in_pos(&myuart->CONTROL_REG, CTR_RD, HIGH);
    write_bit_in_pos(&myuart->CONTROL_REG, CTR_RD, LOW);
}

void myUART_Iack_w(myUART * myuart){
    write_bit_in_pos(&myuart->CONTROL_REG, CTR_IACK, HIGH);
    //IACK è abbassato automaticamente in hw
}
#endif

#ifdef MYUART_KERNEL

uint32_t myUART_en_int_rx_k(int descriptor, uint8_t int_en){
    return write_bit_in_pos_k( descriptor, CONTROL_REG_OFFSET, CTR_IERX, int_en);
   
}

uint32_t myUART_en_int_tx_k(int descriptor, uint8_t int_en){
    return write_bit_in_pos_k( descriptor, CONTROL_REG_OFFSET, CTR_IETX, int_en);
    
}


//ATT: Si dovrebbe gestire il TIMEOUT con un timer
void myUART_transmit_k(int descriptor, uint8_t transmit_data){
    while(!read_bit_in_single_pos_k( descriptor, STATUS_REG_OFFSET, ST_TBE)){}
    write_reg(descriptor, DBIN_OFFSET, transmit_data);
    
}

//ATT: Si dovrebbe gestire il TIMEOUT con un timer
uint8_t myUART_read_k(int descriptor,uint32_t* status_reg){
    while(!read_bit_in_single_pos_k( descriptor, STATUS_REG_OFFSET, ST_RDA)){}
    *(status_reg) = read_reg( descriptor, STATUS_REG_OFFSET);
    uint8_t read_value =read_reg( descriptor, DBOUT_OFFSET);
    myUART_Iack_r_k(descriptor); 
    return read_value;
}

void myUART_read_DBOUT_bloc_k(int descriptor,uint32_t* read_value){
   read_reg_bloc_UART_k(descriptor, DBOUT_OFFSET,read_value);
}

uint8_t myUART_read_status_bit_k(int descriptor, uint32_t pos){
    return read_bit_in_single_pos_k( descriptor, STATUS_REG_OFFSET, pos);

}

uint32_t myUART_read_status_k(int descriptor){
    return read_reg( descriptor, STATUS_REG_OFFSET);
}

void myUART_Iack_r_k(int descriptor){
    write_bit_in_pos_k( descriptor, CONTROL_REG_OFFSET, CTR_RD, HIGH);
    write_bit_in_pos_k( descriptor, CONTROL_REG_OFFSET, CTR_RD, LOW);
}

void myUART_Iack_w_k(int descriptor){
    write_bit_in_pos_k( descriptor, CONTROL_REG_OFFSET, CTR_IACK, HIGH);
}

void  read_reg_bloc_UART_k(int descriptor, uint32_t reg, uint32_t* read_value){
    lseek(descriptor, reg, SEEK_SET);
    read(descriptor, read_value, 2*sizeof(uint32_t));

}

#endif


