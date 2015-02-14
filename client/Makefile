CFLAGS=-g -Wall
CPPFLAGS=-I../model
LDLIBS=-lpthread

all: client 

client: main.o client.o

*.c: *.h
	@touch $@

clean:
	rm -rf *.o client
	