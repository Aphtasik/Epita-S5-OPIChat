CC=gcc
CFLAGS=-g -Wall -Wextra -Werror -pedantic -std=c99 -ggdb
CPPFLAGS= -D_GNU_SOURCE
OBJ= connection.o epoll-server.o utils/xalloc.o
EXEC= epoll_server

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^

clean:
	$(RM) a.out vgcor* *.o $(EXEC)

