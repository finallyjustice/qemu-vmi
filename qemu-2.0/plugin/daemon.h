/*
 * The listener to return xml
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

#ifndef __DAEMON_H__
#define __DAEMON_H__

#include "monitor/monitor.h"

// start the listener by creating a new thread (pthread)
void listener_start(Monitor *mon, void *mem);
// stop the listener 
void listener_stop(Monitor *mon);

#endif
