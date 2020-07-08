/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for STM32
 * Version: V1.01
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * Copyright (c) 2005-2007 Keil Software. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stm32f407xx.h>                        // STM32F10x Library Definitions
#include "CAN.h"                                  // STM32 CAN adaption layer



/**
 * @brief Effettua il setup del CAN effettuando la transizione nello stato di inziializzazione
 * @param enable_interrupts Abilitazione delle interruzioni.
 * CAN_IER_FMPIE0 abilita il segnalamento dell'interruzione quando vi è un messaggio pending nella FIFO 0 di
 * ricezione (segnala quando FMP0 bits di stato nel CAN_RF0R register non sono ‘00’).
 * CAN_IER_TMEIE abilita il segnalamento dell'interruzione quando il messaggio è stato trasmesso dalla mailbox0 di
 * trasmissione (segnala quando l'RQCP0 bit nel CAN_TSR register è asserito via HW).
 */

void setup (uint32_t enable_interrupts)  {

  /* Abilita il clock per il CAN*/
  RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

  // PB8 and PB9 sono utilizzati dal CAN
  // abilita il clock per Alternate Function
  RCC->APB2ENR |= RCC_AHB1LPENR_GPIOBLPEN;

  // Scrivo 10 nei registri moder dei pin in modo da configurarli in Alternate mode 
  GPIOB->MODER |= (0x1<<17);
  GPIOB->MODER &= ~(0x1<<16);
  GPIOB->MODER |= (0x1<<19);
  GPIOB->MODER &= ~(0x1<<18);

  // Imposto AF9 nel'alternate function register
  GPIOB->AFR[1]  &= ~(0xF);
  GPIOB->AFR[1]  |= (0x9);
  GPIOB->AFR[1]  &= ~(0xF<<4);
  GPIOB->AFR[1]  |= (0x9<<4);

  // Configuro le linee PB9 e PB8 come open drain
  GPIOB->OTYPER &= ~(0x1<<8);
  GPIOB->OTYPER &= ~(0x1<<9);

  // Configuro le linee PB9 e PB8 come no pull-up e no pull-down
  GPIOB->PUPDR &= ~(0xF<<16);
  
  // Imposto la velocità di I/O come bassa
  GPIOB->OSPEEDR |= (0xF<<16);
  
  /* Entra nell inizialization mode (abilitando il bit INRQ nel reg. di controllo)
   * e si assicura che sia disabilita la ritransmissione
   */
  CAN1->MCR = (CAN_MCR_NART | CAN_MCR_INRQ);       // init mode, disable auto. retransmission

  /* Abilitazione delle interruzioni
   *La FIFO0 è utilizzata per la ricezione dei messaggi e la mailbox0 per la trasmissione dei messaggi.*/
  /*!<FIFO Message Pending Interrupt Enable */
  /*!<Transmit Mailbox Empty Interrupt Enable */
  CAN1->IER |= enable_interrupts;


  /* Set del registro BTR: definizione del bit time
   * si è scelto di avere il 70 % (ovvero 12/17) della porzione del bit time per il campionamento
   * e il 23 % (ovvero 4/17) per la trasmissione
   * e il resto per il rilevamento del bit change (1/17) (1 bit quantum di default).
   */
  CAN1->BTR &= ~(((        0x03) << 24) | ((        0x07) << 20) | ((         0x0F) << 16) | (          0x3FF));
  CAN1->BTR |=  ((((4-1) & 0x03) << 24) | (((4-1) & 0x07) << 20) | (((12-1) & 0x0F) << 16) | (0x15 & 0x3FF));
}



/**
 * @brief Funzione che si occupa di entrare nel normal mode
 * @details Per entrare nel normal mode basta semplicemente assicurarsi che il bit di inizialization mode INRQ nel registro
 * di controllo MCR sia resettato. Si attende che il bit CAN_MCR_INRQ dello status register MSR sia stato
 * resettato via hardware.
 *
 */

void start (void)  {

  CAN1->MCR &= ~CAN_MCR_INRQ;
  while (CAN1->MSR & CAN_MCR_INRQ);

}



/**
 * @brief Definisce il modo in cui operare
 * @warning Se si vuole resettare il test mode per ripristinare il modo di operare standard del CAN
 * basta passare zero come valore del parametro testmode.
 * @param testmode inserire la or delle modalità CAN_BTR_SILM CAN_BTR_LBKM
 */

void set_testmode (unsigned int testmode) {

  CAN1->BTR &= ~(CAN_BTR_SILM | CAN_BTR_LBKM);     // set testmode
  CAN1->BTR |=  (testmode & (CAN_BTR_SILM | CAN_BTR_LBKM));
}

/**
 * @brief Attesa attiva finchè la transmit mailbox 0 è vuota
 */

void wait_trans_mail0_is_empty (void)  {

  while ((CAN1->TSR & CAN_TSR_TME0) == 0);

}
/**
 * @brief Attesa attiva finchè nella fifo0 di lettura non vi è almeno un messaggio pending
 */
void wait_msg_pending_fifo0_ (void)  {

  while ((CAN1->RF0R & CAN_RF0R_FMP0) == 0);

}



/**
 * @brief Invia un frame tramite la mailbox 0 di trasmissione
 * @warning L'identificativo è considerato STD e il frame di tipo DATA.
 * @details
 * @param data vettore di caratteri da inviare; può essere al massimo di 8 caratteri.
 * @param len numero di caratteri da inviare.
 * @param with_int_trasm valore 1 se si vuole abilitare il segnalamento di completamento della trasmissione
 */

uint8_t frame_transmit (CAN_msg* msg, uint8_t with_int_trasm)  {
  /*Controllo del numero di byte len*/
	if (!((msg->len>0)&&(msg->len<9))) return -1;

  /**TIR: CAN TX mailbox identifier register
   * In base al tipo di identifier del messaggio (format) viene settato in modo opportuno il
   * registro TIR.
   * Tipo di identificativo:
   * Si è supposto in tale implemntazione che si abbiano identificativi Standard (bit2 =0), di conseguenza
   *  l'identificativo (11 bit) deve essere messo a partire dalla posizione 21 del registro TIR
   * Tipo di Frame: DATA FRAME (bi1 =0) Il classico frame di dati che permette di inviare fino a 8 byte.
   */
  CAN1->sTxMailBox[0].TIR  = (unsigned int)0;
  CAN1->sTxMailBox[0].TIR |= (unsigned int)(msg->id << 21);



  /** Inserisco nella mailboxe 0 tanti caratteri quanto specificato dal parametro len */
  CAN1->sTxMailBox[0].TDLR =0U;
  CAN1->sTxMailBox[0].TDHR =0U;
  for (int i=0;i<msg->len;i++){
	  if (i<4){
		  int a= CAN1->sTxMailBox[0].TDLR;
		  CAN1->sTxMailBox[0].TDLR |= ((unsigned int)msg->data[i]<<i*8);
	  }else{
		  CAN1->sTxMailBox[0].TDHR |= ((unsigned int)msg->data[i]<<(i-4)*8);
	  }
  }
  /** Setto la lunghezza */
  CAN1->sTxMailBox[0].TDTR &= ~CAN_TDT0R_DLC;
  CAN1->sTxMailBox[0].TDTR |=  (msg->len & CAN_TDT0R_DLC);
  /** Le interruzione di trasmissione è abilitata prima della trasmissione del msg */
  if (with_int_trasm==1){
	  CAN1->IER |= CAN_IER_TMEIE;
  }
  /** Effettua la trasmsisione del messaggio*/
  CAN1->sTxMailBox[0].TIR |=  CAN_TI0R_TXRQ;

  return 0;
}


/**
 * @brief quest'istruzione
 */



uint8_t frame_read (CAN_msg*msg)  {
  uint8_t return_value = 0;
  /*!< CAN receive FIFO mailbox identifier register */
  msg->id     = (uint32_t)0x000007FF & (CAN1->sFIFOMailBox[0].RIR >> 21);


  /** Numero di byte ricevuti*/
  msg->len = (unsigned char)0x0000000F & CAN1->sFIFOMailBox[0].RDTR;
  /** leggo i byte */
  for (int i=0;i<msg->len;i++){
  	  if (i<4){
  		msg->data[i] = (unsigned int)0x000000FF & (CAN1->sFIFOMailBox[0].RDLR >> i*8);

  	  }else{
  		  msg->data[i] = (unsigned int)0x000000FF & (CAN1->sFIFOMailBox[0].RDHR >> (i-4)*8);
  	  }
    }
  if (CAN1->RF0R & CAN_RF0R_FMP0) return_value = -1; //overrun

  /** Rilascio della FIFO0 output mailbox: libera la locazione nella FIFO permettendo
   * a un nuovo dato ricevuto di andare ad occupare tale posizione.
   */
  CAN1->RF0R |= CAN_RF0R_RFOM0;                    // Release FIFO 0 output mailbox
  return return_value;
}

/**
 * @brief Settaggio di un banco di filtri
 * @details Si è scelto di fornire un la configurazione di un filtro in modalità Lista (e non maschera)
 * Inoltre si è scelta un'organizzazione dei registri del banco in modo tale da poter inserire 4
 * identifier standard.
 * @param id vettore di al massimo 4 identificativi standard da filtrare nella ricezione
 * @param num_id lunghezza del vettore id
 * @param CAN_filterIdx Il filtro che si vuole settare (da 0 a 27)
 */

uint8_t config_bank_filter (uint16_t* id, uint8_t num_id, uint32_t CAN_filterIdx )  {
  if (!((num_id>0)&&(num_id<5)) || !((CAN_filterIdx>=0)&&(CAN_filterIdx<14))) return -1;
  /** Imposta l'Initialisation mode per il filter banks ((FINIT=1) nel CAN_FMR)*/
  CAN1->FMR  |=  CAN_FMR_FINIT;
  /** Bisogna deattivare il filtro prima di settarlo dinamicamente*/
  CAN1->FA1R &=  ~(unsigned int)(1 << CAN_filterIdx); // deactivate filter

  /** Filter bank scale configuration:
   * Nelle due istruzioni seguenti pongo FSCx=0 e FBMx=1:
   * - FBMx=1: Listo Mode (e non Mask mode)
   * - FSCx=0: organizza i due registri da 32 bit ( dedicati al banco di filtri ) in
   * 4 registro di 16 bit (di cui solo i primi 11 di ciascuno utilizzo per l'identifier STD)
   * Quindi
   * due identificativi standard li posso scrivere nei bit  (31 ; 31-11) e (15 ; 15-11) del FxR1
   * e altri due li posso scrivere scrivere nei bit (31 ; 31-11) e (15 ; 15-11) del FxR2
   */
  CAN1->FM1R |= (unsigned int)(1 << CAN_filterIdx);
  CAN1->FS1R &= ~((unsigned int)(1 << CAN_filterIdx));

  /** Inserimento degli identifier nella lista del filter bank*/
  CAN1->sFilterRegister[CAN_filterIdx].FR1 = 0U;
  CAN1->sFilterRegister[CAN_filterIdx].FR2 = 0U;

  uint32_t CAN_msgId1 = 0;
  uint32_t CAN_msgId2 = 0;
  uint32_t CAN_msgId3 = 0;
  uint32_t CAN_msgId4 = 0;



  if (num_id==1 || num_id==2 || num_id==3 || num_id==4){
	 CAN_msgId1 = id[0] & (0x7FF); //considero solo gli 11 bit
	 CAN_msgId1  = (unsigned int)(CAN_msgId1 << 5); //li metto nella posizione giusta
	 CAN1->sFilterRegister[CAN_filterIdx].FR1 |= CAN_msgId1;
  }
  if (num_id==2|| num_id==3|| num_id==4){
	  CAN_msgId2 = id[1] & (0x7FF); //considero solo gli 11 bit
	  CAN_msgId2  = (unsigned int)(CAN_msgId2 << 21); //li metto nella posizione giusta
	  CAN1->sFilterRegister[CAN_filterIdx].FR1 |= CAN_msgId2;
  }

  if (num_id==3||num_id==4){
	 CAN_msgId3 = id[2] & (0x7FF); //considero solo gli 11 bit
	 CAN_msgId3  = (unsigned int)(CAN_msgId3 << 5); //li metto nella posizione giusta
	 CAN1->sFilterRegister[CAN_filterIdx].FR2 |= CAN_msgId3;
  }

  if(num_id==4){
	  CAN_msgId4 = id[3] & (0x7FF); //considero solo gli 11 bit
	  CAN_msgId4  = (unsigned int)(CAN_msgId4 << 21); //li metto nella posizione giusta
	  CAN1->sFilterRegister[CAN_filterIdx].FR2 |= CAN_msgId4;
  }

  /** Assigna il filter alla FIFO 0 */
  CAN1->FFA1R &= ~(unsigned int)(1 << CAN_filterIdx);


  /** Reattivazione del filtro */
  CAN1->FA1R  |=  (unsigned int)(1 << CAN_filterIdx);
  /** Fine dell'Initialisation mode */
  CAN1->FMR &= ~CAN_FMR_FINIT;
  return 0;
}



