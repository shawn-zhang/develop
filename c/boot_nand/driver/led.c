#include <driver/led.h>
#include <common/common.h>

void led_on(void)
{
    GPMCON &= ~0xffff;
    GPMCON |= 0x1111;
    
    GPMDAT &= ~(0xf);
}
