/**
* @file usb_vcp_debugger.h
* @brief Questa liberia definisce una semplice libreria per effettuare delle stampe
* di debug usando usb otg configurata con la classe CDC
* @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
*			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
*			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
* @date 106/07/2020
*
*
* @{
*/

#ifndef INC_USB_VCP_DEBUGGER_H_
#define INC_USB_VCP_DEBUGGER_H_

/************************** Constant Definitions *****************************/

#define MAX_MSG_LEN 12

/**
 * @name Definizioni delle macro per definire i livelli di debug e i messaggi
 * @{
 */
#define INFO_L 0
#define INFO_MSG "[INFO]: "
#define ERROR_L 1
#define ERROR_MSG "[ERROR]: "
#define WARN_L 2
#define WARN_MSG "[WARN]: "
/*@}**/

/***************************** Include Files *********************************/
#include "stdio.h"

/***************************** Type Definition *******************************/
/**
 * @struct vcp_debugger
 * @brief struct che permette di gestire la configurazione del debugger
 * @var vcp_debugger::fingerprint_len
 * Lunghezza della firma usate per le stampe di debug
 * @var vcp_debugger::fingerprint
 * firma mostrata in ogni print
 */

typedef struct{

	uint16_t fingerprint_len;
	uint8_t *fingerprint;

} vcp_debugger;

/***************************** Function Definition *******************************/

/**
 * @brief Funzione di inizializzazione
 * @param debugger puntatore alla variabile che contiene le informazioni del debugger
 * @param fingerprint firma mostrata in ogni stampa
 *
 */
void USB_VCP_DEBUGGER_init(vcp_debugger *debugger, const uint8_t *fingerprint);/**

/**
 * @brief Stampa messaggi di livello info
 * @param debugger: puntatore alla variabile che contiene le informazioni del debugger
 * @param info: messaggio da stampare
 * @retval stato della periferica USB
 */
uint8_t USB_VCP_DEBUGGER_print_info(vcp_debugger *debugger, const uint8_t *info);

/**
 * @brief seleziona il messaggio in base al livello di debug
 * @param debug_level: livello di debug
 * @param debug_msg: messaggio di debug
 * @retval livello di debug riconosciuto
 */
int8_t get_debug_msg_from_level(uint8_t debug_level, uint8_t *debug_msg);

/**
 * @brief Stampa messaggi di livello debug_level
 * @param debugger: puntatore alla variabile che contiene le informazioni del debugger
 * @param debug_level: livello di debug da stampare
 * @param msg: messaggio da stampare
 * @retval stato della periferica USB
 */
uint8_t USB_VCP_DEBUGGER_print(vcp_debugger *debugger, const uint8_t debug_level , const uint8_t *msg);


#endif /* INC_USB_VCP_DEBUGGER_H_ */

/** @} */

