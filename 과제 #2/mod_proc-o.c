#include <linux/slab.h>
#include <linux/module.h>       /* for module programming */
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>

struct foo_info {
        int a;
        int b;
        struct list_head list;
};

extern struct mutex foo_lock;
extern struct list_head foo_list;


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Youngil, Moon <jake9999@dreamwiz.com>");
MODULE_DESCRIPTION("A sample driver");
MODULE_LICENSE("Dual BSD/GPL");
/*--------------------------------------------------------*/
/* 1) Generate sample data                                */
/*--------------------------------------------------------*/

DEFINE_MUTEX(foo_lock);
LIST_HEAD(foo_list);

static int add_data(int a, int b)
{
        struct foo_info *info;

        printk(KERN_INFO "%s %d, %d\n", __func__, a, b);

        info = kzalloc(sizeof(*info), GFP_KERNEL);
        if (!info)
                return -ENOMEM;

        INIT_LIST_HEAD(&info->list);
        info->a = a;
        info->b = b;

        mutex_lock(&foo_lock);
        list_add(&info->list, &foo_list);
        mutex_unlock(&foo_lock);

        return 0;
}

static int add_sample_data(void)
{
        if (add_data(10, 20))
                return -ENOMEM;
        if (add_data(30, 40))
                return -ENOMEM;
        return 0;
}

static int remove_sample_data(void)
{
        struct foo_info *tmp;
        struct list_head *node, *q;

        list_for_each_safe(node, q, &foo_list){
                tmp = list_entry(node, struct foo_info, list);
                list_del(node);
                kfree(tmp);
        }

        return 0;
}

/*--------------------------------------------------------*/
/* 2) proc operations part                                */
/*--------------------------------------------------------*/

static int foo_simple_show(struct seq_file *s, void *unused)
{
        struct foo_info *info;

        list_for_each_entry(info, &foo_list, list)
                seq_printf(s, "%d + %d = %d\n", info->a, info->b, info->a + info->b);

        return 0;
}

static int foo_proc_open(struct inode *inode, struct file *file)
{
        return single_open(file, foo_simple_show, NULL);
}


static const struct file_operations foo_proc_ops = {
        .owner          = THIS_MODULE,
        .open           = foo_proc_open,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = seq_release,
};

/*--------------------------------------------------------*/
/* 3) proc interface part  (/proc/foo-dir/foo)            */
/*--------------------------------------------------------*/

#define FOO_DIR "foo-dir"
#define FOO_FILE "foo"

static struct proc_dir_entry *foo_proc_dir = NULL;
static struct proc_dir_entry *foo_proc_file = NULL;

int foo_proc_init(void)
{
        foo_proc_dir = proc_mkdir(FOO_DIR, NULL);
        if (foo_proc_dir == NULL)
        {
                printk("Unable to create /proc/%s\n", FOO_DIR);
                return -1;
        }

        foo_proc_file = proc_create(FOO_FILE, 0, foo_proc_dir, &foo_proc_ops); /* S_IRUGO */
        if (foo_proc_file == NULL)
        {
                printk("Unable to create /proc/%s/%s\n", FOO_DIR, FOO_FILE);
                remove_proc_entry(FOO_DIR, NULL);
                return -1;
        }

        printk(KERN_INFO "Created /proc/%s/%s\n", FOO_DIR, FOO_FILE);
        return 0;
}

void foo_proc_exit(void)
{
        /* remove directory and file from procfs */
        remove_proc_subtree(FOO_DIR, NULL);

        /* remove proc_dir_entry instance */
        proc_remove(foo_proc_file);
        proc_remove(foo_proc_dir);

        printk(KERN_INFO "Removed /proc/%s/%s\n", FOO_DIR, FOO_FILE);
}

/*--------------------------------------------------------*/
/* 2) Module part                                         */
/*--------------------------------------------------------*/

static int __init foo_init(void)
{
        if (add_sample_data())
        {
                printk(KERN_INFO "add_sample_data() failed.\n");
                return -ENOMEM;
        }

        return foo_proc_init();
}

static void __exit foo_exit(void)
{
        remove_sample_data();
        foo_proc_exit();

        return;
}

module_init(foo_init);
module_exit(foo_exit);
