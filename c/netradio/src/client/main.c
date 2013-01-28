#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <comm.h>
#include <signal.h>

#include <errno.h>
#include "conf.h"

#define REVBUFSIZE	64 * 1024

struct conf_st global_conf = {
	.mgroup = DEFAULT_MGROUP,
	.port	= DEFAULT_PORT,
	.player_cmdl = "/usr/local/bin/mplayer -cache 1024 -",
	.eth = DEFAULT_ETH
};

int recv_sd;

struct player_st {
	chn_id_t chn_id;
	int  pipefd;
	pid_t pid;
};

static int play_job(struct player_st *ptr)
{
	struct sockaddr_in peer_addr;
	socklen_t peer_addr_len;
	struct msg_data_st *data = NULL;
	int rec,retw;


	data = malloc(REVBUFSIZE);
	peer_addr_len = sizeof(peer_addr);
	while (1) {
		rec = recvfrom(recv_sd,data,REVBUFSIZE,0,(void *)&peer_addr,&peer_addr_len);
		if (rec < 0){
			if (errno == EINTR){
				continue;
			}
			break;
		}	
			
		if (data->chn_id != ptr->chn_id) {
			continue;
		}
		while (1){
			retw = write(ptr->pipefd,data->data,rec - sizeof (data->chn_id));
			if (retw < 0){
				if (errno == EINTR){
					continue;
				}
				perror("write");
				break;
			}
			break;
		}
	}
	return 0;
}

/*
 *	-M	指定多播组地址
 *	-P	指定接收端口
 *	-W	指定播放器命令行
 *	-H	显示帮助
 */

static int init_socket()
{
	int sd;
	struct ip_mreqn mreq;
	struct sockaddr_in local_addr;
	int val;

	sd = socket(AF_INET,SOCK_DGRAM,0);

	inet_pton(AF_INET,global_conf.mgroup,&mreq.imr_multiaddr);
	inet_pton(AF_INET,"0.0.0.0",&mreq.imr_address);
	mreq.imr_ifindex = if_nametoindex(global_conf.eth);
	val = 1;
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0){
		perror("setsocketopt() addr reuse");
		return -1;
	}
	if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0){
		perror("setsockopt() multicast if");
		return -1;
	}


	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(atoi(global_conf.port));
	inet_pton(AF_INET, "0.0.0.0", &local_addr.sin_addr);
	if (bind(sd, (void*)&local_addr, sizeof(local_addr)) < 0) {
		perror("bind()");
		return -1;
	}

	return sd;
}

static int travel_list(struct msg_chn_list_st *chn_list)
{
	int i;
	struct msg_chn_entry_st *chn_entry;
	char *pos = NULL;

	printf("==========================================================\n");
	printf("The Channel's count %d\n",chn_list->chn_nr);
	printf("And the channel list:");
	chn_entry = chn_list->entry;
	for(i = 0; i < chn_list->chn_nr; i++){
		printf("\n------------------------------------\n");
		printf("channel NO. : %d\n",chn_entry->chn_id);
		if (chn_entry->status == ST_ON)
			printf("channel ST. ON\n");
		else
			printf("channel ST. OFF\n");
		printf("channel descript:\n");
		write(1,chn_entry->descript,ntohs(chn_entry->len));
		printf("------------------------------------\n");
		pos =(void *) chn_entry;
		chn_entry = (void *)(pos + ntohs(chn_entry->len) + ENTRY_SIZE);
	
	}
	printf("==========================================================\n");
	
	return 0;
}

static int choose(struct msg_chn_list_st *chn_list)
{
	int c = -1;
	int i;
	struct msg_chn_entry_st *chn_entry;
	char *pos = NULL;
	int flag = 0;


	while(1){
		printf("Please input channel NO.:");
		scanf("%d",&c);
		chn_entry = chn_list->entry;
		for(i = 0; i < chn_list->chn_nr; i++){
			if (c == chn_entry->chn_id){
				if (chn_entry->status == ST_ON){
					flag = 1;
					break;
				}
				printf("channel ST. OFF.\n");
				break;
			}
			pos =(void *) chn_entry;
			chn_entry = (void *)(pos + ntohs(chn_entry->len) + ENTRY_SIZE);
		}
		if (flag == 0){
			continue;
		}
		break;

	}

	return c;
}

static int rcv_list(struct msg_chn_list_st **chn_list)
{
	struct sockaddr_in peer_addr;
	socklen_t peer_addr_len;
	int rcv_l;

	printf("recv list start ....\n");
	*chn_list = malloc(REVBUFSIZE);
	peer_addr_len = sizeof(peer_addr);
	while(1){
		printf("revsd%d\n",recv_sd);
		rcv_l = recvfrom(recv_sd,*chn_list,REVBUFSIZE,0,(void *)&peer_addr,&peer_addr_len);
		printf("chn_list chn_id:%d",rcv_l);
		if (rcv_l < 0){
			if (errno == EINTR)
				continue;
			break;		
		}else if(rcv_l == 0)
			continue;
		else if((*chn_list)->chn_id != CHNID_LIST)
			continue;
		else {
			printf("recved list ....\n");
			travel_list(*chn_list);
			break;
		}
	}

	return 0;
}

int
main(int argc, char **argv)
{
	struct player_st player;
	struct msg_chn_list_st *chn_list = NULL;
	FILE *fp;
	/* Parse .conf */
	/* Check environ */
/*	while (1) {
		c=getopt();
	}
*/
	recv_sd = init_socket();
	if (recv_sd < 0){
		exit(1);
	}
	rcv_list(&chn_list);
	player.chn_id = choose(chn_list);
	free(chn_list);
	fp = popen(global_conf.player_cmdl,"w");
	player.pipefd = fileno(fp);
	play_job(&player);

	pclose(fp);
	while(1){
		pause();
	}
	return 0;
}
