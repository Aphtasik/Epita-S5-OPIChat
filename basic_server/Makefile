CC=gcc
CFLAGS=-g -Wall -Wextra -Werror -pedantic -std=c99 -ggdb
CPPFLAGS= -D_GNU_SOURCE
EXEC= basic_server

all: $(EXEC)

$(EXEC): basic_server.o
	$(CC) $^ -o $@

clean:
	$(RM) a.out vgcor* *.o $(EXEC)

