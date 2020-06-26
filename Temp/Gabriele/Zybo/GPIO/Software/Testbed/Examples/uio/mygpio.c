/**
* @file mygpio.c
* @brief Questa è l'implementazione della liberia <b>mygpio.h</b> definisce un board 
* che definisce un board support package per la gestione semplificata
* di una periferica AXI Lite che implementa un GPIO.
* @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
*			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
*			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
* @date 13/06/2020
*
* @details
*   Questa è l'implementazione della liberia <b>mygpio.h</b> definisce un board 
*   che definisce un board support package per la gestione semplificata
*   di una periferica AXI Lite che implementa un GPIO. API che permettono di utilizzare 
*   una periferica AXI Lite che implementa un
*   GPIO descritto nel file <b>myGPIO_Int_sel_AXI.vhd</b> presente all'interno
*   del repository.
*
*
* @addtogroup myGPIO
* @{
*/

/***************************** Include Files *********************************/
#include "mygpio.h"
#include "utils.h"
#include "stdio.h"

/***************************** Function Implementation *******************************/

myGPIO* myGPIO_init(uint32_t * peripheral_address){
    return (myGPIO*) peripheral_address;
}

void myGPIO_set_mode_mask(myGPIO * mygpio, uint32_t mode_mask){
    mygpio->MODE = mode_mask;
}

uint32_t myGPIO_set_mode(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t mode){
    return write_bit_in_pos(&mygpio->MODE, GPIO_pins, mode);
}

uint32_t myGPIO_en_int(myGPIO * mygpio, uint8_t int_en){
    return write_bit_in_pos(&mygpio->GIES, GIES_IE, int_en);
}

uint32_t myGPIO_en_pins_int(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t int_en){
    return write_bit_in_pos(&mygpio->PIE, GPIO_pins, int_en);
}

uint32_t myGPIO_set_irq_mode(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t int_mode){
    return write_bit_in_pos(&mygpio->IRQ_MODE, GPIO_pins, int_mode);
}

uint32_t myGPIO_set_edge(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t int_egde){
    return write_bit_in_pos(&mygpio->IRQ_EDGE, GPIO_pins, int_egde);
}

uint32_t myGPIO_write(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t level){
    return write_bit_in_pos(&mygpio->WRITE, GPIO_pins, level);
}

void myGPIO_write_mask(myGPIO * mygpio, uint32_t levels){
    mygpio->WRITE = levels;
}

uint8_t myGPIO_read_pin(myGPIO * mygpio, uint32_t GPIO_pin){
    return read_bit_in_single_pos(&mygpio->READ, GPIO_pin);
}

uint32_t myGPIO_read(myGPIO * mygpio){
    return mygpio->READ;
}

uint32_t myGPIO_clear_irq(myGPIO * mygpio, uint32_t GPIO_pins){
    return write_bit_in_pos(&mygpio->IACK, GPIO_pins, 1);
}

uint32_t myGPIO_read_irq(myGPIO * mygpio){
    return mygpio->IRQ;
}

uint32_t myGPIO_read_pin_irq_status(myGPIO * mygpio, uint32_t GPIO_pin){
    return read_bit_in_single_pos(&mygpio->IRQ, GPIO_pin);
}

/***************************** UIO functions *******************************/

#ifdef MYGPIO_UIO

int32_t wait_interrupt(int uio_descriptor, int32_t *interrupt_count){
    printf("Aspetto interrupt");
     if (read(uio_descriptor, interrupt_count, sizeof(uint32_t)) != sizeof(uint32_t)) {
        printf("%sSWT]%s Read error!\n",COL_BLUE,COL_GRAY);
        return -1;
    }

    return 1;            
}

int32_t reenable_interrupt(int uio_descriptor, int32_t *reenable){   
    if (write(uio_descriptor, (void*)reenable, sizeof(uint32_t)) != sizeof(uint32_t)) {
        printf("%s[SWT]%s Write error!\n",COL_BLUE,COL_GRAY);
        return -2;
    }

    return 1; 
}

void* configure_uio_mygpio(char* filename, int* file_descriptor){
   
    void* vrt_gpio = NULL;

	if (*file_descriptor < 1) {
		printf("Errore nell'aprire il descrittore  del file %s\n", filename);
		return NULL;
	}

    printf("File aperto con successo descrittore: %d \n", *file_descriptor);

    uint32_t page_size = sysconf(_SC_PAGESIZE);		

	vrt_gpio = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, *file_descriptor, 0);
	if (vrt_gpio == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		return NULL;
	}

    printf("Mapping indirizzo avvenuto con successo indirizzo: %08x\n", vrt_gpio);
    
    return vrt_gpio;
}
#endif

#ifdef MYGPIO_NO_DRIVER

void* configure_no_driver_mygpio(int* file_descriptor, uint32_t phy_address){
	// dimensione della pagina di memoria
	uint32_t page_size = sysconf(_SC_PAGESIZE);		
	/* maschera di bit per ottenere l'indirizzo della pagina fisica
	* in cui è mappato l'indirizzo la nostra periferica 
	*/
	uint32_t page_mask = ~(page_size-1);			
	// indirizzo della "pagina fisica" a cui è mappato il device
	uint32_t page_addr =  phy_address & page_mask;		
	// offset del device rispetto all'indirizzo della pagina
	uint32_t offset = phy_address - page_addr;		

	//mapping della pagina fisica, contenente il nostro device, nello spazio d'indirizzamento del processo
	void* vrt_page_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, page_addr);
	if (vrt_page_addr == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		return NULL;
	}
	// indirizzo virtuale del device gpio nello spazio d'indirizzamento del processo
	void* vrt_gpio_addr = vrt_page_addr + offset;	

    return vrt_gpio_addr;
}

#endif


/** @} */
