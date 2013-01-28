CFLAGS+=-I../include -Wall -O1 -g3
CC=gcc

all:client

client:main.o
	$(CC) $^ -o $@ $(LDFLAGS)

%.o:%.c conf.h

	$(CC) -c $< -o $@ $(CFLAGS)
clean:
	rm -f *.o client


