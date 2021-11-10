CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99 -ggdb
CPPFLAGS= -D_GNU_SOURCE
EXEC= basic_client

all: $(EXEC)

$(EXEC): basic_client.o
	$(CC) $^ -o $@

clean:
	$(RM) a.out vgcor* *.o $(EXEC)
