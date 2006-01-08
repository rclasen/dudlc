AUTOMAKE_OPTIONS=foreign

CFLAGS:= -g -Wall -W -Wunused -Wmissing-prototypes -Wcast-qual -Wcast-align \
	-Werror -I../lib -I../cmd

bin_PROGRAMS=dudlc

dudlc_LDFLAGS=-L../lib -L../cmd -ldudlc -ldudlccmd -lreadline -lcurses
dudlc_SOURCES= events.c \
	main.c \
	tty.c \
	\
	events.h \
	main.h \
	tty.h
