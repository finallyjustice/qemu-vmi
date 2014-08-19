/*
 * header file to list tasks
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

#ifndef __TASKS_H__
#define __TASKS_H__

#include "monitor/monitor.h"
#include "list.h"

#define TASK_NAME_LEN 30

struct task
{
	int pid;
	char name[TASK_NAME_LEN];
	unsigned long cr3;
	struct list_head list;
};

// return the linked list of task struct
struct list_head *traverse_tasks(void *mem);
// list all tasks in guest VM
void list_tasks(Monitor *mon, void *mem);

#endif
