#include <stdio.h>
#include <stdlib.h>
#include "kboard.h"

int main()
{
	int *clip, i;
	clip=malloc(sizeof(int)*5);
	i=kboard_paste(clip);

	if(i==0)
		printf("Paste Success : %d\n", *clip);
	else
		printf("Fail: KBoard is empty\n");

	return 0;
}
