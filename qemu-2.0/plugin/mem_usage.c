/*
 * memory utilization computation
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

#include "linux-x86_64.h"
#include "symbol-x86_64.h"
#include "list.h"
#include "mem_usage.h"

// get total memory in KB
unsigned long getMemTotal(void *mem)
{
	unsigned long *totalram_pages_hva = 
		(unsigned long *)(mem+virt_to_phys(TOTALRAM_PAGES_GVA));

	unsigned long MemTotal = (*totalram_pages_hva) * PAGE_KB;
	return MemTotal;
}

// get free memory in KB
unsigned long getMemFree(void *mem)
{
	unsigned long *vm_stat_hva = 
		(unsigned long *)(mem+virt_to_phys(VM_STAT_GVA));

	unsigned long MemFree = (*(vm_stat_hva+NR_FREE_PAGES)) * PAGE_KB;
	return MemFree;
}

// get buffers memory in KB
unsigned long getBuffers(void *mem)
{
	unsigned long tot = 0;
	// all_bdevs is the list_head header of all struct block_device
	struct list_head *all_bdevs_hva = 
		(struct list_head *)(mem+virt_to_phys(ALL_BDEVS_GVA));

	void *next_bd_list_gva;
	struct list_head *next_bd_list_hva;
	void *bdev_base_hva;
	unsigned long inode_base_gva;
	void *inode_base_hva;
	unsigned long address_space_gva;
	void *address_space_hva;
	unsigned long nrpages;

	next_bd_list_gva = (void *)(all_bdevs_hva->next);

	while(next_bd_list_gva != (void *)ALL_BDEVS_GVA)
	{
		// convert gva to hva
		next_bd_list_hva = (struct list_head *)(mem+virt_to_phys((unsigned long)next_bd_list_gva));
		// hva of block_device base addr
		bdev_base_hva = ((void *)next_bd_list_hva)-BLOCK_DEVICE_BD_LIST_OFFSET;
		// gva of base of block_device.bd_inode
		inode_base_gva = *(unsigned long *)(bdev_base_hva+BLOCK_DEVICE_BD_INODE_OFFSET);
		// hva of base of inode
		inode_base_hva = mem+virt_to_phys(inode_base_gva);
		// gva of base of address_space
		address_space_gva = *(unsigned long *)(inode_base_hva+INODE_I_MAPPING_OFFSET);
		// hva of base of address_space
		address_space_hva = mem+virt_to_phys(address_space_gva);
		
		nrpages = *(unsigned long *)(address_space_hva+ADDRESS_SPACE_NRPAGES_OFFSET);
		tot += nrpages;

		next_bd_list_gva = next_bd_list_hva->next;
	}

	return tot * PAGE_KB;
}

// get file pages in kB
unsigned long getMemFile(void *mem)
{
	unsigned long *vm_stat_hva = 
		(unsigned long *)(mem+virt_to_phys(VM_STAT_GVA));

	unsigned long MemFile = (*(vm_stat_hva+NR_FILE_PAGES)) * PAGE_KB;
	
	return MemFile;
}

// get swap cache pages in kB
unsigned long getMemSwapCache(void *mem)
{
	unsigned long swapcache_pages = 0;
	int i;
	
	void *swapper_spaces_hva = mem+virt_to_phys(SWAPPER_SPACES_GVA); 

	for(i=0; i<MAX_SWAPFILES; i++)
	{
		unsigned long *swapper_space_nrpages_hva = 
			(unsigned long *)(swapper_spaces_hva+ADDRESS_SPACE_NRPAGES_OFFSET);
		swapcache_pages += *swapper_space_nrpages_hva;

		swapper_spaces_hva += ADDRESS_SPACE_SIZE;
	}
		
	return swapcache_pages * PAGE_KB;
}

// get cached memory in KB
unsigned long getCached(void *mem)
{
	unsigned long cached;
	// calculate file pages
	unsigned long file_size = getMemFile(mem);
	// calculate swap cache pages
	unsigned swapcache_size = getMemSwapCache(mem);	
	// calculate cache pages
	unsigned long bufferram = getBuffers(mem);

	// equation
	cached = file_size-swapcache_size-bufferram;

	return cached;
}

void get_mem_usage(void *mem, struct mem_usage_stat *stat)
{
	float mem_usage;
	unsigned long MemTotal = getMemTotal(mem);
	unsigned long MemFree  = getMemFree(mem);
	unsigned long Buffers  = getBuffers(mem);
	unsigned long Cached   = getCached(mem);

	// the equation to compute the Memory utilization
	mem_usage = ((float)(MemTotal-MemFree-Buffers-Cached))/(float)MemTotal;
	mem_usage = mem_usage * 100.0f;

	stat->MemTotal  = MemTotal;
	stat->MemFree   = MemFree;
	stat->Buffers   = Buffers;
	stat->Cached    = Cached;
	stat->mem_usage = mem_usage;
}

// obtain the memory usage info by creating a thread
void mem_usage_handle(Monitor *mon, void *mem)
{
	struct mem_usage_stat stat;
	get_mem_usage(mem, &stat);

	// output result to QEMU monitor
	monitor_printf(mon, "MemTotal  : %lu kB\n", stat.MemTotal);
	monitor_printf(mon, "MemFree   : %lu kB\n", stat.MemFree);
	monitor_printf(mon, "Buffers   : %lu kB\n", stat.Buffers);
	monitor_printf(mon, "Cached    : %lu kB\n", stat.Cached);
	monitor_printf(mon, "Mem Usage : %.1f%%\n", stat.mem_usage);

	return;
}
