/**
* @file usb_vcp_debugger.c
* @brief Implementazione della libreria definita in usb_vcp_debugger.h
*
* @authors <b> Giorgio Farina</b> <giorgio.fari96@gmail.com> <br>
*			 <b> Luca Giamattei</b>  <lgiamattei@gmail.com> <br>
*			 <b> Gabriele Previtera</b>  <gabrieleprevitera@gmail.com> <br>
* @date 106/07/2020
*
*
* @{
*/

/***************************** Include Files *********************************/
#include "usb_vcp_debugger.h"
#include "usb_device.h"
#include "string.h"

/***************************** Function Implementation *******************************/
void USB_VCP_DEBUGGER_init(vcp_debugger *debugger, const uint8_t *fingerprint){
	int fing_len = strlen(fingerprint);
	debugger->fingerprint = malloc(sizeof(uint8_t) * (fing_len + 4));

	strncpy(debugger->fingerprint, fingerprint, strlen(fingerprint));
	debugger->fingerprint[fing_len]  = '\0';

	strncat(debugger->fingerprint, ":\r\n", 3);
	debugger->fingerprint[debugger->fingerprint_len]  = '\0';
	debugger->fingerprint_len = fing_len + 4;

	USB_VCP_DEBUGGER_print(debugger, INFO_L, "Debugger started");
}

uint8_t USB_VCP_DEBUGGER_print_info(vcp_debugger *debugger, const uint8_t *msg){
	return USB_VCP_DEBUGGER_print(debugger, INFO_L, msg);
}

uint8_t USB_VCP_DEBUGGER_print(vcp_debugger *debugger, const uint8_t debug_level , const uint8_t *msg){

	uint8_t *debug_msg = malloc(sizeof(uint8_t) * (MAX_MSG_LEN));
	get_debug_msg_from_level(debug_level, debug_msg);

	uint8_t msg_len = strlen(msg);
	uint16_t tot_msg_len = msg_len + debugger->fingerprint_len + strlen(debug_msg) + 3;
	uint8_t *str = malloc(sizeof(uint8_t) * (tot_msg_len));

	strncpy(str, debugger->fingerprint, debugger->fingerprint_len);
	strncat(str, debug_msg, strlen(debug_msg));
	strncat(str, msg, msg_len);

	strncat(str, "\r\n", 2);
	str[tot_msg_len]  = '\0';

	int status =  CDC_Transmit_FS(str, tot_msg_len );

	free(str);
	free(debug_msg);

	return status;
}

int8_t get_debug_msg_from_level(uint8_t debug_level, uint8_t *debug_msg){

	int8_t status = -1;

	switch(debug_level){
		case INFO_L:
			strncpy(debug_msg, INFO_MSG, strlen(INFO_MSG));
			debug_msg[strlen(INFO_MSG)] = '\0';
			status = INFO_L;
			break;
		case ERROR_L:
			strncpy(debug_msg, ERROR_MSG, strlen(ERROR_MSG));
			debug_msg[strlen(ERROR_MSG)] = '\0';
			status = ERROR_L;
			break;
		case WARN_L:
			strncpy(debug_msg, WARN_MSG, strlen(WARN_MSG));
			debug_msg[strlen(WARN_MSG)] = '\0';
			status = WARN_L;
			break;
		default:
			status = -1;
			break;
	}

	return status;
}

/** @} */

