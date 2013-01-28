#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>

#include <stdio.h>
#include <pthread.h>
#include <comm.h>

#include "conf.h"
#include "thr_hoster.h"
#include "mytbf.h"

struct __arg{
	struct msg_chn_entry_st *chn_entry_list;
	int chn_nr;
};

int sd;
pthread_t tid;
struct msg_chn_list_st *chn_list;

static int create_socket(void)
{
	struct ip_mreqn mreq;
	
	sd = socket(AF_INET,SOCK_DGRAM,0);
	
	inet_pton(AF_INET,global_conf.mgroup,&mreq.imr_multiaddr);
	inet_pton(AF_INET,"0.0.0.0",&mreq.imr_address);
	mreq.imr_ifindex = if_nametoindex(global_conf.eth);
	if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0){
		perror("setsockopt() multicast if");
		return -1;
	}
	
	return sd;
}

static void thr_exit(void *usearg)
{
	close(sd);
	sd = -1;
	free(chn_list);
	free(usearg);
}

static void *thr_func(void *p)
{
	struct __arg *arg  = p;
	struct msg_chn_entry_st *chn_entry_list = NULL;
	int i;
	int ret;
	sigset_t set;
	struct sockaddr_in peer_addr;
	int list_size = 0,list_len;

	peer_addr.sin_family = AF_INET;
	peer_addr.sin_port = htons(atoi(global_conf.port));
	inet_pton(AF_INET,global_conf.mgroup,&peer_addr.sin_addr);

	pthread_cleanup_push(thr_exit,arg);
	chn_entry_list = arg->chn_entry_list;
	for(i = 0; i < arg->chn_nr; i++){
		list_size += ntohs(chn_entry_list->len) + ENTRY_SIZE;
		chn_entry_list =(struct msg_chn_entry_st *)((char *)chn_entry_list + ntohs(chn_entry_list->len) + ENTRY_SIZE);
	}
	list_len = sizeof(*chn_list) - sizeof(struct msg_chn_entry_st) + list_size;
	chn_list = malloc(list_len);
	chn_list->chn_id = CHNID_LIST;
	chn_list->chn_nr = arg->chn_nr;
	memcpy(chn_list->entry,arg->chn_entry_list,list_size);
	sigemptyset(&set);
	sigaddset(&set,SIGALRM);
	//sigprocmask(SIG_UNBLOCK,&set,NULL);
	pthread_sigmask(SIG_UNBLOCK,&set,NULL);
	while(1){
		pause();
		printf("the list send start ...*************************** \n");
		ret = sendto(sd,chn_list,list_len,0,(void *)&peer_addr,sizeof(peer_addr));
		if (ret < 0){
			if (errno == EINTR)
				continue;
			break;
		}
		printf("send end ... the list length is %d **********\n",ret);
	}
	pthread_exit(NULL);
	pthread_cleanup_pop(0);
}

int thr_hoster_create(struct msg_chn_entry_st *chn_entry_list, int chn_nr)
{
	int ret;
	struct __arg *arg;

	if (chn_entry_list == NULL){
		return -1;
	}
	ret = create_socket();
	if (ret == -1){
		return -1;
	}

	arg = malloc(sizeof(*arg));
	arg->chn_entry_list = chn_entry_list;
	arg->chn_nr = chn_nr;

	ret = pthread_create(&tid,NULL,thr_func,arg);
	if (ret){
		free(arg);
		close(sd);
		return -1;
	}

	return 0;
}

int thr_hoster_destroy(void)
{
	if (sd <= 0)
		return -1;
	pthread_cancel(tid);
	pthread_join(tid,NULL);

	return 0;
}
