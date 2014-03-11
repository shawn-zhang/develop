#include <common/common.h>
#include <net/net.h>
#include <driver/dm9000.h>
#include <lib/string.h>

void eth_unpack(void)
{
	eth_t *eth = (void *)recv_buf;
	
	if (memcmp(eth->dest,(void *)eeth,6))
		return;

	return ip_unpack();
}

void ip_unpack(void)
{
	unsigned short check = 0;

	ip_t *ip = (void *)(recv_buf + sizeof(eth_t));
	check = ip->check;
	ip->check = 0;
	if (cal_sum((void *)ip,ip->ihl << 2) != check){
		return ;
	}
	switch(ip->protocol){
		case IPPROTOCOL_ICMP:icmp_unpack();break;
		case IPPROTOCOL_UDP:udp_unpack();break;
		default:break;
	}

	return;
}

void icmp_unpack(void)
{
	struct iphdr *ip;
	struct icmphdr *icmp;
	int len;
	int i;

	ip = (void *)(recv_buf + sizeof(eth_t));

	icmp = (void *)(ip + 1);

	if (icmp->type == ICMP_ECHO){
		len = ntohs(ip->tot_len) - (ip->ihl << 2) - sizeof(icmp_t);
		memcpy((void *)send_buf + sizeof(eth_t) + sizeof(ip_t) + sizeof(icmp_t),(void *)(icmp + 1),len);
		icmp_pack(ICMP_ECHOREPLY,ntohs(icmp->sequence),len,ntohs(icmp->id));
		
		return;
	}
	
	printf("%d bytes from %08x: icmp_seq=%d ttl=%d times= %8s\n",
		(ip->ihl << 2) + ICMP_SIZE,
		ip->saddr,
		ntohs(icmp->sequence),
		ip->ttl,
		(char *)icmp + 8);
	
	for(i = 0; i < 2048; i++){
		recv_buf[i] = 0;
	}
}


void udp_unpack(void)
{
	unpackhandler();
}
