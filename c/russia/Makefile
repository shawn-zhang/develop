CFLAGS=-Wall -g -O1

all:russia.o framebuffer.o
	gcc -o russia russia.o framebuffer.o $(CFLAGS)

%.o:%c
	gcc -c -o $@ $< $(CFLAGS)

install:russia
	cp russia ~/bin
	mv russia /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/russia

clean:
	rm -f *.o russia
	
