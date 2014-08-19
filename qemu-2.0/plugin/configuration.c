/*
 * Obtain the configuration info of guest VM (Linux)
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
 
#include <stdio.h>
#include <stdlib.h>
#include "symbol-x86_64.h"
#include "linux-x86_64.h"
#include "configuration.h"
#include "list.h"
#include "tasks.h"
#include "memory.h"

// obtain the hostname of VM
void get_hostname(void *mem, struct hostname_stat *stat)
{
	// obtain list of modules
	unsigned long cr3 = 0x0;

	// obtain valid cr3 value
	struct list_head *tasks = traverse_tasks(mem);
	struct task *tsk;
	list_for_each_entry(tsk, tasks, list)
	{
		if(tsk->cr3 != 0x0)
		{
			cr3 = tsk->cr3;
			break;
		}
	}

	// free the returned list
	list_for_each_entry(tsk, tasks, list)
	{
		list_del(&(tsk->list));
		free(tsk);
	}
	 
	// cr3 cannot be 0
	if(cr3 == 0x0)
	{
		printf("cr3 is zero!\n");
		return;
	}

	// gpa of init_task kernel symbol
	void *init_task_gpa  = (void *)virt_to_phys(INIT_TASK_GVA);

	// hva of base address of init_task 
	void *init_task_base = mem + (unsigned long)init_task_gpa;
	// hva of nsproxy field of init_task 
	unsigned long *init_task_nsproxy  = (unsigned long *)(init_task_base + TASK_NSPROXY_OFFSET);

	// gva of nsproxy
	unsigned long nsproxy_gva = *init_task_nsproxy;
	// hva of nsproxy
	void *nsproxy_hva = mem + virt_to_phys(nsproxy_gva);

	// gva of uts_namespace
	unsigned long uts_namespace_gva = *(unsigned long *)(nsproxy_hva+NSPROXY_UTS_NS_OFFSET);

	// gva of new_name
	unsigned long new_name_gva = uts_namespace_gva + UTS_NAMESPACE_NAME;

	// hva of new name
	char *new_name_hva = (char *)mem + virt_to_phys(new_name_gva);

	sprintf(stat->sysname, "%s", new_name_hva);
	sprintf(stat->nodename, "%s", new_name_hva+__NEW_UTS_LEN_JUMP);
	sprintf(stat->release, "%s", new_name_hva+2*__NEW_UTS_LEN_JUMP);
	sprintf(stat->version, "%s", new_name_hva+3*__NEW_UTS_LEN_JUMP);
	sprintf(stat->machine, "%s", new_name_hva+4*__NEW_UTS_LEN_JUMP);
	sprintf(stat->domainname, "%s", new_name_hva+5*__NEW_UTS_LEN_JUMP);
}

void print_hostname(Monitor *mon, void *mem)
{
	struct hostname_stat stat;
	get_hostname(mem, &stat);

	monitor_printf(mon, "sysname    : %s\n", stat.sysname);
	monitor_printf(mon, "nodename   : %s\n", stat.nodename);
	monitor_printf(mon, "release    : %s\n", stat.release);
	monitor_printf(mon, "version    : %s\n", stat.version);
	monitor_printf(mon, "machine    : %s\n", stat.machine);
	monitor_printf(mon, "domainname : %s\n", stat.domainname);
}
