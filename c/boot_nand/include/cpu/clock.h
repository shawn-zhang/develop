#ifndef SH_CLOCK_H
#define SH_CLOCK_H


#define APLL_CON	    (*(u32 *)0x7E00F00C)
#define MPLL_CON        (*(u32 *)0x7E00F010)
#define EPLL_CON0       (*(u32 *)0x7E00F014)
#define EPLL_CON1       (*(u32 *)0x7E00F018)
#define CLK_SRC         (*(u32 *)0x7E00F01C)
#define CLK_DIV0        (*(u32 *)0x7E00F020)
#define CLK_DIV1        (*(u32 *)0x7E00F024)
#define CLK_DIV2        (*(u32 *)0x7E00F028)
#define CLK_OUT         (*(u32 *)0x7E00F02C)
#define HCLK_GATE       (*(u32 *)0x7E00F030)
#define PCLK_GATE       (*(u32 *)0x7E00F034)
#define SCLK_GATE       (*(u32 *)0x7E00F038)
#define CLK_SRC2        (*(u32 *)0x7E00F10C)
#define MISC_CON        (*(u32 *)0x7E00F838)
#define OTHERS          (*(u32 *)0x7E00F900)


#define PLL_ENABLE      (1 << 31)
#define MDIV(num)       ((num) << 16)
#define PDIV(num)       ((num) << 8)
#define SDIV(num)       ((num) << 0)
#define KDIV(num)       ((num) << 0)
#define APLL_SEL        (1 << 0)
#define MPLL_SEL        (1 << 1)
#define EPLL_SEL        (1 << 2)
#define SYNCMUXSEL	    set_0(OTHERS,6)
#define SYNC667		    set_0(MISC_CON,19)


#define ARM_RATIO(num)		((num) << 0)
#define MPLL_RATIO(num)		((num) << 4)
#define HCLK_RATIO(num)		((num) << 8)
#define HCLKX2_RATIO(num)	((num) << 9)
#define PCLK_RATIO(num)		((num) << 12)	
#define CLKJPEG_RATIO(num)	((num) << 24)
#define CLKSECUR_RATIO(num)	((num) << 18)


int clock_init(void);

#endif
