#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include <comm.h>
#include "conf.h"
#include "thr_channel.h"
#include "thr_hoster.h"
#include "medialib.h"

#define CONFIG_FILE	"netradio.conf"

/*
 *	-M	指定多播组
 *  	-P	指定端口
 *  	-L	指定媒体存储路径
 *  	-D	DEBUG方式运行
 */
struct conf_st global_conf = {
	.mgroup = DEFAULT_MGROUP,
	.port	= DEFAULT_PORT,
	.media_path = DEFAULT_MEDIA_PATH,
	.eth	= DEFAULT_ETH,
	.debug	= 0
};

static int write_config_file()
{
	FILE *fp;

	fp = fopen(CONFIG_FILE,"w");
	if (fp == NULL){
		return -1;
	}
	if (fprintf(fp,"mgroup=%s\n",global_conf.mgroup) < 0){
		return -1;
	}
	if (fprintf(fp,"port=%s\n",global_conf.port) < 0){
		return -1;
	}
	if (fprintf(fp,"media_path=%s\n",global_conf.media_path) < 0){
		return -1;
	}
	if (fprintf(fp,"eth=%s\n",global_conf.eth) < 0){
		return -1;
	}
	if (fprintf(fp,"debug=%d\n",global_conf.debug) < 0){
		return -1;
	}
	fflush(fp);

	fclose(fp);
	return 0;
}
/* the argment value try realloc,so you must free it*/

static int read_conf_key(FILE *fp,const char *key, char **value)
{
	size_t size;
	char *str = NULL;
	char *tmp = NULL;
	long save;
	int retr = 0;
	int rgetline;


	save = ftell(fp);
	if (save == -1){
		return -1;
	}
	fseek(fp,0,SEEK_SET);
	while(1){
		if ((rgetline = getline(&str,&size,fp) < 0)){
			break;
		}
		if (str != NULL){
			if (*str == '#'){
				memset(str,0,size);
				continue;
			}
			tmp = realloc(tmp,size);
			if ((retr = sscanf(str,key,tmp)) <= 0){
				memset(str,0,size);
				continue;
			}
			*value = realloc(*value,strlen(tmp) + 1);
			strcpy(*value,tmp);
			break;
		}

	}
	
	fseek(fp,save,SEEK_SET);
	
	free(tmp);
	free(str);
	
	return retr;

}
static int read_config_file()
{
	FILE *fp = NULL;
	char *tmp = NULL;

	fp = fopen(CONFIG_FILE,"r");
	if (fp == NULL){
		if (write_config_file()){
			printf("write config failed!\n");
			return -1;
		}
	}
	printf("readconf start########################\n");
	if (read_conf_key(fp,"mgroup=%s\n",&tmp) > 0){
		global_conf.mgroup = strdup(tmp);
	}
	
	if (read_conf_key(fp,"port=%s\n",&tmp) > 0){
		global_conf.port = strdup(tmp);
	}
	
	if (read_conf_key(fp,"media_path=%s\n",&tmp) > 0){
		global_conf.media_path = strdup(tmp);
	}
	
	if(read_conf_key(fp,"eth=%s\n",&tmp) > 0){
		global_conf.eth = strdup(tmp);
	}
	if(read_conf_key(fp,"debug=%s\n",&tmp) > 0){
		global_conf.debug = atoi(tmp);
	}

	free(tmp);
	fclose(fp);

	return 0;
}

static int config_opt(int argc,char *argv[])
{
	int c;

	opterr = 0;
	while((c = getopt(argc,argv,"M:P:L:DE:")) != -1){
		switch(c){
			case 'M':
				global_conf.mgroup = optarg;
				break;
			case 'P':
				global_conf.mgroup = optarg;
				break;
			case 'L':
				global_conf.media_path = optarg;
				break;
			case 'E':
				global_conf.eth = optarg;
				break;
			case 'D':
				global_conf.debug = 1;
				break;
			default:
				break;
		}

	}

	return c;
}

static void term_handler(int s)
{
	exit(1);
}

int main(int argc,char *argv[])
{
	struct msg_chn_entry_st *chn_list = NULL, *curr;
	int chn_nr;
	char *pos;
	int ret,i;
	sigset_t set;
	struct sigaction sa;

	sigemptyset(&set);
	sigaddset(&set,SIGALRM);
	pthread_sigmask(SIG_UNBLOCK,&set,NULL);
	pthread_sigmask(SIG_BLOCK,&set,NULL);

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGINT);
	sigaddset(&sa.sa_mask,SIGTERM);
	sigaddset(&sa.sa_mask,SIGQUIT);
	sa.sa_handler = term_handler;

	read_config_file();
	config_opt(argc,argv);

	/* 获取频道列表 */
	printf("the media_get_channel start ....\n");
	ret = media_get_channel_list(&chn_list, &chn_nr);
	if (ret < 0) {
		goto returnerr1;
	}

	/* 创建hoster线程 */
	printf("thr_hoster start ....\n");
	if (thr_hoster_create(chn_list, chn_nr) < 0){
		goto returnerr2;	
	}

	/* 创建channel线程 */
	printf("thr_channel start ....***************\n");
	curr = chn_list;
	for (i = 0; i < chn_nr; ++i) {
		thr_channel_create(curr,ntohs(curr->len));
		pos = (void*)curr;
		curr = (void*)(pos + ntohs(curr->len) + ENTRY_SIZE);
	}
	thr_channel_destroy(chn_list,0);
	printf("sucess\n");
	while (1) //sigwait(&set,&ret);
	pause();

	return 0;

returnerr2:
	free(chn_list);
returnerr1:
	return -1;
}
