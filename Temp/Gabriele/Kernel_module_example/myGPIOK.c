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
#include <linux/cdev.h>

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

#define  DEVICE_NAME "myGPIOc"    ///< The device will appear at /dev/ebbchar using this value
#define  CLASS_NAME  "myGPIO"        ///< The device class -- this is a character device driver
 
static int 			myGPIOK_open			(struct inode *inode, struct file *file_ptr);
static int 			myGPIOK_release			(struct inode *inode, struct file *file_ptr);
static ssize_t 		myGPIOK_read			(struct file *file_ptr, char *buf, size_t count, loff_t *ppos);
static ssize_t 		myGPIOK_write 			(struct file *file_ptr, const char *buf, size_t size, loff_t *off);


MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Gabriele Previtere");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

 
static int    majorNumber;                  ///< Stores the device number -- determined automatically
//llstatic int    numberOpens = 0;  

static struct class*  myGPIOK_class  = NULL;
struct device* myGPIOK_device  = NULL; ///< The device-driver device struct pointer
struct cdev *cdev1;

dev_t myGPIOK_dev,devno;

static struct file_operations fops = {
		.owner		= THIS_MODULE,
		.read		= myGPIOK_read,
		.write		= myGPIOK_write,
		.open		= myGPIOK_open,
		.release	= myGPIOK_release
};

static int __init myGPIOK_init(void)
{
	printk(KERN_INFO "Inserimento, myGPIOK \n");

    int error = alloc_chrdev_region (&myGPIOK_dev, 0, 1, DEVICE_NAME);

    if (error<0){
      printk(KERN_ALERT "myGPIOK failed to register a major number\n");
      return majorNumber;
    }
    majorNumber = MAJOR(myGPIOK_dev);
    printk(KERN_INFO "myGPIOK: registered correctly with major number %d\n", majorNumber);

    myGPIOK_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(myGPIOK_class)){                // Check for error and clean up if there is
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(myGPIOK_class);          // Correct way to return an error on a pointer
    }
    printk(KERN_INFO "myGPIOK: device class registered correctly\n");
    
    // Register the device driver
   	int err, myGPIOK_dev = MKDEV(majorNumber, 0);
    cdev1 = cdev_alloc( );
    cdev1->ops = &fops;
    cdev1->owner = THIS_MODULE;

   	cdev_init(cdev1, &fops);
	err = cdev_add (cdev1, myGPIOK_dev, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding cdev", err);

    myGPIOK_device = device_create(myGPIOK_class, NULL, myGPIOK_dev, NULL, DEVICE_NAME);
    if (IS_ERR(myGPIOK_device)){               // Clean up if there is an error
        class_destroy(myGPIOK_class);           // Repeated code but the alternative is goto statements
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(myGPIOK_device);
    }
    printk(KERN_INFO "myGPIOK: device class created correctly\n"); // Made it! device was initialized

    return 0;

}

static void __exit myGPIOK_exit(void)
{
    
    cdev_del(cdev1);
    unregister_chrdev_region(myGPIOK_dev, 1);
	printk(KERN_INFO "Disinserimento myGPIOK\n");
}

static ssize_t myGPIOK_write (struct file *file_ptr, const char *buf, size_t size, loff_t *off) {
	printk(KERN_INFO "Chiamata %s\n", __func__);
	return 0;
}

static ssize_t myGPIOK_read (struct file *file_ptr, char *buf, size_t count, loff_t *off) {
	printk(KERN_INFO "Chiamata %s\n", __func__);
	return 0;
}

static int myGPIOK_open(struct inode *inode, struct file *file_ptr) {
    printk(KERN_INFO "Chiamata %s\n", __func__);
	return 0;
}

static int myGPIOK_release(struct inode *inode, struct file *file_ptr) {
	printk(KERN_INFO "Chiamata %s\n", __func__);
	return 0;
}

module_init(myGPIOK_init);
module_exit(myGPIOK_exit);