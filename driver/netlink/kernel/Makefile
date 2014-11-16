
obj-m += nt.o
nt-y = nltest.o

KVER  := $(shell uname -r)
KID := /lib/modules/$(KVER)/build
PWD := $(shell pwd)

all:
	make -C $(KID) M=$(PWD) modules
clean:
	rm -rf *.o .cmd *.ko *.mod.c .tmp_versions
