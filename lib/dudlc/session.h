#ifndef _DUDLC_SESSION_H
#define _DUDLC_SESSION_H

#include "sock.h"


typedef struct {
	void *ev;

	char *user;
	char *pass;

	t_duc_sock *con;

	int inreply;
	char code[4];
	const char *line;
} dudlc;


dudlc *duc_new( const char *hostname, int port);
void duc_free( dudlc *p );

int duc_setauth( dudlc *c, const char *user, const char *pass );
int duc_open( dudlc *p );
void duc_close( dudlc *p );

int duc_fd( dudlc *p );
void duc_poll( dudlc *p );




int duc_cmd_auth( dudlc *p );

#endif