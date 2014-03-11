#include <common/common.h>
#include <driver/nand.h>
#include <common/command.h>

#define NAND_CHIP_ENABLE	(NFCONT &= ~(1 << 1))
#define NAND_CHIP_DISABLE	(NFCONT |= (1 << 1))
#define CHECK_RB		while(!(NFSTAT & 0x1))

/*
* 1000000000.O / HCLK = 7.5ns = 1clk
*/

/*
* 6410.pdf p247 Figure 8-2
* TACLS是当CLE/ALE有效时过了多少时间后nWE/nRE才有效
* K9F2G08U0A.pdf p20 TACLS = (tALS / tCLS) - tWP
* K9F2G08U0A.pdf p13 TACLS = (21 / 21) - 21 = 0ns = 0clk
*/
#define TACLS	0

/*
* 6410.pdf p247 Figure 8-3
* TWRPH0是nWE/nRE的有效时间 
* K9F2G08U0A.pdf p20 (TWRPH0 + 1) = (tWP / tRP)
* K9F2G08U0A.pdf p13 (TWRPH0 + 1) = (21 / 21) = 21ns = 3clk
*/
#define TWRPH0	2

/*
* 6410.pdf p247 Figure 8-3
* TWRPH1是当nWE/nRE无效后DATA的保持时间
* K9F2G08U0A.pdf p20 (TWRPH1 + 1) = tDH
* K9F2G08U0A.pdf p13 (TWRPH1 + 1) = 5ns = 1clk
*/
#define TWRPH1	0	

void nand_init(void)
{
	set_val(GPOCON, 0x2, 2, 0); //p341
	set_val(GPPCON, 0x2aa, 10, 6); //p343

	NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH0 << 4) | (1 << 2);

	NFCONT = (3 << 6) | 7;


	NAND_CHIP_ENABLE;
	NFCMMD = 0xff;  //reset
	CHECK_RB;
	NAND_CHIP_DISABLE;
}

void send_addr(u32 addr)
{
	u32 column = addr & 0x7ff;
	u32 row = (addr >> 11) & 0x1ffff;

	NFADDR = column & 0xff;
	NFADDR = (column >> 8) & 0xf;

	NFADDR = row & 0xff;
	NFADDR = (row >> 8) & 0xff;
	NFADDR = (row >> 16) & 0x1;
}

void nand_read(u32 dst_addr, u32 src_addr, u32 len)
{
	u32 addr;
	u32 i;

	NAND_CHIP_ENABLE;

	for(addr = src_addr; addr < src_addr + len; addr += 2048)
	{

		NFCMMD = 0x00;

		send_addr(addr);

		NFCMMD = 0x30;

		CHECK_RB;
#if 0
		for(i = 0; i < 2048; i += 1)
		{
			*(u8 *)dst_addr = *(u8 *)&NFDATA;
			dst_addr += 1;
		}
#else
		for(i = 0; i < 2048; i += 4)
		{
			*(u32 *)dst_addr = NFDATA;
			dst_addr += 4;
		}
#endif

	}	

	NAND_CHIP_DISABLE;
}

void nand_write(u32 src_addr, u32 dst_addr, u32 len)
{
	u32 addr;
	u32 i;

	NAND_CHIP_ENABLE;

	for(addr = dst_addr; addr < dst_addr + len; addr += 2048)
	{
		NFCMMD = 0x80;

		send_addr(addr);

		for(i = 0; i < 2048; i += 4)
		{
			NFDATA = *(u32 *)src_addr;
			src_addr += 4;
		}

		NFCMMD = 0x10;

		CHECK_RB;
	}	

	NAND_CHIP_DISABLE;
}

void send_block_addr(u32 addr)
{
	u32 row = addr >> 11;

	NFADDR = row & 0xc0;
	NFADDR = (row >> 8) & 0xff;
	NFADDR = (row >> 16) & 0x1;
}

void nand_erase(u32 dst_addr, u32 len)
{
	u32 addr;
		
	NAND_CHIP_ENABLE;

	for(addr = dst_addr; addr < dst_addr + len; addr += 0x20000)
	{
		NFCMMD = 0x60;

		send_block_addr(addr);

		NFCMMD = 0xd0;

		CHECK_RB;
	}

	NAND_CHIP_DISABLE;
}

