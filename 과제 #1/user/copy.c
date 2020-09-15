#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include "kboard.h"

int main(int argc, char **argv)
{
	int clip, i;

	clip=atoi(argv[1]);
	i=kboard_copy(clip);

	if(i==0)
		printf("Copy success : %d\n", clip);
	else
		printf("Fail: KBoard is full or invalid clip\n");

	return 0;
}
