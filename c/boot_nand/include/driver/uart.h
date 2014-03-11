#ifndef SH_UART_H
#define SH_UART_H

#define ULCON0		(*(u32 *)0x7F005000)
#define UCON0 		(*(u32 *)0x7F005004)
#define UTXH0		(*(u32 *)0x7F005020)
#define URXH0		(*(u32 *)0x7F005024)
#define UBRDIV0		(*(u32 *)0x7F005028)
#define UDIVSLOT0 	(*(u32 *)0x7F00502C)
#define UTRSTAT0 	(*(u32 *)0x7F005010)

void uart_init();

int getc(void);

int putc(char c);

int gets(char *s);

int puts(const char *s);

#endif
