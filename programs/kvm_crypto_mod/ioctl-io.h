#ifndef __IOCTL_IO_H__
#define __IOCTL_IO_H__

#include <linux/ioctl.h>

#define AES_BLOCK_SIZE 16

struct job_struct
{
	char *input;  // input address of message to process (encrypt or decrypt)
	char *output; // output address
	int len;      // length of data
};

#define KC_IO_MAGIC 0xAF

// encryption
#define KC_IOENC  _IOWR(KC_IO_MAGIC, 0xE1, struct job_struct)
// decryption
#define KC_IODEC  _IOWR(KC_IO_MAGIC, 0xE2, struct job_struct)

int submit_job(struct job_struct *js, int operation);


#endif
