#include <linux/kvm_host.h>
#include "irq.h"
#include "mmu.h"
#include "i8254.h"
#include "tss.h"
#include "kvm_cache_regs.h"
#include "x86.h"
#include "cpuid.h" 

#include <linux/interrupt.h>
#include <linux/kvm.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/mman.h>
#include <linux/highmem.h>
#include <linux/iommu.h>
#include <linux/intel-iommu.h>
#include <linux/cpufreq.h>
#include <linux/user-return-notifier.h>
#include <linux/srcu.h>
#include <linux/slab.h>
#include <linux/perf_event.h>
#include <linux/uaccess.h>
#include <linux/hash.h>
#include <linux/pci.h>
#include <linux/timekeeper_internal.h>
#include <linux/pvclock_gtod.h>
#include <trace/events/kvm.h>

#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/crypto.h>

#include <asm/debugreg.h>
#include <asm/msr.h>
#include <asm/desc.h>
#include <asm/mtrr.h>
#include <asm/mce.h>
#include <asm/i387.h>
#include <asm/fpu-internal.h> /* Ugh! */
#include <asm/xcr.h>
#include <asm/pvclock.h>
#include <asm/div64.h>

#include "kvm_crypto.h"

#define KTHREAD_POOL_SIZE 4      // number of kernel threads in pool
#define AES_BLOCK_SIZE 16

int global_job_id = 0;           // incremented job id
struct crypto_queue cq;          // job queue
DEFINE_MUTEX(cq_mutex);          // mutex for job queue
DECLARE_WAIT_QUEUE_HEAD(cq_wq);
DECLARE_WAIT_QUEUE_HEAD(finish_wq);
struct task_struct **ckt_ptr;    // array of kernel threads

void aes_encrypt(u8 *pdata, u8 *cdata, u8 *key, int len)
{
	struct crypto_cipher *tfm;
	u8 *ptmp, *ctmp;
	int i;

	tfm = crypto_alloc_cipher("aes", 4, CRYPTO_ALG_ASYNC);

	ptmp = pdata;
	ctmp = cdata;
	for(i=0; i<len; i+=AES_BLOCK_SIZE)
	{
		crypto_cipher_encrypt_one(tfm, ctmp, ptmp);
		ptmp += AES_BLOCK_SIZE;
		ctmp += AES_BLOCK_SIZE;
	}

	crypto_free_cipher(tfm);
}

void aes_decrypt(u8 *cdata, u8 *pdata, u8 *key, int len)
{
	struct crypto_cipher *tfm;
	u8 *ptmp, *ctmp;
	int i;

	tfm = crypto_alloc_cipher("aes", 4, CRYPTO_ALG_ASYNC);

	ptmp = pdata;
	ctmp = cdata;
	for(i=0; i<len; i+=AES_BLOCK_SIZE)
	{
		crypto_cipher_decrypt_one(tfm, ptmp, ctmp);
		ptmp += AES_BLOCK_SIZE;
		ctmp += AES_BLOCK_SIZE;
	}

	crypto_free_cipher(tfm);
}

// insert a new job into the tail of the job queue
int crypto_produce_job(struct kvm *kvm, unsigned long vjob_gpa, int vjob_len)
{
	struct vm_job *vjob_hva;
	int ret;
	int hc_ret = 0;
	struct crypto_job *j = NULL;
	char *ibuf = NULL;
	char *obuf = NULL;

	vjob_hva = (struct vm_job *)kmalloc(sizeof(struct vm_job), GFP_KERNEL);
	if(vjob_hva == NULL)
	{
		printk(KERN_ALERT "[CRYPTO] kmalloc allocation of vjob_hva failed\n");
		hc_ret = 1;
		goto out;
	}

	ret = kvm_read_guest(kvm, vjob_gpa, (void *)vjob_hva, vjob_len);
	if(ret != 0)
	{
		printk(KERN_ALERT "[CRYPTO] Failed to read vjob_gpa\n");
		hc_ret = 1;
		goto out;
	}

	ibuf = (char *)kmalloc(vjob_hva->len, GFP_KERNEL);
	obuf = (char *)kmalloc(vjob_hva->len, GFP_KERNEL);
	memset(ibuf, 0, vjob_hva->len);
	memset(obuf, 0, vjob_hva->len);

	ret = kvm_read_guest(kvm, vjob_hva->input_gpa, (void *)ibuf, vjob_hva->len);
	if(ret != 0)
	{
		printk(KERN_ALERT "[CRYPTO] Failed to read input\n");
		hc_ret = 1;
		goto out;
	}

	// alloc and init job
	j = (struct crypto_job *)kmalloc(sizeof(struct crypto_job), GFP_KERNEL);
	j->kvm         = kvm;
	j->job_id      = global_job_id>MAX_JOB_ID?0:global_job_id++;
	j->input       = ibuf;
	j->output      = obuf;
	j->len         = vjob_hva->len;
	j->operation   = vjob_hva->operation;
	j->status      = 0;
	j->next        = NULL;

	//insert new job into queue tail
	mutex_lock(&cq_mutex);

	if(cq.num == 0)  // if job queue is empty
	{
		cq.head = j;
		cq.tail = j;
	}
	else             // if job queue is not empty
	{
		cq.tail->next = j;
		cq.tail = j;
	}
	cq.num++;

	printk(KERN_ALERT "[CRYPTO] Job %d is submitted.\n", j->job_id);
	
	// notify the consumer kthread
	wake_up_interruptible(&cq_wq);

	mutex_unlock(&cq_mutex);

	// wait until the job is processed
	wait_event_interruptible(finish_wq, j->status==1);
	// copy result back to guest VM
	ret = kvm_write_guest(kvm, vjob_hva->output_gpa, (void *)obuf, vjob_hva->len);
	if(ret != 0)
	{
		printk(KERN_ALERT "[CRYPTO] Failed to write the data back\n");
		hc_ret = 1;
		goto out;
	}

out:
	if(vjob_hva)
		kfree(vjob_hva);

	if(ibuf)
		kfree(ibuf);

	if(obuf)
		kfree(obuf);

	if(j)
		kfree(j);

	return hc_ret;
}

// pick up a job from the head of job queue
int crypto_consume_job(void *data)
{
	struct crypto_job *nj;
	char *key = "justice";

	printk(KERN_ALERT "[CRYPTO] kernel thread %d is created\n", current->pid);

	do {
		// wait until job is available
		wait_event_interruptible(cq_wq, cq.num>0||cq.stop==1);
		mutex_lock(&cq_mutex);
		nj = NULL;
		if(cq.num > 0)
		{
			// get the job from queue
			nj = cq.head;
			cq.head = cq.head->next;
			nj->next = NULL;
			cq.num--;

			if(cq.num == 0)
				cq.tail = NULL;
		}
		mutex_unlock(&cq_mutex);

		// process the job
		if(nj != NULL)
		{
			printk(KERN_ALERT "[CRYPTO] Thread %d process job %d\n", current->pid, nj->job_id);
			
			if(nj->operation == KVM_ENCRYPTION)
			{
				aes_encrypt((u8 *)nj->input, (u8 *)nj->output, key, nj->len);
			}
			else if(nj->operation == KVM_DECRYPTION)
			{

				aes_decrypt((u8 *)nj->input, (u8 *)nj->output, key, nj->len);
			}
			else
			{
				printk(KERN_ALERT "[CRYPTO] Error: unkown operation\n");
			}

			// wake up the pending finish queue
			nj->status = 1;
			wake_up_interruptible(&finish_wq);

			schedule();
		}

	} while(!kthread_should_stop() && cq.stop!=1);

	return 0;
}

// init kernel thread pool and job queue
void crypto_pool_initialization(void)
{
	int i;

	printk(KERN_ALERT "[CRYPTO] init crypto_pool\n");

	// init the job queue
	cq.head = NULL;
	cq.tail = NULL;
	cq.num  = 0;
	cq.stop = 0;

	// init all kthreads
	ckt_ptr = (struct task_struct **)kmalloc(sizeof(struct task_struct)*KTHREAD_POOL_SIZE, GFP_KERNEL);
	for(i=0; i<KTHREAD_POOL_SIZE; i++)
	{
		ckt_ptr[i] = kthread_run(crypto_consume_job, NULL, "thread_pool_%d", i);
	}
}

// exit kernel thread pool and job queue
void crypto_pool_reclaim(void)
{
	int i;
	struct crypto_job *head, *tmp; 

	// locl before reclaiming the resource
	mutex_lock(&cq_mutex);	

	// free all pending jobs
	head = cq.head;
	while(head)
	{
		tmp = head;
		head = head->next;
		kfree(tmp);
	}

	cq.head = NULL;
	cq.tail = NULL;
	cq.num  = 0;

	mutex_unlock(&cq_mutex);

	cq.stop = 1;
	// kill all threads
	for(i=0; i<KTHREAD_POOL_SIZE; i++)
	{
		wake_up_interruptible(&cq_wq);
		kthread_stop(ckt_ptr[i]);
	}
	kfree(ckt_ptr);
	
	printk(KERN_ALERT "[CRYPTO] exit crypto_pool\n");	
}
