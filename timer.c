#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("/sys timer module");

static struct timer_list my_timer;

void timer_callback( unsigned long data )
{
    printk("timer_callback called\n");
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
}

static int __init timer_init(void)
{
    printk(KERN_INFO "Timer module loaded\n");

    setup_timer(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));

    return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit timer_cleanup(void)
{
    del_timer(&my_timer);
    printk(KERN_INFO "Timer module unloaded\n");
}

module_init(timer_init);
module_exit(timer_cleanup);
