
CFLAGS=-g -Wall -Wextra
LDFLAGS=-lm
CC=gcc

OBJECTS=zxberry.o z80.o

all: zxberry

zxberry: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o zxberry $(LDFLAGS)

clean:
	-rm zxberry *.o