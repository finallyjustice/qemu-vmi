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

#include <stdio.h>
#include "linux-x86_64.h"
#include "memory.h"
#include "linux/kvm.h"

// read unsigned long from gpa
u64 read_gpa_ul(void *mem, u64 gpa)
{
	unsigned long *addr = (unsigned long *)(mem+gpa);
	if(addr)
		return *addr;

	return 0;
}

// translate the gva to gpa in VM
u64 trans_addr(void *mem, u64 addr, u64 cr3)
{
	u64 pgd_gva;
	u64 pud_gva;
	u64 pmd_gva;
	u64 pte_gva;

	pgd_t *pgd_hva;
	pud_t *pud_hva;
	pmd_t *pmd_hva;
	pte_t *pte_hva;

	// Page Global Directory
	pgd_gva = pgd_offset(cr3, addr);
	//printf("pgd_gva: 0x%016lx\n", pgd_gva);
	pgd_hva = (pgd_t *)(mem+virt_to_phys(pgd_gva));
	//printf("pgd_hva: 0x%016lx\n", (unsigned long)pgd_hva);

	// Page Upper Directory
	pud_gva = pud_offset(pgd_hva, addr); 
	//printf("pud_gva: 0x%016lx\n", pud_gva);
	pud_hva = (pud_t *)(mem+virt_to_phys(pud_gva));
	//printf("pud_hva: 0x%016lx\n", (unsigned long)pud_hva->pud);

	// Page Middle Directory
	pmd_gva = pmd_offset(pud_hva, addr);
	//printf("pmd_gva: 0x%016lx\n", pmd_gva);
	pmd_hva = (pmd_t *)(mem+virt_to_phys(pmd_gva));
	//printf("pmd_hva: 0x%016lx\n", (unsigned long)pmd_hva->pmd);

	// Page Table Entry
	pte_gva = pte_offset_kernel(pmd_hva, addr);
	//printf("pte_gva: 0x%016lx\n", pte_gva);
	pte_hva = (pte_t *)(mem+virt_to_phys(pte_gva));
	//printf("pte_hva: 0x%016lx\n", (unsigned long)pte_hva->pte);

	u64 page_mask = page_level_mask(1);
	u64 phys_addr = pte_pfn(pte_hva) << PAGE_SHIFT;
	u64 offset    = addr & ~page_mask;

	//printf("Physical Address: 0x%016lx\n", (unsigned long)(phys_addr|offset));

	return (unsigned long)(phys_addr|offset);
}

// start or stop logger in kvm-kmod
struct KVMState;
extern struct KVMState *kvm_state;
int kvm_vm_ioctl(struct KVMState *s, int type, ...);
void mem_logger(int action)
{
	struct KVMState *ks = kvm_state;
	kvm_vm_ioctl(ks, KVM_MEM_LOGGER, action);
}
