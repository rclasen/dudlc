AUTOMAKE_OPTIONS=foreign

# TODO: ln -s . mservclient

pkginclude_HEADERS= album.h \
	artist.h \
	client.h \
	command.h \
	event.h \
	help.h \
	history.h \
	iterate.h \
	player.h \
	proto.h \
	queue.h \
	random.h \
	session.h \
	sleep.h \
	sock.h \
	tag.h \
	track.h \
	user.h

lib_LTLIBRARIES=libdudlc.la

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
	$(pkginclude_HEADERS)

