DIRS		:= board common cpu driver lib net

START_OBJ	:= start.o

OBJS		:= main.o 
OBJS		+= irq.o
DERIVER     	:= uart.o
DERIVER     	+= dram.o 
DERIVER     += nand.o 
DERIVER     += dm9000.o 
DERIVER     += led.o 
CPU         := clock.o
LIBS        += common.o
LIBS        += command.o 
LIBS        += string.o 
LIBS        += ctype.o 
LIBS        += vsprintf.o 
NETS        := tftp.o
NETS        += net.o
NETS        += pack.o
NETS        += unpack.o
CMDS            += cmd_go.o
CMDS            += cmd_tftp.o
CMDS            += cmd_boot.o
CMDS            += cmd_nand.o




TARGET		:= bootloader
BOOTLDS 	:= board/bootloader.lds

all:
%:
	@for dir in $(DIRS) ; do make -C $$dir $@ ; done
	@arm-linux-ld -o $(TARGET) $(START_OBJ) $(OBJS) $(DERIVER) $(CPU) $(LIBS) $(NETS) $(CMDS) -T $(BOOTLDS)
	@arm-linux-objcopy -O binary $(TARGET) $(TARGET).bin
	@rm -f *.o
    
clean:
	@for dir in $(DIRS) ;\
        do make -C $$dir $@ ;       done
	@rm -f *.o *~
