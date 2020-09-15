#include <linux/slab.h>
#include <linux/module.h>       /* for module programming */
#include <linux/mutex.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

struct Queue_info {
        int count;
        int ring;
        struct list_head list;
};
static int id=0;
struct Queue_info *info;

extern struct mutex queue_lock;
extern struct list_head queue_list;


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Youngil, Moon <jake9999@dreamwiz.com>");
MODULE_DESCRIPTION("A sample driver");
MODULE_LICENSE("Dual BSD/GPL");

DEFINE_MUTEX(queue_lock);
LIST_HEAD(queue_list);

static ssize_t enqueue(struct file *inode, const char __user *gdata, size_t length, loff_t *off_what)
{
	info=kmalloc(sizeof(*info), GFP_KERNEL);

	if(!info)
	{
		printk(KERN_ERR "Memory Allocation Failed\n");
		return 0;
	}

	if(length>5) printk(KERN_ERR "Ring is full!!\n");

	else
	{
		copy_from_user(info->ring, gdata, sizeof(int));
		list_add(&info->list, &queue_list);
		info->count=id++;
		printk(KERN_INFO "%s %d %d\n", __func__, info->ring, id);
	}

	return length;
}

static ssize_t dequeue(struct file *inode, char  *gdata, size_t length, loff_t *off_what)
{
	if(list_empty(&info->list))
	{
		printk(KERN_ERR "Ring is empty!\n");
		return -1;
	}
	else
	{
		list_del(&info->list);
		info->count=id--;
	}

	return length;
}

static const struct file_operations kboard_proc_ops1 = {
        .owner          = THIS_MODULE,
        .read           = dequeue,
	.write		= enqueue,
        .llseek         = seq_lseek,
        .release        = seq_release,
};


static int count_simple_show(struct seq_file *s, void *unused)
{
        seq_printf(s, "count = %d\n", id);

        return 0;
}

static int count_proc_open(struct inode *inode, struct file *file)
{
        return single_open(file, count_simple_show, NULL);
}

static const struct file_operations kboard_proc_ops2 = {
        .owner          = THIS_MODULE,
        .open           = count_proc_open,
	.read		= seq_read,
        .llseek         = seq_lseek,
        .release        = seq_release,
};


static int dump_simple_show(struct seq_file *s, void *unused)
{
	list_for_each_entry(info, &queue_list, list) {
		printk(KERN_INFO "%02d : %d", info->count, info->ring);
	}
	return 0;
}

static int dump_proc_open(struct inode *inode, struct file *file)
{
        return single_open(file, dump_simple_show, NULL);
}


static const struct file_operations kboard_proc_ops3 = {
        .owner          = THIS_MODULE,
        .open           = dump_proc_open,
	.read		= seq_read,
        .llseek         = seq_lseek,
        .release        = seq_release,
};

static int reader_show(struct seq_file *s, void *unused)
{
	struct list_head *temp;
	struct Queue_info *p;

	temp=queue_list.prev;
	p=list_entry(temp, struct Queue_info, list);	

	printk(KERN_INFO "%02d : %d", p->count, p->ring);
	temp=temp->prev;
	return 0;
}

static int reader_proc_open(struct inode *inode, struct file *file)
{
        return single_open(file, reader_show, NULL);
}


static const struct file_operations kboard_proc_ops4 = {
        .owner          = THIS_MODULE,
        .open           = reader_proc_open,
	.read		= seq_read,
        .llseek         = seq_lseek,
        .release        = seq_release,
};



/*--------------------------------------------------------*/
/* 1) proc interface part  (/proc/Kboard/---)            */
/*--------------------------------------------------------*/

#define Kboard_DIR "kboard"

#define write_FILE "writer"
#define read_FILE "reader"
#define count_FILE "counter"
#define dump_FILE "dump"

static struct proc_dir_entry *Kboard_proc_dir = NULL;

static struct proc_dir_entry *Kboard_proc_file1 = NULL;
static struct proc_dir_entry *Kboard_proc_file2 = NULL;
static struct proc_dir_entry *Kboard_proc_file3 = NULL;
static struct proc_dir_entry *Kboard_proc_file4 = NULL;

int kboard_proc_init(void)
{
        Kboard_proc_dir = proc_mkdir(Kboard_DIR, NULL);
        if (Kboard_proc_dir == NULL)
        {
                printk("Unable to create /proc/%s\n", Kboard_DIR);
                return -1;
        }

        Kboard_proc_file1 = proc_create(write_FILE, 0, Kboard_proc_dir, &kboard_proc_ops1);
	Kboard_proc_file2 = proc_create(read_FILE, 0, Kboard_proc_dir, &kboard_proc_ops4);
	Kboard_proc_file3 = proc_create(count_FILE, 0, Kboard_proc_dir, &kboard_proc_ops2);
	Kboard_proc_file4 = proc_create(dump_FILE, 0, Kboard_proc_dir, &kboard_proc_ops3);

        if (Kboard_proc_file1 == NULL || Kboard_proc_file2 == NULL || Kboard_proc_file3 == NULL || Kboard_proc_file4 == NULL)
        {
                printk("Unable to create /proc/%s/%s\n", Kboard_DIR, write_FILE);
		printk("Unable to create /proc/%s/%s\n", Kboard_DIR, read_FILE);
		printk("Unable to create /proc/%s/%s\n", Kboard_DIR, count_FILE);
		printk("Unable to create /proc/%s/%s\n", Kboard_DIR, dump_FILE);

                remove_proc_entry(Kboard_DIR, NULL);
                return -1;
        }

        printk(KERN_INFO "Created /proc/%s/%s\n", Kboard_DIR, write_FILE);
	printk(KERN_INFO "Created /proc/%s/%s\n", Kboard_DIR, read_FILE);
	printk(KERN_INFO "Created /proc/%s/%s\n", Kboard_DIR, count_FILE);
	printk(KERN_INFO "Created /proc/%s/%s\n", Kboard_DIR, dump_FILE);

        return 0;
}

void kboard_proc_exit(void)
{
        /* remove directory and file from procfs */
        remove_proc_subtree(Kboard_DIR, NULL);

        /* remove proc_dir_entry instance */
        proc_remove(Kboard_proc_file1);
        proc_remove(Kboard_proc_file2);
        proc_remove(Kboard_proc_file3);
        proc_remove(Kboard_proc_file4);

        proc_remove(Kboard_proc_dir);

        printk(KERN_INFO "Removed /proc/%s/%s\n", Kboard_DIR, write_FILE);
        printk(KERN_INFO "Removed /proc/%s/%s\n", Kboard_DIR, read_FILE);
        printk(KERN_INFO "Removed /proc/%s/%s\n", Kboard_DIR, count_FILE);
        printk(KERN_INFO "Removed /proc/%s/%s\n", Kboard_DIR, dump_FILE);
}

/*--------------------------------------------------------*/
/* 2) Module part                                         */
/*--------------------------------------------------------*/

static int __init kboard_init(void)
{
        return kboard_proc_init();
}

static void __exit kboard_exit(void)
{
        list_empty(&queue_list);
        kboard_proc_exit();

        return;
}

module_init(kboard_init);
module_exit(kboard_exit);
