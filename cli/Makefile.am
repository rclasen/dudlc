
CFLAGS:= -g -Wall -Werror -I../libdudlc

bin_PROGRAMS=dudlc

dudlc_LDFLAGS=-L../libdudlc -ldudlc -lreadline
dudlc_SOURCES= command.c \
	events.c \
	formatter.c \
	mservcli.c \
	tty.c

