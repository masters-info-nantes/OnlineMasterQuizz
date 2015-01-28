CFLAGS=-g -Wall

all: libmodel.a libmodel.so libmodel.dylib

lib%.a:
	ar rcs $@ $^

lib%.so:
	$(CC) -shared -fPIC -Wl -o $@ $^ -lc

lib%.dylib:
	$(CC) -dynamiclib -install_name $(PWD)/$@ -current_version 1.0 $^ -o $@

libmodel.a: datatype.o data.o question.o
libmodel.so: datatype.o data.o question.o
libmodel.dylib: datatype.o data.o question.o

clean: 
	rm -rf *.o *.a *.so *.dylib