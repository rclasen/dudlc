#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include "dudlc/event.h"
#include "dudlc/proto.h"

#define BUFLENLINE	2048

int _duc_vsend( dudlc *p, const char *fmt, va_list ap )
{
	char buf[BUFLENLINE];
	int len;

	if( p->inreply )
		return -1;

	if( duc_open(p) )
		return -1;

	len = vsnprintf( buf, BUFLENLINE -1, fmt, ap );
	if( len < 0 || len > BUFLENLINE -1 )
		return -1;

	buf[len++] = '\n';
	buf[len++] = 0;

	*p->code = 0;
	p->line = NULL;
	if( ! duc_sock_send( p->con, buf ) ){
		p->inreply = 1;
		return 0;
	}

	duc_close( p );
	return -1;
}

int _duc_send( dudlc *p, const char *fmt, ... )
{
	va_list ap;
	int r;

	va_start( ap, fmt );
	r = _duc_vsend( p, fmt, ap );
	va_end( ap );

	return r;
}

int _duc_cmd( dudlc *p, const char *fmt, ... )
{
	va_list ap;

	va_start( ap, fmt );
	if( _duc_vsend( p, fmt, ap )){
		va_end(ap);
		return -1;
	}
	va_end( ap );

	return _duc_rlast(p);
}

int _duc_cmd_succ( dudlc *c, const char *fmt, ... )
{
	va_list ap;

	va_start(ap, fmt );
	if( _duc_vsend(c, fmt, ap ) ){
		va_end(ap);
		return -1;
	}
	va_end(ap);

	if( _duc_rlast(c) )
		return -1;

	if( *_duc_rcode(c) == '2' || *_duc_rcode(c) == '3' )
		return 0;

	return -1;
}

char *_duc_cmd_string( dudlc *c, const char *fmt, ... )
{
	va_list ap;

	va_start(ap, fmt );
	if( _duc_vsend(c, fmt, ap ) ){
		va_end(ap);
		return NULL;
	}
	va_end(ap);

	if( _duc_rlast(c) )
		return NULL;

	if( *_duc_rcode(c) == '2' )
		return strdup(_duc_rline(c));

	return  NULL;
}

int _duc_cmd_int( dudlc *c, const char *fmt, ... )
{
	va_list ap;

	va_start(ap, fmt );
	if( _duc_vsend(c, fmt, ap ) ){
		va_end(ap);
		return -1;
	}
	va_end(ap);

	if( _duc_rlast(c) )
		return -1;

	if( *_duc_rcode(c) == '2' )
		return atoi(_duc_rline(c));

	return  -1;
}

void *_duc_cmd_conv( dudlc *c, _duc_converter conv, 
		const char *fmt, ... )
{
	va_list ap;

	va_start(ap, fmt );
	if( _duc_vsend(c, fmt, ap ) ){
		va_end(ap);
		return NULL;
	}
	va_end(ap);

	if( _duc_rlast(c) )
		return NULL;

	if( *_duc_rcode(c) == '2' )
		return (*conv)(_duc_rline(c), NULL);

	return  NULL;
}


int _duc_rnext( dudlc *p )
{
	char *l = NULL;

	if( ! p->inreply )
		return -2;

	while( 1 ){

		/* process next line */
		if( NULL != (l = duc_sock_getline(p->con)) ){
			if( strlen(l) < 4 ){
				duc_close( p );
				return -1;
			}

			/* leave this loop for non-broadcasts */
			if( *l != '6' ){
				break;
			}

			_duc_bcast( p, l );
			continue;
		} 
		
		/* or fetch next input from server */
		if( duc_sock_recv( p->con)){
			duc_close( p );
			return -1;
		}
	}

	p->line = l + 4;

	if( *p->code ){
		if( strncmp(p->code, l, 3) ){
			duc_close( p );
			return -1;
		}
	} else {
		strncpy( p->code, l, 3 );
		p->code[3] = 0;
	}

	p->inreply = (l[3] == '-');

	return 0;
}

int _duc_rlast( dudlc *p )
{
	int r;

	/* read everything up to the last line */
	while( ! (r = _duc_rnext(p)));

	if( r == -1 )
		return -1;

	return 0;
}

int _duc_rend( dudlc *p )
{
	return ! p->inreply;
}

const char *_duc_rcode( dudlc *p )
{
	if( ! *p->code )
		return NULL;

	return p->code;
}

const char *_duc_rline( dudlc *p )
{
	if( ! p->line )
		return NULL;

	return p->line;
}

void duc_poll( dudlc *p )
{
	char *l;

	if( duc_open(p) )
		return;

	if( duc_sock_recv(p->con) )
		duc_close( p );

	while( NULL != (l = duc_sock_getline(p->con))){
		if( strlen(l) < 4 ){
			duc_close( p );
			return;
		}

		if( *l != '6' ){
			fprintf( stderr, "found unprocessed input: %s\n", l );
			continue;
		}

		_duc_bcast( p, l );
	}
}

const char *_duc_skipspace( const char *s )
{
	while( *s && isspace(*s))
		s++;

	return s;
}

char *_duc_fielddup( char *s, char **end )
{
	char *dup;
	char *p;
	int escape = 0;

	/* alloc a string large enough for escaped string - the result
	 * will be smaller */
	if( NULL == (dup = malloc(1 + strcspn(s,"\t"))))
		return NULL;

	p = dup;
	for(; *s && *s != '\t'; ++s ){
		if( escape ){
			switch( *s ){
				case 't':
					*p++ = '\t';
					break;
				case '\\':
				default:
					*p++ = *s;
					break;
			}
			escape = 0;
			continue;
		} 

		if( *s == '\\' ){
			escape++;
			continue;
		}

		*p++ = *s;
	}
	*p++= 0;

	if( end ) *end = s;

	return dup;
}


