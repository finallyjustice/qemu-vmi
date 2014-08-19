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
#define INIT_TASK_GVA 0xffffffff81c14440
#define TASK_PID_OFFSET 732
#define TASK_COMM_OFFSET 1184
#define TASK_TASKS_OFFSET 616
#define TASK_MM_OFFSET 672
#define TASK_NSPROXY_OFFSET 1424

// information for nsproxy
#define NSPROXY_UTS_NS_OFFSET 8
#define UTS_NAMESPACE_NAME 4
#define __NEW_UTS_LEN_JUMP 65

// information for mm_struct
#define MM_PGD_OFFSET 64

// information for guest OS memory mapping
#define __START_KERNEL_map 0xffffffff80000000
#define PAGE_OFFSET 0xffff880000000000

// information for guest OS system call table
// search "SYS_CALL_TABLE" in System.map
#define SYSTEM_CALL_GVA 0xffffffff81801420

// information for CPU utilization
#define CPU_POSSIBLE_MASK_GVA 0xffffffff81d03dc0
#define __PER_CPU_OFFSET_GVA 0xffffffff81d030a0
#define KERNEL_CPUSTAT 0x000000000000e340

enum cpu_stat {
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
#define ALL_BDEVS_GVA 0xffffffff81c6e520
#define TOTALRAM_PAGES_GVA 0xffffffff81d06228
#define VM_STAT_GVA 0xffffffff81c06200
#define SWAPPER_SPACES_GVA 0xffffffff81c5cc60
#define ADDRESS_SPACE_SIZE 168 
#define ADDRESS_SPACE_NRPAGES_OFFSET 96
#define MAX_SWAPFILES 29
#define BLOCK_DEVICE_BD_LIST_OFFSET 168
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
#define MODULES_HEAD_GVA 0xffffffff81c4e130
#define MODULE_LIST_OFFSET 8
#define MODULE_NAME_OFFSET 24
#define MODULE_MODULE_CORE_OFFSET 352

// information for network device
#define DEV_BASE_HEAD_GVA 0xffffffff81cd2008
#define ND_DEVICE_LIST_OFFSET 80
#define ND_NAME_OFFSET 0
#define ND_DEV_ADDR_OFFSET 672
#define ND_IP_PTR_OFFSET 624
#define IND_IFA_LIST_OFFSET 16
#define INA_IFA_ADDRESS_OFFSET 52
#define INA_IFA_BROADCAST_OFFSET 60
#define INA_IFA_MASK_OFFSET 56

#endif
