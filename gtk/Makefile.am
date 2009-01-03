AUTOMAKE_OPTIONS=foreign

CFLAGS:= -g -Wall  -W -Wunused -Wmissing-prototypes -Wcast-qual -Wcast-align \
	-Werror -I../lib -I../cmd

AM_CFLAGS=-g ${GLIB_CFLAGS} ${GTK_CFLAGS}
AM_LDFLAGS=${GLIB_LIBS} ${GTK_LIBS}

bin_PROGRAMS=gtkdudlc

#man1_MANS=gtkdudlc.man

EXTRA_DIST=$(MANS)

gtkdudlc_LDFLAGS=-L../lib -L../cmd -ldudlc #-ldudlccmd 
gtkdudlc_SOURCES= \
	main.c \
	options.c \
	playbox.c \
	\
	options.h \
	playbox.h
