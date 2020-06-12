#include "xparameters.h"
#include "xscugic.h"
#include "xil_printf.h"

void loop (void);

int setup (void);

XScuGic gic_inst;

void gpio_swt_IRQHandler(void*);
void gpio_btn_IRQHandler(void*);
uint32_t* swt = (uint32_t*)SWT_BA;
uint32_t* btn = (uint32_t*)BTN_BA;
uint32_t* led = (uint32_t*)LED_BA;

XScuGic gic_inst;


int main(){
	setup();
	while(1){

		loop();
	}


	return 0;
}

void loop(){

}

int setup(void){
	xil_printf("Setup \n");
	XScuGic_Config * gic_conf ;

	//configurazione periferica Switch
	//gpio mode
	swt[0] = 0x0;
	//gpio GIES
	swt[3]= 0x1;
	//gpio PIE
	swt[4] = 0xf;

	//Modalita� edge
	swt[7] = 0xf;
	//rising edge
	swt[8] = 0x0;

	//configurazione periferica Button
	//gpio mode
	btn[0] = 0x0;
	//gpio GIES
	btn[3]= 0x1;
	//gpio PIE
	btn[4] = 0xf;

	//Modalita� edge
	btn[7] = 0xf;
	//rising edge
	btn[8] = 0xf;

	//configurazione base del gic
	gic_conf = XScuGic_LookupConfig(GIC_ID);
	int status = XScuGic_CfgInitialize(&gic_inst, gic_conf, gic_conf->CpuBaseAddress);

	if (status != XST_SUCCESS){
		return status;
	}
	//abilitazione del gic per gestire gli interrupt esterni
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,( void*) &gic_inst);
	Xil_ExceptionEnable();

	//registrazione dell interrupt esterno e associazione del handler
	status = XScuGic_Connect(&gic_inst, SWT_IRQN, (Xil_InterruptHandler)gpio_swt_IRQHandler, ( void*) &gic_inst);
	if (status != XST_SUCCESS){
			return status;
		}

	status = XScuGic_Connect(&gic_inst, BTN_IRQN, (Xil_InterruptHandler)gpio_btn_IRQHandler, ( void*) &gic_inst);
	if (status != XST_SUCCESS){
				return status;
			}


	//configurare interrupt periferica pulendo il registro irq
	XScuGic_Enable( &gic_inst, SWT_IRQN);
	XScuGic_Enable( &gic_inst, BTN_IRQN);

	return XST_SUCCESS;
}



void gpio_swt_IRQHandler(void* data){
	int gpio1 = (swt[5] & GPIO1_MASK) && 1;
	int gpio2 = (swt[5] & GPIO2_MASK) && 1;
	int gpio3 = (swt[5] & GPIO3_MASK) && 1;
	int gpio4 = (swt[5] & GPIO4_MASK) && 1;

	xil_printf("Interrupt Switch %d %d %d %d\n\r",gpio1, gpio2, gpio3, gpio4);
	swt[6] = 0xF;

}


void gpio_btn_IRQHandler(void* data){
	xil_printf("Interrupt Button \n\r");
	btn[6] = 0xF;

}
