#export LINUXROOT := /home/zhangshuxiao/Hi3716M/source/osdrv/kernel/linux-2.6.35
#export LINUXROOT := /home/zhangshuxiao/Hi3716MV100R001C00SPC090/source/osdrv/kernel/linux-2.6.35
export LINUXROOT := /home/shawn/product/hisdk/Hi3716XV100R001C00SPC0A1/source/osdrv/kernel/linux-2.6.35

ifndef LINUXROOT
export LINUXROOT := $(SRC_ROOT)/os/linux-2.6.31/linux-2.6.31
endif

obj-m += opQam.o

opQam-y := gpio.o

all:
	make -C $(LINUXROOT) M=$(PWD) modules
clean:
	make -C $(LINUXROOT) M=$(PWD) clean

