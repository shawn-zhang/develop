
all: cmd_boot.o cmd_go.o cmd_nand.o cmd_tftp.o
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
