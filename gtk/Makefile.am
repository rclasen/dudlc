AUTOMAKE_OPTIONS=foreign

CFLAGS:= -g -Wall  -W -Wunused -Wmissing-prototypes -Wcast-qual -Wcast-align \
	-Werror -I../lib -I../cmd

AM_CFLAGS=-g ${GLIB_CFLAGS} ${GTK_CFLAGS}
AM_LDFLAGS=${GLIB_LIBS} ${GTK_LIBS}

bin_PROGRAMS=gtkdudlc

man1_MANS=gtkdudlc.man

EXTRA_DIST=$(MANS)

gtkdudlc_LDFLAGS=-L../lib -ldudlc
gtkdudlc_SOURCES= \
	main.c \
	options.c \
	playbox.c \
	childwindow.c \
	context.c \
	sortfunc.c \
	cellfunc.c \
	treehelper.c \
	track.c \
	album.c \
	queue.c \
	tracksearch.c \
	artist.c \
	browse.c
noinst_HEADERS= \
	common.h
