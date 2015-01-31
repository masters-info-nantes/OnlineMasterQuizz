CFLAGS=-g -Wall
CPPFLAGS=-I../model

all: client 

client: main.o client.o

*.c: *.h
	@touch $@

clean:
	rm -rf *.o client
	