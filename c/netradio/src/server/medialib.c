#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <glob.h>
#include <string.h>
#include <arpa/inet.h>

#include <comm.h>

#include "mytbf.h"
#include "medialib.h"
#include "conf.h"

#define FORMAT			"*.mp3"
#define DESCRIPT		"descript.txt"
#define DESCRIPT_MAX_SIZE	1024 * 1024 


struct media_channel_st {
	chn_id_t chn_id;
	glob_t globres;
	int fd;
	int curr_sub;
	off_t curr_pos;
	tbf_context_t *tbf;
};

static struct media_channel_st channel[CHNID_MAX+1];

static size_t read_descript(int fd,char **str)
{
	int retr,pos = 0;
	size_t file_size,ret_size;

	file_size = ret_size = lseek(fd,0,SEEK_END);
	if (file_size > DESCRIPT_MAX_SIZE){
		return -1;
	}
	*str = realloc(*str,file_size);
	if (*str == NULL){
		return -1;
	}

	lseek(fd,0,SEEK_SET);
	while(file_size > 0){
		retr = read(fd,*str + pos,file_size);
		if (retr < 0){
			if (errno == EINTR)
				continue;
			return -1;
		}
		if (retr == 0){
			break;
		}
		file_size -= retr;
		pos += retr;
	}

	return ret_size;
}

static int travel_channel(struct msg_chn_entry_st *chn_list,int chn_nr)
{
	int i;
	struct msg_chn_entry_st *curr;
	char *pos;
	


	curr = chn_list;
	for (i = 0; i < chn_nr; ++i) {
		printf("channel[%d].id:%d\n",i,curr->chn_id);
		printf("channel[%d].len:%d\n",i,ntohs(curr->len));
		write(1,curr->descript,ntohs(curr->len));
		pos = (void*)curr;
		curr = (void*)(pos + ntohs(curr->len) + ENTRY_SIZE);
	}

	return 0;
}

/**
	The function's argv[0] is must NULL,and you must free it
*/

int media_get_channel_list(struct msg_chn_entry_st **msg_chn_list,int *chn_nr)
{
	int ret;
	glob_t globbuf;
	int fd_save,fd_media,fd_descript;

	chn_id_t chn_id;
	int i;
	char *str_descript = NULL;
	unsigned int list_size = 0;
	size_t descript_size = 0;
	struct msg_chn_entry_st *msg_chn_node = NULL;
	struct msg_chn_entry_st *msg_chn_entry_list = NULL;

	fd_save = open(".",O_RDONLY);
	if (fd_save < 0){
		goto returnerr1;
	}
	ret = chdir(global_conf.media_path);
	if (ret == -1){
		goto returnerr2;
	}
	fd_media = open(".",O_RDONLY);
	if (fd_media < 0){
		goto returnerr2;
	}

	glob("*",GLOB_ONLYDIR,NULL,&globbuf);
	if (globbuf.gl_pathc <= 0){
		goto returnerr3;
	}
	for(i = 0, chn_id = 0; i < globbuf.gl_pathc && chn_id <= CHNID_MAX; i++){
		ret = chdir(globbuf.gl_pathv[i]);
		if (ret == -1){
			continue;
		}
		channel[chn_id].globres.gl_offs = 1;
		glob(FORMAT,GLOB_DOOFFS | GLOB_NOSORT,NULL,&channel[chn_id].globres);      
		channel[chn_id].globres.gl_pathv[0] = globbuf.gl_pathv[i];
		if (channel[chn_id].globres.gl_pathc < 1){
			goto continuepoint;
		}
		channel[chn_id].curr_sub = 1;
		if (global_conf.debug)
			printf("get_media_list....\n");
		while(1){
			channel[chn_id].fd = open(channel[chn_id].globres.gl_pathv[channel[chn_id].curr_sub],O_RDONLY);
			if (channel[chn_id].fd < 0){
				channel[chn_id].curr_sub++;
			}else
				break;
			if (channel[chn_id].curr_sub > channel[chn_id].globres.gl_pathc){
				goto continuepoint;
			}
		}
		if (global_conf.debug)
			printf("get_media_list.... open channel[%d].fd:%d\n",chn_id,channel[chn_id].fd);
		fd_descript = open(DESCRIPT,O_RDONLY);	
		if (fd_descript < 0){
			goto continuepoint;
		}
		if (global_conf.debug)
			printf("get_media_list.... open descript:%d \n",fd_descript);
		if ((descript_size = read_descript(fd_descript,&str_descript)) < 0){
			goto continuepoint;
		}
		close(fd_descript);

		list_size += descript_size + ENTRY_SIZE;
		if (global_conf.debug)
			printf("get_media_list.... list_size: %d \n",list_size);
		msg_chn_entry_list = realloc(msg_chn_entry_list,list_size);
		msg_chn_node =(void *)msg_chn_entry_list + (list_size - descript_size - ENTRY_SIZE);

		msg_chn_node->len = htons(descript_size);
		if (global_conf.debug)
			printf("get_media_list.... msg_chn_node->len: %d \n",descript_size);
		msg_chn_node->chn_id = channel[chn_id].chn_id = chn_id;
		memcpy((char *)msg_chn_node->descript,str_descript,descript_size);

		channel[chn_id].curr_pos = 0;
		channel[chn_id].tbf = tbf_ctx_new(DEFAULT_CPS,DEFAULT_BUST);
		if (global_conf.debug)
			printf("msn_chn_entry:id=%d\t",msg_chn_node->chn_id);
		chn_id++;
		if (global_conf.debug)
			printf("over:chn_id=%d\n",chn_id);
continuepoint:
		fchdir(fd_media);
	}
	travel_channel(msg_chn_entry_list,chn_id);
	*chn_nr = chn_id;
	*msg_chn_list = realloc(*msg_chn_list,list_size);
	memcpy(*msg_chn_list,msg_chn_entry_list,list_size);

	fchdir(fd_save);
	free(str_descript);
	free(msg_chn_entry_list);
	close(fd_save);
	close(fd_media);

	return 0;
returnerr3:
	close(fd_media);
returnerr2:
	fchdir(fd_save);
	close(fd_save);

returnerr1:
	return -1;
}

ssize_t media_read(chn_id_t chn_id,char *buf,size_t len)
{
	int retr;
	int fd;

	while((retr = tbf_read(channel[chn_id].fd,buf,len,channel[chn_id].tbf)) <= 0){
		if (retr <= 0){
			close(channel[chn_id].fd);
			fd = open(".",O_RDONLY);
			chdir(global_conf.media_path);
			chdir(channel[chn_id].globres.gl_pathv[0]);

			while (1){
				channel[chn_id].curr_sub++;
				if (channel[chn_id].globres.gl_pathc <  channel[chn_id].curr_sub){
					channel[chn_id].curr_sub = 1;
				}
				channel[chn_id].fd = open(channel[chn_id].globres.gl_pathv[channel[chn_id].curr_sub],O_RDONLY);
				if (channel[chn_id].fd < 0){
					continue;
				}else{
					break;
				}
			}
			fchdir(fd);
			close(fd);
		}
	}

	return retr;
}
void media_free_channel_list(struct msg_chn_entry_st *msg_chn_list,int chn_nr)
{
	int i;

	for(i = (chn_nr - 1); i >= 0; i--){
		tbf_ctx_delete(channel[i].tbf);
	}
	
	free(msg_chn_list);
}

void media_free_channel_entry(struct msg_chn_entry_st *msg_chn_list,chn_id_t chn_id)
{
	tbf_ctx_delete(channel[chn_id].tbf);
}
