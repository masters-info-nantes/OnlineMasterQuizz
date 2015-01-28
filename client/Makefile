CFLAGS=-g -Wall
CPPFLAGS=-I../model
LDFLAGS=-g -Wl,-rpath,../model
LDLIBS=-lmodel
LOADLIBES=-L../model

all: lib client 

client: main.o client.o
lib:
	cd ../model; make; echo

*.c: *.h
	@touch $@

clean:
	rm -rf *.o client
	