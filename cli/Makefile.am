AUTOMAKE_OPTIONS=foreign

CFLAGS:= -g -Wall -W -Wunused -Wmissing-prototypes -Wcast-qual -Wcast-align \
	-Werror -I../lib -I../cmd

# TODO: properly test for glib

AM_CFLAGS=-g $(shell pkg-config --cflags glib-2.0) 

AM_LDFLAGS=$(shell pkg-config --libs glib-2.0)

bin_PROGRAMS=dudlc

man1_MANS=dudlc.man
man5_MANS=dudlc.conf.man

EXTRA_DIST=$(MANS)

dudlc_LDFLAGS=-L../lib -L../cmd -ldudlc -ldudlccmd -lreadline -lcurses
dudlc_SOURCES= events.c \
	main.c \
	tty.c \
	\
	events.h \
	main.h \
	tty.h
