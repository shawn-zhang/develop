#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>


static struct tasklet_struct tasklet;
static struct cdev chardev;
struct timer_list timer;

static struct file_operations fops = {
	.open = copen,
	.release = cclose,
};

static void timer_handler(unsigned long data)
{
	printk("[%s:%d]\n",__func__,__LINE__);
}

static void tasklet_handle(unsigned long data)
{
	printk("[%s:%d]\n",__func__,__LINE__);
}

static void work_handle(struct work_struct *work)
{
	printk("[%s:%d]\n",__func__,__LINE__);
}


static __init int preempt_module_init()
{
	struct task_struct *get;		//进程信息 get =  current
	struct thread_info *tinfo;	//内核线程信息
	unsigned long data = 0;		//tasklet data
	

	
	printk("HZ = %d\n", HZ);	//  HZ = 1s;HZ数就是一秒
	printk("jiffies = %d\n", jiffies); //jiffies = 开机到现在的HZ数

	int min = jiffies / (60 * HZ);
	int sec = (jiffies % (60 * HZ)) / HZ;

	memset(&chardev,0,sizeof(chardev));
	memset(&fops,0,sizeof(fops));
	memset(&tasklet,0,sizeof(tasklet));

	cdev_init(&chardev,&fops);
	cdev_add(&chardev,MKDEV(99,0),1);
	
	/*thread_info*/
	tinfo = current_thread_info();
	get = tinfo->task;
	//get->pid,get->comm
	
	//lock的形式
	tinfo->preempt_count = 1;	//当期内核线程枷锁  或者 preempt_disable();
	//....				//处理逻辑
	tinfo->preempt_count = 0;	//解锁	或者		preempt_enable();
	
	spin_lock_t lock;

	spin_lock_init(&lock);
	spin_lock(&lock);
	spin_unlock(&lock);
	
	spin_lock_irq();		// = spin_lock() + local_irq_disable();
	spin_unlock_irq();		// = spin_unlock() + local_irq_enable();
	
	/*禁止中断*/
	local_irq_disable();
	local_irq_enable();

	local_irq_save(flag);
	local_irq_restore(flag);

	/*tasklet*/
	tasklet_init(&tasklet,tasklet_handle,data);
	
	tasklet_schedule(&tasklet);		//一般在中断中执行该语句
	
	/**timer*/
	init_timer(&timer);
	mytimer.expires = jiffies + 1 * HZ;	//一秒后执行
	mytimer.function = timer_handler;
	mytimer.data = 456;

	add_timer(&timer);

	/*schedule */
	set_current_state(TASK_INTERRUPTIBLE);	//current->state = TASK_INTERRUPTIBLE;
	schedule_timeout( 3 * HZ);		//延迟3秒
	
	if (timer_before(jiffies, jiffies + 3 * HZ)){
		schedule();			//让出CPU，给内核其他线程使用
	}
	
	/*工作队列*/
	struct work_struct mywork;
	INIT_WORK(&mywork, work_handler);
	
	schedule_work(&mywork);
	//schedule_delayed_work(&mywork,jiffies + 1 * HZ);
	
	/*信号量的使用*/
	struct semaphore sema;

	sema_init(&sema,1);

	down(&sema);		//相当与加锁
	while(1){
		...;
		break;
	}
	up(&sema);
	down_interruptible(&sema);
	down_trylock(&sema);

	/*完成量*/
	struct completion done;
	init_completion(&done);
	
	wait_for_completion(&done);
	wait_for_completion_interruptible(&done);
	completion(&done);	//如果有多个等待的，唤醒其中一个
	completion_all(&done);	//唤醒所有等待完成量

	//原子操作
	atomic_t a;
	int tmp;

	atomic_set(&a, 100); //a = 100;
	atomic_add(5, &a);//a += 5;
	atomic_sub(10, &a);//a -= 10;
	tmp = atomic_add_return(5, &a); // tmp = (a += 5); 
	tmp = atomic_sub_return(10, &a);// tmp = (a -= 10);
	atomic_inc(&a); //a += 1;
	atomic_dec(&a); //a -= 1;

	//等待队列
	wait_queue_head_t wqh;
	int flag = 0;
	
	init_wait_queue_head(&wqh);
	wait_event(&wqh, flag == 1);
	
	flag = 1;
	wake_up(&wqh);
	
	/*内存分配*/
	char *p = NULL;

	p = kzalloc(SZ_4K,GFP_KERNEL);	//GFP_KERNEL:内核内存的正常分配. 可能睡眠. //GFP_ATOMIC:用来从中断处理和进程上下文之外的其他代码中分配内存. 从不睡眠.
	//kmalloc能够处理的最小分配时32和64字节，同时不能分配大于128K的内存分配
	kfree(p);

	p = vmalloc(512 * 1024);	//不连续的物理内存，kmalloc是连续的物理内存
	vfree(p);
	
	
	
	/*kemem*/
	kmem_cache_t *testc_cache;
	struct file_operations *fp;

	ctest_cache = kmem_cache_create("shawn", sizeof(file_operations),SLAB_HWCACHE_ALIGN,0, NULL);//最后一个参数ctor（void *） 为结构体初始化函数
	fp = kmem_cache_alloc(ctest_cache,GFP_KERNEL);
	fp.open = ...;
	fp.release = ...;
	...;
	kmem_cache_free(ctest_cache, fp);
	kmem_cache_destroy(ctest_cache);
	
	/*mempool*/
	mempool_t *pool;

	pool = mempool_create(10,mempool_alloc_slab, mempool_free_slab,testc_cache);
	fp = mempool_alloc(pool, GFP_KERNEL);
	mempool_free(fp,pool);
	mempool_destroy(pool);

	
	/*alloc_page*/
	struct page *p;
	char *buf = NULL;

	p = alloc_page(GFP_KERNEL);
	buf = page_address(p);

	__free_page(p);
	
	p = alloc_pages(GFP_KERNEL,1);
	buf = page_address(p);
	__free_pages(p,1);

	buf = __get_free_pages(GFP_KERNEL, 1);
	free_pages(buf,1);
	
	/*cdev 真正用法*/
	struct class *test_class;
	int gmajor	 = 0;

	test_class = class_create(THIS_MODULE, "classcdev");
	gmajor = register_chrdev(0, "cdev", &fps);
	device_create(test_class,NULL,MKDEV(gmajor,0),NULL,"testcdev");
	//这样创建的设备可以在/dev下面显示



	printk("[%s:%d]\n",__func__,__LINE__);
}

static __exit void module_release()
{

	printk("[%s:%d]\n",__func__,__LINE__);

}

module_init(preempt_module_init);
module_exit(module_release);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shawn Zhang");
