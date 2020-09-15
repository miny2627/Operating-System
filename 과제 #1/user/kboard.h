#define _GNU_SOURCE
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>

long kboard_copy(int clip);
int kboard_paste(int *clip);

long kboard_copy(int clip)
{
	int i;
	i=syscall(335, clip);

	return i;
}

int kboard_paste(int *clip)
{
	int i;
	i=syscall(336,clip);
	
	return i;
}
	
