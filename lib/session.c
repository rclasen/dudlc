#include <stdlib.h>
#include <string.h>

#include "dudlc/session.h"
#include "dudlc/event.h"
#include "dudlc/proto.h"

mservclient *msc_new( const char *hostname, int port )
{
	mservclient *p;

	if( NULL == (p = malloc(sizeof(mservclient))))
		return NULL;

	*p->code = 0;
	p->line = NULL;
	p->inreply = 0;
	p->ev = NULL;
	p->user = NULL;
	p->pass = NULL;

	if( NULL == (p->con = msc_sock_open( hostname, port )))
		goto clean1;

	return p;

clean1:
	free(p);
	return NULL;
}

void msc_free( mservclient *p )
{
	if( ! p )
		return;

	msc_sock_close( p->con );
	free(p);
}

int msc_fd( mservclient *p )
{
	return msc_sock_fd( p->con );
}

int msc_open( mservclient *p )
{
	const char *c;

	if( -1 != msc_sock_fd(p->con))
		return 0;

	if( msc_sock_connect(p->con) )
		return -1;

	/* we are waiting for the hello message, but we are only
	 * interested in the reply code */
	p->inreply = 1;
	if( _msc_rlast(p) )
		goto clean1;

	c = _msc_rcode(p);
	if( ! c || *c != '2' )
		goto clean1;

	if( p->user && msc_cmd_auth( p ))
		goto clean1;


	_MSC_EVENT(p,connect,p);
	return 0;

clean1:
	return -1;
}

void msc_close( mservclient *p )
{
	p->line = NULL;
	p->inreply = 0;
	*p->code = 0;

	msc_sock_disconnect( p->con );
	_MSC_EVENT(p,disconnect,p);
}


int msc_setauth( mservclient *c, const char *user, const char *pass )
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

int msc_cmd_auth( mservclient *c )
{
	if( ! c->user || ! c->pass )
		return -1;

	if( _msc_cmd_succ( c, "user %s", c->user ))
		return -1;

	if( _msc_cmd_succ( c, "pass %s", c->pass ))
		return -1;

	return 0;
}


