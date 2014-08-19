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

#ifndef __MODULE_H__
#define __MODULE_H__

#include "list.h"
#include "monitor/monitor.h"

#define MOD_NAME_LEN 32

struct module
{
	char name[MOD_NAME_LEN];
	unsigned long core;
	struct list_head list;
};

// traverse module list
struct list_head *traverse_modules(void *mem);
// list all modules in guest VM
void list_modules(Monitor *mon, void *mem);

#endif
