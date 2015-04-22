#ifndef __KVM_CRYPTO_H__
#define __KVM_CRYPTO_H__

enum {
	KVM_ENCRYPTION,
	KVM_DECRYPTION,
};

struct vm_job
{
	unsigned long input_gpa;
	unsigned long output_gpa;
	int len;
	int operation; // encryption or decryption
};

#endif
