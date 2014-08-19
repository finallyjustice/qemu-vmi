/*
 * CPU utilization computation
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

#include <pthread.h>
#include "linux-x86_64.h"
#include "symbol-x86_64.h"
#include "cpu_usage.h"

#define TOP_SLEEP_TIME 2

// param to pthread thread function
struct cpu_usage_info
{
	Monitor *mon;
	void *mem;
};

// obtain the number of cpu cores in guest VM
int get_cpu_num(void *mem)
{
	unsigned long *cpu_possible_mask_hva = 
		(unsigned long *)(mem+virt_to_phys(CPU_POSSIBLE_MASK_GVA));
	// each bit represents a possible CPU core
	unsigned long cpu_bit = *cpu_possible_mask_hva;
	
	int num = 0;
	while(cpu_bit)
	{
		num++;
		cpu_bit = cpu_bit >> 1;
	}

	return num;
}

// get the user cpu time for cpu c
unsigned long cpu_user_time(int c, void *mem, unsigned long *cpustat_hva)
{
	return cpustat_hva[CPUTIME_USER];
}

// get the nice cpu time for cpu c
unsigned long cpu_nice_time(int c, void *mem, unsigned long *cpustat_hva)
{
	return cpustat_hva[CPUTIME_NICE];
}

// get the system cpu time for cpu c
unsigned long cpu_sys_time(int c, void *mem, unsigned long *cpustat_hva)
{
	return cpustat_hva[CPUTIME_SYSTEM];
}

// get the idle cpu time for cpu c
unsigned long cpu_idle_time(int c, void *mem, unsigned long *cpustat_hva)
{
	return cpustat_hva[CPUTIME_IDLE];
}

// get the iowait cpu time for cpu c
unsigned long cpu_iowait_time(int c, void *mem, unsigned long *cpustat_hva)
{
	return cpustat_hva[CPUTIME_IOWAIT];
}

// get the hardware irq cpu time for cpu c
unsigned long cpu_hirq_time(int c, void *mem, unsigned long *cpustat_hva)
{
	return cpustat_hva[CPUTIME_IRQ];
}

// get the software irq cpu time for cpu c
unsigned long cpu_sirq_time(int c, void *mem, unsigned long *cpustat_hva)
{
	return cpustat_hva[CPUTIME_SOFTIRQ];
}

// get the steal cpu time for cpu c 
unsigned long cpu_steal_time(int c, void *mem, unsigned long *cpustat_hva)
{
	return cpustat_hva[CPUTIME_STEAL];
}

// get cpustat for each cpu c, returned by cpustat
void get_cpustat(void *mem, unsigned long cpustat[][NR_STATS], int num_cpu)
{
	int c;
	
	for(c=0; c<NR_STATS; c++)
		cpustat[num_cpu][c] = 0;

	for(c=0; c<num_cpu; c++)
	{
		unsigned long *__per_cpu_offset_hva = 
			(unsigned long *)(mem+virt_to_phys(__PER_CPU_OFFSET_GVA));
		// gva of base->cpustat in guest Linux VM
		unsigned long base_cpustat_gva = 
			(unsigned long)(__per_cpu_offset_hva[c]+KERNEL_CPUSTAT);
		unsigned long *base_cpustat_hva = 
			(unsigned long *)(mem+virt_to_phys(base_cpustat_gva));	

		// c is index of VM cpu core
		cpustat[c][CPUTIME_USER]    = cpu_user_time(c, mem, base_cpustat_hva);  
		cpustat[c][CPUTIME_NICE]    = cpu_nice_time(c, mem, base_cpustat_hva);  
		cpustat[c][CPUTIME_SYSTEM]  = cpu_sys_time(c, mem, base_cpustat_hva);  
		cpustat[c][CPUTIME_IDLE]    = cpu_idle_time(c, mem, base_cpustat_hva);  
		cpustat[c][CPUTIME_IOWAIT]  = cpu_iowait_time(c, mem, base_cpustat_hva);  
		cpustat[c][CPUTIME_IRQ]     = cpu_hirq_time(c, mem, base_cpustat_hva);  
		cpustat[c][CPUTIME_SOFTIRQ] = cpu_sirq_time(c, mem, base_cpustat_hva);  
		cpustat[c][CPUTIME_STEAL]   = cpu_steal_time(c, mem, base_cpustat_hva);

		cpustat[c][CPUTIME_TOTAL] = 0;
		cpustat[c][CPUTIME_TOTAL] += cpustat[c][CPUTIME_USER];
		cpustat[c][CPUTIME_TOTAL] += cpustat[c][CPUTIME_NICE];
		cpustat[c][CPUTIME_TOTAL] += cpustat[c][CPUTIME_SYSTEM];
		cpustat[c][CPUTIME_TOTAL] += cpustat[c][CPUTIME_IDLE];
		cpustat[c][CPUTIME_TOTAL] += cpustat[c][CPUTIME_IOWAIT];
		cpustat[c][CPUTIME_TOTAL] += cpustat[c][CPUTIME_IRQ];
		cpustat[c][CPUTIME_TOTAL] += cpustat[c][CPUTIME_SOFTIRQ];
		cpustat[c][CPUTIME_TOTAL] += cpustat[c][CPUTIME_STEAL];
		
		cpustat[num_cpu][CPUTIME_USER]    += cpustat[c][CPUTIME_USER];
		cpustat[num_cpu][CPUTIME_NICE]    += cpustat[c][CPUTIME_NICE];
		cpustat[num_cpu][CPUTIME_SYSTEM]  += cpustat[c][CPUTIME_SYSTEM];
		cpustat[num_cpu][CPUTIME_IDLE]    += cpustat[c][CPUTIME_IDLE];
		cpustat[num_cpu][CPUTIME_IOWAIT]  += cpustat[c][CPUTIME_IOWAIT];
		cpustat[num_cpu][CPUTIME_IRQ]     += cpustat[c][CPUTIME_IRQ];
		cpustat[num_cpu][CPUTIME_SOFTIRQ] += cpustat[c][CPUTIME_SOFTIRQ];
		cpustat[num_cpu][CPUTIME_STEAL]   += cpustat[c][CPUTIME_STEAL];
		cpustat[num_cpu][CPUTIME_TOTAL]   += cpustat[c][CPUTIME_TOTAL];
	}
}

void comp_cpu_usage(void *mem, struct cpu_usage_stat *stat)
{
	// obtain the num of CPU cores in guest VM
	int num_cpu = get_cpu_num(mem);
	unsigned long cpustat1[num_cpu+1][NR_STATS];
	unsigned long cpustat2[num_cpu+1][NR_STATS];
	unsigned long cpustat_diff[num_cpu+1][NR_STATS];

	// obtain the cpustat info, similar to /proc/stat
	get_cpustat(mem, cpustat1, num_cpu);
	// wait for a while
	sleep(TOP_SLEEP_TIME);
	// obtain the cpustat again, similar to /proc/stat
	get_cpustat(mem, cpustat2, num_cpu);
	
	// compute the difference of two cpustat, in cpustat_diff
	int c, i;
	for(c=0; c<=num_cpu; c++)
	{
		for(i=0; i<NR_STATS; i++)
		{
			cpustat_diff[c][i] = cpustat2[c][i]-cpustat1[c][i];	
		}
	}

	// calculate the cpu utilization over 100%
	for(c=0; c<=num_cpu; c++)
	{
		// if the total cpu time does NOT change
		if(cpustat_diff[c][CPUTIME_TOTAL] == 0)
		{
			// %us
			stat[c].usage[CPUTIME_USER]    = 0.0f; 
			// %sy
			stat[c].usage[CPUTIME_SYSTEM]  = 0.0f; 
			// %id
			stat[c].usage[CPUTIME_IDLE]    = 100.0f; 
			// %ni
			stat[c].usage[CPUTIME_NICE]    = 0.0f;
			// %wa
			stat[c].usage[CPUTIME_IOWAIT]  = 0.0f; 
			// %hi
			stat[c].usage[CPUTIME_IRQ]     = 0.0f; 
			// %si
			stat[c].usage[CPUTIME_SOFTIRQ] = 0.0f; 
			// %st
			stat[c].usage[CPUTIME_STEAL]   = 0.0f; 
		}
		else // if the total cpu time DOES change
		{
			float tot_time = (float)cpustat_diff[c][CPUTIME_TOTAL];
			// %us
			stat[c].usage[CPUTIME_USER]    = 100.0f * 
				(float)(cpustat_diff[c][CPUTIME_USER]+cpustat_diff[c][CPUTIME_NICE])/tot_time;
			// %sy
			stat[c].usage[CPUTIME_SYSTEM]  = 100.0f * 
				(float)(cpustat_diff[c][CPUTIME_SYSTEM]+cpustat_diff[c][CPUTIME_IRQ]+cpustat_diff[c][CPUTIME_SOFTIRQ])/tot_time;
			// %id
			stat[c].usage[CPUTIME_IDLE]    = 100.0f * 
				(float)cpustat_diff[c][CPUTIME_IDLE]/tot_time;
			// %ni
			stat[c].usage[CPUTIME_NICE]    = 100.0f * 
				(float)cpustat_diff[c][CPUTIME_NICE]/tot_time;
			// %wa
			stat[c].usage[CPUTIME_IOWAIT]  = 100.0f * 
				(float)cpustat_diff[c][CPUTIME_IOWAIT]/tot_time;
			// %hi
			stat[c].usage[CPUTIME_IRQ]     = 100.0f * 
				(float)cpustat_diff[c][CPUTIME_IRQ]/tot_time;	
			// %si
			stat[c].usage[CPUTIME_SOFTIRQ] = 100.0f * 
				(float)cpustat_diff[c][CPUTIME_SOFTIRQ]/tot_time;
			// %st
			stat[c].usage[CPUTIME_STEAL]   = 100.0f * 
				(float)cpustat_diff[c][CPUTIME_STEAL]/tot_time;
		}
	}
}

// thread function for pthread
// obtain the cpu usage info and output to monitor
void * cpu_usage_thread(void *arg)
{
	struct cpu_usage_info *info = (struct cpu_usage_info *)arg;
	Monitor *mon = info->mon;
	void *mem = info->mem;
	free(info);

	// obtain the num of CPU cores in guest VM
	int num_cpu = get_cpu_num(mem);
	struct cpu_usage_stat stat[num_cpu+1];	
	comp_cpu_usage(mem, stat);

	// output cpu utilization to monitor
	monitor_printf(mon, "\n");
	// average cpu utilization
	monitor_printf(mon, "CPU AVG : "
			"%5.1f%%us, %5.1f%%sy, %5.1f%%id, %5.1f%%ni, "
			"%5.1f%%wa, %5.1f%%hi, %5.1f%%si, %5.1f%%st\n", 
			stat[num_cpu].usage[CPUTIME_USER], 
			stat[num_cpu].usage[CPUTIME_SYSTEM],
			stat[num_cpu].usage[CPUTIME_IDLE],
			stat[num_cpu].usage[CPUTIME_NICE],
			stat[num_cpu].usage[CPUTIME_IOWAIT],
			stat[num_cpu].usage[CPUTIME_IRQ],
			stat[num_cpu].usage[CPUTIME_SOFTIRQ],
			stat[num_cpu].usage[CPUTIME_STEAL]);

	// cpu utilization for each cpu core
	int c;
	for(c=0; c<num_cpu; c++)
	{
		monitor_printf(mon, "CPU %3d : "
				"%5.1f%%us, %5.1f%%sy, %5.1f%%id, %5.1f%%ni, "
				"%5.1f%%wa, %5.1f%%hi, %5.1f%%si, %5.1f%%st\n", 
				c, 
				stat[c].usage[CPUTIME_USER], 
				stat[c].usage[CPUTIME_SYSTEM],
				stat[c].usage[CPUTIME_IDLE],
				stat[c].usage[CPUTIME_NICE],
				stat[c].usage[CPUTIME_IOWAIT],
				stat[c].usage[CPUTIME_IRQ],
				stat[c].usage[CPUTIME_SOFTIRQ],
				stat[c].usage[CPUTIME_STEAL]);
	}

	monitor_printf(mon, "(qemu) ");
	monitor_flush(mon);

	return NULL;
}

// obtain the cpu usage info by creating a thread
void cpu_usage_handle(Monitor *mon, void *mem)
{
	pthread_t tid;
	struct cpu_usage_info *info = 
		(struct cpu_usage_info *)malloc(sizeof(struct cpu_usage_info)); 
	info->mon = mon;
	info->mem = mem;
	
	if(pthread_create(&tid, NULL, cpu_usage_thread, (void *)info) != 0)
	{
		printf("Failed to create thread\n");
			return;
	}
}
