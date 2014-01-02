#ifndef _LED_H_
#define _LED_H_

#define GPMCON  (*(u32 *)0x7F008820)
#define GPMDAT  (*(u32 *)0x7F008824)

void led_on(void);

#endif
