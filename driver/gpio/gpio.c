#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include "gpio.h"

static void *qamCtrlBase = NULL;

static int reg = 0x6b;
static int value = 0;
static int mode = 0;

module_param(reg, int, S_IRUGO);
module_param(value, int, S_IRUGO);
module_param(mode, int, S_IRUGO);

static void qamPwmSetValue(int arg_reg,int arg_value)
{
	writel((readl(qamCtrlBase + arg_reg) & (~0xff) )| arg_value,qamCtrlBase + arg_reg);
}

static int qamPwmGetValue(int arg_reg)
{
	return readl(qamCtrlBase + arg_reg);
}

static void qam_init(void)
{
	qamCtrlBase = ioremap(QAM_ADDRESS_BASE,SZ_4K);
}


static int __init module_qam_init(void)
{
	printk("[QAM] init . \n");
    	qam_init();
	
	if (mode == 1){
		qamPwmSetValue(reg,value);
		printk("Reg:%x = %x.\n",reg,qamPwmGetValue(reg));
	}
	else
	{
		printk("Reg:%x = %x.\n",reg,qamPwmGetValue(reg));
	}

	return 0;
}

static void __exit module_qam_exit(void)
{
	printk("[QAM]:exit.\n");
	iounmap(qamCtrlBase);
}

module_init(module_qam_init);
module_exit(module_qam_exit);

MODULE_AUTHOR("Shawn Zhang");
MODULE_DESCRIPTION("Novel CA Device Driver");
MODULE_LICENSE("GPL");
