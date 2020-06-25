/**
* @file myuart.h
* @brief Questa liberia definisce un board support package per la gestione semplificata
* di una periferica AXI Lite che implementa un UART.
* @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
*			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
*			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
* @date 12/06/2020
*
* @details
*   API che permettono di utilizzare una periferica AXI Lite che implementa un
*   UART descritto nel file <b>myUart_AXI.vhd</b> presente all'interno
*   del repository.
*
*
* @addtogroup myUART
* @{
*/

#ifndef MYUART_H
#define MYUART_H

/************************** Constant Definitions *****************************/
/**
 * @name Definizioni delle macro per identificare i bit
 * @brief Macro utili a identificare i bit dei registri di controllo e di stato
 * @{
 */ 
#define CTR_RD      (uint32_t) 1 << 0
#define CTR_WR      (uint32_t) 1 << 1 
#define CTR_IERX    (uint32_t) 1 << 2 
#define CTR_IETX    (uint32_t) 1 << 3
#define CTR_IACK    (uint32_t) 1 << 4

#define ST_RDA      (uint32_t) 1 << 0
#define ST_TBE      (uint32_t) 1 << 1
#define ST_PE       (uint32_t) 1 << 2
#define ST_FE       (uint32_t) 1 << 3
#define ST_OE       (uint32_t) 1 << 4
/** @} */

/**
 * @name Definizioni delle macro per il livello logico del valore dei letto o scritto sui pin
 * @{
 */
#define LOW         0
#define HIGH        1
/** @} */

/**
 * @name Definizioni delle macro per l'abilitazione dell'interrupt
 * @{
 */
#define INT_EN      1
#define INT_DIS     0
/** @} */


/**
 * @name Definizioni delle macro per identificare l'offset dei registri della periferica
 * @{
 */
#define DBIN_OFFSET             0x00
#define DBOUT_OFFSET            0x04
#define CONTROL_REG_OFFSET      0x08
#define STATUS_REG_OFFSET       0x0C
/** @} */

/***************************** Include Files *********************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
/***************************** Type Definition *******************************/

/**
 * @name myUART STRUCT
 * @struct myUART
 * @brief struct che permette una semplice gestione della periferica
 * @var myUART::DBIN
 * Registro dati contenente il byte che verrà inviato sulla periferica
 * @var myUART::DBOUT
 * Registro dati  contenente il byte che è stato ricevuto sulla periferica
 * @var myGPIO::CONTROL_REG
 * Registro di controllo, consente di controllare la periferica con i seguenti bit:
 * - IACK_T => Acknowledgement dell'interrupt in scrittura
 * - IE     => Interrupt enable, abilita le interruzioni
 * - WR     => Write, indica alla periferica che può inviare il contenuto di DBIN
 * - RD     => Read, indica alla periferica che il PS ha consumato il byte contenuto in DBOUT
 * @var myGPIO::STATUS_REG
 * Registro di stato, con i seguenti flag:
 * - RDA    => Read Data Available, indica che la periferica ha ricevuto in byte ed è pronto
 *  per essere consumato
 * - TBE    => Transmission Buffer Empty, Indica al PS che la periferica ha trasmesso e 
 * che DBIN è vuoto
 * - PE	    => Parity Error, indica che è avvenuto un'errore di parità nella ricezione
 * - FE	    => Frame Error, indica che è stato ricevuto un bit in più
 * - OE	    => Overwrite Error, indica che è stato ricevuto in ulteriore byte prima che il
 *  precedente fosse consumato, e di conseguenza, quest'ultimo, è perso.
 * @{
 */
typedef struct{
    uint32_t DBIN; 
    uint32_t DBOUT;
    uint32_t CONTROL_REG;
    uint32_t STATUS_REG;
} myUART;
/** @} */
/***************************** Function Definition *******************************/

/**
 * @name myUART API
 * @brief API per poter configurare e utilizzare la periferica UART
 * @{
 */

 /** @brief Funzione di inizializzazione,Effettua un casting dell'indirizzo in modo tale da 
 * poter utilizzare la struct al meglio
 * @param peripheral_address indirizzo base della periferica AXI Lite
 * @retval puntatore ad una struct myUART il cui indirzzo base è quello di peripheral_address
 * @{
 */
myUART* myUART_init(uint32_t *peripheral_address);
/** @} */

/**
 * @brief Abilita/Disabilita le interruzioni per la ricezione
 * @param myuart puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param int_en INT_EN abilitato, INT_DIS GIES disabilitato 
 * @retval valore del registro di controllo dopo l'operazione di scrittura
 * @{
 */
uint32_t myUART_en_int_rx(myUART * myuart, uint8_t int_en);
/** @} */

/**
 * @brief Abilita/Disabilita le interruzioni per la tramissione
 * @param myuart puntatore alla sctruct che identifica la periferica che si vuole configurare
 * @param int_en INT_EN abilitato, INT_DIS GIES disabilitato 
 * @retval valore del registro di controllo dopo l'operazione di scrittura
 * @{
 */
uint32_t myUART_en_int_tx(myUART * myuart, uint8_t int_en);
/** @} */

/**
 * @brief Funzione di trasmissione di un byte in modalità interrupt
 * Scrive il byte sul bus di ingresso e alzando il bit WR inizierà la trasmissione.
 * @param myuart puntatore alla sctruct che identifica la periferica che si vuole utilizzare
 * @param transmit_data byte di dati da trasmettere
 * @{
 */
void myUART_transmit_int(myUART * myuart, uint8_t transmit_data);
/** @} */

/**
 * @brief Funzione di trasmissione di un byte in modalità polling
 * Scrive il byte sul bus di ingresso e alzando il bit WR inizierà la trasmissione,
 * aspetterà attivamente fino al completamento della trasmissione.
 * @param myuart puntatore alla sctruct che identifica la periferica che si vuole utilizzare
 * @param transmit_data byte di dati da trasmettere
 * @{
 */
void myUART_transmit(myUART * myuart, uint8_t transmit_data);
/** @} */

/**
 * @brief Funzione di ricezione di un byte in modalità polling
 * Questa funzione mette in attesa attiva fintanto che non si riceve un byte.
 * @param myuart puntatore alla sctruct che identifica la periferica che si vuole utilizzare
 * @retval byte letto 
 * @{
 */
uint8_t myUART_read(myUART * myuart, uint32_t* status_reg);
/** @} */

/**
 * @brief Funzione di lettura del bus DBOUT
 *
 * @param myuart puntatore alla sctruct che identifica la periferica che si vuole utilizzare
 * @retval byte contenuto in DBOUT
 * @{
 */
uint8_t myUART_read_DBOUT(myUART * myuart);
/** @} */

/**
 * @brief Funzione di lettura di un bit del registro di stato
 * Questa funzione legge dal registro di stato il bit indicato
 * @param myuart puntatore alla sctruct che identifica la periferica 
 * @param pos maschera che indica il bit da leggere
 * @retval byte letto 
 * @{
 */
uint8_t myUART_read_status_bit(myUART * myuart, uint32_t pos);
/** @} */

/**
 * @brief Funzione di lettura del registro di stato
 * Questa funzione legge l'intero registro di stato
 * @param myuart puntatore alla sctruct che identifica la periferica  
 * @retval registro di stato
 * @{
 */
uint32_t myUART_read_status(myUART * myuart);
/** @} */

/**
 * @brief Acknowledgement dell'interruzione in lettura
 * Alzando il bit RD del registro di controllo, resetta la periferica per una nuova read
 * @param myuart puntatore alla sctruct che identifica la periferica
 * @{
 */
void myUART_Iack_r(myUART * myuart);
/** @} */

/**
 * @brief Acknowledgement dell'interruzione in scrittura
 * Alzando il bit IACK_T del registro di controllo, abbassa la linea di interruzione di scrittura
 * per indicare che l'interruzione di scrittura è stata gestita.
 * @param myuart puntatore alla sctruct che identifica la periferica
 * @{
 */
void myUART_Iack_w(myUART * myuart);
/** @} */

/** @} */


#endif
