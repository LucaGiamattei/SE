/**
* @file mygpio.h
* @brief Questa liberia definisce un board support package per la gestione semplificata
* di una periferica AXI Lite che implementa un GPIO.
* @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
*			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
*			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
* @date 12/06/2020
*
* @details
*   API che permettono di utilizzare una periferica AXI Lite che implementa un
*   GPIO descritto nel file <b>myGPIO_Int_sel_AXI.vhd</b> presente all'interno
*   del repository.
*
*
* @addtogroup myGPIO
* @{
*/

#ifndef MYGPIO_H
#define MYGPIO_H


/************************** Constant Definitions *****************************/
/**
 * @name Definizioni delle macro per identificare i pin
 * @brief <b>Nota</b> per permettere utilizzare i restanti 24 pin è possibile 
 * crearsi delle machere oppure continuare la definizione delle seguenti macro
 * @{
 */
#define GPIO_PIN_0 (uint32_t) 1 << 0
#define GPIO_PIN_1 (uint32_t) 1 << 1
#define GPIO_PIN_2 (uint32_t) 1 << 2
#define GPIO_PIN_3 (uint32_t) 1 << 3
#define GPIO_PIN_4 (uint32_t) 1 << 4
#define GPIO_PIN_5 (uint32_t) 1 << 5
#define GPIO_PIN_6 (uint32_t) 1 << 6
#define GPIO_PIN_7 (uint32_t) 1 << 7
/** @} */

/**
 * @name Definizioni delle macro per identificare la modalità di funzionamento dei pin
 * @{
 */
#define READ_MODE   0
#define WRITE_MODE  1
/** @} */

/**
 * @name Definizioni delle macro per il livello logico del valore dei letto o scritto sui pin
 * @{
 */
#define LOW         0
#define HIGH        1
/** @} */

/**
 * @name Definizioni delle macro per identificare i bit del GIES
 * @{
 */
#define GIES_IE     (uint32_t) 1 << 0   /** Interrupt Enable */
#define GIES_IS     (uint32_t) 1 << 1   /** Interrupt Status */
/** @} */

/**
 * @name Definizioni delle macro per l'abilitazione dell'interrupt di un pin
 * @{
 */
#define INT_EN      1
#define INT_DIS     0
/** @} */

/**
 * @name Definizioni delle macro per scegliere la modalità di funzionamento dell'interrupt di un pin
 * @{
 */
#define INT_LEVEL   0
#define INT_EDGE    1
/** @} */

/**
 * @name Definizioni delle macro per scegliere il fronte su cui è sensibile il pin per generare l'interrupt 
 * @{
 */
#define INT_RE      1
#define INT_FE      0
/** @} */

/**
 * @name Definizioni delle macro per identificare l'offset dei registri della periferica
 * @{
 */
#define MODE_OFFSET     0x00
#define WRITE_OFFSET    0x04
#define READ_OFFSET     0x08
#define GIES_OFFSET     0x0C
#define PIE_OFFSET      0x10
#define IRQ_OFFSET      0x14
#define IACK_OFFSET     0x18
#define IRQ_MODE_OFFSET 0x1C
#define IRQ_EDGE_OFFSET 0x1F
/** @} */

/***************************** Include Files *********************************/
#include "stdlib.h"
#include "stdio.h"

/***************************** Type Definition *******************************/

/**
 * @name myGPIO STRUCT
 * @struct myGPIO
 * @brief struct che permette una semplice gestione della periferica
 * @var myGPIO::MODE
 * Registro MODE, permette di settare la modalità di funzionamento del pin
 * @var myGPIO::WRITE
 * Registro WRITE, permette di scrivere un valore logico sul pin
 * @var myGPIO::READ
 * Registro READ, permette di leggere il pin
 * @var myGPIO::GIES
 * Registro GIES, permette di abilitare le interruzioni globali e di leggere
 * lo stato dell'interrupt bit
 * @var myGPIO::PIE
 * Registro PIE, permette di abilitare l'interruzione sul singolo pin
 * @var myGPIO::IRQ
 * Registro IRQ, indica quale pin è stata sorgente di interruzione
 * @var myGPIO::IACK
 * Registro IACK, permette segnare come servita un'interruzione
 * @var myGPIO::IRQ_MODE
 * Registro IRQ_MODE, permette di settare la modalità di funzionamento 
 * dell'interruzione per il pin
 * @var myGPIO::IRQ_EDGE
 * Registro IRQ_EDGE, permette di settare il fronte su cui è sensibile il pin
 * per la generazione dell'interruzione
 * @{
 */
typedef struct{
    uint32_t MODE; 
    uint32_t WRITE;
    uint32_t READ;
    uint32_t GIES;
    uint32_t PIE;
    uint32_t IRQ;
    uint32_t IACK;
    uint32_t IRQ_MODE;
    uint32_t IRQ_EDGE;
} myGPIO;
/** @} */
/***************************** Function Definition *******************************/

/**
 * @name myGPIO API
 * @brief API per poter configurare e utilizzare il GPIO
 * @{
 */

 /** @brief Funzione di inizializzazione,Effettua un casting dell'indirizzo in modo tale da 
 * poter utilizzare la struct al meglio
 * @param peripheral_address indirizzo base della periferica AXI Lite
 * @retval puntatore ad una struct myGPIO il cui indirzzo base è quello di peripheral_address
 * @{
 */
myGPIO* myGPIO_init(uint32_t *peripheral_address);
/** @} */

/**
 * 
 * @brief Imposta lo stesso valore, mode, per i pin identificati dalla maschera GPIO_pins
 * <b>Nota</b>: setta lo stesso valore per tutti i pin.
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param GPIO_pins maschera dei pin che si vogliono configurare, può essere costruita 
 * mettendo in <b>or</b> ('|') le macro GPIO_PIN_X
 * @param mode modalità di funzionamento dei pin 
 * @retval valore del registro MODE dopo l'operazione di scrittura
 * @{
 */
uint32_t myGPIO_set_mode(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t mode);
/** @} */

/**
 * @brief Imposta il registro MODE della periferica con la maschera data in input
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param mode_mask maschera dei bit che si vogliono scrivere sulla periferica
 * @{
 */
void myGPIO_set_mode_mask(myGPIO * mygpio, uint32_t mode_mask);
/** @} */

/**
 * @brief Abilita/Disabilita il GIES
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param int_en INT_EN abilitato, INT_DIS GIES disabilitato 
 * @retval valore del registro GIES dopo l'operazione di scrittura
 * @{
 */
uint32_t myGPIO_en_int(myGPIO * mygpio, uint8_t int_en);
/** @} */

/**
 * @brief Abilitazione interrupt per il pin, setta il registro PIE.
 * <b>Nota</b>: setta lo stesso valore per tutti i pin.
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param GPIO_pins maschera dei pin che si vogliono configurare, può essere costruita 
 * mettendo in <b>or</b> ('|') le macro GPIO_PIN_X
 * @param int_en INT_EN interrupt abilitato, INT_DIS interrupt disabilitato 
 * @retval valore del registro PIE dopo l'operazione di scrittura
 * @{
 */
uint32_t myGPIO_en_pins_int(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t int_en);
 /** @} */

/**
 * @brief Selezione modalità di interrupt per il pin, setta il registro IRQ_MODE. 
 * <b>Nota</b>: setta lo stesso valore per tutti i pin.
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param GPIO_pins maschera dei pin che si vogliono configurare, può essere costruita 
 * mettendo in <b>or</b> ('|') le macro GPIO_PIN_X
 * @param int_en INT_LEVEL livelli, INT_EDGE sensibile sul fronte
 * @retval valore del registro IRQ_MODE dopo l'operazione di scrittura
 * @{
 */
 uint32_t myGPIO_set_irq_mode(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t int_mode);
/** @} */

/**
 * @brief Seleziona il fronte su cui è sensibile il pin per la generazione dell' interrupt,
 *  setta il registro IRQ_MODE. 
 * <b>Nota</b>: setta lo stesso valore per tutti i pin.
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param GPIO_pins maschera dei pin che si vogliono configurare, può essere costruita 
 * mettendo in <b>or</b> ('|') le macro GPIO_PIN_X
 * @param int_egde INT_RE Fronte di salita, INT_FE fronte di discesa
 * @retval valore del registro IRQ_EDGE dopo l'operazione di scrittura
 * @{
 */
uint32_t myGPIO_set_edge(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t int_egde);
/** @} */

/**
 * @brief Scrittura del valore sui i pin GPIO_pins. 
 * Scrive sul registro WRITE della periferica.
 * <b>Nota</b>: setta lo stesso valore per tutti i pin.
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param GPIO_pins maschera dei pin che si vogliono configurare, può essere costruita 
 * mettendo in <b>or</b> ('|') le macro GPIO_PIN_X
 * @param level HIGH valore logico alto, LOW valore logico basso
 * @retval valore del registro WRITE dopo l'operazione di scrittura
 * @{
 */
uint32_t myGPIO_write(myGPIO * mygpio, uint32_t GPIO_pins, uint8_t level);
/** @} */

/**
 * @brief Scrittura della maschera dei valori sui pin. 
 * Scrive sul registro WRITE della periferica.
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param levels maschera dei valori che si volgiono scrivere sui pin,
 *  HIGH valore logico alto, LOW valore logico basso
 * @{
 */
void myGPIO_write_mask(myGPIO * mygpio, uint32_t levels);
/** @} */

/**
 * @brief Lettura del valore di un pin . 
 * Legge il registro READ della periferica.
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param GPIO_pin Pin di cui si vuole leggere il valore
 * @retval valore letto, HIGH valore logico alto, LOW valore logico basso
 * @{
 */
uint8_t myGPIO_read_pin(myGPIO * mygpio, uint32_t GPIO_pin);
/** @} */

/**
 * @brief Lettura del registro READ. 
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @retval valore del registro READ
 * @{
 */
uint32_t myGPIO_read(myGPIO * mygpio);
/** @} */


/**
 * @brief Acknowledgement di un'interruzione
 * Legge il registro READ della periferica.
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param GPIO_pins maschera dei pin di cui si vuol pulire lo stato d'interruzione, può essere costruita 
 * mettendo in <b>or</b> ('|') le macro GPIO_PIN_X
 * @retval valore del registro IRQ dopo l'operazione
 * @{
 */
uint32_t myGPIO_clear_irq(myGPIO * mygpio, uint32_t GPIO_pins);
/** @} */

/**
 * @brief Lettura del registro IRQ. 
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @retval valore del registro IRQ
 * @{
 */uint32_t myGPIO_read_irq(myGPIO * mygpio);
/** @} */

/**
 * @brief Lettura dello stato d'interruzione per il singolo pin.
 * @param mygpio puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param GPIO_pin Pin di cui si vuole leggere lo stato d'interruzione
 * @retval valore del registro IRQ in corrispondenza dle bit idindicato da GPIO_pin, '1' richiesta
 * di interrupt
 * @{
 */
uint32_t myGPIO_read_pin_irq_status(myGPIO * mygpio, uint32_t GPIO_pin);
/** @} */

/***************************** UIO functions *******************************/

#ifdef MYGPIO_UIO

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
void* configure_uio_mygpio(char* filename, int* file_descriptor);
/** @} */
#endif

#ifdef MYGPIO_NO_DRIVER
/**
 * @brief Esegue mmap sul file /dev/mem aperto con la seguente configurazione: PROT_READ | PROT_WRITE e MAP_SHARED
 * @param file_descriptor descrittore del file /dev/mem aperto
 * @param phy_address indirizzo fisico base della periferica memory mapped
 * @retval NULL nel caso in cui "mmap" ritorni MAP_FAILED, altrimenti l'indirizzo virtuale base della periferica
 * @{
 */
void* configure_no_driver_mygpio(int* file_descriptor, uint32_t phy_address);
/** @} */
#endif


/** @} */


#endif
