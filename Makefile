CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -fsanitize=address
CPPFLAGS= -D_GNU_SOURCE
LDFLAGS=-lcriterion
OBJ= $(wildcard src/*.o)
EXEC= opichat_server

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^

opichat_client:

opichat_server:

check: $(OBJ) tests/tests.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.a *.o a.out vgcore*


