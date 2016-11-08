CC=gcc
CFLAGS=-std=c99 -Wall -O2 -static -g
CLIBS=-lm
PRGNAME=bfckr
CFILES=$(shell find . -name '*.c')
HFILES=$(shell find . -name '*.h')
STYLE=astyle --style=1tbs
RUN=valgrind --leak-check=full
DEBUG=gdb --args
ARGS="examples/hello.bf"

all: build run

clean:
	$(STYLE) $(CFILES)
	rm -f *.o
	rm -f $(PRGNAME)
build:
	$(CC) $(CFLAGS) $(CLIBS) -o $(PRGNAME) -Isrc $(CFILES)
run:
	./$(PRGNAME) $(ARGS)
memtest:
	$(RUN) ./$(PRGNAME) $(ARGS)
debug:
	$(DEBUG) ./$(PRGNAME) $(ARGS)
