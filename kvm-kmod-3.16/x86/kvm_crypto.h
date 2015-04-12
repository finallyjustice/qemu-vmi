#ifndef __KVM_CRYPTO__
#define __KVM_CRYPTO__

// crypto_job.operation
// vm_job.operation
enum{
	KVM_ENCRYPTION,
	KVM_DECRYPTION,
};

// job submitted by driver in VM via hypercall
struct vm_job
{
	unsigned long input_gpa;   // guest physical addree of input buffer
	unsigned long output_gpa;  // guest physical addree of output buffer
	int len;                   // lenghth of buffer
	int operation;             // KVM_ENCRYPTION or KVM_DECRYPTION
};

// job submitted to thread pool
struct crypto_job
{
	int job_id;                // global job id in kvm
	struct kvm *kvm;           // kvm submitting the job
	char *input;               // input buffer - to encrypt or decrypt
	char *output;              // output buffer
	int len;                   // lenghth of buffer
	int operation;             // KVM_ENCRYPTION or KVM_DECRYPTION
	int status;                // 0 pending, 1 finish
	struct crypto_job *next;
};

// job queue processed by thread pool
struct crypto_queue
{
	struct crypto_job *head;    // head of job queue
	struct crypto_job *tail;    // tail of job queue
	int num;                    // number of jobs in queue
	int stop;                   // 1-the thread pool will shutdown
};

#define MAX_JOB_ID 65536

void crypto_pool_initialization(void);
void crypto_pool_reclaim(void);
int crypto_produce_job(struct kvm *kvm, unsigned long vjob_gpa, int vjob_len);

#endif
