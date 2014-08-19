/*
 * Memory information
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

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "lib.h"
#include "linux-x86_64.h"
#include "types.h"

typedef unsigned long pteval_t;
typedef unsigned long pmdval_t;
typedef unsigned long pudval_t;
typedef unsigned long pgdval_t;
typedef unsigned long pgprotval_t;

typedef struct { pgdval_t pgd; } pgd_t;
typedef struct { pudval_t pud; } pud_t;
typedef struct { pmdval_t pmd; } pmd_t;
typedef struct { pteval_t pte; } pte_t;

/*
 * PGDIR_SHIFT determines what a top-level page table entry can map
 */
#define PGDIR_SHIFT 39
#define PTRS_PER_PGD 512

/*
 * 3rd level page
 */
#define PUD_SHIFT 30
#define PTRS_PER_PUD 512

/*
 * PMD_SHIFT determines the size of the area a middle-level
 * page table can map
 */
#define PMD_SHIFT 21
#define PTRS_PER_PMD 512
		   
/*
 * entries per page directory level
 */
#define PTRS_PER_PTE 512

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT  12

/*  
 *  masks: Linux x86_64 support 46-bit in physical address
 */
#define PTE_PFN_MASK 0x00003ffffffff000

/*
 * the pgd page can be thought of an array like this: pgd_t[PTRS_PER_PGD]
 * 
 * this macro returns the index of the entry in the pgd page which would
 * control the given virtual address
 */
#define pgd_index(address) (((address) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))

/*
 * pgd_offset() returns a (pgd_t *)
 * pgd_index() is used get the offset into the pgd page's array of pgd_t's;
 */
#define pgd_offset(cr3, address) (cr3 + sizeof(unsigned long)*pgd_index((address)))

static inline unsigned long pgd_page_vaddr(pgd_t *pgd)
{
	return __va((unsigned long)(pgd->pgd) & PTE_PFN_MASK);
}

/* to find an entry in a page-table-directory. */
static inline unsigned long pud_index(unsigned long address)
{
	return (address >> PUD_SHIFT) & (PTRS_PER_PUD - 1);
}

static inline unsigned long pud_offset(pgd_t *pgd, unsigned long address)
{
	return pgd_page_vaddr(pgd) + pud_index(address)*sizeof(unsigned long);	
}

static inline unsigned long pud_page_vaddr(pud_t *pud)
{
	return __va((unsigned long)(pud->pud) & PTE_PFN_MASK);
}

/*
 * the pmd page can be thought of an array like this: pmd_t[PTRS_PER_PMD]
 * 
 * this macro returns the index of the entry in the pmd page which would
 * control the given virtual address
 */
static inline unsigned long pmd_index(unsigned long address)
{
	return (address >> PMD_SHIFT) & (PTRS_PER_PMD - 1);
}

/* Find an entry in the second-level page table.. */
static inline unsigned long pmd_offset(pud_t *pud, unsigned long address)
{
	return pud_page_vaddr(pud) + pmd_index(address)*sizeof(unsigned long);
}

static inline unsigned long pmd_page_vaddr(pmd_t *pmd)
{
	return __va((unsigned long)(pmd->pmd) & PTE_PFN_MASK);
}

/*
 * the pte page can be thought of an array like this: pte_t[PTRS_PER_PTE]
 *
 * this function returns the index of the entry in the pte page which would
 * control the given virtual address
 */
static inline unsigned long pte_index(unsigned long address)
{
	return (address >> PAGE_SHIFT) & (PTRS_PER_PTE - 1);
}

static inline unsigned long pte_offset_kernel(pmd_t *pmd, unsigned long address)
{
	return pmd_page_vaddr(pmd) + pte_index(address)*sizeof(unsigned long);
}

enum pg_level {
	PG_LEVEL_NONE,
	PG_LEVEL_4K,
	PG_LEVEL_2M,
	PG_LEVEL_1G,
	PG_LEVEL_NUM
};

#define PTE_SHIFT ilog2(PTRS_PER_PTE)
static inline int page_level_shift(enum pg_level level)
{
	return (PAGE_SHIFT - PTE_SHIFT) + level * PTE_SHIFT;
}
static inline unsigned long page_level_size(enum pg_level level)
{     
	return 1UL << page_level_shift(level);
}
static inline unsigned long page_level_mask(enum pg_level level)
{
	return ~(page_level_size(level) - 1);
}

static inline unsigned long pte_pfn(pte_t *pte)
{
	return (((unsigned long)pte->pte) & PTE_PFN_MASK) >> PAGE_SHIFT;
}

// read unsigned long from gpa
u64 read_gpa_ul(void *mem, u64 gpa);
// translate the gva to gpa in VM
u64 trans_addr(void *mem, u64 addr, u64 cr3);

// start or stop logger in kvm-kmod
void mem_logger(int action);

#endif
