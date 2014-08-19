/*
 * The integrity of system call table
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
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "symbol-x86_64.h"
#include "linux-x86_64.h"
#include "syscall_checker.h"

// pthread thread function to check integrity of system call table
void * thread_func(void *arg);

// frequency to check the integrity of system call table
#define SYSCALL_CHECK_FREQUENCY 5

pthread_t tid;
static int is_check_started = 0;

// parameter to thread
struct syscall_info
{
	unsigned long *syscall_table_hva;    // original syscall table address
	unsigned long *syscall_table_shadow; // copy of syscall table
	unsigned int syscall_table_size;     // size of syscall table
	Monitor *mon;
};

// pthread thread function to check integrity of system call table
void * thread_func(void *arg)
{
	struct syscall_info *info = (struct syscall_info *)arg;
	Monitor *mon = info->mon;

	// thread is as a loop
	// thread does NOT stop until having the top command received by QEMU monitor
	while(1)
	{
		// checks the integrity periodically
		sleep(SYSCALL_CHECK_FREQUENCY);

		unsigned int sys_num = 0;
		while(info->syscall_table_hva[sys_num])
		{
			// if system call address is different with the backup address
			if(info->syscall_table_hva[sys_num] != info->syscall_table_shadow[sys_num])
			{
				monitor_printf(mon, "\nALERT: syscall hook detected on %u\n", sys_num);
				// !!! recovery the system call to original GOOD one !!!
				info->syscall_table_hva[sys_num] = info->syscall_table_shadow[sys_num];
				
				monitor_printf(mon, "(qemu) ");
				monitor_flush(mon);
			}
			sys_num++;
		}	
	}
}

// start the syscall checker by create a new monitoring thread (pthread)
void syscall_checker_start(Monitor *mon, void *mem)
{
	// return if the checker already started
	if(is_check_started == 1)
	{
		monitor_printf(mon, "error: system call checker already started.\n");
		return;
	}

	// gpa of base of system call table in VM 
	void *syscall_table_gpa = (void *)virt_to_phys(SYSTEM_CALL_GVA);
	// hva of base of system call table
	unsigned long *syscall_table_hva = (unsigned long *)(mem + (unsigned long)syscall_table_gpa);

	// obtain the system call table size
	unsigned int syscall_table_size = 0;
	while(syscall_table_hva[syscall_table_size])
		syscall_table_size++;
	syscall_table_size = syscall_table_size * sizeof(unsigned long *);

	// allocate memory for backup of system call table
	// the thread created later would periodically compare entries from both table
	unsigned long *syscall_table_shadow = (unsigned long *)malloc(syscall_table_size);
	memcpy(syscall_table_shadow, syscall_table_hva, syscall_table_size);

	// make parameters for thread function
	struct syscall_info *info = (struct syscall_info *)malloc(sizeof(struct syscall_info));
	info->syscall_table_hva    = syscall_table_hva;
	info->syscall_table_shadow = syscall_table_shadow;
	info->syscall_table_size   = syscall_table_size;
	info->mon = mon;

	// create the pthread thread 
	if(pthread_create(&tid, NULL, thread_func, (void *)info) != 0)
	{
		printf("Failed to create thread\n");
		return;
	}

	is_check_started = 1;
	monitor_printf(mon, "system call checker started.\n\n");
}

// stop the syscall checker
void syscall_checker_stop(Monitor *mon)
{
	// return if the checker is not started yet
	if(is_check_started == 0)
	{
		monitor_printf(mon, "system call checker NOT started.\n");
		return;
	}

	if(pthread_cancel(tid) != 0)
	{
		printf("Failed to cancel thread\n");
		return;
	}

	if(pthread_join(tid, NULL) != 0)
	{
		printf("Failed to join thread\n");
		return;
	}

	monitor_printf(mon, "system call checker stopped.\n");
}
