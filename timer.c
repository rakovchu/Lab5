#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/timer.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("/sys timer module");

static struct timer_list my_timer;
static int my_timer_interval = 2000;

void timer_callback( unsigned long data )
{
    printk("timer_callback called\n");
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(my_timer_interval));        //restart timer
}

static ssize_t timer_write_callback(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
    char read_buf[128];
    int read_buf_size = 128;

    if (count < read_buf_size)
        read_buf_size = count;
    if (copy_from_user(read_buf, buf, read_buf_size))
        return -EINVAL;

    if (sscanf(buf, "%d", &my_timer_interval) == 1) {
        my_timer_interval *= 1000;
        mod_timer(&my_timer, jiffies + msecs_to_jiffies(my_timer_interval));    // activate timer
    } else {
        printk("Wrong input format: %s\n", buf);
    }

    return read_buf_size;
}

static const struct file_operations timer_fops = {
    .owner = THIS_MODULE,
    .write = timer_write_callback,
};

static struct miscdevice timer_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "my_timer",
    &timer_fops,
};

static int __init timer_init(void)
{
    misc_register(&timer_dev);
    setup_timer(&my_timer, timer_callback, 0);
    printk(KERN_INFO "Timer module loaded\n");
    return 0;
}

static void __exit timer_cleanup(void)
{
    misc_deregister(&timer_dev);
    del_timer(&my_timer);
    printk(KERN_INFO "Timer module unloaded\n");
}

module_init(timer_init);
module_exit(timer_cleanup);
