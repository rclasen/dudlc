#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>


#include "proto.h"

t_proto_func_disconnect cb_disconnect = NULL;;

#define BUFLENLINE	2048

t_proto *proto_new( const char *hostname, int port, 
		const char *user, const char *pass )
{
	t_proto *p;

	if( NULL == (p = malloc(sizeof(t_proto))))
		return NULL;

	*p->code = 0;
	p->line = NULL;
	p->inreply = 0;

	if( NULL == (p->con = server_open( hostname, port )))
		goto clean1;

	if( NULL == (p->user = strdup( user )))
		goto clean2;

	if( NULL == (p->pass = strdup( pass )))
		goto clean3;

	return p;

clean3:
	free(p->user);
clean2:
	server_close(p->con);
clean1:
	free(p);
	return NULL;
}

void proto_free( t_proto *p )
{
	if( ! p )
		return;

	server_close( p->con );
	free(p);
}

int proto_fd( t_proto *p )
{
	return server_fd( p->con );
}

int proto_open( t_proto *p )
{
	const char *c;

	if( -1 != server_fd(p->con))
		return 0;

	if( server_connect(p->con) )
		return 1;

	/* we are waiting for the hello message, but we are only
	 * interested in the reply code */
	p->inreply = 1;
	if( proto_last(p) )
		goto clean1;

	c = proto_rcode(p);
	if( ! c || *c != '2' )
		goto clean1;

	if( proto_fsend( p, "user %s", p->user ))
		return 1;
	if( proto_last(p) )
		goto clean1;

	c = proto_rcode(p);
	if( !c || *c != '3' )
		goto clean1;

	if( proto_fsend( p, "pass %s", p->pass ))
		return 1;
	if( proto_last(p) )
		goto clean1;

	c = proto_rcode(p);
	if( !c || *c != '2' )
		goto clean1;

	return 0;

clean1:
	server_disconnect(p->con);
	return 1;
}

static void proto_restart( t_proto *p )
{
	p->line = NULL;
	p->inreply = 0;
	*p->code = 0;

	server_disconnect( p->con );
	if( cb_disconnect )
		(*cb_disconnect)( p );
}

int proto_send( t_proto *p, const char *msg )
{
	if( p->inreply )
		return 1;

	if( msg[strlen(msg)-1] != '\n' )
		return 1;

	if( proto_open(p) )
		return 1;

	*p->code = 0;
	p->line = NULL;
	if( ! server_send( p->con, msg ) ){
		p->inreply = 1;
		return 0;
	}

	proto_restart( p );
	return 1;
}

static int proto_vsend( t_proto *p, const char *fmt, va_list ap )
{
	char buf[BUFLENLINE];
	int len;

	len = vsnprintf( buf, BUFLENLINE -1, fmt, ap );
	if( len < 0 || len > BUFLENLINE -1 )
		return 1;

	buf[len++] = '\n';
	buf[len++] = 0;

	return proto_send( p, buf );
}

int proto_fsend( t_proto *p, const char *fmt, ... )
{
	va_list ap;
	int r;

	va_start( ap, fmt );
	r = proto_vsend( p, fmt, ap );
	va_end( ap );

	return r;
}


static void proto_bcast( t_proto *p, const char *line )
{
	// TODO: bcast
	(void) p;
	(void)line;
}

int proto_nextline( t_proto *p )
{
	const char *l = NULL;

	if( ! p->inreply )
		return 1;

	while( 1 ){

		/* process next line */
		if( NULL != (l = server_getline(p->con)) ){
			if( strlen(l) < 4 ){
				proto_restart( p );
				return -1;
			}

			/* leave this loop for non-broadcasts */
			if( *l != '6' ){
				break;
			}

			proto_bcast( p, l );
			continue;
		} 
		
		/* or fetch next input from server */
		if( server_recv( p->con)){
			proto_restart( p );
			return -1;
		}
	}

	p->line = l + 4;

	if( *p->code ){
		if( strncmp(p->code, l, 3) ){
			proto_restart( p );
			return -1;
		}
	} else {
		strncpy( p->code, l, 3 );
		p->code[3] = 0;
	}

	p->inreply = (l[3] == '-');

	return 0;
}

int proto_last( t_proto *p )
{
	int r;

	/* read everything up to the last line */
	while( (r = proto_nextline(p)));

	if( r < 0 )
		return 1;

	return 0;
}

int proto_rend( t_proto *p )
{
	return ! p->inreply;
}

const char *proto_rcode( t_proto *p )
{
	if( ! *p->code )
		return NULL;

	return p->code;
}

const char *proto_rline( t_proto *p )
{
	if( ! p->line )
		return NULL;

	return p->line;
}

void proto_poll( t_proto *p )
{
	const char *l;

	if( proto_open(p) )
		return;

	if( server_recv(p->con) )
		proto_restart( p );

	while( NULL != (l = server_getline(p->con))){
		if( strlen(l) < 4 ){
			proto_restart( p );
			return;
		}

		if( *l != '6' ){
			fprintf( stderr, "found unprocessed input\n");
			continue;
		}

		proto_bcast( p, l );
	}
}


