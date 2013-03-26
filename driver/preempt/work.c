#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/workqueue.h>

static struct workqueue_struct *my_wq;
typedef struct {
	struct work_struct my_work;
	int    x;

} my_work_t;

my_work_t work,work1;

static void my_wq_function( struct work_struct *work)
{
	my_work_t *my_work = (my_work_t *)work;

	printk( "my_work.x %d\n", my_work->x );
	
	return;
}


static __init int module_init()
{
	my_wq = create_workqueue("work");

	INIT_WORK(((struct work_struct *)(&work),my_wq_function);
	work->x = 1;
	queue_work( my_wq, (struct work_struct *)&work );

	INIT_WORK( (struct work_struct *)&work1, my_wq_function );
	queue_work( my_wq, (struct work_struct *)&work1 );

}

static __exit void module_exit()
{


}

MODULE_LICENSE("GPL");
