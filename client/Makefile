CFLAGS=-g -Wall

all: client
client: main.o client.o data.o

*.c: *.h
	@touch $@

clean:
	rm -rf *.o client
	