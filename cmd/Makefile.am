AUTOMAKE_OPTIONS=foreign

CFLAGS:= -g -Wall  -W -Wunused -Wmissing-prototypes -Wcast-qual -Wcast-align -Werror -I../lib

lib_LTLIBRARIES=libdudlccmd.la

include_HEADERS= dudlccmd.h

libdudlccmd_la_SOURCES= formatter.c msg.c command.c 

checklib:
	make -C ../lib/commands.txt
	while read cmd; do \
		grep $$cmd command.c > /dev/null || echo $$cmd ; \
	done < ../lib/commands.txt
