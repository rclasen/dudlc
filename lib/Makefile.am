AUTOMAKE_OPTIONS=foreign

CFLAGS:= -g -Wall  -W -Wunused -Wmissing-prototypes -Wcast-qual -Wcast-align -Werror -I../lib

SUBDIRS = dudlc

lib_LTLIBRARIES=libdudlc.la

include_HEADERS= dudlc.h

libdudlc_la_SOURCES= album.c \
	artist.c \
	client.c \
	event.c \
	help.c \
	history.c \
	iterate.c \
	player.c \
	proto.c \
	queue.c \
	random.c \
	session.c \
	sleep.c \
	sock.c \
	tag.c \
	track.c \
	user.c \
	sfilter.c 

.PHONY: commands.txt
commands.txt:
	grep duc_cmd_ dudlc/*.h  | \
		sed -e 's/.*\(duc_cmd_[a-z]*\).*/\1/' | \
		sort -u > $@

.PHONY: checkserver
checkserver:
	make -C ../../dudld commands.txt
	while read cmd; do \
		grep duc_cmd_$$cmd *.c > /dev/null || echo $$cmd ; \
	done < ../../dudld/commands.txt

	
