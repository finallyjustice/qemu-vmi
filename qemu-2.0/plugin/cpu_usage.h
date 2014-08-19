/*
 * CPU utilization computation header file
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

#ifndef __CPU_USAGE_H__
#define __CPU_USAGE_H__

#include "symbol-x86_64.h"
#include "monitor/monitor.h"

struct cpu_usage_stat
{
	float usage[NR_STATS];
};

// obtain the number of cpu cores in guest VM
int get_cpu_num(void *mem);
// get cpustat for each cpu c, returned by cpustat
void get_cpustat(void *mem, unsigned long cpustat[][NR_STATS], int num_cpu);
void comp_cpu_usage(void *mem, struct cpu_usage_stat *stat);
// obtain the cpu usage info and output to monitor
void * cpu_usage_thread(void *arg);
// obtain the cpu usage info by creating a thread
void cpu_usage_handle(Monitor *mon, void *mem);


// get the user cpu time for cpu c
unsigned long cpu_user_time(int c, void *mem, unsigned long *cpustat_hva);
// get the nice cpu time for cpu c
unsigned long cpu_nice_time(int c, void *mem, unsigned long *cpustat_hva);
// get the system cpu time for cpu c
unsigned long cpu_sys_time(int c, void *mem, unsigned long *cpustat_hva);
// get the idle cpu time for cpu c
unsigned long cpu_idle_time(int c, void *mem, unsigned long *cpustat_hva);
// get the iowait cpu time for cpu c
unsigned long cpu_iowait_time(int c, void *mem, unsigned long *cpustat_hva);
// get the hardware irq cpu time for cpu c
unsigned long cpu_hirq_time(int c, void *mem, unsigned long *cpustat_hva);
// get the software irq cpu time for cpu c
unsigned long cpu_sirq_time(int c, void *mem, unsigned long *cpustat_hva);
// get the steal cpu time for cpu c
unsigned long cpu_steal_time(int c, void *mem, unsigned long *cpustat_hva);

#endif
