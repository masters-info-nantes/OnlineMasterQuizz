CFLAGS=-g -Wall
LDLIBS=-lpthread

all: server
server: server.o player.o

*.c: *.h
	@touch $@

clean:
	rm -rf *.o server