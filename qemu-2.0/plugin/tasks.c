/*
 * Obtain list of tasks in guest VM (Linux)
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
#include "linux-x86_64.h"
#include "symbol-x86_64.h"
#include "list.h"
#include "tasks.h"

// return the linked list of task struct
struct list_head *traverse_tasks(void *mem)
{
	// define list to return result
	struct list_head *tasks = (struct list_head *)malloc(sizeof(struct list_head));
	INIT_LIST_HEAD(tasks);

	// gva of init_task kernel symbol
	void *init_task_gva  = (void *)INIT_TASK_GVA;
	// gpa of init_task kernel symbol
	void *init_task_gpa  = (void *)virt_to_phys(INIT_TASK_GVA);

	// hva of base address of init_task 
	void *init_task_base = mem + (unsigned long)init_task_gpa;
	// hva of pid field of init_task 
	void *init_task_pid  = init_task_base + TASK_PID_OFFSET;
	// hva of comm field of init_task 
	void *init_task_comm = init_task_base + TASK_COMM_OFFSET;
	// hva of tasks field of init_task 
	unsigned long *init_task_tasks = init_task_base + TASK_TASKS_OFFSET;

	// hva of init_task.mm
	void *init_task_mm = init_task_base + TASK_MM_OFFSET;
	void *cr3 = 0x0;
	// gva of base of mm
	void *mm_base = (void *)(*(unsigned long *)init_task_mm);
	void *mm_pgd;
	// if task_struct.mm is NOT NULL
	if(mm_base)
	{
		// hva of base of mm (mm_struct)
		mm_base = mem + virt_to_phys((unsigned long)mm_base);
		mm_pgd = mm_base + MM_PGD_OFFSET;
		cr3 = (void *)(*(unsigned long *)mm_pgd);
	}

	// add result to list
	struct task *tsk = (struct task *)malloc(sizeof(struct task));
	tsk->pid = *(int *)init_task_pid;
	sprintf(tsk->name, "%s", (char *)init_task_comm);
	tsk->cr3 = (unsigned long)cr3;
	list_add_tail(&(tsk->list), tasks);

	// gva of the next task_struct 
	void *next_task_gva = (void *)(*init_task_tasks-TASK_TASKS_OFFSET);
	// gpa of the next task_struct 
	void *next_task_gpa;
	// hva of the base address of next task_struct 
	void *next_task_base;
	// hva of pid field of next task_struct
	void *next_task_pid;
	// hva of comm field of next task_struct
	void *next_task_comm;
	// hva of tasks field of next task_struct
	unsigned long *next_task_tasks;

	// gva of mm field of next task_struct
	void *next_task_mm;

	// loop until the begin of task_struct list
	while(next_task_gva != init_task_gva)
	{
		next_task_gpa = (void *)virt_to_phys((unsigned long)next_task_gva);
		next_task_base = mem + (unsigned long)next_task_gpa;
		next_task_pid  = next_task_base + TASK_PID_OFFSET;
		next_task_comm = next_task_base + TASK_COMM_OFFSET;
		next_task_tasks = next_task_base + TASK_TASKS_OFFSET;

		next_task_mm = next_task_base + TASK_MM_OFFSET;
		cr3 = 0x0;
		mm_base = (void *)(*(unsigned long *)next_task_mm);
		// task_struct.mm==NULL means kernel thread
		if(mm_base)
		{
			mm_base = mem + virt_to_phys((unsigned long)mm_base);
			mm_pgd = mm_base + MM_PGD_OFFSET;
			cr3 = (void *)(*(unsigned long *)mm_pgd);
		}

		// add result to list
		struct task *tsk = (struct task *)malloc(sizeof(struct task));
		tsk->pid = *(int *)next_task_pid;
		sprintf(tsk->name, "%s", (char *)next_task_comm);
		tsk->cr3 = (unsigned long)cr3;
		list_add_tail(&(tsk->list), tasks);

		next_task_gva = (void *)(*next_task_tasks-TASK_TASKS_OFFSET);
	}

	return tasks;
}

// list all tasks in guest VM
void list_tasks(Monitor *mon, void *mem)
{
	// result returned as linked list
	struct list_head *head = traverse_tasks(mem);
	int tot_task = 0;
	
	if(head == NULL)
		goto result;

	struct task *tsk;
	list_for_each_entry(tsk, head, list)
	{
		tot_task++;
		monitor_printf(mon, "%d, %d, %s, 0x%016lx\n", 
				tot_task, 
				tsk->pid, 
				tsk->name, 
				tsk->cr3);
	}

	// free the linked list
	list_for_each_entry(tsk, head, list)
	{
		list_del(&(tsk->list));
		free(tsk);
	}

result:
	monitor_printf(mon, "Total Tasks: %d\n", tot_task);
}
