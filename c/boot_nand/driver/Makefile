
CFLAGS :=  -Wall -fno-builtin -Os -I../include/
all: dm9000.o dram.o led.o nand.o uart.o
	@cp $^ ../
#########################################################################
	

%.o:	%.S
	@arm-linux-gcc $(CFLAGS) -c -o $@ $<

%.o:	%.s
	@arm-linux-gcc $(CFLAGS) -c -o $@ $<

%.o:	%.c
	@arm-linux-gcc $(CFLAGS) -c -o $@ $<

#########################################################################

clean:
	@rm -f *.o
