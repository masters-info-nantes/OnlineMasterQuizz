CFLAGS=-g -Wall -std=c99
CPPFLAGS=-I../model
LDLIBS=-lpthread

all: server
server: main.o player.o server.o

*.c: *.h
	@touch $@

clean:
	rm -rf *.o server
	