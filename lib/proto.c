#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include <mservclient/command.h>
#include <mservclient/event.h>
#include <mservclient/proto.h>

#define BUFLENLINE	2048

int _msc_vsend( mservclient *p, const char *fmt, va_list ap )
{
	char buf[BUFLENLINE];
	int len;

	if( p->inreply )
		return -1;

	if( msc_open(p) )
		return -1;

	len = vsnprintf( buf, BUFLENLINE -1, fmt, ap );
	if( len < 0 || len > BUFLENLINE -1 )
		return -1;

	buf[len++] = '\n';
	buf[len++] = 0;

	*p->code = 0;
	p->line = NULL;
	if( ! msc_sock_send( p->con, buf ) ){
		p->inreply = 1;
		return 0;
	}

	msc_close( p );
	return -1;
}

int _msc_send( mservclient *p, const char *fmt, ... )
{
	va_list ap;
	int r;

	va_start( ap, fmt );
	r = _msc_vsend( p, fmt, ap );
	va_end( ap );

	return r;
}

int _msc_cmd( mservclient *p, const char *fmt, ... )
{
	va_list ap;
	int r;

	va_start( ap, fmt );
	r = _msc_vsend( p, fmt, ap );
	va_end( ap );

	if( r )
		return r;

	return _msc_rlast(p);
}



static void _msc_bcast( mservclient *p, const char *line )
{
	switch(line[1]){
		case '3':
			_msc_bcast_user( p, line );
			break;

		case '4':
			_msc_bcast_player( p, line );
			break;
			
		case '5':
			_msc_bcast_random( p, line );
			break;
			
		case '6':
			_msc_bcast_queue( p, line );
			break;
			
		default:
			_MSC_EVENT(p,bcast,p,line);
	}
}

int _msc_rnext( mservclient *p )
{
	const char *l = NULL;

	if( ! p->inreply )
		return -2;

	while( 1 ){

		/* process next line */
		if( NULL != (l = msc_sock_getline(p->con)) ){
			if( strlen(l) < 4 ){
				msc_close( p );
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
			msc_close( p );
			return -1;
		}
	}

	p->line = l + 4;

	if( *p->code ){
		if( strncmp(p->code, l, 3) ){
			msc_close( p );
			return -1;
		}
	} else {
		strncpy( p->code, l, 3 );
		p->code[3] = 0;
	}

	p->inreply = (l[3] == '-');

	return 0;
}

int _msc_rlast( mservclient *p )
{
	int r;

	/* read everything up to the last line */
	while( ! (r = _msc_rnext(p)));

	if( r == -1 )
		return -1;

	return 0;
}

int _msc_rend( mservclient *p )
{
	return ! p->inreply;
}

const char *_msc_rcode( mservclient *p )
{
	if( ! *p->code )
		return NULL;

	return p->code;
}

const char *_msc_rline( mservclient *p )
{
	if( ! p->line )
		return NULL;

	return p->line;
}

void msc_poll( mservclient *p )
{
	const char *l;

	if( msc_open(p) )
		return;

	if( msc_sock_recv(p->con) )
		msc_close( p );

	while( NULL != (l = msc_sock_getline(p->con))){
		if( strlen(l) < 4 ){
			msc_close( p );
			return;
		}

		if( *l != '6' ){
			fprintf( stderr, "found unprocessed input\n");
			continue;
		}

		_msc_bcast( p, l );
	}
}

const char *_msc_skipspace( const char *s )
{
	while( *s && isspace(*s))
		s++;

	return s;
}

char *_msc_fielddup( const char *s, char **end )
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

	if( end ) (const char *)*end = s;

	return dup;
}


