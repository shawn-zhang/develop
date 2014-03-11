
CFLAGS :=  -Wall -fno-builtin -Os -I../include/
all: net.o pack.o tftp.o unpack.o
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
