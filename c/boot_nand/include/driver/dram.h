#ifndef _DRAM_H_
#define _DRAM_H_

#define MEM_SYS_CFG 	(*(volatile unsigned long *)0x7E00F120) //pg160

#define P1MEMSTAT 	(*(volatile unsigned long *)0x7E001000)
#define P1MEMCCMD 	(*(volatile unsigned long *)0x7E001004)
#define P1DIRECTCMD 	(*(volatile unsigned long *)0x7E001008)
#define P1MEMCFG 	(*(volatile unsigned long *)0x7E00100C)
#define P1REFRESH 	(*(volatile unsigned long *)0x7E001010)
#define P1CASLAT 	(*(volatile unsigned long *)0x7E001014)
#define P1T_DQSS 	(*(volatile unsigned long *)0x7E001018)
#define P1T_MRD 	(*(volatile unsigned long *)0x7E00101C)
#define P1T_RAS 	(*(volatile unsigned long *)0x7E001020)
#define P1T_RC 		(*(volatile unsigned long *)0x7E001024)
#define P1T_RCD 	(*(volatile unsigned long *)0x7E001028)
#define P1T_RFC 	(*(volatile unsigned long *)0x7E00102C)
#define P1T_RP 		(*(volatile unsigned long *)0x7E001030)
#define P1T_RRD 	(*(volatile unsigned long *)0x7E001034)
#define P1T_WR 		(*(volatile unsigned long *)0x7E001038)
#define P1T_WTR 	(*(volatile unsigned long *)0x7E00103C)
#define P1T_XP 		(*(volatile unsigned long *)0x7E001040)
#define P1T_XSR 	(*(volatile unsigned long *)0x7E001044)
#define P1T_ESR 	(*(volatile unsigned long *)0x7E001048)
#define P1MEMCFG2 	(*(volatile unsigned long *)0x7E00104C)
#define P1MEMCFG3 	(*(volatile unsigned long *)0x7E001050)
#define P1_chip_0_cfg 	(*(volatile unsigned long *)0x7E001200)
#define P1_chip_1_cfg 	(*(volatile unsigned long *)0x7E001204)
#define P1_user_stat 	(*(volatile unsigned long *)0x7E001300)
#define P1_user_cfg 	(*(volatile unsigned long *)0x7E001304)

void dram_init(void);


#endif
