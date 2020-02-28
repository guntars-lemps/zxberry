
CFLAGS=-g -Wall -Wextra
LDFLAGS=-lm
CC=gcc
###SOURCES=$(wildcard *.c) $(wildcard */*.c)

OBJECTS=zxberry.o z80.o
Z80_TEST_OBJECTS=test_z80.o z80.o

all: zxberry

zxberry: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o zxberry $(LDFLAGS)

test_z80.o: tests/test_z80.c
	$(CC) $(CFLAGS) tests/test_z80.c -o test_z80.o

test_z80: $(Z80_TEST_OBJECTS)
	$(CC) $(CFLAGS) $(Z80_TEST_OBJECTS) -o test_z80 $(LDFLAGS)

clean:
	-rm zxberry test_z80 *.o