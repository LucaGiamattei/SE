#include "utils.h"
#include "xscugic.h"

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
