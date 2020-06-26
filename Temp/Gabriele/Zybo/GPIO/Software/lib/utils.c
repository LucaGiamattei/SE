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


#if defined MYGPIO_BARE_METAL || defined MYUART_BARE_METAL

uint32_t gic_enable(uint32_t gic_id, XScuGic* gic_inst){
    XScuGic_Config * gic_conf ;

    //Configuriamo il GIC
    gic_conf = XScuGic_LookupConfig(gic_id);
	uint32_t status = XScuGic_CfgInitialize(gic_inst, gic_conf, gic_conf->CpuBaseAddress);

	if (status != XST_SUCCESS){
		return status;
	}

	//abilitazione del gic per gestire gli interrupt esterni
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,( void*) gic_inst);
	Xil_ExceptionEnable();

    return XST_SUCCESS;
}

uint32_t gic_disable(uint32_t gic_id){
	Xil_ExceptionDisable();
	return 0;
}

uint32_t gic_register_interrupt(XScuGic *gic_inst ,uint32_t interrupt_line, void* interrupt_handler){

    uint32_t status = XScuGic_Connect(gic_inst, interrupt_line, (Xil_InterruptHandler)interrupt_handler, ( void*) &gic_inst);
	if (status != XST_SUCCESS){
			return status;
		}
    
    XScuGic_Enable( gic_inst, interrupt_line);

    return XST_SUCCESS ;
}

uint32_t gic_register_interrupt_handler(XScuGic *gic_inst, interrupt_handler* interrupt_handler){
    return gic_register_interrupt(gic_inst , interrupt_handler->interrupt_line, interrupt_handler->interrupt_handler);
}

#endif


#if defined MYGPIO_KERNEL || defined MYUART_KERNEL

int open_device(int* file_descriptor, char* device_file){
    printf("Open device_file: %s \n", device_file);
    *file_descriptor = open(device_file, O_RDWR);
	if (*file_descriptor < 1) {
		perror(device_file);
		return -1;
	}
	return 0;
}


uint32_t write_bit_in_pos_k(int descriptor, int32_t reg, uint32_t pos, uint32_t bit){
    lseek(descriptor, reg, SEEK_SET);
    int flags = fcntl(descriptor, F_GETFL, 0);
    fcntl(descriptor, F_SETFL, flags | O_NONBLOCK);
    int32_t write_value = 0;
    read(descriptor, &write_value, sizeof(__uint32_t));
    fcntl(descriptor, F_SETFL, flags & ~O_NONBLOCK);
    if(bit == 0){
        write_value &= ~pos;
    }else if(bit == 1){
        write_value |= pos;
    }else{
        //invalid bit
        return 0;
    }
    write(descriptor, &write_value , sizeof(__uint32_t));
    return write_value;
}
uint8_t  read_bit_in_single_pos_k(int descriptor, int32_t reg, uint8_t pos){
    lseek(descriptor, reg, SEEK_SET);
    int flags = fcntl(descriptor, F_GETFL, 0);
    fcntl(descriptor, F_SETFL, flags | O_NONBLOCK);
    int32_t read_value = 0;
    read(descriptor, &read_value, sizeof(__uint32_t));
    fcntl(descriptor, F_SETFL, flags & ~O_NONBLOCK);
    if(pos && !(pos & (pos-1))){
        return (read_value & pos) && 1;
    }
    return 0;
}

void write_reg(int descriptor, int32_t reg, int32_t write_value){
    lseek(descriptor, reg, SEEK_SET);
    write(descriptor, &write_value , sizeof(__uint32_t));
}
uint8_t  read_reg(int descriptor, int32_t reg){
    lseek(descriptor, reg, SEEK_SET);
    int flags = fcntl(descriptor, F_GETFL, 0);
    fcntl(descriptor, F_SETFL, flags | O_NONBLOCK);
    int32_t read_value = 0;
    read(descriptor, &read_value, sizeof(__uint32_t));
    fcntl(descriptor, F_SETFL, flags & ~O_NONBLOCK);
    return read_value;
}
uint8_t  read_reg_bloc(int descriptor, int32_t reg){
    lseek(descriptor, reg, SEEK_SET);
    int32_t read_value = 0;
    read(descriptor, &read_value, sizeof(__uint32_t));
    return read_value;
}
#endif

#if defined MYGPIO_UIO || defined MYUART_UIO
int32_t wait_interrupt(int uio_descriptor, int32_t *interrupt_count){
    printf("Aspetto interrupt");
     if (read(uio_descriptor, interrupt_count, sizeof(uint32_t)) != sizeof(uint32_t)) {
        printf("Read error!\n");
        return -1;
    }

    return 1;            
}

int32_t reenable_interrupt(int uio_descriptor, int32_t *reenable){   
    if (write(uio_descriptor, (void*)reenable, sizeof(uint32_t)) != sizeof(uint32_t)) {
        printf(" Write error!\n");
        return -2;
    }

    return 1; 
}

#endif
