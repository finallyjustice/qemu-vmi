/*
 * Obtain list of modules in guest VM (Linux)
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
#include "list.h"
#include "tasks.h"
#include "memory.h"
#include "module.h"

// traverse module list
struct list_head *traverse_modules(void *mem)
{
	// init the list to return
	struct list_head *mods = (struct list_head *)malloc(sizeof(struct list_head));
	INIT_LIST_HEAD(mods);

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
		return NULL;
	}

	// start to traverse module list
	struct list_head *mod_hva = (mem+virt_to_phys(MODULES_HEAD_GVA));
	unsigned long mod_list_gva = (unsigned long)(mod_hva->next);
	unsigned long mod_list_gpa;
	struct list_head *mod_list_hva;
	void *mod_base_hva;
	while(mod_list_gva != MODULES_HEAD_GVA)
	{
		mod_list_gpa = trans_addr(mem, mod_list_gva, cr3);
		mod_base_hva = mem+mod_list_gpa-MODULE_LIST_OFFSET;

		char *name = (char *)(mod_base_hva+MODULE_NAME_OFFSET);
		unsigned long core = *(unsigned long *)(mod_base_hva+MODULE_MODULE_CORE_OFFSET);

		// add module to linked list
		struct module *m = (struct module *)malloc(sizeof(struct module));
		sprintf(m->name, "%s", name);
		m->core = core;
		list_add_tail(&(m->list), mods);
		//printf("%s  0x%016lx\n", name, core);

		// compute gva for next module.list
		mod_list_hva = (struct list_head *)(mem+mod_list_gpa);
		mod_list_gva = (unsigned long)(mod_list_hva->next);
	}

	return mods;
}

// list all modules in guest VM
void list_modules(Monitor *mon, void *mem)
{
	// result returned as linked list
	struct list_head *head = traverse_modules(mem);
	int tot_mod = 0;
	
	if(head == NULL)
		goto result;

	struct module *mod;
	list_for_each_entry(mod, head, list)
	{
		tot_mod++;
		monitor_printf(mon, "%d, %s, 0x%016lx\n", 
				tot_mod, 
				mod->name, 
				mod->core);
	}

	// free the linked list
	list_for_each_entry(mod, head, list)
	{
		list_del(&(mod->list));
		free(mod);
	}

result:
	monitor_printf(mon, "Total Modules: %d\n", tot_mod);

	return;
}
