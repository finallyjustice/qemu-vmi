/*
 * guest VM (Linux) kernel symbol file
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

#ifndef __SYMBOL_X86_64_H__
#define __SYMBOL_X86_64_H__

// information for task_struct
#define INIT_TASK_GVA 0xffffffff81e10440
#define TASK_PID_OFFSET 676
#define TASK_COMM_OFFSET 1096
#define TASK_TASKS_OFFSET 560
#define TASK_MM_OFFSET 616

// information for mm_struct
#define MM_PGD_OFFSET 80

// information for guest OS memory mapping
#define __START_KERNEL_map 0xffffffff80000000
#define PAGE_OFFSET 0xffff880000000000

// information for guest OS system call table
// search "SYS_CALL_TABLE" in System.map
#define SYSTEM_CALL_GVA 0xffffffff81a001a0

// information for CPU utilization
#define CPU_POSSIBLE_MASK_GVA 0xffffffff81ebc658
#define __PER_CPU_OFFSET_GVA 0xffffffff81ebbfa0
#define KERNEL_CPUSTAT 0x000000000000d340

enum cpu_usage_stat {
	CPUTIME_USER,
	CPUTIME_NICE,
	CPUTIME_SYSTEM,
	CPUTIME_SOFTIRQ,
	CPUTIME_IRQ,
	CPUTIME_IDLE,
	CPUTIME_IOWAIT,
	CPUTIME_STEAL,
	CPUTIME_TOTAL,
	NR_STATS,
};

// information for memory utilization
#define PAGE_KB 4
#define ALL_BDEVS_GVA 0xffffffff81e3f540
#define TOTALRAM_PAGES_GVA 0xffffffff81ebdd68
#define VM_STAT_GVA 0xffffffff81e04200
#define SWAPPER_SPACES_GVA 0xffffffff81e3b440
#define ADDRESS_SPACE_SIZE 160 
#define ADDRESS_SPACE_NRPAGES_OFFSET 88
#define MAX_SWAPFILES 30
#define BLOCK_DEVICE_BD_LIST_OFFSET 160
#define BLOCK_DEVICE_BD_INODE_OFFSET 8
#define INODE_I_MAPPING_OFFSET 48

// from include/linux/mmzone.h of Linux 3.9 code
enum zone_stat_item {
	/* First 128 byte cacheline (assuming 64 bit words) */
	NR_FREE_PAGES,
	NR_LRU_BASE,
	NR_INACTIVE_ANON = NR_LRU_BASE, /* must match order of LRU_[IN]ACTIVE */
	NR_ACTIVE_ANON,		/*  "     "     "   "       "         */
	NR_INACTIVE_FILE,	/*  "     "     "   "       "         */
	NR_ACTIVE_FILE,		/*  "     "     "   "       "         */
	NR_UNEVICTABLE,		/*  "     "     "   "       "         */
	NR_MLOCK,		/* mlock()ed pages found and moved off LRU */
	NR_ANON_PAGES,	/* Mapped anonymous pages */
	NR_FILE_MAPPED,	/* pagecache pages mapped into pagetables.
			   only modified from process context */
	NR_FILE_PAGES,
	NR_FILE_DIRTY,
	NR_WRITEBACK,
	NR_SLAB_RECLAIMABLE,
	NR_SLAB_UNRECLAIMABLE,
	NR_PAGETABLE,		/* used for pagetables */
	NR_KERNEL_STACK,
	/* Second 128 byte cacheline */
	NR_UNSTABLE_NFS,	/* NFS unstable pages */
	NR_BOUNCE,
	NR_VMSCAN_WRITE,
	NR_VMSCAN_IMMEDIATE,	/* Prioritise for reclaim when writeback ends */
	NR_WRITEBACK_TEMP,	/* Writeback using temporary buffers */
	NR_ISOLATED_ANON,	/* Temporary isolated pages from anon lru */
	NR_ISOLATED_FILE,	/* Temporary isolated pages from file lru */
	NR_SHMEM,		/* shmem pages (included tmpfs/GEM pages) */
	NR_DIRTIED,		/* page dirtyings since bootup */
	NR_WRITTEN,		/* page writings since bootup */
#ifdef CONFIG_NUMA
	NUMA_HIT,		/* allocated in intended node */
	NUMA_MISS,		/* allocated in non intended node */
	NUMA_FOREIGN,		/* was intended here, hit elsewhere */
	NUMA_INTERLEAVE_HIT,	/* interleaver preferred this zone */
	NUMA_LOCAL,		/* allocation from local node */
	NUMA_OTHER,		/* allocation from other node */
#endif
	NR_ANON_TRANSPARENT_HUGEPAGES,
	NR_FREE_CMA_PAGES,
	NR_VM_ZONE_STAT_ITEMS 
};

// information for module
#define MODULES_HEAD 0x0

struct list_head {
	struct list_head *next, *prev;
};

#endif
