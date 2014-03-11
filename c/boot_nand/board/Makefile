
all: irq.o  main.o start.o
	@cp $^ ../
#########################################################################
	
CFLAGS :=  -Wall -fno-builtin -Os -I../include/

%.o:	%.S
	@arm-linux-gcc $(CFLAGS) -c -o $@ $<

%.o:	%.s
	@arm-linux-gcc $(CFLAGS) -c -o $@ $<

%.o:	%.c
	@arm-linux-gcc $(CFLAGS) -c -o $@ $<

#########################################################################

clean:
	@rm -f *.o
