AUTOMAKE_OPTIONS=foreign

CFLAGS:= -g -Wall -Werror -I../lib

bin_PROGRAMS=dudlc

dudlc_LDFLAGS=-L../lib -ldudlc -lreadline
dudlc_SOURCES= command.c \
	events.c \
	formatter.c \
	main.c \
	tty.c \
	\
	command.h \
	events.h \
	formatter.h \
	main.h \
	tty.h
