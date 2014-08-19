/*
 * memory utilization computation header file
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

#ifndef __MEM_USAGE_H__
#define __MEM_USAGE_H__

#include "symbol-x86_64.h"
#include "monitor/monitor.h"

struct mem_usage_stat
{
	unsigned long MemTotal;  // in kB
	unsigned long MemFree;   // in kB
	unsigned long Buffers;   // in kB
	unsigned long Cached;    // in kB
	float mem_usage;
};

// get total memory in KB
unsigned long getMemTotal(void *mem);
// get free memory in KB
unsigned long getMemFree(void *mem);
// get buffers memory in KB
unsigned long getBuffers(void *mem);
// get file pages in kB
unsigned long getMemFile(void *mem);
// get swap cache pages in kB
unsigned long getMemSwapCache(void *mem);
// get cached memory in KB
unsigned long getCached(void *mem);
void get_mem_usage(void *mem, struct mem_usage_stat *stat);
// obtain the memory usage info 
void mem_usage_handle(Monitor *mon, void *mem);

#endif
