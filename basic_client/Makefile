CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99 -ggdb
CPPFLAGS= -D_GNU_SOURCE

all: basic_client 

basic_client: basic_client.o
	$(CC) $^



clean:
	$(RM) a.out vgcor* *.o
