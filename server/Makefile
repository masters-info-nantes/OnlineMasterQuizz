CFLAGS=-g -Wall
CPPFLAGS=-I../model

all: server
server: main.o player.o server.o

*.c: *.h
	@touch $@

clean:
	rm -rf *.o server
	