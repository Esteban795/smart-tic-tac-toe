SRCDIR = src
HEADDIR = include
LIBDIR = ./src

CFLAGS = -W -Wall -Wextra -Wvla -fsanitize=address

LDFLAGS = -export-dynamic
DEPENDANCES = $(SRCDIR)/dicts.c

build: $(SRCDIR)/tic_tac_toe.c $(SRCDIR)/dicts.c
	gcc $(SRCDIR)/tic_tac_toe.c -o bin/tic_tac_toe.out $(CFLAGS) $(DEPENDANCES)

run : bin/tic_tac_toe.out
	bin/tic_tac_toe.out;clear

clean: bin/tic_tac_toe.out
	rm bin/tic_tac_toe.out

all: build run
	