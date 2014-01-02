#ifndef _COMMON_H_
#define _COMMON_H_

#include "stdarg.h"

typedef volatile unsigned int u32;
typedef volatile unsigned short u16;
typedef volatile unsigned char u8;
typedef unsigned int    size_t;

#define TEXT_BASE       0x57000000
#define TEXT_BASE_IRQ   0x58000000
#define NULL	((void *)0)


#define PROMPT          "[shawn@shawn]# "
#define COMMAND_BUFSIZE 2048


#define set_1(var, shift) ((var) |= (1 << (shift)))

#define set_0(var, shift) ((var) &= ~(1 << (shift)))

#define set_bit(var, bit, shift) ((var) |= ((bit) << (shift)))

#define clr_bit(var, bit, shift) ((var) &= ~((bit) << (shift)))

#define set_val(var, val, count, shift) 			\
	{						\
		clr_bit(var, ~(~0 << count), shift);	\
		set_bit(var, val, shift);		\
	}


int atoi(const char *cp);

int parse_cmd(char *,char *[]);

int vsprintf(char *buf, const char *fmt, va_list args);
void printf (const char *fmt, ...);

void mdelay(int);

#define swap(x, y) {typeof(x) tmp = x; x = y; y = tmp;}
unsigned int swapl(unsigned int d);
unsigned short swaps(unsigned short d);
unsigned short cal_sum(unsigned short *buf, int len);


#endif
