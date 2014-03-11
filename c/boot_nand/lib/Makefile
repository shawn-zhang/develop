
CFLAGS :=  -Wall -fno-builtin -Os -I../include/
all: command.o common.o ctype.o string.o vsprintf.o
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
