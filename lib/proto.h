#ifndef _MSERVCLIENT_PROTO_H
#define _MSERVCLIENT_PROTO_H

#include <stdarg.h>

#include <mservclient/client.h>

/************************************************************
 * internal protocol parsing functions
 ************************************************************/

#define MSC_EVENT(c,name,arg...)	\
	if( (c)->ev && ((msc_events*)(c)->ev)->name ){ \
		(*((msc_events*)(c)->ev)->name)(arg); \
	}

int _msc_vsend( mservclient *p, const char *fmt, va_list ap );
int _msc_send( mservclient *p, const char *fmt, ... );

int _msc_rlast( mservclient *p );
int _msc_rnext( mservclient *p );
int _msc_rend( mservclient *p );
const char *_msc_rcode( mservclient *p );
const char *_msc_rline( mservclient *p );

/* convenience pack for single-line responses: */
int _msc_cmd( mservclient *p, const char *fmt, ... );

char *_msc_fielddup( const char *s, char **end );
const char *_msc_skipspace( const char *s );

#endif

