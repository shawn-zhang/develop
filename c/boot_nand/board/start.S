    .text
    .align 2
.global _start
_start:
    b       reset
    ldr pc, _undefined_instruction
    ldr pc, _software_interrupt
    ldr pc, _prefetch_abort
    ldr pc, _data_abort
    ldr pc, _not_used
    ldr pc, _irq
    ldr pc, _fiq

_undefined_instruction:
    .word undefined_instruction
_software_interrupt:
    .word software_interrupt
_prefetch_abort:
    .word prefetch_abort
_data_abort:
    .word data_abort
_not_used:
    .word not_used
_irq:
    .word irq 
_fiq:
    .word fiq 

reset:
    /*
     * set the cpu to SVC32 mode
     */
    
    mrs     r0,cpsr
    bic     r0,r0,#0x1f
    orr     r0,r0,#0xd3
    msr     cpsr,r0

    /* Peri port setup */
    mov     r0,#0x70000000
    orr     r0,#0x13
    mcr     p15, 0, r0, c15, c2, 4

    //disable watchdog

    ldr     r0,=0x7E004000
    mov     r1,#0
    str     r1,[r0]

    //disable irq

    mrs     r0,cpsr
    orr     r0,#0xc0
    msr     cpsr,r0

    @start
    mov     sp,#0x2000
    bl      start_
    ldr     sp,=0x57000000
    ldr     pc,=main

1:   b   1b

.align 2
undefined_instruction:
.align 2
software_interrupt:
.align 2
prefetch_abort:
.align 2
data_abort:
.align 2
not_used:
.align 2
irq:
    ldr     sp,=0x58000000
    push    {r0 - r12,lr}
    bl      do_irq
    pop     {r0 - r12,lr}
    subs    pc,lr,#4
.align 2
fiq:

    .align 2
    .global mdelay
mdelay:
    ldr r1, =10000
    mul r2, r1, r0
1:
    subs r2, r2, #1
    bne 1b
    bx  lr

