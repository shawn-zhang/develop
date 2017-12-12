#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <asm/io.h>
//#include <moduleparam.h>
#include <linux/sched.h>


#include "init.h"


static ssize_t test_d_state_show(struct device *dev,
        struct device_attribute *mattr,
        char *data)
{
        struct task_struct *task = current;
        __set_task_state(task, TASK_UNINTERRUPTIBLE);

        return 0;
}

static DEVICE_ATTR(test_d_state, S_IRUGO, test_d_state_show, NULL);
static int test_d_probe(struct platform_device *pdev)
{
    device_create_file(&(pdev->dev), &test_d_state);

}

static struct platform_device test_d_device = {
    .name = "test_d",
};

static struct platform_driver test_d_driver = {
    .probe = test_d_probe,
    .driver = {
        .name = "test_d",
        .owner = THIS_MODULE,
    },
};

static int __init module_qam_init(void)
{
    platform_device_register(&test_d_device);
    platform_driver_register(&test_d_driver);

	return 0;
}

static void __exit module_qam_exit(void)
{
}

module_init(module_qam_init);
module_exit(module_qam_exit);

MODULE_AUTHOR("Shawn Zhang");
MODULE_DESCRIPTION("Novel rf Device Driver");
MODULE_LICENSE("GPL");
