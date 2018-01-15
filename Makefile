CC=gcc
CFLAGS=-std=c99 -Wall -O3 -static
CLIBS=-lm
PRGNAME=bfckr
CFILES=$(shell find . -name '*.c')
HFILES=$(shell find . -name '*.h')
STYLE=astyle --style=1tbs
RUN=valgrind --leak-check=full
DEBUG=gdb --args
ARGS=-d -f "examples/pi.bf"

all: build

clean:
	$(STYLE) $(CFILES)
	rm -f *.o
	rm -f $(PRGNAME)

build:
	$(CC) $(CFLAGS) $(CLIBS) -o $(PRGNAME) -Isrc $(CFILES)

debug:
	$(CC) $(CFLAGS) -g $(CLIBS) -o $(PRGNAME) -Isrc $(CFILES)

run_debug:
	$(DEBUG) ./$(PRGNAME) $(ARGS)

run:
	./$(PRGNAME) $(ARGS)

memtest:
	$(RUN) ./$(PRGNAME) $(ARGS)
