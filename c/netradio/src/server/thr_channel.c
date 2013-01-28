#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <comm.h>
#include "thr_channel.h"
#include "medialib.h"
#include "conf.h"

#define BUFSIZE 1024 * 1024

pthread_t chn2tid[CHILD_MAX + 1];

static int sd;
static pthread_once_t ton = PTHREAD_ONCE_INIT;

static void create_socket(void)
{
	struct ip_mreqn mreq;
	
	sd = socket(AF_INET,SOCK_DGRAM,0);
	
	inet_pton(AF_INET,global_conf.mgroup,&mreq.imr_multiaddr);
	inet_pton(AF_INET,"0.0.0.0",&mreq.imr_address);
	mreq.imr_ifindex = if_nametoindex(global_conf.eth);
	if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0){
		perror("setsockopt() multicast if");
		return ;
	}


	return;
}
static void thr_cleanup(void *arg)
{
	free(arg);
}

static void *thr_func(void *p)
{
	struct msg_chn_entry_st *chn_entry = p;
	struct msg_data_st *data = NULL;
	int retr,retw;
	int len;
	char buf[BUFSIZE];
	sigset_t set;
	struct sockaddr_in peer_addr;

	peer_addr.sin_family = AF_INET;
	peer_addr.sin_port = htons(atoi(global_conf.port));
	inet_pton(AF_INET,global_conf.mgroup,&peer_addr.sin_addr);
	
	pthread_cleanup_push(thr_cleanup,data);
	sigemptyset(&set);
	sigaddset(&set,SIGALRM);
	pthread_sigmask(SIG_BLOCK,&set,NULL);

	while(1){
		printf("media_read...start....\n");
		retr = media_read(chn_entry->chn_id,buf,BUFSIZE);
		if (retr < 0){
			if (errno == EINTR){
				continue;
			}
			break;
		}else if (retr == 0){
			continue;
		}
		printf("channel[%d] read len :%d.\n",chn_entry->chn_id,retr);
		len = sizeof(*data) + retr - sizeof(data->data);
		data = realloc(data,len);
		memset(data,0,len);
		data->chn_id = chn_entry->chn_id;
		memcpy(data->data,buf,retr);
		while(1){
			retw = sendto(sd,data,len,0,(void *)&peer_addr,sizeof(peer_addr));
			if (retw == -1){
				if (errno == EINTR)
					continue;
				perror("sendto");
			}
			break;
		}
		printf("channel[%d] sendto length is %d\n",chn_entry->chn_id,retw);

	}
	pthread_exit(NULL);
	pthread_cleanup_pop(0);
}
int thr_channel_create(struct msg_chn_entry_st *chn_entry, int len)
{
	int ret;

	pthread_once(&ton,create_socket);
	
	ret = pthread_create(chn2tid + chn_entry->chn_id,NULL,thr_func,chn_entry);
	if (ret != 0){
		perror("pthread_create");
		return -1;
	}
	chn_entry->status = ST_ON;

	return 0;
}
int thr_channel_destroy(struct msg_chn_entry_st *chn_entry,chn_id_t chn_id)
{
	int i;

	if (chn_id > CHNID_MAX){
		return -1;
	}
	
	for(i = 0; i < chn_id;i++){
		chn_entry = (void *)chn_entry + (chn_entry->len + ENTRY_SIZE);
	}
	chn_entry = (struct msg_chn_entry_st *)(chn_entry);
	chn_entry->status = ST_OFF;
	pthread_cancel(chn2tid[chn_id]);
	pthread_join(chn2tid[chn_id],NULL);
	media_free_channel_entry(chn_entry,chn_id);

	return 0;
}

int thr_channel_destroy_all(struct msg_chn_entry_st *chn_entry,int chn_nr)
{
	int i;
	
	for(i = 0; i <= CHNID_MAX && i < chn_nr;i++){
		if (chn_entry->status == ST_ON){
			pthread_cancel(chn2tid[i]);
			pthread_join(chn2tid[i],NULL);
			chn_entry->status = ST_OFF;
		}
		chn_entry =(struct msg_chn_entry_st *)((void *)chn_entry + ENTRY_SIZE + chn_entry->len);
	}
	close(sd);
	media_free_channel_list(chn_entry,chn_nr);

	return 0;
}

