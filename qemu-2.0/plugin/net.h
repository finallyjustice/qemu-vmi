/*
 * Networking of VM (Linux)
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

#ifndef __NET_H__
#define __NET_H__

#include "monitor/monitor.h"

#define IP_LEN 16

// obtain network device info
void print_net_device(Monitor *mon, void *mem);

#endif
