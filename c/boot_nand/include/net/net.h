#ifndef _NET_H_
#define _NET_H_


#define RECV_POLLING
//#define RECV_INTUPT
#define htonl(d) swapl(d)
#define ntohl(d) swapl(d)
#define htons(d) swaps(d)
#define ntohs(d) swaps(d)

#define BUFSIZE	 2048

#define IPPROTOCOL_ICMP	1
#define IPPROTOCOL_UDP	17

#define ETHNETPROTO_IP	0x800

enum {
	ERR = -1,
	OK
};


struct ethhdr{
	unsigned char	dest[6];	/* destination eth addr	*/
	unsigned char	source[6];	/* source ether addr	*/
	unsigned short	proto;		/* packet type ID field	*/
} __attribute__((packed));

typedef struct ethhdr eth_t;

struct iphdr{
    unsigned int 	ihl:4;
    unsigned int 	version:4;
    unsigned char 	tos;
    unsigned short 	tot_len;
    unsigned short 	id;
    unsigned short 	frag_off;
    unsigned char 	ttl;
    unsigned char 	protocol;
    unsigned short 	check;
    unsigned int 	saddr;
    unsigned int 	daddr;
} __attribute__((packed));

typedef struct iphdr ip_t;

struct udphdr{
  unsigned short source;
  unsigned short dest;
  unsigned short len;
  unsigned short check;
} __attribute__((packed));

typedef struct udphdr udp_t;

struct arphdr
{
	unsigned short	ar_hrd;		/* format of hardware address	*/
	unsigned short	ar_pro;		/* format of protocol address	*/
	unsigned char	ar_hln;		/* length of hardware address	*/
	unsigned char	ar_pln;		/* length of protocol address	*/
	unsigned short	ar_op;		/* ARP opcode (command)		*/
	unsigned char	ar_sha[6];	/* sender hardware address	*/
	unsigned int 	ar_sip;		/* sender IP address		*/
	unsigned char	ar_tha[6];	/* target hardware address	*/
	unsigned int	ar_tip;		/* target IP address		*/
} __attribute__((packed));

typedef struct arphdr arp_t;

struct icmphdr
{
	unsigned char 	type;		/* message type */
	unsigned char 	code;		/* type sub-code */
	unsigned short 	checksum;
	unsigned short	id;
	unsigned short	sequence;
} __attribute__((packed));

typedef struct icmphdr icmp_t;
typedef void unpackhandler_t(void);

#define ICMP_ECHOREPLY		0	/* Echo Reply			*/
#define ICMP_ECHO		8	/* Echo Request			*/
#define ICMP_SIZE 		16

//pack.c
extern int eth_pack(u32, u16);
extern int ip_pack(u32, u8);
extern int udp_pack(u32);
extern int icmp_pack(char type,int seq,int len,short id);
unpackhandler_t *unpackhandler;

//unpack.c
extern void eth_unpack(void);
extern void ip_unpack(void);
extern void icmp_unpack(void);
extern void udp_unpack(void);

extern int tftp(int,char *[]);

//main.c
extern u8 heth[6];
extern u8 eeth[6];
extern u32 hip;
extern u32 eip;
extern u16 hport;
extern u16 eport;
extern u8 send_buf[BUFSIZE];
extern u8 recv_buf[BUFSIZE];

#endif

