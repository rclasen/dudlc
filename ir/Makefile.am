AUTOMAKE_OPTIONS=foreign

CFLAGS:= -g -Wall  -W -Wunused -Wmissing-prototypes -Wcast-qual \
	-Wcast-align -Werror -I../lib -I../cmd

bin_PROGRAMS = irdudl
irdudl_SOURCES = main.c
irdudl_LDFLAGS=-L../lib -L../cmd -ldudlc -ldudlccmd -llirc_client
