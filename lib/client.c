#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>


#include <mservclient/client.h>

#define BUFLENLINE	2048

t_mservclient *msc_new( const char *hostname, int port, 
		const char *user, const char *pass )
{
	t_mservclient *p;

	if( NULL == (p = malloc(sizeof(t_mservclient))))
		return NULL;

	*p->code = 0;
	p->line = NULL;
	p->inreply = 0;

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

void msc_free( t_mservclient *p )
{
	if( ! p )
		return;

	msc_sock_close( p->con );
	free(p);
}

int msc_fd( t_mservclient *p )
{
	return msc_sock_fd( p->con );
}

int msc_open( t_mservclient *p )
{
	const char *c;

	if( -1 != msc_sock_fd(p->con))
		return 0;

	if( msc_sock_connect(p->con) )
		return 1;

	/* we are waiting for the hello message, but we are only
	 * interested in the reply code */
	p->inreply = 1;
	if( _msc_last(p) )
		goto clean1;

	c = _msc_rcode(p);
	if( ! c || *c != '2' )
		goto clean1;

	if( _msc_fsend( p, "user %s", p->user ))
		return 1;
	if( _msc_last(p) )
		goto clean1;

	c = _msc_rcode(p);
	if( !c || *c != '3' )
		goto clean1;

	if( _msc_fsend( p, "pass %s", p->pass ))
		return 1;
	if( _msc_last(p) )
		goto clean1;

	c = _msc_rcode(p);
	if( !c || *c != '2' )
		goto clean1;

	return 0;

clean1:
	msc_sock_disconnect(p->con);
	return 1;
}

static void msc_restart( t_mservclient *p )
{
	p->line = NULL;
	p->inreply = 0;
	*p->code = 0;

	msc_sock_disconnect( p->con );
	if( cb_disconnect )
		(*cb_disconnect)( p );
}

int _msc_send( t_mservclient *p, const char *msg )
{
	if( p->inreply )
		return 1;

	if( msg[strlen(msg)-1] != '\n' )
		return 1;

	if( msc_open(p) )
		return 1;

	*p->code = 0;
	p->line = NULL;
	if( ! msc_sock_send( p->con, msg ) ){
		p->inreply = 1;
		return 0;
	}

	msc_restart( p );
	return 1;
}

static int _msc_vsend( t_mservclient *p, const char *fmt, va_list ap )
{
	char buf[BUFLENLINE];
	int len;

	len = vsnprintf( buf, BUFLENLINE -1, fmt, ap );
	if( len < 0 || len > BUFLENLINE -1 )
		return 1;

	buf[len++] = '\n';
	buf[len++] = 0;

	return _msc_send( p, buf );
}

int _msc_fsend( t_mservclient *p, const char *fmt, ... )
{
	va_list ap;
	int r;

	va_start( ap, fmt );
	r = _msc_vsend( p, fmt, ap );
	va_end( ap );

	return r;
}


static void _msc_bcast( t_mservclient *p, const char *line )
{
	if( cb_bcast )
		(*cb_bcast)(line);

	// TODO: bcast
	(void) p;
	(void)line;
}

int _msc_nextline( t_mservclient *p )
{
	const char *l = NULL;

	if( ! p->inreply )
		return 1;

	while( 1 ){

		/* process next line */
		if( NULL != (l = msc_sock_getline(p->con)) ){
			if( strlen(l) < 4 ){
				msc_restart( p );
				return -1;
			}

			/* leave this loop for non-broadcasts */
			if( *l != '6' ){
				break;
			}

			_msc_bcast( p, l );
			continue;
		} 
		
		/* or fetch next input from server */
		if( msc_sock_recv( p->con)){
			msc_restart( p );
			return -1;
		}
	}

	p->line = l + 4;

	if( *p->code ){
		if( strncmp(p->code, l, 3) ){
			msc_restart( p );
			return -1;
		}
	} else {
		strncpy( p->code, l, 3 );
		p->code[3] = 0;
	}

	p->inreply = (l[3] == '-');

	return 0;
}

int _msc_last( t_mservclient *p )
{
	int r;

	/* read everything up to the last line */
	while( (r = _msc_nextline(p)));

	if( r < 0 )
		return 1;

	return 0;
}

int _msc_rend( t_mservclient *p )
{
	return ! p->inreply;
}

const char *_msc_rcode( t_mservclient *p )
{
	if( ! *p->code )
		return NULL;

	return p->code;
}

const char *_msc_rline( t_mservclient *p )
{
	if( ! p->line )
		return NULL;

	return p->line;
}

void msc_poll( t_mservclient *p )
{
	const char *l;

	if( msc_open(p) )
		return;

	if( msc_sock_recv(p->con) )
		msc_restart( p );

	while( NULL != (l = msc_sock_getline(p->con))){
		if( strlen(l) < 4 ){
			msc_restart( p );
			return;
		}

		if( *l != '6' ){
			fprintf( stderr, "found unprocessed input\n");
			continue;
		}

		_msc_bcast( p, l );
	}
}


