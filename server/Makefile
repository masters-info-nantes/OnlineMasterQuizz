CFLAGS=-g -Wall
CPPFLAGS=-I../model
LDFLAGS=-g -Wl,-rpath,../model
LDLIBS=-lpthread -lmodel
LOADLIBES=-L../model

all: lib server
server: main.o player.o server.o
lib:
	cd ../model; make; echo
	
*.c: *.h
	@touch $@

clean:
	rm -rf *.o server
	