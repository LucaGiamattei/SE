/**
 * @file myUARTK_list.h
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 24 06 2017
 *
 * @copyright
 * Copyright 2017 Salvatore Barone <salvator.barone@gmail.com>
 *
 * This file is part of Zynq7000DriverPack
 *
 * Zynq7000DriverPack is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * Zynq7000DriverPack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 * @addtogroup myUART
 * @{
 * @addtogroup Linux-Driver
 * @{
 * @addtogroup DeviceList
 * @{
 * @brief Definisce la struttura dati myUARTK_list_t, la quale mantiene un riferimento agli oggetti myUARTK_t gestiti dal driver
 */

#ifndef __myUARTK_DEVICE_LIST__
#define __myUARTK_DEVICE_LIST__

#include "myUARTK_t.h"

/**
 * @brief Struttura dati per la gestione degli oggetti myUARTK_t gestiti dal driver
 *
 * La struttura dati, sebbene non strettamente necessaria alla gestione dei diversi oggetti myUARTK_t, ciascuno dei
 * quali corrispondente ad un diverso device gestito dal driver myUARTK, è pensata per semplificare l'accesso a
 * questi ultimi, tenendo un riferimento a tutti gli oggetti e le strutture dati coinvolte nel funzionamento del modulo
 *  in un unico "posto", accessibile attraverso questa struttura dati.
 */
typedef struct {
	myUARTK_t **device_list;	/**< 	array di puntatori a struttura myUARTK_t, ciascuno dei quali si riferisce ad un device differente  */
	uint32_t list_size;			/**<	dimensione dell'array, corrisponde al numero massimo di device gestibili, definito in fase di
										inizializzazione */
	uint32_t device_count;		/**< 	numero di device correntemente attivi e gestiti dal driver */
} myUARTK_list_t;

extern int myUARTK_list_Init(myUARTK_list_t *list, uint32_t list_size);

extern void myUARTK_list_Destroy(myUARTK_list_t* list);

extern int myUARTK_list_add(myUARTK_list_t *list, myUARTK_t *device);

extern myUARTK_t* myUARTK_list_find_by_op(myUARTK_list_t *list, struct platform_device *op);

extern myUARTK_t* myUARTK_list_find_by_minor(myUARTK_list_t *list, dev_t dev);

extern myUARTK_t* myUARTK_list_find_irq_line(myUARTK_list_t *list, int irq_line);

extern uint32_t myUARTK_list_device_count(myUARTK_list_t *list);

#endif

/**
 * @}
 * @}
 * @}
 */
