CFLAGS=-std=c99 -pedantic -pedantic-errors -O2 -s -Wall -Werror -Wno-unused-function

randpass: randpass.c nstrings.h
	$(CC) $(CFLAGS) -o randpass randpass.c

clean:
	rm -f randpass

.PHONY: clean
