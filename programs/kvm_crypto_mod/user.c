// sample user program to interact with driver in VM

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ioctl-io.h"

void dump(char *str, char *ptr, int len)
{
	int i;

	printf("[%d] %s", getpid(), str);
	for (i = 0; i < len; i++)
	{
		printf("%02x ", (unsigned char)ptr[i]);
	}

	printf("\n");
}

int main(int argc, char **argv)
{
	int i, ret;

	for(i=0; i<60; i++)
	{
		ret = fork();
		if(ret == 0)      // in child
		{
			break;
		}
		else if(ret > 0)  // in parent
		{
			continue;
		}
		else              // error
		{
			printf("fork failed\n");
			return;
		}
	}

	printf("This is process %d.\n", getpid());

	int fd = 0;

	fd = open("/dev/kvm-crypto", O_RDWR);
	if(fd < 0)
	{
		printf("Failed to open the device\n");
		return -1;
	}

	struct job_struct *js = (struct job_struct *)malloc(sizeof(struct job_struct));
	char *input   = (char *)malloc(sizeof(char)*32);
	char *output  = (char *)malloc(sizeof(char)*32);
	char *output2 = (char *)malloc(sizeof(char)*32);
	int len = 32;

	memset(input, 0, 32);
	memset(output, 0, 32);
	memset(output2, 0, 32);
	sprintf(input, "123456781234567812345678123456");

	js->input  = input;
	js->output = output;
	js->len    = len;
	
	if(ioctl(fd, KC_IOENC, js) < 0)
	{
		printf("Failed to use ioctl\n");
		return -1;
	}

	dump("plain", input, 32);
	dump("crypt", output, 32);

	js->input  = output;
	js->output = output2;
	js->len    = len;
	
	if(ioctl(fd, KC_IODEC, js) < 0)
	{
		printf("Failed to use ioctl\n");
		return -1;
	}
	
	dump("nlain", output2, 32);


	close(fd);

	free(input);
	free(output);
	free(output2);
	free(js);

	return 0;
}
