/**
 * @file myUARTK_t.c
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
 */
#include "myUARTK_t.h"
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/poll.h>

/**
 * @brief Inizializza una struttura myUARTK_t e configura il device corrispondente
 *
 * @param [in]	myUARTK_device puntatore a struttura myUARTK_t, che si riferisce al device su cui operare
 * @param [in]	owner puntatore a struttura struct module, proprietario del device (THIS_MODULE)
 * @param [in]	op puntatore a struct platform_device, costituito dal parametro "op" con cui viene invocata probe() o la remove()
 * @param [in]	class puntatore a struct class, classe del device, deve essere stata precedentemente creata con class_create()
 * @param [in]	driver_name nome del driver
 * @param [in]	device_name nome del device
 * @param [in]	serial numero seriale del device
 * @param [in]	f_ops puntatore a struttura struct file_operations, specifica le funzioni che agiscono sul device
 * @param [in]	irq_handler puntatore irq_handler_t alla funzione che gestirà gli interrupt generati dal device
 * @param [in]	irq_mask maschera delle interruzioni del device
 *
 * @retval "0" se non si è verificato nessun errore
 *
 * @details
 */
int myUARTK_Init(	myUARTK_t* myUARTK_device,
					struct module *owner,
					struct platform_device *op,
					struct class*  class,
					const char* driver_name,
					const char* device_name,
					uint32_t serial,
					struct file_operations *f_ops,
					irq_handler_t irq_handler,
					uint32_t irq_mask) {
	int error = 0;
	struct device *dev = NULL;
	char *file_name = kmalloc(strlen(driver_name) + 5, GFP_KERNEL);
	sprintf(file_name, device_name, serial);
	myUARTK_device->op = op;
	myUARTK_device->class = class;
/** <h5>Major-number e Minor-number</h5>
 * Ai device drivers sono associati un major-number ed un minor-number. Il major-number viene usato dal kernel
 * per identificare il driver corretto corrispondente ad uno specifico device, quando si effettuano operazioni
 * su di esso. Il ruolo del minor number dipende dal device e viene gestito internamente dal driver.
 * Questo driver, così come molti altri, usa il Major ed il minor number per distinguere le diverse istanze di
 * device myUART che usano il device-driver myUARTK.
 * La registrazione di un device driver può essere effettuata chiamando <b>alloc_chrdev_region()</b>, la quale
 * alloca un char-device numbers. Il major number viene scelto dinamicamente e restituito dalla funzione
 * attraverso il parametro dev. La funzione restituisce un valore negativo nel caso in cui si verifichino errori,
 * 0 altrimenti.
 * @code
 * int alloc_chrdev_region (dev_t * dev, unsigned baseminor, unsigned count, const char *name);
 * @endcode
 *  - dev: major e minor number
 *  - baseminor: primo dei minor number richiesti
 *  - count: numero di minornumber richiesti
 *  - name: nome del device
 */
	if ((error = alloc_chrdev_region(&myUARTK_device->Mm, 0 , 1, file_name)) != 0) {
		printk(KERN_ERR "%s: alloc_chrdev_region() ha restituito %d\n", __func__, error);
		return error;
	}

	cdev_init (&myUARTK_device->cdev, f_ops);
	myUARTK_device->cdev.owner = owner;

	if ((myUARTK_device->dev = device_create(class, NULL, myUARTK_device->Mm, NULL, file_name)) == NULL) {
		printk(KERN_ERR "%s: device_create() ha restituito NULL\n", __func__);
		error = -ENOMEM;
		goto device_create_error;
	}

	if ((error = cdev_add(&myUARTK_device->cdev, myUARTK_device->Mm, 1)) != 0) {
		printk(KERN_ERR "%s: cdev_add() ha restituito %d\n", __func__, error);
		goto cdev_add_error;
	}

	dev = &op->dev;
	if ((error = of_address_to_resource(dev->of_node, 0, &myUARTK_device->rsrc)) != 0) {
		printk(KERN_ERR "%s: request_irq() ha restituito %d\n", __func__, error);
		goto of_address_to_resource_error;
	}
	myUARTK_device->rsrc_size = myUARTK_device->rsrc.end - myUARTK_device->rsrc.start + 1;

	if ((myUARTK_device->mreg = request_mem_region(myUARTK_device->rsrc.start, myUARTK_device->rsrc_size, file_name)) == NULL) {
		printk(KERN_ERR "%s: request_mem_region() ha restituito NULL\n", __func__);
		error = -ENOMEM;
		goto request_mem_region_error;
	}

 
	if ((myUARTK_device->vrtl_addr = ioremap(myUARTK_device->rsrc.start, myUARTK_device->rsrc_size))==NULL) {
		printk(KERN_ERR "%s: ioremap() ha restituito NULL\n", __func__);
		error = -ENOMEM;
		goto ioremap_error;
	}

	myUARTK_device->irqNumber = irq_of_parse_and_map(dev->of_node, 0);
	if ((error = request_irq(myUARTK_device->irqNumber , irq_handler, 0, file_name, NULL)) != 0) {
		printk(KERN_ERR "%s: request_irq() ha restituito %d\n", __func__, error);
		goto irq_of_parse_and_map_error;
	}
	myUARTK_device->irq_mask = irq_mask;

	init_waitqueue_head(&myUARTK_device->read_queue);
	init_waitqueue_head(&myUARTK_device->poll_queue);

	spin_lock_init(&myUARTK_device->slock_int);
	spin_lock_init(&myUARTK_device->sl_total_irq);
	myUARTK_device->can_read = 0;
	myUARTK_device->total_irq = 0;


	myUARTK_GlobalInterruptEnable(myUARTK_device);

	goto no_error;

irq_of_parse_and_map_error:
	iounmap(myUARTK_device->vrtl_addr);
ioremap_error:
	release_mem_region(myUARTK_device->rsrc.start, myUARTK_device->rsrc_size);
request_mem_region_error:
of_address_to_resource_error:
cdev_add_error:
	device_destroy(myUARTK_device->class, myUARTK_device->Mm);
device_create_error:
	cdev_del(&myUARTK_device->cdev);
	unregister_chrdev_region(myUARTK_device->Mm, 1);

no_error:
	return error;
}

/**
 * @brief Deinizializza un device, rimuovendo le strutture kernel allocate per il suo funzionamento
 *
 * @param [in] device puntatore a struttura myUARTK_t, specifica il particolare device su cui agire
 */
void myUARTK_Destroy(myUARTK_t* device) {

	myUARTK_GlobalInterruptDisable(device);
	//myUARTK_PinInterruptDisable(device, device->irq_mask);

	free_irq(device->irqNumber, NULL);
	iounmap(device->vrtl_addr);
	release_mem_region(device->rsrc.start, device->rsrc_size);
	device_destroy(device->class, device->Mm);
	cdev_del(&device->cdev);
	unregister_chrdev_region(device->Mm, 1);
}

void myUARTK_SetCanRead(myUARTK_t* device) {
	unsigned long flags;
	spin_lock_irqsave(&device->slock_int, flags);
	device-> can_read = 1;
	spin_unlock_irqrestore(&device->slock_int, flags);
}


void myUARTK_ResetCanRead(myUARTK_t* device) {
	unsigned long flags;
	spin_lock_irqsave(&device->slock_int, flags);
	device-> can_read = 0;
	spin_unlock_irqrestore(&device->slock_int, flags);
}


void myUARTK_TestCanReadAndSleep(myUARTK_t* device) {
	wait_event_interruptible(device->read_queue, (device->can_read != 0));
}


unsigned myUARTK_GetPollMask(myUARTK_t *device, struct file *file_ptr, struct poll_table_struct *wait) {
	unsigned mask;
	poll_wait(file_ptr, &device->poll_queue,  wait);
	spin_lock(&device->slock_int);
	if(device->can_read)
		mask = POLLIN | POLLRDNORM;
	spin_unlock(&device->slock_int);
	return mask;
}


void myUARTK_IncrementTotal(myUARTK_t* device) {
	unsigned long flags;
	spin_lock_irqsave(&device->sl_total_irq, flags);
	device->total_irq++;
	spin_unlock_irqrestore(&device->sl_total_irq, flags);
}


void myUARTK_WakeUp(myUARTK_t* device) {
	wake_up_interruptible(&device->read_queue);
	wake_up_interruptible(&device->poll_queue);
}

/**
 * @brief Restituisce l'indirizzo virtuale di memoria cui è mappato un device
 *
 * @param [in] device puntatore a struttura myUARTK_t, che si riferisce al device su cui operare
 */
void* myUARTK_GetDeviceAddress(myUARTK_t* device) {
	return device->vrtl_addr;
}

/**
 * @brief Abilita gli interrupt globali;
 *
 * @param [in] device puntatore a struttura myUARTK_t, che si riferisce al device su cui operare
 */
void myUARTK_GlobalInterruptEnable(myUARTK_t* device) {
	volatile unsigned reg_value = ioread32((device->vrtl_addr + myUARTK_CONTROL_REG_OFFSET));
	reg_value |= CTR_IERX ;
	iowrite32(reg_value, (device->vrtl_addr + myUARTK_CONTROL_REG_OFFSET));
	
}

/**
 * @brief Disabilita gli interrupt globali;
 *
 * @param [in] device puntatore a struttura myUARTK_t, che si riferisce al device su cui operare
 */
void myUARTK_GlobalInterruptDisable(myUARTK_t* device) {
	unsigned reg_value = ioread32((device->vrtl_addr + myUARTK_CONTROL_REG_OFFSET));
	reg_value &= ~CTR_IERX;
	iowrite32(reg_value, (device->vrtl_addr + myUARTK_CONTROL_REG_OFFSET));}

/**
 * @brief Invia al device notifica di servizio di un interrupt;
 *
 * @param [in] device puntatore a struttura myUARTK_t, che si riferisce al device su cui operare
 *
 * @param [in] mask mask maschera di selezione degli interrupt da notificare; quelli non selezionati non vengono notificati;
 */
void myUARTK_PinInterruptAck(myUARTK_t* device) {
	unsigned ctr_reg = ioread32((device->vrtl_addr + myUARTK_CONTROL_REG_OFFSET));
	/*unsigned status_reg = ioread32((device->vrtl_addr + myUARTK_STATUS_REG_OFFSET));
	printk("status reg before write RD: %08x\n", status_reg);
	printk("control reg before write RD: %08x\n", ctr_reg);*/
	ctr_reg |= CTR_RD;
	iowrite32(ctr_reg, (device->vrtl_addr + myUARTK_CONTROL_REG_OFFSET));

	/*ctr_reg = ioread32((device->vrtl_addr + myUARTK_CONTROL_REG_OFFSET));
	status_reg = ioread32((device->vrtl_addr + myUARTK_STATUS_REG_OFFSET));
	printk("status reg after write RD: %08x\n", status_reg);
	printk("control reg after write RD: %08x\n", ctr_reg);*/

	ctr_reg &= ~CTR_RD;
	iowrite32(ctr_reg, (device->vrtl_addr + myUARTK_CONTROL_REG_OFFSET));

	/*ctr_reg = ioread32((device->vrtl_addr + myUARTK_CONTROL_REG_OFFSET));
	status_reg = ioread32((device->vrtl_addr + myUARTK_STATUS_REG_OFFSET));
	printk("status reg after write not RD: %08x\n", status_reg);
	printk("control reg after write not RD: %08x\n", ctr_reg);*/
	


}

