#ifndef UTILS_H
#define UTILS_H



/**
 * @file utils.h
 * @brief La liberia utils definisce delle funzioni comuni alla gestione semplificata di una periferica AXI Lite generica \n
 * @details Le funzioni da prendere in considerazione dipendono dal contesto di esecuzione. \n
 * Bare metal considera un contesto in cui non è presente il sistema operativo e si è a diretto contatto con l'hardware. \n
 * Le altre tipologie di API presenti nella libreria considerano la presenza di un sistema operativo.
 *  @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
 *			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
 *			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
 * @date 15/06/2020
 * @{
 */
 


#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"

#if defined MYGPIO_NO_DRIVER|| defined MYUART_NO_DRIVER || defined MYGPIO_BARE_METAL || defined MYUART_BARE_METAL||defined MYGPIO_UIO || defined MYUART_UIO

/** 
 * @brief Permette di scrivere un bit in una specifica posizione in un determinato registro
 * @param address indirizzo del registro
 * @param pos posizione del bit
 * @param bit 1 asserisce il livello alto, 0 asserisce il livello basso
 * @param writed_value valore del registro scritto
 * @retval codice di stato: 0 nessun errore, -1 errore
 * 
 */
int8_t write_bit_in_pos(uint32_t* address, uint32_t pos, uint32_t bit, uint32_t* writed_value);

/** 
 * @brief Permette di leggere un bit in una specifica posizione in un determinato registro
 * @param address indirizzo del registro
 * @param pos posizione del bit
 * @param buff valore del bit letto
 * @retval codice di stato: 0 nessun errore, -1 errore
 * 
 */
int8_t read_bit_in_single_pos(uint32_t* address, uint8_t pos, uint8_t* buff);

#endif 

#if defined MYGPIO_KERNEL || defined MYUART_KERNEL
/**
 * @name API per interfacciamento tramito modulo kernel
 * Funzioni utilizzate per interfacciarsi con la periferica tramite un modulo kernel. 
//@{
 */
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

 /** 
 * @brief Richiama la system call open
 * @param file_descriptor descrittore del file aperto dalla funzione (parametro di ingresso uscita)
 * @param device_file nome del file da aprire
 * @retval result 0 se l'operazione di apertura è andata a buon fine e il descrittore è valido, -1 altrimenti
 * 
 */
int8_t open_device(int* file_descriptor, char* device_file);


/**
 * @brief Permette di scrivere un bit in una specifica posizione in un determinato registro prendendo in ingresso il descrittore del file aperto 
 * @param descriptor descrittore del file aperto dalla funzione (parametro di ingresso uscita)
 * @param reg offset del registro 
 * @param pos posizione del bit
 * @param bit 1 asserisce il livello alto, 0 asserisce il livello basso
 * @param writed_value writed_value
 * @retval codice di stato: 0 nessun errore, -1 errore
 * 
 */
int8_t write_bit_in_pos_k(int descriptor, int32_t reg, uint32_t pos, uint32_t bit, uint32_t* writed_value);


/** 
 * @brief Permette di leggere un bit in una specifica posizione in un determinato registro prendendo in ingresso il descrittore del file aperto 
 * @param descriptor descrittore del file aperto dalla funzione (parametro di ingresso uscita)
 * @param reg offset del registro 
 * @param pos posizione del bit 
 * @param buff valore del bit letto
 * @retval register il valore del registro aggiornato
 */
int8_t  read_bit_in_single_pos_k(int descriptor, int32_t reg, uint8_t pos, uint8_t* buff);


/** 
 * @brief Permette di scrivere un intero registro di 32 bit  prendendo in ingresso il descrittore del file aperto 
 * @param descriptor descrittore del file aperto dalla funzione (parametro di ingresso uscita)
 * @param reg offset del registro 
 * @param write_value valore da asserire al registro
 * @retval In caso di successo è il numero di byte scritti. in caso di errore ritorna -1  
 */
size_t     write_reg(int descriptor, int32_t reg, int32_t write_value);


/** 
 * @brief Permette di leggere un intero registro di 32 bit  prendendo in ingresso il descrittore del file aperto 
 * @param descriptor descrittore del file aperto dalla funzione (parametro di ingresso uscita)
 * @param reg offset del registro 
 * @retval In caso di successo è il numero di byte scritti. in caso di errore ritorna -1  
 */
size_t  read_reg(int descriptor, int32_t reg);


/** 
 * @brief Permette di leggere un intero registro di 32 bit utilizzando una read bloccante prendendo in ingresso il descrittore del file aperto 
 * @param descriptor descrittore del file aperto dalla funzione (parametro di ingresso uscita)
 * @param reg offset del registro 
 * @retval In caso di successo è il numero di byte scritti. in caso di errore ritorna -1  
 */
size_t  read_reg_bloc(int descriptor, int32_t reg);

//@}
#endif

#if defined MYGPIO_BARE_METAL || defined MYUART_BARE_METAL
/**
 * @name API per interfacciamento bare metal
 * Funzioni utilizzate per interfacciarsi con la periferica in modo bare metal, ovvero senza sistema operativo.
//@{
 */
#include "xscugic.h"

/**
 * @struct interrupt_handler
 * @brief Struttura che astrae le proprietà di un interrupt handler
 * @var interrupt_handler::interrupt_line linea su cui registrare l'interrupt handler
 * @var interrupt_handler::interrupt_handler puntatore alla definizione della funzione interrupt handler da associare alla linea
 */
typedef struct{
    uint32_t interrupt_line;
    void* interrupt_handler;
}interrupt_handler;



/**
 * @brief Inizializza e configura l'Interrupt controller. Abilitazione del gic alla gestione delle interruzioni.
 * @param gic_id identificativo del gic
 * @param gic_inst instanza di XScuGic
 * @retval result XST_SUCCESS in caso di successo,
 *  lo status di inizializzazione nel caso in cui l'inizializzazione dell'interrupt controller non è andata a buon fine
 */
uint32_t gic_enable(uint32_t gic_id,XScuGic* gic_inst);


/**
 * @brief Disabilita l'interrupt controller
*/
void gic_disable();


/**
 * @brief Registra per l'interrupt line  passata in ingresso la funzione interrupt handler 
 * @param gic_inst instanza del gic
 * @param interrupt_line linea su cui registrare l'interrupt handler
 * @param interrupt_handler definizione della funzione interrupt handler da associare alla linea
 * @retval result XST_SUCCESS in caso di corretta registrazione,altrimenti lo status della connessione
 */
uint32_t gic_register_interrupt(XScuGic* gic_inst, uint32_t interrupt_line, void* interrupt_handler);


/**
 * @brief Registra per l'interrupt line  passata in ingresso la funzione interrupt handler 
 * @param gic_inst instanza del gic
 * @param interrupt_handler instanza della struttura interrupt_handler
 * @retval result XST_SUCCESS in caso di corretta registrazione,altrimenti lo status della connessione
 */
uint32_t gic_register_interrupt_handler(XScuGic* gic_inst, interrupt_handler* interrupt_handler);



//@}
#endif

#if defined MYGPIO_UIO || defined MYUART_UIO
/**
 * @name API per interfacciamento tramite driver UIO
 * Funzioni utilizzate per interfacciarsi con la periferica tramite il driver generico UIO.
//@{
 */
#include <sys/mman.h>
#include <unistd.h>
/**
 * @brief Read UIO, pone in attesa di un interruzione
 * @param uio_descriptor descrittore del file uio aperto
 * @param interrupt_count variabile di conteggio delle interruzioni, aggiornata dalla read
 * @retval result valore negativo in caso di errore della read, postitivo altrimenti
 */
int32_t wait_interrupt(int uio_descriptor, int32_t* interrupt_count);


/**
 * @brief Write UIO, riabilita le interruzioni
 * @param uio_descriptor descrittore del file uio aperto
 * @param reenable valore "1" per la riabilitazione delle interruzioni
 * @retval result valore negativo in caso di errore della write, postitivo altrimenti
 */
int32_t reenable_interrupt(int uio_descriptor, int32_t* reenable);


/**
 * @brief Esegue mmap sul file uio aperto con la seguente configurazione: PROT_READ | PROT_WRITE e MAP_SHARED
 * @param filename nome del file, per la stampa di errore sul file descriptor
 * @param file_descriptor descrittore del file uio aperto
 * @retval vrt_addr NULL nel caso in cui il descrittore non sia valido o nel caso in cui 
 * "mmap" ritorni MAP_FAILED, altrimenti l'indirizzo virtuale a cui è stata mappata la periferica 
 */
void* configure_uio(char* filename, int* file_descriptor);

//@}
#endif


#if defined MYGPIO_NO_DRIVER|| defined MYUART_NO_DRIVER
#include <sys/mman.h>
#include <unistd.h> 

/** @name API per interfacciamento no driver
 *  Funzioni utilizzate per interfacciarsi con la periferica bypassando il sistema operativo.
 */
//@{

/**
 * @brief Esegue mmap sul file /dev/mem aperto con la seguente configurazione: PROT_READ | PROT_WRITE e MAP_SHARED
 * @param file_descriptor descrittore del file /dev/mem aperto
 * @param vrt_page_addr puntatore all'indirizzo virtuale della pagina a cui appartiene l'indirizzo virtuale della periferica memory mapped (parametro di ingresso uscita)
 * @param phy_address indirizzo fisico base della periferica memory mapped
 * @retval vrt_addr NULL nel caso in cui "mmap" ritorni MAP_FAILED, altrimenti l'indirizzo virtuale base della periferica
 * 
 */
void* configure_no_driver(int file_descriptor, void** vrt_page_addr, uint32_t phy_address);
//@}
#endif
#endif


 /* @} */
