/*----------------------------------------------------------------------------
 * Name:    CAN.h
 * Purpose: CAN interface for STM32
 * Version: V1.00
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * Copyright (c) 2005-2007 Keil Software. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef _CAN_H_
#define _CAN_H_


typedef struct  {
  uint16_t   id;
  unsigned char data [8] ;
  uint8_t  len;
} CAN_msg;

void setup (uint32_t enable_interrupts) ;
void start (void) ;
void set_testmode (unsigned int testmode) ;
void wait_trans_mail0_is_empty (void);
void wait_msg_pending_fifo0_ (void) ;
uint8_t frame_transmit (CAN_msg* msg, uint8_t with_int_trasm);
uint8_t frame_read (CAN_msg*msg);
uint8_t config_bank_filter (uint16_t* id, uint8_t num_id, uint32_t CAN_filterIdx ) ;


#endif // _CAN_H_


