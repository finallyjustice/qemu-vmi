/** File:		main.c
 ** Author:		Dongli Zhang
 ** Contact:	dongli.zhang0129@gmail.com
 **
 ** Copyright (C) Dongli Zhang 2013
 **
 ** This program is free software;  you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 2 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY;  without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 ** the GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program;  if not, write to the Free Software 
 ** Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* run dmesg to see the list */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/kernel_stat.h>
#include <linux/swap.h>
#include <linux/blkdev.h>
#include <linux/utsname.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>

#include <asm/current.h>
#include <asm/segment.h>
#include <asm/uaccess.h>

struct module *m = &__this_module;

static int __init gos_info_init(void)
{
	struct address_space as;
	struct block_device bd;
	struct inode in;
	struct net_device nd;
	struct in_device ind;
	struct in_ifaddr ina;

	printk(KERN_ALERT "PAGE_OFFSET: 0x%016lx\n", PAGE_OFFSET);
	printk(KERN_ALERT "__START_KERNEL_map: 0x%016lx\n", __START_KERNEL_map);

	printk(KERN_ALERT "init_task: 0x%016lx\n", (unsigned long)&init_task);
	printk(KERN_ALERT "task_struct.pid offset: %lu\n", (unsigned long)&(current->pid) - (unsigned long)current);
	printk(KERN_ALERT "task_struct.comm offset: %lu\n", (unsigned long)current->comm - (unsigned long)current);
	printk(KERN_ALERT "task_struct.tasks offset: %lu\n", (unsigned long)&(current->tasks) - (unsigned long)current);
	printk(KERN_ALERT "task_struct.mm offset: %lu\n", (unsigned long)&(current->mm) - (unsigned long)current);
	printk(KERN_ALERT "task_struct.nsproxy: %lu\n", (unsigned long)&(current->nsproxy) - (unsigned long)current);
	printk(KERN_ALERT "mm_struct.pgd offset: %lu\n", (unsigned long)&(current->mm->pgd) - (unsigned long)current->mm);
	printk(KERN_ALERT "nsproxy.uts_ns: %lu\n", (unsigned long)&(current->nsproxy->uts_ns) - (unsigned long)current->nsproxy);
	printk(KERN_ALERT "uts_namespace.name: %lu\n", (unsigned long)&(current->nsproxy->uts_ns->name) - (unsigned long)current->nsproxy->uts_ns);
	printk(KERN_ALERT "new_utsname.sysname: %lu\n", (unsigned long)&(current->nsproxy->uts_ns->name.sysname) - (unsigned long)&(current->nsproxy->uts_ns->name));
	printk(KERN_ALERT "new_utsname.nodename: %lu\n", (unsigned long)&(current->nsproxy->uts_ns->name.nodename) - (unsigned long)&(current->nsproxy->uts_ns->name));
	printk(KERN_ALERT "new_utsname.release: %lu\n", (unsigned long)&(current->nsproxy->uts_ns->name.release) - (unsigned long)&(current->nsproxy->uts_ns->name));
	printk(KERN_ALERT "new_utsname.version: %lu\n", (unsigned long)&(current->nsproxy->uts_ns->name.version) - (unsigned long)&(current->nsproxy->uts_ns->name));
	printk(KERN_ALERT "new_utsname.machine: %lu\n", (unsigned long)&(current->nsproxy->uts_ns->name.machine) - (unsigned long)&(current->nsproxy->uts_ns->name));
	printk(KERN_ALERT "new_utsname.domainname: %lu\n", (unsigned long)&(current->nsproxy->uts_ns->name.domainname) - (unsigned long)&(current->nsproxy->uts_ns->name));
	printk(KERN_ALERT "cpu_possible_mask: 0x%016lx\n", (unsigned long)cpu_possible_mask);
	printk(KERN_ALERT "__per_cpu_offset: 0x%016lx\n", (unsigned long)__per_cpu_offset);
	printk(KERN_ALERT "kernel_cpustat: 0x%016lx\n", (unsigned long)&kernel_cpustat);
	printk(KERN_ALERT "MAX_SWAPFILES: %u\n", MAX_SWAPFILES);
	printk(KERN_ALERT "address_space size: %lu\n", sizeof(struct address_space));
	printk(KERN_ALERT "address_space.nrpages: %lu\n", (unsigned long)&(as.nrpages) - (unsigned long)&as);
	printk(KERN_ALERT "block_device.bd_list offset: %lu\n", (unsigned long)&(bd.bd_list) - (unsigned long)&bd);
	printk(KERN_ALERT "bloc_device.bd_inode offset: %lu\n", (unsigned long)&(bd.bd_inode) - (unsigned long)&bd);
	printk(KERN_ALERT "inode.i_mapping offset: %lu\n", (unsigned long)&(in.i_mapping) - (unsigned long)&in);
	printk(KERN_ALERT "module.list offset: %lu\n", (unsigned long)&(m->list)-(unsigned long)m);
	printk(KERN_ALERT "module.name offset: %lu\n", (unsigned long)(m->name)-(unsigned long)m);
	printk(KERN_ALERT "module.module_core offset: %lu\n", (unsigned long)&(m->module_core)-(unsigned long)m);

	printk(KERN_ALERT "dev_base_head: 0x%016lx\n", (unsigned long)&(init_net.dev_base_head));
	printk(KERN_ALERT "net_device.dev_list offset: %lu\n", (unsigned long)&(nd.dev_list) - (unsigned long)&nd);
	printk(KERN_ALERT "net_device.name offset: %lu\n", (unsigned long)&(nd.name) - (unsigned long)&nd);
	printk(KERN_ALERT "net_device.dev_addr offset: %lu\n", (unsigned long)&(nd.dev_addr) - (unsigned long)&nd);
	printk(KERN_ALERT "net_device.ip_ptr offset: %lu\n", (unsigned long)&(nd.ip_ptr) - (unsigned long)&nd);
	printk(KERN_ALERT "in_device.ifa_list offset: %lu\n", (unsigned long)&(ind.ifa_list) - (unsigned long)&ind);
	printk(KERN_ALERT "in_ifaddr.ifa_address offset: %lu\n", (unsigned long)&(ina.ifa_address) - (unsigned long)&ina);
	printk(KERN_ALERT "in_ifaddr.ifa_broadcast offset: %lu\n", (unsigned long)&(ina.ifa_broadcast) - (unsigned long)&ina);
	printk(KERN_ALERT "in_ifaddr.ifa_mask offset: %lu\n", (unsigned long)&(ina.ifa_mask) - (unsigned long)&ina);

	return 0;
}

static void __exit gos_info_exit(void)
{
	printk(KERN_ALERT "exit the module\n");
}

MODULE_LICENSE("GPL");
module_init(gos_info_init);
module_exit(gos_info_exit);
