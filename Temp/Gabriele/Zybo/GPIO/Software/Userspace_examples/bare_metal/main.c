/**
* @file main.c
* @brief Questo è un esempio di utilizzo della libreria @ref mygpio.h
* @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
*			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
*			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
* @date 12/06/2020
*
* @details
*   In questo esempio è utilizzato il file .xsa generato dai file VHDL presenti in
*   Hardware/GPIO/VHDL. Nell'esempio si utilizzano led in modalità WRITE e switch
*   in modalità READ con interruzioni abilitate con la seguente configurazione:
*   - SW0 e SW1 a livelli
*   - SW2 sensibile sul fronte di salita
*   - SW3 sensibile sul fronte di discesa
*
*   In caso di interruzione da parte di uno switch si accenderà il corrispettivo led.
*   Il led acceso non si spegnerà almeno che non si presenti una interruzione
*   da parte di uno switch differente.
*   <b>NB:</b>mantenendo gli switch in modalità a livello in posizione alta questi
*   continueranno a sollevare interruzioni.
*
* @{
*/


/***************************** Include Files *******************************/
#include "xparameters.h"
#include "xscugic.h"
#include "xil_printf.h"
#include "config.h"
#include "mygpio.h"
#include "utils.h"

/***************************** Variables declaration *******************************/

myGPIO* led;
myGPIO* swt;
//myGPIO* btn;
XScuGic gic_inst;

/***************************** Functions prototype *******************************/
/**
 * @brief Funzione eseguita all'interno del while(1), vuota nello specifico caso
 * @{
 */
void loop (void);
/** @} */

/**
 * @brief Funzione di inizializzazione e configurazione delle periferiche
 * @{
 */
uint32_t setup (void);
/** @} */

/**
 * @brief Funzione chiamata in caso di interrupt sul gpio associato agli switch
 * @{
 */
void gpio_swt_IRQHandler(void*);
/** @} */

//void gpio_btn_IRQHandler(void*);

/***************************** main *******************************/

int main(){

	setup();

	while(1){
		loop();
	}

	return 0;
}

/***************************** Functions definition *******************************/

void loop(){}


uint32_t setup(void){
	xil_printf("Setup \n");

	swt = myGPIO_init(SWT_BA);
	led = myGPIO_init(LED_BA);
	
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

	myGPIO_clear_irq(swt,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	//configurazione led
	myGPIO_set_mode(led, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,  WRITE_MODE);


	gic_enable(GIC_ID, &gic_inst);

	interrupt_handler swt_irq_handler;
	swt_irq_handler.interrupt_line = SWT_IRQN;
	swt_irq_handler.interrupt_handler = gpio_swt_IRQHandler;

	gic_register_interrupt_handler(&gic_inst , &swt_irq_handler);


	return 0;
}


/***************************** Interrupt Handlers *******************************/

void gpio_swt_IRQHandler(void* data){
	int gpio1 = myGPIO_read_pin_irq_status(swt,GPIO_PIN_0);
	int gpio2 = myGPIO_read_pin_irq_status(swt,GPIO_PIN_1);
	int gpio3 = myGPIO_read_pin_irq_status(swt,GPIO_PIN_2);
	int gpio4 = myGPIO_read_pin_irq_status(swt,GPIO_PIN_3);

	xil_printf("Interrupt Switch %d %d %d %d\n\r",gpio1, gpio2, gpio3, gpio4);

	myGPIO_write(led,GPIO_PIN_0, gpio1);
	myGPIO_write(led,GPIO_PIN_1, gpio2);
	myGPIO_write(led,GPIO_PIN_2, gpio3);
	myGPIO_write(led,GPIO_PIN_3, gpio4);

	myGPIO_clear_irq(swt,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

}


//void gpio_btn_IRQHandler(void* data){
//	xil_printf("Interrupt Button \n\r");
//	//myGPIO_clear_irq(btn,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
//
//}

/** @} */
