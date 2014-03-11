#include <common/common.h>
#include <driver/uart.h>
#include <driver/gpio.h>

void uart_init(void)
{
	set_val(GPACON,0x22,8,0);
	set_val(ULCON0,0x3,8,0);
	set_val(UCON0,0x5,4,0);

	UBRDIV0 = 34;
	UDIVSLOT0 = 0xdfdd;
}


int putc(char c)
{
	while(!(UTRSTAT0 & 0x2));
	
	UTXH0 = c;

	return c;
}

int puts(const char *s)
{
    int i;

    for(i = 0; s[i] != '\0'; i++){
        if (s[i] == '\n'){
            putc('\r');
        }
        putc(s[i]);
    }

    return i;
}

int getc()
{
    char c;

    while(!(UTRSTAT0 & 0x1));

    c = URXH0;
    if (c == '\r')
        c = '\n';
    putc(c);
    if (c == '\b'){
        putc(' ');
        putc('\b');
    }

    return c;

}

int gets(char *s)
{
    char c;
    int i = 0;

    while((c = getc()) != '\n'){
        if (c == '\b'){
            if (i <= 0){
                continue;
            }
            s--;
            i--;
        }else
        {
            *s++ = c;
            i++;
        }
    }
    *s = 0;

    return i;
}
