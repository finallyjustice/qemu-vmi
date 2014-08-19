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

#ifndef __SYSCALL_CHECKER_H__
#define __SYSCALL_CHECKER_H__

#include "monitor/monitor.h"

// start the syscall checker by create a new monitoring thread (pthread)
void syscall_checker_start(Monitor *mon, void *mem);
// stop the syscall checker 
void syscall_checker_stop(Monitor *mon);

#endif
