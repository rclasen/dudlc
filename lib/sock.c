
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "dudlc/sock.h"

t_duc_sock *duc_sock_open( const char *h, int p )
{
	t_duc_sock *s;

	if( NULL == (s = malloc(sizeof(t_duc_sock))))
		return NULL;

	if( NULL == (s->host = strdup( h )))
		goto clean1;

	s->port = p;

	s->ilen = 0;
	s->llen = 0;
	s->fd = -1;

	return s;

clean1:
	free(s);
	return NULL;
}

void duc_sock_close( t_duc_sock *s )
{
	if( ! s )
		return;

	duc_sock_disconnect( s );
	free(s->host);
	free(s);
}


int duc_sock_connect( t_duc_sock *s )
{
	struct protoent *prot;
	struct hostent *he;
	struct sockaddr_in sa;

	if( ! s )
		return -1;

	/* already open? */
	if( s->fd != -1 )
		return 0;

	if( NULL == (prot = getprotobyname("IP") ))
		return -1;

	if( NULL == (he = gethostbyname(s->host)))
		return -1;

	if( 0 > (s->fd = socket( PF_INET, SOCK_STREAM, prot->p_proto)))
		return -1;

	sa.sin_family = AF_INET;
	sa.sin_port = htons(s->port);
	sa.sin_addr = *(struct in_addr *)he->h_addr_list[0];

	if( 0 > connect( s->fd, (const struct sockaddr*)&sa, 
				sizeof(struct sockaddr_in))){
		duc_sock_disconnect( s );
		return -1;
	}

	return 0;
}

void duc_sock_disconnect( t_duc_sock *s )
{
	if( ! s )
		return;

	if( s->fd < 0 )
		return;

	shutdown(s->fd, 2);
	close(s->fd);

	s->fd = -1;
	s->ilen = 0;
	s->llen = 0;
}

int duc_sock_reconnect( t_duc_sock *s )
{
	duc_sock_disconnect( s );
	return duc_sock_connect(s);
}

int duc_sock_fd( t_duc_sock *s )
{
	return s->fd;
}

int duc_sock_send( t_duc_sock *s, const char *cmd )
{
	int len;

	if( s->fd == -1 )
		return -1;

	len = strlen(cmd);
	if( 0 > send(s->fd, cmd, len, MSG_DONTWAIT )){
		return -1;
	}

	return 0;
}

static inline void freeline( t_duc_sock *s )
{
	if( ! s->llen )
		return;

	memmove(s->buf, s->buf + s->llen, s->ilen - s->llen );
	s->ilen -= s->llen;
	s->llen = 0;
	s->buf[s->ilen] = 0; 
}

int duc_sock_recv( t_duc_sock *s )
{
	int len;

	if( s->fd == -1 )
		return -1;

	if( 0 > (len = recv( s->fd, s->buf + s->ilen, 
					BUFLENRCV - s->ilen-1, 0 ))){
		return -1;
	}

	if( ! len ){
		duc_sock_disconnect( s );
		return -1;
	}

	s->ilen += len;
	s->buf[s->ilen] = 0;

	return 0;
}
	
char *duc_sock_getline( t_duc_sock *s )
{
	freeline( s );

	if( s->ilen == 0 )
		return NULL;

	s->llen = strcspn( s->buf, "\n\r" );
	if( s->llen == s->ilen ){
		/* no linebreak found */

		if( s->llen < BUFLENRCV -1 ){
			s->llen = 0;
			return NULL;
		}

		fprintf( stderr, "line too long for BUFLENRCV, " 
				"truncating\n" );
	}

	/* split buffer in found line and remainig stuff */
	s->buf[s->llen++] = 0;
	s->llen += strspn( s->buf + s->llen, "\n\r" );

	return s->buf;
}


