/**
 * @file myUARTK_t.h
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
 * @addtogroup myUARTK_t
 * @{
 * @brief Definisce l'oggetto myUARTK_t, che rappresenta un device myUART a livello kernel
 */
#ifndef __myUARTK_T__
#define __myUARTK_T__

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

#define myUARTK_DBIN_OFFSET             0x00	//!< @brief Offset, rispetto all'indirizzo base, del registro "DBIN"
#define myUARTK_DBOUT_OFFSET            0x04	//!< @brief Offset, rispetto all'indirizzo base, del registro "DBOUT"
#define myUARTK_CONTROL_REG_OFFSET      0x08	//!< @brief Offset, rispetto all'indirizzo base, del registro "CONTROL"
#define myUARTK_STATUS_REG_OFFSET       0x0C	//!< @brief Offset, rispetto all'indirizzo base, del registro "STATUS"
#define ST_TBE      (uint32_t) 1 << 1

#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#include <linux/wait.h>
#include <linux/spinlock.h>

#include <asm/uaccess.h>
#include <asm/io.h>

/**
 * @brief Stuttura per l'astrazione di un device myUART in kernel-mode
 *
 * è buona abitudine, se non quasi indispensabile, definire una struttura dati nella quale contenere tutto
 * ciò che è legato al device o al driver. In questo modulo viene usata la struttura myUARTK_t per contenere
 * tutto ciò che è necessario al funzionamento del driver.
 */
typedef struct {
	dev_t Mm;					/**<	Major e minor number associati al device */
	struct platform_device *op; /**<	Puntatore a struttura platform_device cui l'oggetto myUARTK_t si riferisce */
	struct cdev cdev;			/**<	Stuttura per l'astrazione di un device a caratteri
										Il kernel usa, internamente, una struttura cdev per rappresentare i device a
										caratteri. Prima che il kernel invochi le funzioni definite dal driver per il
										device, bisogna allocare e registrare uno, o più, oggetti cdev. In questo
										caso è sufficiente allocare uno solo di questi oggetti. */
	struct device* dev;			/**< */
	struct class*  class;		/**< */
	uint32_t irqNumber; 		/**< 	interrupt-number a cui il device è connesso. Restituito dalla
										chiamata alla funzione irq_of_parse_and_map() */
	uint32_t irq_mask;			/**<	maschera delle interruzioni interne per il device */
	struct resource rsrc; 		/**<	Struttura che astrae una risorsa device, dal punto di vista della
										memoria alla quale la risorsa è mappata. In particolare i campi
										"start" ed "end" contengono, rispettivamente, il primo e l'ultimo
										indirizzo fisico a cui il device è mappato. */
	struct resource *mreg;		/**<	puntatre alla regione di memoria cui il device è mapapto */
	uint32_t rsrc_size; 		/**<	rsrc.end - rsrc.start
	 	 	 	 	 	 	 			numero di indirizzi associati alla periferica.
	 	 	 	 	 	 	 			occorre per effettuare il mapping indirizzo fisico - indirizzo
	 	 	 	 	 	 	 			virtuale */
	void *vrtl_addr; 			/**<	indirizzo virtuale della periferica */
	wait_queue_head_t read_queue; /**<  wait queue per la system-call read()
										Una chiamata a read() potrebbe arrivare quando i dati non sono
										disponibili, ma potrebbero esserlo in futuro, oppure, una chiamata a
										write() potrebbe avvenire quando il device non è in grado di accettare
										altri dati (perché il suo buffer di ingresso potrebbe essere pieno).
										Il processo chiamante non ha la minima conoscenza delle dinamiche
										interne del device, per cui, nell'impossibilità di servire la
										richiesta, il driver deve bloccare il processo e metterlo tra i
										processi "sleeping", fin quando la richiesta non può essere servita.
										Tutti i processi in attesa di un particolare evento vengono posti
										all'interno della stessa wait queue. In linux una wait queue viene
										implementata da una struttura dati wait_queue_head_t, definita in
										<linux/wait.h>. */
	wait_queue_head_t poll_queue; /**< 	wait queue per la system-call poll() */
	uint32_t can_read; 			/**< 	Flag "puoi leggere"
	 	 	 	 	 	 				Il valore viene settato dalla funzione myUARTK_irq_handler() al manifestarsi
	 	 	 	 	 	 				di un interrupt, prima di risvegliare i processi in attesa di un interrupt.
										I processi che effettuano read() bloccante restano bloccati finoché
										int_occurred = 0 */
	spinlock_t slock_int; /**<			Spinlock usato per garantire l'accesso in mutua esclusione alla variabile
										int_occurred da parte delle funzioni del modulo.
										I semafori sono uno strumento potentissimo per per l'implementazione di
										sezioni	critiche, ma non possono essere usati in codice non interrompibile.
										Gli spilock sono come i semafori, ma possono essere usati anche in codice
										non interrompibile,	come può esserlo un modulo kernel.
										Sostanzialmente se uno spinlock è già stato acquisito da qualcun altro, si
										entra in un hot-loop dal quale si esce solo quando chi possiede lo spinlock
										lo rilascia. Trattandosi di moduli kernel, è di vitale importanza che la
										sezione critica sia quanto più piccola possibile. Ovviamente
										l'implementazione è "un pò" più complessa di come è stata descritta,
										ma il concetto è questo. Gli spinlock sono definiti in <linux/spinlock.h>. */
	uint32_t total_irq;			/**< 	numero totale di interrupt manifestatesi */
	spinlock_t sl_total_irq; 	/**<	Spinlock usato per garantire l'accesso in mutua esclusione alla variabile
								 		total_irq da parte delle funzioni del modulo */
	wait_queue_head_t write_queue; 
	uint32_t can_write; 			

} myUARTK_t;

extern int myUARTK_Init(	myUARTK_t* myUARTK_device,
							struct module *owner,
							struct platform_device *op,
							struct class*  class,
							const char* driver_name,
							const char* device_name,
							uint32_t serial,
							struct file_operations *f_ops,
							irq_handler_t irq_handler,
							uint32_t irq_mask);

extern void myUARTK_Destroy(myUARTK_t* device);

extern void myUARTK_SetCanRead(myUARTK_t* device);

extern void myUARTK_ResetCanRead(myUARTK_t* device);

extern void myUARTK_TestCanReadAndSleep(myUARTK_t* device);

extern void myUARTK_SetCanWrite(myUARTK_t* device);

extern void myUARTK_ResetCanWrite(myUARTK_t* device);

extern void myUARTK_TestCanWriteAndSleep(myUARTK_t* device);

extern unsigned myUARTK_GetPollMask(myUARTK_t *device, struct file *file_ptr, struct poll_table_struct *wait);

extern void myUARTK_IncrementTotal(myUARTK_t* device);

extern void myUARTK_WakeUp(myUARTK_t* device);

extern void* myUARTK_GetDeviceAddress(myUARTK_t* device);

extern void myUARTK_GlobalInterruptEnable(myUARTK_t* myUARTK_device);

extern void myUARTK_GlobalInterruptDisable(myUARTK_t* myUARTK_device);

extern void myUARTK_PinInterruptAck(myUARTK_t* myUARTK_device);

/**
 * @cond
 * Funzioni e definizioni di servizio per GPIO Xilinx
 * Non verranno documentate.
 */


#endif

/**
 * @}
 * @}
 * @}
 */
