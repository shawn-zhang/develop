#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <asm/io.h>
//#include <moduleparam.h>

#include "init.h"

static void *qamCtrlBase = NULL;

static int reg = 0x6b;
static int value = 0;
static int mode = 0;


module_param(reg, int, S_IRUGO);
module_param(value, int, S_IRUGO);

module_param(mode, int, S_IRUGO);

static int m_N=100;

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

static double m_coef(int n)
{
    double t;

    if(n == 0)
        return 0;
    t = 1.0 / n;
    if( n % 2 == 0)
        t = -t;
    return t;
}//x^n的系数

static double m_horner(double x)
{
    double u;
    int i;
    
    u = m_coef(m_N);
    for(i = m_N - 1; i>=0; i--)
        u = u * x + m_coef(i);
    return u;
}
static double m_sqrt(double b)
{
    double x=1;
    int step=0;
    
    while((x*x-b<-0.000000000000001||x*x-b>0.000000000000001)&&step<50)
    {
        x=(b/x+x)/2.0;
        step++;
    }
    return x;
}//开平方

static double m_ln(double x)//ln(1+x)=x-x^2/2+x^3/3-x^4/4……
{
    int i, j;
    double dret = 0.00;
    
    if(x > 1.5)
    {
        for(i = 0; x > 1.25; i++)
            x = m_sqrt( x );
        j = 1 << i;
        dret = (1 << i) * m_horner( x - 1 );
    }
    else if( x < 0.7 && x > 0)
    {
        for(i = 0; x < 0.7; i++)
            x = m_sqrt(x);\
        j = 1 << i;
        dret = ( 1 << i ) * m_horner( x - 1);
    }
    else if(x > 0){
        j = 1;
    }
    
    dret = j *  m_horner( x - 1 );
    
    return dret;
    //return m_horner(x - 1);
}


static int __init module_qam_init(void)
{
    int x = 0;
    double testdi;
    
    testdi = 4.890;
    testdi = m_coef(x);

    x = (int)testdi;
    
    m_ln(testdi);

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
MODULE_DESCRIPTION("Novel rf Device Driver");
MODULE_LICENSE("GPL");
