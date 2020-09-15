#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/printk.h>
#include <linux/uaccess.h>

#define MAX_CLIP (6)
int ring[MAX_CLIP];
int front=0;
int rear=0;

long do_sys_kb_enqueue(int item) {
	printk(KERN_DEBUG "HCPARK : do_sys_kb_enqueue() CALLED!! clip = %d\n", item);
	
	if(item<0)
	{
		printk(KERN_DEBUG "HCPARK : do_sys_kb_enqueue() invalid integer clip = %d\n", item);
		return -2;
	}
	else
	{
		if((rear+1)%MAX_CLIP==front)
		{
			printk(KERN_DEBUG "HCPARK : do_sys_kb_enqueue() ring is full!!\n");
			return -1;
		}

		rear=(rear+1)%MAX_CLIP;
		ring[rear]=item;
	}
	
	return 0;
}

long do_sys_kb_dequeue(int *user_buf) {
	int clip;
	printk(KERN_DEBUG "HCPARK : do_sys_kb_dequeue() CALLED!!\n");
	
	if(front==rear)
	{
		printk(KERN_DEBUG "HCPARK : do_sys_kb_dequeue() ring is empty!\n");
		return -1;
	}
	else
	{
		front=(front+1) % MAX_CLIP;
		clip=copy_to_user(user_buf, &ring[front], sizeof(ring[front]));
		if(clip!=0)
		{
			return clip;
		}
	}

	return 0;
}

SYSCALL_DEFINE1(kb_enqueue, int, item) {
	return do_sys_kb_enqueue(item);
}

SYSCALL_DEFINE1(kb_dequeue, int __user *, user_buf) {
	return do_sys_kb_dequeue(user_buf);
}
