#include <common/common.h>
#include <board/irq.h>

void do_irq(void)
{
	void (*handler)(void) = NULL;


	if (VIC0ADDRESS){
		handler = (void (*)(void))(VIC0ADDRESS);
		VIC0ADDRESS = 0;
		VIC0SOFTINTCLEAR = VIC0SOFTINT;
	}else if (VIC1ADDRESS){
		handler = (void (*)(void))(VIC1ADDRESS);
		VIC1ADDRESS = 0;
		VIC1SOFTINTCLEAR = VIC1SOFTINT;
	}
	
	if ((unsigned int)handler){
		handler();
	}

	return;
}

int request_irq(int irqno,void (*handler)(void))
{
	if (irqno < 0 || irqno > 63){
		return -1;
	}

	if (irqno < 32){
		VIC0VECTADDR[irqno] = (unsigned int)handler;
		set_1(VIC0INTENABLE,irqno);
	}else{
		irqno = irqno - 32;
		VIC1VECTADDR[irqno] = (unsigned int)handler;
		set_1(VIC1INTENABLE,irqno);
	}
	__asm__ __volatile__(
		"mrs	r0,cpsr\n\t"
		"bic	r0,r0,#0x80\n\t"
		"msr	cpsr,r0\n\t"
		:
		:
		:"r0"
	);
	
	return 0;
}
