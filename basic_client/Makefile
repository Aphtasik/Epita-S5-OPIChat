CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -ggdb
CPPFLAGS= -D_GNU_SOURCE

all: basic_client.o
	$(CC) $^

clean:
	$(RM) a.out vgcor* *.o
