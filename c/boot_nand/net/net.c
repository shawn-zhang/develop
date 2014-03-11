#include <common/common.h>
#include <net/net.h>
#include <board/irq.h>
#include <net/tftp.h>
#include <lib/string.h>
#include <driver/dm9000.h>


#define GPNCON (*(u32 *)0x7F008830)
#define EINT0CON0 (*(u32 *)0x7F008900)
#define EINT0MASK (*(u32 *)0x7F008920)
#define EINT0PEND (*(u32 *)0x7F008924)


//00:30:18:A3:EB:03 
u8 heth[6] = {0x00, 0x0a, 0xeb, 0xfa, 0x72, 0x23};
//u8 heth[6] = {0x00, 0x30, 0x18, 0xa3, 0xeb, 0x03};
u8 eeth[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

u32 hip = 192 | ( 168 << 8) | (1 << 16) | (156 << 24); //192.168.1.254
u32 eip = 192 | ( 168 << 8) | (1 << 16) | (253 << 24); //192.168.1.253

u16 hport;
u16 eport;

u8 send_buf[2048];
u8 recv_buf[2048];

enum __status{
	STAT_REC = 1,
	STAT_WRT,
	STAT_ACK,
	STAT_ST,
	STAT_EX
};

struct machine_st{
	char *start_pos;
	int status;
	int len;
	int i;
	int before_block;
	tftp_t *msg;
	int msg_len;
};


void do_error(int errno)
{
	switch(errno){
		case EUNDEF:
			printf("not defined.\n");
			break;
		case ENOTFOUND:
			printf("file not found.\n");
			break;
		case EACCESS:
			printf("access violation.\n");
			break;
		case ENOSPACE:
			printf("disk full or allocation exceeded.\n");
			break;
		case EBADOP:
			printf("illegal TFTP operation.\n");
			break;
		case EBADID:
			printf("unknown transfer ID.\n");
			break;
		case EEXISTS:
			printf("file already exists.\n");
			break;
		case ENOUSER:
			printf("no such user.\n");
			break;

	}

	return;
}

static void do_machine(struct machine_st *machine)
{
	switch(machine->status){
		case STAT_REC:
			machine->len = recv(machine->msg,machine->msg_len);
			if (machine->len < 0 || ntohs(machine->msg->th_opcode) == TFTP_ERROR){
				machine->status = STAT_EX;
				break;
			}
			if (ntohs(machine->msg->th_block) == machine->before_block){
				machine->status = STAT_ACK;
				break;
			}
			machine->before_block = ntohs(machine->msg->th_block);
			machine->status = STAT_WRT;
			break;
		case STAT_ACK:
			send_ack(machine->before_block);
			if (machine->len - (sizeof(tftp_t) - 1) < SEGSIZE){
				machine->status = STAT_ST;
				break;
			}
			machine->status = STAT_REC;
			break;
		case STAT_WRT:
			memcpy(machine->start_pos + (machine->before_block - 1) * SEGSIZE,machine->msg->th_data,machine->len - (sizeof(tftp_t) - 1));
			memset(machine->msg,0,machine->len);
			machine->status = STAT_ACK;
			printf("#");
			machine->i++;
			if (machine->i > 50){
				printf("\n\t");
				machine->i = 0;
			}
			break;
		case STAT_EX:
			if (machine->len < 0){
				printf("TFTP recv timeout...\n");
			}else if (ntohs(machine->msg->th_opcode) == TFTP_ERROR){
				do_error(ntohs(machine->msg->th_code));
			}
			machine->status = STAT_ST;
			break;
		
		case STAT_ST:
			break;
		default:
			break;
	
	}
	return;
}

#ifdef RECV_INTUPT
void do_net(void)
{
	u32 len;
	
	if(!(EINT0PEND & (1 << 7)))
		goto end;

	if((len = eth_recv((void *)recv_buf)) > 14)
		eth_unpack();

end:
	EINT0PEND = EINT0PEND;
}
#endif
int tftp(int argc, char *argv[])
{
	hport = htons(DEFAULTPORT);
	eport = htons(4321);
	struct machine_st mach;
	char msg[BUFSIZE];

	if (argc < 3){
		printf("Usage:tftp position filename.\n");
		return 0;
	}

	unpackhandler = tftp_unpack;
	//init EINT7
#ifdef RECV_INTUPT
	set_val(GPNCON, 0x2, 2, 14);
	set_val(EINT0CON0, 0x1, 3, 12);
	set_0(EINT0MASK, 7);

	request_irq(INT_EINT1, do_net);
#endif
	mach.status = STAT_REC;
	mach.start_pos = (char *)atoi(argv[1]);
	mach.len = 0;
	mach.before_block = 0;
	mach.msg = (void *)msg;
	mach.msg_len = BUFSIZE;
	mach.i = 0;
	
	send_rrq(argv[2]);

	printf("sendrrq ok.\n");
	while(mach.status != STAT_ST)
	{
		do_machine(&mach);
	}
	printf("\ntanser over.\n");

	return 0;
}


