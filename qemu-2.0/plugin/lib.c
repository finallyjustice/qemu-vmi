/*
 * library functions
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

#include "lib.h"
#include <math.h>

unsigned long ilog2(unsigned long v)
{
	unsigned long len = (unsigned long)log2((double)v);	
	return len;
}
