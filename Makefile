PREFIX?=        /usr/local
BINDIR=         ${PREFIX}/bin
LIBDIR=         ${PREFIX}/lib
MANDIR=         ${PREFIX}/share/man

MANTARGET=man

PROG=   randpass
SRCS=   randpass.c
MAN=    ${PROG}.1
CFLAGS= -std=c99 -pedantic -pedantic-errors -O2 -s -Wall -Werror \
	-Wno-unused-function \
	-Wno-dollar-in-identifier-extension \
	-Wno-gnu-zero-variadic-macro-arguments \
	-Wno-strict-prototypes

.include <bsd.prog.mk>
