/*
 * Extra Human Monitor Interface
 *
 * Copyright Dongli Zhang, 2014
 *
 * Authors: 
 *  Dongli Zhang   <dongli.zhang0129@gmail.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 *
 */

#include "hmp.h"
#include "net/net.h"
#include "sysemu/char.h"
#include "qemu/option.h"
#include "qemu/timer.h"
#include "qmp-commands.h"
#include "qemu/sockets.h"
#include "monitor/monitor.h"
#include "qapi/opts-visitor.h"
#include "ui/console.h"
#include "block/qapi.h"
#include "qemu-io.h"
#include "exec/ram_addr.h"

// header define by developer
#include "linux-x86_64.h"
#include "syscall_checker.h"
#include "tasks.h"
#include "module.h"
#include "cpu_usage.h"
#include "mem_usage.h"
#include "symbol-x86_64.h"
#include "memory.h"
#include "configuration.h"
#include "net.h"
#include "daemon.h"

// command to list all tasks in guest VM
void hmp_list_process(Monitor *mon, const QDict *qdict)
{
	void *mem = qemu_get_ram_ptr(0x0);
	list_tasks(mon, mem);
}

// command to list all modules in guest VM
void hmp_list_modules(Monitor *mon, const QDict *qdict)
{
	void *mem = qemu_get_ram_ptr(0x0);
	list_modules(mon, mem);
}

// command to check (and correct) the modification of system call table
void hmp_syscall_checker(Monitor *mon, const QDict *qdict)
{
	void *mem = qemu_get_ram_ptr(0x0);
	const char *action = qdict_get_str(qdict, "action");

	if(strcmp(action, "start") == 0)
	{
		// start system call integrity checker
		syscall_checker_start(mon, mem);
	}
	else if(strcmp(action, "stop") == 0)
	{
		// stop system call integrity checker
		syscall_checker_stop(mon);
	}
	else
	{
		// wrong parameters - show usage
		monitor_printf(mon, "Usage: check_syscall start|stop.\n");
	}
}

// command to get the CPU utilization
void hmp_top(Monitor *mon, const QDict *qdict)
{
	void *mem = qemu_get_ram_ptr(0x0);
	cpu_usage_handle(mon, mem);
}

// command to get the memory utilization
void hmp_free(Monitor *mon, const QDict *qdict)
{
	void *mem = qemu_get_ram_ptr(0x0);
	mem_usage_handle(mon, mem);
}

// command to read data at gpa
void hmp_read_data(Monitor *mon, const QDict *qdict)
{
	void *mem = qemu_get_ram_ptr(0x0);
	const char *addr_class = qdict_get_str(qdict, "class");
	const char *addr_type  = qdict_get_str(qdict, "type");
	u64 addr_size          = (u64)qdict_get_int(qdict, "size");
	u64 addr_base          = (u64)qdict_get_int(qdict, "addr");

	u64 val = read_gpa_ul(mem, addr_base);
	
	printf("%s, %s, %lu, 0x%016lx, 0x%016lx\n", addr_class, addr_type, addr_size, addr_base, val);
}

// command to translate the gva to gpa by page table walking
void hmp_trans_addr(Monitor *mon, const QDict *qdict)
{
	void *mem = qemu_get_ram_ptr(0x0);
	// address to be translated
	u64 gva = (u64)qdict_get_int(qdict, "gva");
	// address space of process
	u64 cr3 = (u64)qdict_get_int(qdict, "cr3");

	// translate gva to gpa according to cr3 register
	u64 gpa = trans_addr(mem, gva, cr3);

	monitor_printf(mon, "gva: 0x%016lx\n", gva);
	monitor_printf(mon, "cr3: 0x%016lx\n", cr3);
	monitor_printf(mon, "gpa: 0x%016lx\n", gpa);
}

// command to obtain the hostname information
void hmp_hostname(Monitor *mon, const QDict *qdict)
{
	void *mem = qemu_get_ram_ptr(0x0);
	// print the hostname of VM
	print_hostname(mon, mem);
}

// command to obtain the network device info
void hmp_ifconfig(Monitor *mon, const QDict *qdict)
{
	void *mem = qemu_get_ram_ptr(0x0);
	// print the ifconfig result
	print_net_device(mon, mem);
}

// start or stop the listening daemon
void hmp_listen(Monitor *mon, const QDict *qdict)
{
	void *mem = qemu_get_ram_ptr(0x0);
	const char *action = qdict_get_str(qdict, "action");

	if(strcmp(action, "start") == 0)
	{
		// start the listening daemon
		listener_start(mon, mem);
	}
	else if(strcmp(action, "stop") == 0)
	{
		// stop the listening daemon
		listener_stop(mon);
	}
	else
	{
		// wrong parameters - show usage
		monitor_printf(mon, "Usage: listen start|stop.\n");
	}
}

// start or stop the memory access logging in VM
void hmp_mem_logger(Monitor *mon, const QDict *qdict)
{
	const char *action = qdict_get_str(qdict, "action");
	if(strcmp(action, "start") == 0)
	{
		monitor_printf(mon, "Waiting...\n");
		mem_logger(1);    //start mem-logger
		monitor_printf(mon, "Mem-Logger Started.\n");
	}
	else if(strcmp(action, "stop") == 0)
	{
		monitor_printf(mon, "Waiting...\n");
		mem_logger(2);    //stop mem-logger
		monitor_printf(mon, "Mem-Logger Stopped.\n");
	}
	else
	{
		monitor_printf(mon, "Usage: mem-logger start|stop.\n");
	}
}
