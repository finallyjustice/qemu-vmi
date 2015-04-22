#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/slab.h>

#include <linux/delay.h>

#include <asm/current.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>

#include "ioctl-io.h"
#include "kvm-crypto.h"

// print ciphertext or plaintext message for debugging
void crypto_dump(char *str, char *ptr, int len)
{
	int i;

	printk("%s", str);
	for (i = 0; i < len; i++)
	{
		printk("%02x ", (unsigned char)ptr[i]);
	}
	
	printk("\n");
}

// submit job to hypervisor
// vmcall is the instruction for hypercall
int submit_job(struct job_struct *js, int operation)
{
	struct vm_job *vjob_gva;
	unsigned long vjob_gpa;
	int vjob_len;

	char *input_gva;
	char *output_gva;

	int ret;

	// allocate all memory
	vjob_gva   = (struct vm_job *)kmalloc(sizeof(struct vm_job), GFP_KERNEL);
	input_gva  = (char *)kmalloc(js->len, GFP_KERNEL);
	output_gva = (char *)kmalloc(js->len, GFP_KERNEL);
	memset(input_gva, 0, js->len);
	memset(output_gva, 0, js->len);

	// copy the memory
	copy_from_user(input_gva, js->input, js->len);

	vjob_gva->input_gpa  = __pa(input_gva);
	vjob_gva->output_gpa = __pa(output_gva);
	vjob_gva->len        = js->len;
	vjob_gva->operation  = operation;

	vjob_gpa = __pa(vjob_gva);
	vjob_len = sizeof(struct vm_job);

	__asm__ volatile("mov $15, %%rax\n\t"
			         "mov %1, %%rbx\n\t"
					 "mov %2, %%rcx\n\t"
					 "vmcall\n\t"
					 "mov %%rax, %0"
					 : "=m"(ret)
					 : "m"(vjob_gpa), "m"(vjob_len)
					 : "%rax", "%rbx", "%rcx");

	copy_to_user(js->output, output_gva, js->len);

	// free all memory
	kfree(input_gva);
	kfree(output_gva);
	kfree(vjob_gva);

	return 0;
}
