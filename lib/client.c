#include <stdlib.h>
#include <string.h>

#include <mservclient/client.h>
#include <mservclient/proto.h>

mservclient *msc_new( const char *hostname, int port, 
		const char *user, const char *pass )
{
	mservclient *p;

	if( NULL == (p = malloc(sizeof(mservclient))))
		return NULL;

	*p->code = 0;
	p->line = NULL;
	p->inreply = 0;
	p->ev = NULL;

	if( NULL == (p->con = msc_sock_open( hostname, port )))
		goto clean1;

	if( NULL == (p->user = strdup( user )))
		goto clean2;

	if( NULL == (p->pass = strdup( pass )))
		goto clean3;

	return p;

clean3:
	free(p->user);
clean2:
	msc_sock_close(p->con);
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

msc_events *msc_getevents( mservclient *c )
{
	return c->ev;
}

msc_events *msc_setevents( mservclient *c, msc_events *e )
{
	msc_events *old;

	old = c->ev;
	c->ev = e;
	return old;
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

	if( _msc_cmd( p, "user %s", p->user ))
		goto clean1;

	c = _msc_rcode(p);
	if( !c || *c != '3' )
		goto clean1;

	if( _msc_cmd( p, "pass %s", p->pass ))
		goto clean1;

	c = _msc_rcode(p);
	if( !c || *c != '2' )
		goto clean1;

	MSC_EVENT(p,connect,p);
	return 0;

clean1:
	msc_sock_disconnect(p->con);
	return -1;
}

void msc_close( mservclient *p )
{
	p->line = NULL;
	p->inreply = 0;
	*p->code = 0;

	msc_sock_disconnect( p->con );
	MSC_EVENT(p,disconnect,p);
}

const char *msc_rmsg( mservclient *p )
{
	return _msc_rline( p );
}


int msc_cmd_disconnect( mservclient *c, int id )
{
	if( _msc_cmd(c, "disconnect %u", id ))
		return -1;
	if( *_msc_rcode(c) != '2' )
		return -1;
	return 0;
}


