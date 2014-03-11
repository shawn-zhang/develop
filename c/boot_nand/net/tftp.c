#include <common/common.h>
#include <net/net.h>
#include <net/tftp.h>
#include <driver/dm9000.h>
#include <lib/string.h>


static char tftp_buf[BUFSIZE];
static int len;
static int flag = 1;


int send_rrq(char *filename)
{
	tftp_t *tftp = (void *)((char *)send_buf + sizeof(eth_t) + sizeof(ip_t) + sizeof(udp_t));
	
	memset((void *)send_buf,0,BUFSIZE);
	tftp->th_opcode = htons(TFTP_RRQ);
	memcpy(tftp->th_stuff,filename,strlen(filename) + 1);
	memcpy(tftp->th_stuff + strlen(filename) + 1,octet,strlen(octet) + 1);
	udp_pack(sizeof(tftp_t) + strlen(filename) + strlen(octet) - 1);

	return OK;
}

int send_ack(short block)
{	
	tftp_t *tftp = (void *)((char *)send_buf + sizeof(eth_t) + sizeof(ip_t) + sizeof(udp_t));

	memset((void *)send_buf,0,BUFSIZE);
	tftp->th_opcode = htons(TFTP_ACK);
	tftp->th_block = htons(block); 
	
	udp_pack(sizeof(tftp_t) - 1);

	return OK;
}

void tftp_unpack()
{
	ip_t *ip = (void *)((char *)recv_buf + sizeof(eth_t));
	udp_t *udp = (void *)((char *)ip + (ip->ihl << 2));
	
	if (udp->dest != eport){
		memset((void *)recv_buf,0,BUFSIZE);
		return ;
	}
	hport = udp->source;
	len = ntohs(ip->tot_len) - ((ip->ihl << 2) + sizeof(udp_t));
	memcpy(tftp_buf,(char *)udp + sizeof(udp_t),len);
	memset((void *)recv_buf,0,BUFSIZE);
	flag = 0;

	return;
}

int recv(tftp_t *msg,int msg_len)
{
#ifdef RECV_POLLING
	int ret;
	
	while (1){
		ret = eth_recv((void *)recv_buf);
		if (ret > 14){
			eth_unpack();
			if (flag == 0){
				flag = 1;
				break;
			}
		}
	}
#endif
#ifdef RECV_INTUPT
	while(1){
		mdelay(1);
		if (flag == 0){
			flag = 1;	
			break;
		}

	}
#endif
	memcpy(msg,tftp_buf,min(msg_len,len));
	memset(tftp_buf,0,BUFSIZE);

	return min(len,msg_len);
}
