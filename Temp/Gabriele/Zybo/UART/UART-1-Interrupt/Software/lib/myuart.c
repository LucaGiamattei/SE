
/***************************** Include Files *********************************/
#include "myuart.h"


/***************************** Function Implementation *******************************/
#if defined MYUART_NO_DRIVER || defined MYUART_BARE_METAL ||defined MYUART_UIO
myUART* myUART_init(uint32_t *peripheral_address){
    return (myUART*) peripheral_address;
}

int8_t myUART_en_int_rx(myUART * myuart, uint8_t int_en, uint32_t* writed_value){
    return write_bit_in_pos(&myuart->CONTROL_REG, CTR_IERX, int_en, writed_value);
}

int8_t myUART_en_int_tx(myUART * myuart, uint8_t int_en, uint32_t* writed_value){
    return write_bit_in_pos(&myuart->CONTROL_REG, CTR_IETX, int_en, writed_value);
}

int8_t myUART_transmit_int(myUART * myuart, uint8_t transmit_data, uint32_t* writed_value){
    myuart->DBIN = transmit_data;
    int8_t status;
    status = write_bit_in_pos(&myuart->CONTROL_REG, CTR_WR, HIGH, writed_value);
    if(!status)
        write_bit_in_pos(&myuart->CONTROL_REG, CTR_WR, LOW, writed_value);
}

//ATT: Si dovrebbe gestire il TIMEOUT con un timer
int8_t myUART_transmit(myUART * myuart, uint8_t transmit_data){
    myuart->DBIN = transmit_data;
    int8_t status = 0;
    int8_t TBE = 0;

    status = write_bit_in_pos(&myuart->CONTROL_REG, CTR_WR, HIGH, NULL);
    if(!status){
        status = write_bit_in_pos(&myuart->CONTROL_REG, CTR_WR, LOW, NULL);
        do{
            read_bit_in_single_pos(&myuart->STATUS_REG, ST_TBE, &TBE);
        }while(!TBE);
    }
    return status;
}

//ATT: Si dovrebbe gestire il TIMEOUT con un timer
uint8_t myUART_read(myUART * myuart,uint32_t* status_reg){
    uint8_t RDA = 0;
    do {
        read_bit_in_single_pos(&myuart->STATUS_REG, ST_RDA, &RDA);
    }while(!RDA);
    
    *(status_reg) = myuart->STATUS_REG;
    uint8_t read_value = myuart->DBOUT;
    
    myUART_Iack_r(myuart);
    
    return read_value;
}

uint8_t myUART_read_DBOUT(myUART * myuart){
	return myuart->DBOUT;
}

uint8_t myUART_read_status_bit(myUART * myuart, uint32_t pos, uint8_t* buff){
    return read_bit_in_single_pos(&myuart->STATUS_REG, pos, buff);
}

uint32_t myUART_read_status(myUART * myuart){
    return myuart->STATUS_REG;
}

uint8_t myUART_Iack_r(myUART * myuart){
    int8_t status;

    status = write_bit_in_pos(&myuart->CONTROL_REG, CTR_RD, HIGH, NULL);
    if(!status)
        write_bit_in_pos(&myuart->CONTROL_REG, CTR_RD, LOW, NULL);

    return status;
}

uint8_t myUART_Iack_w(myUART * myuart){
    int8_t status;

    return write_bit_in_pos(&myuart->CONTROL_REG, CTR_IACK, HIGH, NULL);
    //IACK è abbassato automaticamente in hw
}
#endif

#ifdef MYUART_KERNEL

int8_t myUART_en_int_rx_k(int descriptor, uint8_t int_en){
    return write_bit_in_pos_k( descriptor, CONTROL_REG_OFFSET, CTR_IERX, int_en, NULL);
   
}

int8_t myUART_en_int_tx_k(int descriptor, uint8_t int_en){
    return write_bit_in_pos_k( descriptor, CONTROL_REG_OFFSET, CTR_IETX, int_en, NULL);
    
}


//ATT: Si dovrebbe gestire il TIMEOUT con un timer
uint8_t myUART_transmit_k(int descriptor, uint8_t transmit_data){
    uint8_t TBE = 0;
    do{
        read_bit_in_single_pos_k( descriptor, STATUS_REG_OFFSET, ST_TBE, &TBE);
    }while(!TBE);

    return write_reg(descriptor, DBIN_OFFSET, transmit_data);
    
}

//ATT: Si dovrebbe gestire il TIMEOUT con un timer
uint8_t myUART_read_k(int descriptor,uint32_t* status_reg){
    uint8_t RDA = 0;

    do{
        read_bit_in_single_pos_k( descriptor, STATUS_REG_OFFSET, ST_RDA, &RDA);
    }while(!RDA);

    *(status_reg) = read_reg( descriptor, STATUS_REG_OFFSET);
    uint8_t read_value =read_reg( descriptor, DBOUT_OFFSET);
    myUART_Iack_r_k(descriptor); 
    return read_value;
}

void myUART_read_DBOUT_bloc_k(int descriptor,uint32_t* read_value){
   read_reg_bloc_UART_k(descriptor, DBOUT_OFFSET,read_value);
}

int8_t myUART_read_status_bit_k(int descriptor, uint32_t pos, uint8_t *buff){
    return read_bit_in_single_pos_k( descriptor, STATUS_REG_OFFSET, pos, buff);

}

uint32_t myUART_read_status_k(int descriptor){
    return read_reg( descriptor, STATUS_REG_OFFSET);
}

int8_t myUART_Iack_r_k(int descriptor){
    int8_t status = write_bit_in_pos_k( descriptor, CONTROL_REG_OFFSET, CTR_RD, HIGH, NULL);
    if(!status)
        status = write_bit_in_pos_k( descriptor, CONTROL_REG_OFFSET, CTR_RD, LOW, NULL);
    return status;
}

int8_t myUART_Iack_w_k(int descriptor){
    return write_bit_in_pos_k( descriptor, CONTROL_REG_OFFSET, CTR_IACK, HIGH, NULL);
}

void read_reg_bloc_UART_k(int descriptor, uint32_t reg, uint32_t* read_value){
    lseek(descriptor, reg, SEEK_SET);
    read(descriptor, read_value, 2*sizeof(uint32_t));

}

#endif


