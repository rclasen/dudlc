#ifndef _DUDLC_SOCK_H
#define _DUDLC_SOCK_H

#include "config.h"

#define BUFLENRCV	2048

typedef struct {
	char *host;
	int port;

	int fd;

	char buf[BUFLENRCV];
	int ilen;
	int llen;
} t_duc_sock;

t_duc_sock *duc_sock_open( const char *host, int port );
void duc_sock_close( t_duc_sock *s );

int duc_sock_connect( t_duc_sock *s );
void duc_sock_disconnect( t_duc_sock *s );
int duc_sock_reconnect( t_duc_sock *s );

int duc_sock_fd( t_duc_sock *t );

int duc_sock_recv( t_duc_sock *s );
int duc_sock_send( t_duc_sock *s, const char *cmd );
const char *duc_sock_getline( t_duc_sock *s );


#endif
