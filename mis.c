#include <linux/input.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#ifndef mis_MAJOR
#define mis_MAJOR 241
#endif

MODULE_LICENSE("GPL");


struct input_dev *mis_input_dev;
struct cdev *mis_cdev;
dev_t mis_dev;

static int mis_major = mis_MAJOR;
static int step = 3;
static int scrollStep = 1;

static ssize_t mis_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static void mis_exit(void);
static int mis_init(void);


struct file_operations mis_fops = {
	write: mis_write
};

static ssize_t mis_write( struct file *filp, const char *buf, size_t count, loff_t *f_pos) {

	char *write_buffer=(char *)kmalloc(sizeof(char),GFP_KERNEL);

	unsigned char m;
	int result;

	result = copy_from_user(write_buffer,buf,sizeof(char));
	*f_pos += count;
	if ( result > 0) {
		printk("--!RM : Problem in reading data on /dev/mis !!!\n");
		kfree(write_buffer);
		return -EFAULT;
	}
	
	memcpy(&m,write_buffer,sizeof(char));
	
	switch (m) {
	case '1' : 
		input_report_rel(mis_input_dev, REL_X,-step);
		input_report_rel(mis_input_dev, REL_Y,step);
		input_sync(mis_input_dev);
		break;
	case '3' : 
		input_report_rel(mis_input_dev, REL_X,step);
		input_report_rel(mis_input_dev, REL_Y,step);
		input_sync(mis_input_dev);
		break;
	case '9' : 
		input_report_rel(mis_input_dev, REL_X,step);
		input_report_rel(mis_input_dev, REL_Y,-step);
		input_sync(mis_input_dev);
		break;
	case '7' : 
		input_report_rel(mis_input_dev, REL_X,-step);
		input_report_rel(mis_input_dev, REL_Y,-step);
		input_sync(mis_input_dev);
		break;	

	case '4' : 
		input_report_rel(mis_input_dev, REL_X,-step);
		input_sync(mis_input_dev);
		break;
	case '6' : 
		input_report_rel(mis_input_dev, REL_X,step);
		input_sync(mis_input_dev);
		break;
	case '8' : 
		input_report_rel(mis_input_dev, REL_Y,-step);
		input_sync(mis_input_dev);
		break;
	case '2' : 
		input_report_rel(mis_input_dev, REL_Y,step);
		input_sync(mis_input_dev);
		break;
	// Movements of finger are represented as motion in short single steps in 8 basic directions.
	// Directions are represented as numbers on keyboard
	// 7 8 9
	// 4 5 6
	// 1 2 3
	case 'l' : 
		printk(KERN_ALERT "--!RM :LEFT CLICK \n");
		input_report_key(mis_input_dev, BTN_LEFT, 1);
		input_sync(mis_input_dev);
		input_report_key(mis_input_dev, BTN_LEFT, 0);
		input_sync(mis_input_dev);
		break;
	case 'r' : 
		printk(KERN_ALERT "--!RM :RIGHT CLICK \n");
		input_report_key(mis_input_dev, BTN_RIGHT, 1);
		input_sync(mis_input_dev);
		input_report_key(mis_input_dev, BTN_RIGHT, 0);
		input_sync(mis_input_dev);
		break;
	//l, r are used for left and right click respectivelly
	case 'w' :
		input_report_rel(mis_input_dev, REL_WHEEL,scrollStep);
		break;
	case 's' :
		input_report_rel(mis_input_dev, REL_WHEEL,-scrollStep);
		break;
	case 'a' :
		input_report_rel(mis_input_dev, REL_HWHEEL,-scrollStep);
		break;
	case 'd' :
		input_report_rel(mis_input_dev, REL_HWHEEL,scrollStep);
		break;
	// w,a,s,d are used for scrolling on screen using double touch, inspired by classical movements in games w,a,s,d.
	case 'S' : 
		input_report_rel(mis_input_dev, REL_Y,5*step);
		input_sync(mis_input_dev);
		break;
	case 'A' : 
		input_report_rel(mis_input_dev, REL_X,-5*step);
		input_sync(mis_input_dev);
		break;
	case 'D' : 
		input_report_rel(mis_input_dev, REL_X,5*step);
		input_sync(mis_input_dev);
		break;
	case 'W' : 
		input_report_rel(mis_input_dev, REL_Y,-5*step);
		input_sync(mis_input_dev);
		break; 
	// last 4 cases are used for big steps on screen, when we move fast on touchpad
	default:
		printk("--!RM : Wrong char wrriten in buffer \n");
	break;
	}

	kfree(write_buffer);
	return count;
}

static int __init mis_init(void)
{
	int result;
	
	if (mis_major) {
		mis_dev = MKDEV(mis_major, 0);
		result = register_chrdev_region(mis_dev, 1, "mis");
	} else {
		result = alloc_chrdev_region(&mis_dev, 0, 1,"mis");
		mis_major = MAJOR(mis_dev);
	}
	if (result < 0) {
		printk(KERN_ALERT "--!RM :Bad chrdev_region(), major nr: %d\n",mis_major);
		return result;
	}

	mis_cdev = cdev_alloc();
	if (mis_cdev == NULL) {
		printk(KERN_ALERT "--!RM :Bad cdev_alloc()\n");
		return -1;
	}
	mis_cdev->ops = &mis_fops;
	mis_cdev->owner = THIS_MODULE;

	result = cdev_add(mis_cdev, mis_dev, 1);
	if (result < 0) {
		printk(KERN_ALERT "--!RM :cdev_add() error");
		cdev_del(mis_cdev);
		unregister_chrdev_region(mis_dev, 1);
		return -1;
	}

	mis_input_dev = input_allocate_device();

	if (mis_input_dev == NULL) {
		printk(KERN_ALERT "--!RM :Bad input_alloc_device()\n");
		cdev_del(mis_cdev);
		unregister_chrdev_region(mis_dev, 1);
		return -1;
	}

	mis_input_dev->name = "ReMouse@Edo";
	mis_input_dev->phys = "mis";
	mis_input_dev->id.bustype = BUS_VIRTUAL;
	mis_input_dev->id.vendor = 0x0000;
	mis_input_dev->id.product = 0x0000;
	mis_input_dev->id.version = 0x0000;

	set_bit(EV_REL, mis_input_dev->evbit);
	set_bit(REL_X, mis_input_dev->relbit);
	set_bit(REL_Y, mis_input_dev->relbit);
	set_bit(REL_WHEEL, mis_input_dev->relbit);
	set_bit(REL_HWHEEL, mis_input_dev->relbit);

	set_bit(EV_KEY, mis_input_dev->evbit);
	set_bit(BTN_LEFT, mis_input_dev->keybit);
	set_bit(BTN_RIGHT, mis_input_dev->keybit);
	set_bit(BTN_MIDDLE, mis_input_dev->keybit);

	if ( (result = input_register_device(mis_input_dev)) != 0 ) {
		printk(KERN_ALERT "--!RM : cannot register input_device\n");
		cdev_del(mis_cdev);
		unregister_chrdev_region(mis_dev, 1);
		return result;
	}

	printk(KERN_ALERT "--!RM : Mouse Driver Initialized.\n");
	return 0;
}

static void mis_exit(void)
{
	input_unregister_device(mis_input_dev);
	cdev_del(mis_cdev);
	unregister_chrdev_region(mis_dev, 1);
	printk(KERN_ALERT "--!RM : Mouse Driver unloaded.\n");
}

module_init(mis_init);
module_exit(mis_exit);

