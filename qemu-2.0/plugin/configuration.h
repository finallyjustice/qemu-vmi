/*
 * Obtain configuration info of guest VM (Linux)
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

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "monitor/monitor.h"

#define HOSTNAME_LEN 65

struct hostname_stat
{
	char sysname[HOSTNAME_LEN];
	char nodename[HOSTNAME_LEN];
	char release[HOSTNAME_LEN];
	char version[HOSTNAME_LEN];
	char machine[HOSTNAME_LEN];
	char domainname[HOSTNAME_LEN];
};

// obtain the hostname of VM
void get_hostname(void *mem, struct hostname_stat *stat);
void print_hostname(Monitor *mon, void *mem);

#endif
