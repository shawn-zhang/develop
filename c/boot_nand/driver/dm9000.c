#include <common/common.h>
#include <driver/dm9000.h>

u8 dm9000_ior(u8 reg)
{
	*(u8 *)DM9000_IO = reg;
	return *(u8 *)DM9000_DATA;
}

void dm9000_iow(u8 reg, u8 data)
{
	*(u8 *)DM9000_IO = reg;
	*(u8 *)DM9000_DATA = data;
}


#define SROM_BW (*(u32 *)0x70000000)
#define SROM_BC1 (*(u32 *)0x70000008)

void eth_init(void)
{
	int oft,i;
	u8 eeth[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};


	set_1(SROM_BW, 4); //Back1 DataWidth 16b 6410.pdf p208
	set_bit(SROM_BC1, 0x7, 16);

	dm9000_iow(DM9000_GPR, 0);
    dm9000_iow(DM9000_NCR, 1);

	mdelay(4);

	u32 id = 0;
	id = dm9000_ior(DM9000_VIDL);
	id |= dm9000_ior(DM9000_VIDH) << 8;
	id |= dm9000_ior(DM9000_PIDL) << 16;
	id |= dm9000_ior(DM9000_PIDH) << 24;
	if(id != DM9000_ID)
	{
		printf("DM9000 not found...\n");
		return;
	}

	dm9000_iow(DM9000_TCR, 0);
	dm9000_iow(DM9000_SMCR, 0);
	dm9000_iow(DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);
	dm9000_iow(DM9000_ISR, 0x0f);
	//dm9000_iow(DM9000_IMR,IMR_PAR);

	for (i = 0, oft = 0x10; i < 6; i++, oft++)
		dm9000_iow(oft,eeth[i]);
	for (i = 0, oft = 0x16; i < 8; i++, oft++)
		dm9000_iow(oft, 0xff);


	dm9000_iow(DM9000_RCR, RCR_RXEN | RCR_DIS_CRC | RCR_DIS_LONG);
	dm9000_iow(DM9000_IMR,IMR_PAR | IMR_PRM);
}

int eth_recv(char *ptk)
{
	u16 status;
	u16 len;
	u32 i;
	u8 INT_status;
	
	INT_status = dm9000_ior(DM9000_ISR );
	if(!(INT_status & 0x01))
		return -1;
	if((dm9000_ior(DM9000_MRCMDX) & 0x3) != 0x1)
		return -1;
	*(u8 *)DM9000_IO = DM9000_MRCMD;

	status = *(u16 *)DM9000_DATA;
	len    = *(u16 *)DM9000_DATA;

	for(i = 0; i < ((len + 1) >> 1); i++)
		((u16 *)ptk)[i] = *(u16 *)DM9000_DATA;
	
	dm9000_iow(DM9000_ISR,0x01);

	return len;
}
int eth_send(const char *pkt, u32 length)
{
	int i;
	
	//先禁止网卡中断，防止在发送数据时被中断干扰
	dm9000_iow(DM9000_IMR, IMR_PAR);

	dm9000_iow(DM9000_ISR, ISR_PTS);

	*(u8 *)DM9000_IO = DM9000_MWCMD;

	for(i = 0; i < ((length + 1) >> 1); i++)
		*(u16 *)DM9000_DATA = ((u16 *)pkt)[i];

	dm9000_iow(DM9000_TXPLL, length & 0xff);
	dm9000_iow(DM9000_TXPLH, (length >> 8) & 0xff);

	dm9000_iow(DM9000_TCR, TCR_TXREQ);

	i = 0;
	while(!(dm9000_ior(DM9000_ISR) & ISR_PTS))
	{
		if(i > 3000)
		{
			printf("TX timeout...\n");
			return -1;
		}
		mdelay(2);
		i++;
	}

	dm9000_iow(DM9000_ISR, ISR_PTS);
	
	//DM9000网卡的接收中断使能

	dm9000_iow(DM9000_IMR, IMR_PAR | IMR_PRM);

	return 0;
}


