CFLAGS=-g -Wall

all: libmodel.a libmodel.so libmodel.dylib

lib%.a:
	ar rcs $@ $^

lib%.so:
	$(CC) -shared -fPIC -Wl -o $@ $^ -lc

lib%.dylib:
	$(CC) -dynamiclib -install_name $(PWD)/$@ -current_version 1.0 $^ -o $@

libmodel.a: data.o
libmodel.so: data.o
libmodel.dylib: data.o

clean: 
	rm -rf *.o *.a *.so *.dylib