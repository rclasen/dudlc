#include <stdlib.h>
#include <string.h>

#include "dudlc/session.h"
#include "dudlc/event.h"
#include "dudlc/proto.h"

dudlc *duc_new( const char *hostname, int port )
{
	dudlc *p;

	if( NULL == (p = malloc(sizeof(dudlc))))
		return NULL;

	*p->code = 0;
	p->line = NULL;
	p->inreply = 0;
	p->ev = NULL;
	p->user = NULL;
	p->pass = NULL;

	if( NULL == (p->con = duc_sock_open( hostname, port )))
		goto clean1;

	return p;

clean1:
	free(p);
	return NULL;
}

void duc_free( dudlc *p )
{
	if( ! p )
		return;

	duc_sock_close( p->con );
	free(p);
}

int duc_fd( dudlc *p )
{
	return duc_sock_fd( p->con );
}

int duc_open( dudlc *p )
{
	const char *c;

	if( -1 != duc_sock_fd(p->con))
		return 0;

	if( duc_sock_connect(p->con) )
		return -1;

	/* we are waiting for the hello message, but we are only
	 * interested in the reply code */
	p->inreply = 1;
	if( _duc_rlast(p) )
		goto clean1;

	c = _duc_rcode(p);
	if( ! c || *c != '2' )
		goto clean1;

	if( p->user && duc_cmd_auth( p ))
		goto clean1;


	_MSC_EVENT(p,connect,p);
	return 0;

clean1:
	return -1;
}

void duc_close( dudlc *p )
{
	p->line = NULL;
	p->inreply = 0;
	*p->code = 0;

	duc_sock_disconnect( p->con );
	_MSC_EVENT(p,disconnect,p);
}


int duc_setauth( dudlc *c, const char *user, const char *pass )
{
	if( c->user ){
		free(c->user);
		c->user = NULL;
	}
	if( c->pass ){
		free(c->pass);
		c->pass = NULL;
	}

	if( NULL == (c->user = strdup(user)))
		return -1;

	if( NULL == (c->pass = strdup(pass))){
		free(c->user);
		c->user = NULL;
		return -1;
	}

	return 0;
}

int duc_cmd_auth( dudlc *c )
{
	if( ! c->user || ! c->pass )
		return -1;

	if( _duc_cmd_succ( c, "user %s", c->user ))
		return -1;

	if( _duc_cmd_succ( c, "pass %s", c->pass ))
		return -1;

	return 0;
}


