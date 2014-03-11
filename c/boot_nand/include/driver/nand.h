#ifndef __NAND_H_
#define __NAND_H_

#define GPOCON (*(u32 *)0x7f008140)
#define GPPCON (*(u32 *)0x7f008160)

#define NFCONF (*(u32 *)0x70200000)
#define NFCONT (*(u32 *)0x70200004)
#define NFCMMD (*(u8 *)0x70200008)
#define NFADDR (*(u8 *)0x7020000c)
#define NFDATA (*(u32 *)0x70200010)
#define NFMECCD0 (*(u32 *)0x70200014)
#define NFMECCD1 (*(u32 *)0x70200018)
#define NFSTAT (*(u32 *)0x70200028)
#define NFECCERR0 (*(u32 *)0x7020002c)
#define NFMECC0 (*(u32 *)0x70200034)

void nand_init(void);

void send_addr(u32 addr);

void nand_read(u32 dst_addr, u32 src_addr, u32 len);

void nand_write(u32 src_addr, u32 dst_addr, u32 len);

void send_block_addr(u32 addr);

void nand_erase(u32 dst_addr, u32 len);

#endif

