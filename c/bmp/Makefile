CFLAGS=-Wall -g -O1
CFLAGS+=-I .
all:bmp.o bmpview.o framebuffer.o
	gcc $^ -o bmpview
%.o:%.c
	gcc -c $< -o $@ $(CFLAGS)
install:bmpview bmp.s
	cp bmp.s bmp
	cp bmpview ~/bin/
	cp bmp ~/bin
clean:
	rm -f *.o bmpview bmp
