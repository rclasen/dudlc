AUTOMAKE_OPTIONS=foreign

CFLAGS:= -g -Wall  -W -Wunused -Wmissing-prototypes -Wcast-qual -Wcast-align \
	-Werror -I../lib -I../cmd

# TODO: properly test for glib

AM_CFLAGS=-g $(shell pkg-config --cflags glib-2.0 gtk+ ) 

AM_LDFLAGS=$(shell pkg-config --libs glib-2.0 gtk+ )

bin_PROGRAMS=gtkdudlc

#man1_MANS=gtkdudlc.man

EXTRA_DIST=$(MANS)

gtkdudlc_LDFLAGS=-L../lib -L../cmd -ldudlc -ldudlccmd 
gtkdudlc_SOURCES= \
	main.c
