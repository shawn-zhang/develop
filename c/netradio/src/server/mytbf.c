#include <sys/types.h>
#include <sys/time.h>

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "mytbf.h"

struct tbf_context_st {
	int cps;
	int burst;
	unsigned int token;
	int pos;
	pthread_mutex_t mut;
	pthread_cond_t cond;
};

static struct tbf_context_st *job[TBFMAX];

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t extern_cond = PTHREAD_COND_INITIALIZER;
static int module_inited=0;
static struct sigaction oldact;
static struct itimerval oitimer;

static int get_free_pos(void)
{
	int i;

	for (i=0;i<TBFMAX;++i) {
		if (job[i]==NULL) {
			return i;
		}
	}
	return -1;
}

static int min(int a, int b)
{
	if (a<b) {
		return a;
	}
	return b;
}

static void alrm_handler(int s,siginfo_t *info,void *p)
{
	int i;


	pthread_cond_broadcast(&extern_cond);
	for (i = 0; i < TBFMAX; ++i) {
		if (job[i] != NULL) {
			pthread_mutex_lock(&job[i]->mut);
			job[i]->token += job[i]->cps;
			if (job[i]->token > job[i]->burst) {
				job[i]->token = job[i]->burst;
			}
			pthread_cond_broadcast(&job[i]->cond);
			pthread_mutex_unlock(&job[i]->mut);
		}
	}
}

static void module_unload(void)
{
	int i;
	struct itimerval itimer;
	
	itimer.it_interval.tv_sec = itimer.it_value.tv_sec = 0;
	itimer.it_interval.tv_usec = itimer.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL,&itimer,NULL);
	setitimer(ITIMER_REAL,&oitimer,NULL);

	sigaction(SIGALRM,&oldact,NULL);
	for (i = 0; i < TBFMAX; ++i) {
		if (job[i] != NULL) {
			free(job[i]);
		}
	}
}

static void module_load(void)
{
	int i;
	struct sigaction sa;
	struct itimerval itimer;

	for (i=0;i<TBFMAX;++i) {
		job[i]=NULL;
	}
	itimer.it_interval.tv_sec = itimer.it_value.tv_sec = 1;
	itimer.it_interval.tv_usec = itimer.it_value.tv_usec = 0;

	sigemptyset(&(sa.sa_mask));
	sigaddset(&(sa.sa_mask),SIGALRM);
	sa.sa_sigaction = alrm_handler;
	sa.sa_flags =  SA_SIGINFO;
	sigaction(SIGALRM,&sa,&oldact);

	setitimer(ITIMER_REAL,&itimer,&oitimer);

	atexit(module_unload);
}

tbf_context_t *tbf_ctx_new(int cps, int burst)
{
	struct tbf_context_st *tmp = NULL;
	int pos;

	if (!module_inited) {
		module_load();
		module_inited = 1;
	}
	tmp = malloc(sizeof(struct tbf_context_st));
	if (tmp == NULL) {
		perror("malloc()");
		return NULL;
	}

	tmp->cps = cps;
	tmp->burst = burst;
	tmp->token = 0;
	pthread_mutex_init(&tmp->mut,NULL);
	pthread_cond_init(&tmp->cond,NULL);

	pthread_mutex_lock(&mut);
	pos = get_free_pos();
	if (pos < 0) {
		free(tmp);
		return NULL;
	}

	tmp->pos = pos;
	job[pos] = tmp; /* 绝大多数平台上，指针赋值是一条指令 */
	pthread_mutex_unlock(&mut);
	
	return tmp;
}

int tbf_ctx_delete(tbf_context_t *p)
{
	struct tbf_context_st *ptr = p;
	
	pthread_mutex_lock(&mut);
	job[ptr->pos]=NULL;
	pthread_mutex_unlock(&mut);
	if (!pthread_mutex_trylock(&ptr->mut)){
		pthread_mutex_unlock(&ptr->mut);
	}
	pthread_mutex_unlock(&ptr->mut);
	free(p);
	
	return 0;
}

ssize_t tbf_write(int fd, const char *bufp, size_t size, tbf_context_t *p)
{
	int ret;
	struct tbf_context_st *ctx = p;

	ret = write(fd, bufp, min(size, ctx->token));
	if (ret<0) {
		return -1;
	}
	/* BLOCK SIGALRM */
	ctx->token -= ret;
	/* UNBLOCK SIGALRM */
	return ret;
}

ssize_t tbf_read(int fd,char *bufp,size_t size,tbf_context_t *p)
{
	int ret;
	struct tbf_context_st *ctx = p;

	pthread_mutex_lock(&ctx->mut);
	while(ctx->token <= 0){
		pthread_cond_wait(&ctx->cond,&ctx->mut);
	}
	pthread_mutex_unlock(&ctx->mut);
	while(1){
		ret = read(fd,bufp,min(size,ctx->token));
		if (ret < 0){
			if (errno == EINTR)
				continue;
			pthread_mutex_unlock(&ctx->mut);
			return -1;
		}
		break;
	}
	pthread_mutex_lock(&ctx->mut);
	ctx->token -= ret;
	pthread_mutex_unlock(&ctx->mut);

	return ret;
}
