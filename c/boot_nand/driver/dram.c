#include <driver/dram.h>
#include <common/common.h>

#define HCLK            133000000.0000
#define nstocycle(ns)   (ns/7.5)

void dram_init(void)
{
	set_0(MEM_SYS_CFG,7); 

	P1MEMCCMD = 0x4;

	P1REFRESH = 7800 / 7.5;// this is the per row refresh period (refresh interval time (max 64ms ) / (2 ^ 13 row))

	P1CASLAT = (0x3 << 1);
	P1T_DQSS = 0x1;
	P1T_MRD = 0x2;
	P1T_RAS = 0x7;
	P1T_RC = 0xb;
	P1T_RCD = (0x5 << 0) | (0x2 << 3);
	P1T_RFC = (0x12 << 0) | (0xf << 5);
	P1T_RP = (0x5 << 0) | (0x2 << 3);
	P1T_RRD = 0x2;
	P1T_WR = 0x3;
	P1T_WTR = 0x2;
	P1T_XP = 0x2;
	P1T_XSR = 0x10;
	P1T_ESR = 0x14;

	P1MEMCFG = (0x2 << 15) | (0x2 << 3 ) | (0x2 << 0);
	P1MEMCFG2 = (0x01 << 11) | (0x3 << 8) | (0x01 << 6) | (0x1 << 3) | (1 << 0);
	P1MEMCFG3 = 0x7;
	P1_chip_0_cfg = P1_chip_1_cfg = (0x1 << 16) | (0xff << 8);

	//P1_user_cfg = 0x0;

	//mdelay(20000);
	int i;
	for (i = 0; i < 0x20000; i ++) {
		;
	}	

	P1DIRECTCMD = (0x2 << 18);
	
	P1DIRECTCMD = 0x0;
	
	P1DIRECTCMD = (0x3 << 18);
	P1DIRECTCMD = (0x3 << 18);

	P1DIRECTCMD = (0x02 << 18) | (0x2 << 16);
	P1DIRECTCMD = (0x02 << 18) | (0x3 << 4) | (0x2);

	P1MEMCCMD = 0x0;

	while((P1MEMSTAT & 0x3) != 0x1);
}
