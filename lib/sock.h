#ifndef _MSERVCLIENT_SOCK_H
#define _MSERVCLIENT_SOCK_H

#define BUFLENRCV	2048

typedef struct {
	char *host;
	int port;

	int fd;

	char buf[BUFLENRCV];
	int ilen;
	int llen;
} t_msc_sock;

t_msc_sock *msc_sock_open( const char *host, int port );
void msc_sock_close( t_msc_sock *s );

int msc_sock_connect( t_msc_sock *s );
void msc_sock_disconnect( t_msc_sock *s );
int msc_sock_reconnect( t_msc_sock *s );

int msc_sock_fd( t_msc_sock *t );

int msc_sock_recv( t_msc_sock *s );
int msc_sock_send( t_msc_sock *s, const char *cmd );
const char *msc_sock_getline( t_msc_sock *s );


#endif
