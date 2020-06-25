#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/unistd.h>


#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/sched.h>
#include <linux/poll.h>

#include "myUARTK_t.h"
#include "myUARTK_list.h"

/**
 * @brief Nome identificativo del device-driver.
 * DEVE corrispondere al valore del campo "compatible" nel device tree source.
 */
#define DRIVER_NAME "myUARTK"

/**
 * @brief Nome del file creato in /dev/ per ciascuno dei device
 */
#define DRIVER_FNAME "myUARTK%d"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Salvatore Barone <salvator.barone@gmail.com>");
MODULE_DESCRIPTION("myUART device-driver in kernel mode");
MODULE_VERSION("3.2");
MODULE_ALIAS(DRIVER_NAME);

#define MAX_NUM_OF_DEVICES 15

/*
 * Funzioni implementate dal modulo
 */
static int 			myUARTK_probe			(struct platform_device *op);
static int 			myUARTK_remove			(struct platform_device *op);
static int 			myUARTK_open			(struct inode *inode, struct file *file_ptr);
static int 			myUARTK_release			(struct inode *inode, struct file *file_ptr);
static loff_t		myUARTK_llseek			(struct file *file_ptr, loff_t off, int whence);
static unsigned int myUARTK_poll			(struct file *file_ptr, struct poll_table_struct *wait);
static ssize_t 		myUARTK_read			(struct file *file_ptr, char *buf, size_t count, loff_t *ppos);
static ssize_t 		myUARTK_write 			(struct file *file_ptr, const char *buf, size_t size, loff_t *off);
static irqreturn_t	myUARTK_irq_handler		(int irq, struct pt_regs * regs);

static myUARTK_list_t *device_list = NULL;
static struct class*  myUARTK_class  = NULL;


#define myUARTK_USED_INT		0xFFFFFFFFU //!< @brief Maschea di abilitazione degli interrupt per i singoli pin

static struct of_device_id myUARTK_match[] = {
		{.compatible = DRIVER_NAME},
		{},
};

/**
 * @brief Definisce quali funzioni probe() e remove() chiamare quando viene caricato un driver.
 */
static struct platform_driver myUARTK_driver = {
		.probe = myUARTK_probe,
		.remove = myUARTK_remove,
		.driver = {
				.name = DRIVER_NAME,
				.owner = THIS_MODULE,
				.of_match_table = myUARTK_match,
		},
};


MODULE_DEVICE_TABLE(of, myUARTK_match);
module_platform_driver(myUARTK_driver);


static struct file_operations myUARTK_fops = {
		.owner		= THIS_MODULE,
		.llseek		= myUARTK_llseek,
		.read		= myUARTK_read,
		.write		= myUARTK_write,
		.poll		= myUARTK_poll,
		.open		= myUARTK_open,
		.release	= myUARTK_release
};
 
static int myUARTK_probe(struct platform_device *op) {
	int error = 0;
	myUARTK_t *myUARTK_ptr = NULL;
	printk(KERN_INFO "Chiamata %s\n", __func__);

	if (device_list == NULL) {

		if ((device_list = kmalloc(sizeof(myUARTK_list_t), GFP_KERNEL)) == NULL ) {
			printk(KERN_ERR "%s: kmalloc ha restituito NULL\n", __func__);
			return -ENOMEM;
		}

		if ((error = myUARTK_list_Init(device_list, MAX_NUM_OF_DEVICES)) != 0) {
			printk(KERN_ERR "%s: myUARTK_list_Init() ha restituito %d\n", __func__, error);
			kfree(device_list);
			device_list = NULL;
			return error;
		}

		if ((myUARTK_class = class_create(THIS_MODULE, DRIVER_NAME) ) == NULL) {
			printk(KERN_ERR "%s: class_create() ha restituito NULL\n", __func__);
			kfree(device_list);
			device_list = NULL;
			return -ENOMEM;
		}

	}

	/* Allocazione dell'oggetto myUARTK_t */
	if ((myUARTK_ptr = kmalloc(sizeof(myUARTK_t), GFP_KERNEL)) == NULL) {
		printk(KERN_ERR "%s: kmalloc ha restituito NULL\n", __func__);
		return -ENOMEM;
	}

	if ((error = myUARTK_Init(	myUARTK_ptr,
								THIS_MODULE,
								op,
								myUARTK_class,
								DRIVER_NAME,
								DRIVER_FNAME,
								myUARTK_list_device_count(device_list),
								&myUARTK_fops,
								(irq_handler_t) myUARTK_irq_handler,
								myUARTK_USED_INT)) != 0) {
		printk(KERN_ERR "%s: myUARTK_t_Init() ha restituito %d\n", __func__, error);
		kfree(myUARTK_ptr);
		return error;
	}

	myUARTK_list_add(device_list, myUARTK_ptr);

	printk(KERN_INFO "\t%s => %s%d\n", op->name, DRIVER_NAME, myUARTK_list_device_count(device_list)-1);

	return error;
}

/**
 * @breif Viene chiamata automaticamente alla rimozione del mosulo.
 *
 * @param [inout] op
 *
 * @retval 0 se non si verifica nessun errore
 *
 * @details
 * Dealloca tutta la memoria utilizzata dal driver, de-inizializzando il device e disattivando gli interrupt per il
 * device, effettuando tutte le operazioni inverse della funzione myUARTK_probe().
 */
static int myUARTK_remove(struct platform_device *op) {
	myUARTK_t *myUARTK_ptr = NULL;

	printk(KERN_INFO "Chiamata %s\n\tptr: %08x\n\tname: %s\n\tid: %u\n", __func__, (uint32_t) op, op->name, op->id);

	myUARTK_ptr = myUARTK_list_find_by_op(device_list, op);
	if (myUARTK_ptr != NULL) {
		myUARTK_Destroy(myUARTK_ptr);
		kfree(myUARTK_ptr);
	}

	if (myUARTK_list_device_count(device_list) == 0) {
		myUARTK_list_Destroy(device_list);
		kfree(device_list);
		class_destroy(myUARTK_class);
	}

	return 0;
}

static int myUARTK_open(struct inode *inode, struct file *file_ptr) {
	myUARTK_t *myUARTK_ptr;
	printk(KERN_INFO "Chiamata %s\n", __func__);

	printk(KERN_INFO "%s\n\tminor : %d", __func__, MINOR(inode->i_cdev->dev));

	if ((myUARTK_ptr = myUARTK_list_find_by_minor(device_list, inode->i_cdev->dev)) == NULL) {
		printk(KERN_INFO "%s: myUARTK_list_find_by_minor() ha restituito NULL\n", __func__);
		return -1;
	}

	file_ptr->private_data = myUARTK_ptr;
	return 0;
}

static int myUARTK_release(struct inode *inode, struct file *file_ptr) {
	printk(KERN_INFO "Chiamata %s\n", __func__);
	return 0;
}

static loff_t myUARTK_llseek (struct file *file_ptr, loff_t off, int whence) {
	myUARTK_t *myUARTK_dev_ptr;
    loff_t newpos;
	printk(KERN_INFO "Chiamata %s\n", __func__);
	myUARTK_dev_ptr = file_ptr->private_data;
    switch(whence) {
      case 0: /* SEEK_SET */
        newpos = off;
        break;
      case 1: /* SEEK_CUR */
        newpos = file_ptr->f_pos + off;
        break;
      case 2: /* SEEK_END */
        newpos = myUARTK_dev_ptr->rsrc_size + off;
        break;
      default: /* can't happen */
        return -EINVAL;
    }
    if (newpos < 0)
    	return -EINVAL;
    file_ptr->f_pos = newpos;
    return newpos;
}

static unsigned int myUARTK_poll (struct file *file_ptr, struct poll_table_struct *wait) {
	myUARTK_t *myUARTK_dev;
	printk(KERN_INFO "Chiamata %s\n", __func__);
	myUARTK_dev = file_ptr->private_data;
	return myUARTK_GetPollMask(myUARTK_dev, file_ptr, wait);
}

static irqreturn_t myUARTK_irq_handler(int irq, struct pt_regs * regs) {
	myUARTK_t *myUARTK_dev_ptr = NULL;
	printk(KERN_INFO "Chiamata %s\n\tline: %d\n", __func__, irq);

	if ((myUARTK_dev_ptr = myUARTK_list_find_irq_line(device_list, irq)) == NULL) {
		printk(KERN_INFO "%s\n\tmyUARTK_list_find_irq_line() restituisce NULL:\n", __func__);
		return IRQ_NONE;
	}

	myUARTK_GlobalInterruptDisable(myUARTK_dev_ptr);

	myUARTK_SetCanRead(myUARTK_dev_ptr);

	myUARTK_IncrementTotal(myUARTK_dev_ptr);

	myUARTK_WakeUp(myUARTK_dev_ptr);
	return IRQ_HANDLED;
}

static ssize_t myUARTK_read (struct file *file_ptr, char *buf, size_t count, loff_t *off) {
	myUARTK_t *myUARTK_dev_ptr;
	void* read_addr;
	uint32_t data_readed;
	printk(KERN_INFO "Chiamata %s\n", __func__);
	
	myUARTK_dev_ptr = file_ptr->private_data;
	if (*off > myUARTK_dev_ptr->rsrc_size)
		return -EFAULT;

	if ((file_ptr->f_flags & O_NONBLOCK) == 0) {
		printk(KERN_INFO "%s è bloccante\n", __func__);
		myUARTK_TestCanReadAndSleep(myUARTK_dev_ptr);

		myUARTK_ResetCanRead(myUARTK_dev_ptr);
	}
	else {
		printk(KERN_INFO "%s non è bloccante\n", __func__);
	}

	read_addr = myUARTK_GetDeviceAddress(myUARTK_dev_ptr)+*off;
	data_readed = ioread32(read_addr);

	unsigned status_reg = ioread32((myUARTK_GetDeviceAddress(myUARTK_dev_ptr) + myUARTK_STATUS_REG_OFFSET));
	printk("status reg before read: %08x\n", status_reg);
	if (copy_to_user(buf, &data_readed, count))
		return -EFAULT;
		
	myUARTK_PinInterruptAck(myUARTK_dev_ptr);

	myUARTK_GlobalInterruptEnable(myUARTK_dev_ptr);


	return count;
}


static ssize_t myUARTK_write (struct file *file_ptr, const char *buf, size_t size, loff_t *off) {
	myUARTK_t *myUARTK_dev_ptr;
	uint32_t data_to_write;
	void* write_addr;
	uint32_t old_ctr_reg;
	printk(KERN_INFO "Chiamata %s\n", __func__);
	myUARTK_dev_ptr = file_ptr->private_data;
	if (*off > myUARTK_dev_ptr->rsrc_size)
		return -EFAULT;

	if (copy_from_user(&data_to_write, buf, size))
		return -EFAULT;
	write_addr = myUARTK_GetDeviceAddress(myUARTK_dev_ptr)+*off;
	iowrite32(data_to_write, write_addr);

	//se stiamo scrivendo sul dbout, alziamo il bit wr per iviare
	if(*off == myUARTK_DBIN_OFFSET){
		printk("%s: Sto facendo la write per inviare", __func__);
		write_addr = myUARTK_GetDeviceAddress(myUARTK_dev_ptr)+myUARTK_CONTROL_REG_OFFSET;
		old_ctr_reg = ioread32(write_addr);
		printk("%s: Control read before write %08x\n", __func__, old_ctr_reg);
		data_to_write = old_ctr_reg | CTR_WR;
		iowrite32(data_to_write, write_addr);
		data_to_write = old_ctr_reg & ~CTR_WR;
		iowrite32(data_to_write, write_addr);
		printk("%s: Xontrol read before write %08x\n", __func__, old_ctr_reg);


	}
	return size;
}


