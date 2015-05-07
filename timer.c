#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/timer.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("/sys timer module");

struct interval_attr {
    struct attribute attr;
    int value;
};
#define parent_interval_attr(x) container_of(x, struct interval_attr, attr)

static struct timer_list my_timer;

static struct interval_attr timer_interval = {
    .attr.name="interval",
    .attr.mode = 0666,
    .value = 2000,
};

static struct attribute *attr_array[] = {
    &timer_interval.attr,
    NULL
};

static ssize_t interval_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
    struct interval_attr *a = parent_interval_attr(attr);
    return scnprintf(buf, PAGE_SIZE, "%d\n", a->value / 1000);
}

static ssize_t interval_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t len)
{
    struct interval_attr *a = parent_interval_attr(attr);
    sscanf(buf, "%d", &a->value);
    a->value *= 1000;
    return len;
}

static struct sysfs_ops interval_attr_ops = {
    .show = interval_show,
    .store = interval_store,
};

static struct kobj_type timer_kobj_type = {
    .sysfs_ops = &interval_attr_ops,
    .default_attrs = attr_array,
};

void timer_callback( unsigned long data )
{
    printk("timer_callback called\n");
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_interval.value));        //restart timer
}

struct kobject *timer_kobj;
static int __init timer_init(void)
{
    setup_timer(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_interval.value));
    //kobject_init(&timer_kobj);
    timer_kobj = kzalloc(sizeof(*timer_kobj), GFP_KERNEL);
    if (timer_kobj) {
        kobject_init(timer_kobj, &timer_kobj_type);
        if (kobject_add(timer_kobj, NULL, "%s", "timer")) {
             printk("Sysfs creation failed\n");
             kobject_put(timer_kobj);
             timer_kobj = NULL;
             return -1;
        }
    }

    printk(KERN_INFO "Timer module loaded\n");
    return 0;
}

static void __exit timer_cleanup(void)
{
    del_timer(&my_timer);
    if (timer_kobj) {
        kobject_put(timer_kobj);
        kfree(timer_kobj);
    }
    printk(KERN_INFO "Timer module unloaded\n");
}

module_init(timer_init);
module_exit(timer_cleanup);

// http://lwn.net/Articles/54651/
