#ifndef _MSERVCLIENT_SESSION_H
#define _MSERVCLIENT_SESSION_H

#include "sock.h"


typedef struct {
	void *ev;

	char *user;
	char *pass;

	t_msc_sock *con;

	int inreply;
	char code[4];
	const char *line;
} mservclient;


mservclient *msc_new( const char *hostname, int port);
void msc_free( mservclient *p );

int msc_setauth( mservclient *c, const char *user, const char *pass );
int msc_open( mservclient *p );
void msc_close( mservclient *p );

int msc_fd( mservclient *p );
void msc_poll( mservclient *p );




int msc_cmd_auth( mservclient *p );

#endif
