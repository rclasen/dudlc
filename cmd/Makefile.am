AUTOMAKE_OPTIONS=foreign

CFLAGS:= -g -Wall  -W -Wunused -Wmissing-prototypes -Wcast-qual -Wcast-align -Werror -I../lib

lib_LTLIBRARIES=libdudlccmd.la

include_HEADERS=dudlccmd.h

libdudlccmd_la_SOURCES= \
	formatter.c \
	msg.c \
	command.c \
	idlist.c \
	parseval.c
noinst_HEADERS=	\
	idlist.h \
	parseval.h

man1_MANS=dudl_cmd.man

EXTRA_DIST=$(MANS)

checklib:
	make -C ../lib commands.txt
	while read cmd; do \
		grep $$cmd command.c > /dev/null || echo $$cmd ; \
	done < ../lib/commands.txt
