#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/slab.h>

#include <asm/current.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>

#include "ioctl-io.h"
#include "kvm-crypto.h"

struct cdev cdev;
int dev_major = 50;
int dev_minor = 0;

int ioctl_dev_open(struct inode *inode, struct file *filep)
{
	printk(KERN_ALERT "ioctl_dev_open\n");
	return 0;
}

int ioctl_dev_release(struct inode *inode, struct file *filep)
{
	printk(KERN_ALERT "ioctl_dev_release\n");
	return 0;
}

long ioctl_dev_ioctl(struct file *filp, unsigned int ioctl, unsigned long arg)
{
	void *argp = (void *)arg;
	//printk(KERN_ALERT "ioctl_dev_ioctl\n");
	
	switch(ioctl)
	{
		case KC_IOENC:   // encrypt data
		{
			struct job_struct *js = (struct job_struct *)argp;
			submit_job(js, KVM_ENCRYPTION);
			break;
		}
		case KC_IODEC:   // decrypt data
		{
			struct job_struct *js = (struct job_struct *)argp;
			submit_job(js, KVM_DECRYPTION);
			break;
		}
		default:
		{
			printk(KERN_ALERT "command: Unknown\n");
		}
	}
	return 0;
}

struct file_operations ioctl_dev_fops = {
	owner:   THIS_MODULE, 
	open:    ioctl_dev_open,    
	release: ioctl_dev_release,
	unlocked_ioctl :  ioctl_dev_ioctl,
};

int ioctl_dev_install(void)
{
	int ret;
	dev_t devno = MKDEV(dev_major, dev_minor);

	if(dev_major)  // assign static dev number
	{
		ret = register_chrdev_region(devno, 2, "kvm-crypto");
	}
	else  // assign dynamic dev number
	{
		ret = alloc_chrdev_region(&devno, 0, 2, "kvm-crypto");
		dev_major = MAJOR(devno);
	}

	if(ret < 0)
	{
		printk(KERN_ALERT "/dev/kvm-crypto register failed\n");
		return ret;
	}
	else
	{
		printk(KERN_ALERT "/dev/kvm-crypto register successful\n");
	}

	// init the cdev
	cdev_init(&cdev, &ioctl_dev_fops);
	cdev.owner = THIS_MODULE;
	cdev.ops = &ioctl_dev_fops;
	// register the char device
	cdev_add(&cdev, MKDEV(dev_major, 0), 1);

	return 0;
}

void ioctl_dev_uninstall(void)
{
	cdev_del(&cdev);
	unregister_chrdev_region(MKDEV(dev_major, dev_minor), 2);
}

static int __init ioctl_dev_init(void)
{
	int ret;
	printk(KERN_ALERT "init the module\n");
	ret = ioctl_dev_install();
	return ret;
}

static void __exit ioctl_dev_exit(void)
{
	printk(KERN_ALERT "exit the module\n");
	ioctl_dev_uninstall();
}

module_init(ioctl_dev_init);
module_exit(ioctl_dev_exit);
