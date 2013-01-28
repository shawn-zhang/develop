CFLAGS+=-Wall -O1 -g3 -D_FILE_OFFSET_BITS=64 -I../include
LDFLAGS+=-pthread
CC=gcc
all:server

server: main.o thr_channel.o thr_hoster.o medialib.o mytbf.o
	$(CC) $^ -o $@ $(LDFLAGS)
%.o: %.c conf.h medialib.h mytbf.h thr_channel.h thr_hoster.h
	$(CC) -c $< -o $@ $(CFLAGS)
clean:
	rm -f *.o server
