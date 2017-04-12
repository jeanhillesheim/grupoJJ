CC=gcc
CFLAGS=-I.
LIBS=-lpthread
DEPS = module.h pedrolio.h
OBJ = main.o pedrolio.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

control: $(OBJ) mod_control/mod_control.o
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
grupojj: $(OBJ) mod_grupojj/mod_grupojj.o
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

all: control grupojj

PHONY: clean

clean:
	rm -f *.o */*.o control grupojj
