#ifndef _MSERVCLIENT_PROTO_H
#define _MSERVCLIENT_PROTO_H

#include <stdarg.h>

#include <mservclient/client.h>

/************************************************************
 * internal protocol parsing functions
 ************************************************************/

typedef void *(*_msc_converter)( const char *line, char **end );

int _msc_vsend( mservclient *p, const char *fmt, va_list ap );
int _msc_send( mservclient *p, const char *fmt, ... );

int _msc_rlast( mservclient *p );
int _msc_rnext( mservclient *p );
int _msc_rend( mservclient *p );
const char *_msc_rcode( mservclient *p );
const char *_msc_rline( mservclient *p );

/* convenience pack for single-line responses: */
int _msc_cmd( mservclient *p, const char *fmt, ... );
int _msc_cmd_succ( mservclient *c, const char *fmt, ... );
int _msc_cmd_int( mservclient *c, const char *fmt, ... );
void *_msc_cmd_conv( mservclient *c, _msc_converter conv, 
		const char *fmt, ... );


char *_msc_fielddup( const char *s, char **end );
const char *_msc_skipspace( const char *s );

#endif

