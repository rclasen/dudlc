AUTOMAKE_OPTIONS=foreign

CFLAGS:= -g -Wall -W -Wunused -Wmissing-prototypes -Wcast-qual -Wcast-align \
	-Werror -I../lib -I../cmd

# TODO: properly test for curses + readline

AM_CFLAGS=-g ${GLIB_CFLAGS}
AM_LDFLAGS=${GLIB_LIBS}

bin_PROGRAMS=dudlc

man1_MANS=dudlc.man
man5_MANS=dudlc.conf.man

EXTRA_DIST=$(MANS)

dudlc_LDFLAGS=-L../lib -L../cmd -ldudlc -ldudlccmd -lreadline -lcurses
dudlc_SOURCES= events.c \
	main.c \
	tty.c \
	options.c
noinst_HEADERS=\
	events.h \
	main.h \
	tty.h \
	options.h
