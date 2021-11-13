CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 
CPPFLAGS= -D_GNU_SOURCE
LDFLAGS=-lcriterion -fsanitize=address
OBJ= $(wildcard src/*.o)
EXEC= opichat_server

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^

opichat_client:

opichat_server:

parser.o: src/parser.c
	$(CC) -c $^ $(CFLAGS)

connection.o: src/connection.c
	$(CC) -c $^ $(CFLAGS)

opichat.o: src/opichat.c
	$(CC) -c $^ $(CFLAGS) $(CPPFLAGS)

check: $(OBJ) tests/tests.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.a *.o a.out vgcore*


