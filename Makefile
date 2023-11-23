randpass: randpass.c nstring.h
	$(CC) -std=c99 -O2 -s -Wall -Werror -Wno-unused-function -o randpass randpass.c

clean:
	rm -f randpass

.PHONY: clean
