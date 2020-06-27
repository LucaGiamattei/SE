#ifndef UTILS_H
#define UTILS_H

#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"

//Scrive lo stesso bit nelle posizioni desiderate
uint32_t write_bit_in_pos(uint32_t* address, uint32_t pos, uint32_t bit);
uint8_t  read_bit_in_single_pos(uint32_t* address, uint8_t pos);

#if defined MYGPIO_KERNEL || defined MYUART_KERNEL
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

 /** @brief Funzione di inizializzazione,apre il file (in /dev) a cui è mappata la periferica per ottenere il descrittore.
 * @param file_descriptor descrittore del file aperto dalla funzione (parametro di ingresso uscita)
 * @param device_file nome del file da aprire
 * @retval 0 se l'operazione di apertura è andata a buon fine e il descrittore è valido, -1 altrimenti
 * @{
 */
int open_device(int* file_descriptor, char* device_file);
/** @} */

uint32_t write_bit_in_pos_k(int descriptor, int32_t reg, uint32_t pos, uint32_t bit);
uint8_t  read_bit_in_single_pos_k(int descriptor, int32_t reg, uint8_t pos);
void     write_reg(int descriptor, int32_t reg, int32_t write_value);
uint8_t  read_reg(int descriptor, int32_t reg);
uint8_t  read_reg_bloc(int descriptor, int32_t reg);

#endif

#if defined MYGPIO_BARE_METAL || defined MYUART_BARE_METAL
#include "xscugic.h"

typedef struct{
    uint32_t interrupt_line;
    void* interrupt_handler;
}interrupt_handler;

uint32_t gic_enable(uint32_t gic_id,XScuGic* gic_inst);
uint32_t gic_disable(uint32_t gic_id);

uint32_t gic_register_interrupt(XScuGic* gic_inst, uint32_t interrupt_line, void* interrupt_handler);
uint32_t gic_register_interrupt_handler(XScuGic* gic_inst, interrupt_handler* interrupt_handler);
#endif

#if defined MYGPIO_UIO || defined MYUART_UIO
#include <sys/mman.h>
#include <unistd.h>
/**
 * @brief Read UIO, pone in attesa di un interruzione
 * @param uio_descriptor descrittore del file uio aperto
 * @param interrupt_count variabile di conteggio delle interruzioni, aggiornata dalla read
 * @retval valore negativo in caso di errore della read, postitivo altrimenti
 * @{
 */
int32_t wait_interrupt(int uio_descriptor, int32_t* interrupt_count);
/** @} */

/**
 * @brief Write UIO, riabilita le interruzioni
 * @param uio_descriptor descrittore del file uio aperto
 * @param reenable valore "1" per la riabilitazione delle interruzioni
 * @retval valore negativo in caso di errore della write, postitivo altrimenti
 * @{
 */
int32_t reenable_interrupt(int uio_descriptor, int32_t* reenable);
/** @} */

/**
 * @brief Esegue mmap sul file uio aperto con la seguente configurazione: PROT_READ | PROT_WRITE e MAP_SHARED
 * @param filename nome del file, per la stampa di errore sul file descriptor
 * @param file_descriptor descrittore del file uio aperto
 * @retval NULL nel caso in cui il descrittore non sia valido o nel caso in cui 
 * "mmap" ritorni MAP_FAILED 
 * @{
 */
void* configure_uio(char* filename, int* file_descriptor);
/** @} */

#endif
#if defined MYGPIO_NO_DRIVER|| defined MYUART_NO_DRIVER
#include <sys/mman.h>
#include <unistd.h>
/**
 * @brief Esegue mmap sul file /dev/mem aperto con la seguente configurazione: PROT_READ | PROT_WRITE e MAP_SHARED
 * @param file_descriptor descrittore del file /dev/mem aperto
 * @param vrt_page_addr indirizzo virtuale della pagina a cui appartiene quello della periferica memory mapped
 * @param phy_address indirizzo fisico base della periferica memory mapped
 * @retval NULL nel caso in cui "mmap" ritorni MAP_FAILED, altrimenti l'indirizzo virtuale base della periferica
 * @{
 */
void* configure_no_driver(int file_descriptor, void** vrt_page_addr, uint32_t phy_address);
#endif

/** @} */
#endif