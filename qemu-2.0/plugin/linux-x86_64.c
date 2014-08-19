/*
 * Assistant functions for Linux x86_64
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

#include "symbol-x86_64.h"
#include "linux-x86_64.h"

// convert GPA to GVA
unsigned long __va(unsigned long x)
{
	return (x+PAGE_OFFSET);
}

// convery GVA to GPA
unsigned long __pa(unsigned long x)
{
	return virt_to_phys(x);
}

// convert GVA to GPA
unsigned long virt_to_phys(unsigned long x)
{
	unsigned long y = x - __START_KERNEL_map;

	/* use the carry flag to determine if x was < __START_KERNEL_map */
	x = y + ((x > y) ? 0x0 : (__START_KERNEL_map - PAGE_OFFSET));

	return x;
}

