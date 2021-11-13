CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -g
CPPFLAGS= -D_GNU_SOURCE
LDFLAGS=-lcriterion -fsanitize=address
OBJ= src/parser.o src/connection.o src/opichat.o src/commands.o src/utils/xalloc.o
EXEC= opichat_server

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^

opichat_client:

opichat_server:

parser.o: src/parser.c
	$(CC) -c $^ $(CFLAGS)

connection.o: src/connection.c xalloc.o
	$(CC) -c $^ $(CFLAGS)

opichat.o: src/opichat.c
	$(CC) -c $^ $(CFLAGS) $(CPPFLAGS)

commands.o: src/commands.c
	$(CC) -c $^ $(CFLAGS) $(CPPFLAGS)

xalloc.o: src/utils/xalloc.c
	$(CC) -c $^ $(CFLAGS) $(CPPFLAGS)

check: $(OBJ) tests/tests.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.a *.o a.out src/*.o src/utils/*.o src/opichat_server vgcore*


