/*
 * assistant functions for Linux x86_64
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

#ifndef __LINUX_X86_64_H__
#define __LINUX_X86_64_H__

// convert GPA to GVA
unsigned long __va(unsigned long x);

// convery GVA to GPA
unsigned long __pa(unsigned long x);

// convert GVA to GPA
unsigned long virt_to_phys(unsigned long x);

#endif
