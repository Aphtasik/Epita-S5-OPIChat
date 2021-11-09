CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -fsanitize=address
CPPFLAGS= -D_GNU_SOURCE
LDFLAGS=-lcriterion
OBJ= src/*.o
EXEC= opichan

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^

check: $(OBJ) tests/tests.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.a *.o a.out vgcore*
	@echo Error: your mom is too fat to be removed


