#include "xparameters.h"
#include "xscugic.h"
#include "xil_printf.h"
#include "config.h"
#include "mygpio.h"

void loop (void);
int setup (void);
myGPIO* led;
myGPIO* swt;
myGPIO* btn;

XScuGic gic_inst;

void gpio_swt_IRQHandler(void*);
void gpio_btn_IRQHandler(void*);

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

	
	//configurazione periferica Switch
	myGPIO_set_mode(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  READ_MODE);

	//gpio GIES
	myGPIO_en_int(swt, INT_EN);

	//gpio PIE
	myGPIO_en_pins_int(swt, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, INT_EN);

	//gpio IRQ_MODE
	myGPIO_set_irq_mode(swt, GPIO_PIN_0|GPIO_PIN_1, INT_LEVEL);
	myGPIO_set_irq_mode(swt, GPIO_PIN_2|GPIO_PIN_3, INT_EDGE);

	//gpio IRQ_EDGE
	myGPIO_set_edge(swt, GPIO_PIN_2, INT_RE);
	myGPIO_set_edge(swt, GPIO_PIN_3, INT_FE);

	gic_inst = gic_enable(GIC_ID);

	interrupt_handler swt_irq_handler;
	swt_irq_handler.interrupt_line = SWT_IRQN;
	swt_irq_handler.interrupt_handler = gpio_swt_IRQHandler;

	return gic_register_interrupt_handler(gic_inst, swt_irq_handler);

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
