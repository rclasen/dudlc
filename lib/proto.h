#ifndef _MSERVCLIENT_PROTO_H
#define _MSERVCLIENT_PROTO_H

#include <mservclient/client.h>

/************************************************************
 * internal protocol parsing functions
 ************************************************************/

#define MSC_EVENT(c,name,arg...)	\
	if( (c)->ev && ((msc_events*)(c)->ev)->name ){ \
		(*((msc_events*)(c)->ev)->name)(arg); \
	}


int _msc_send( mservclient *p, const char *msg );
int _msc_fsend( mservclient *p, const char *fmt, ... );

int _msc_last( mservclient *p );
int _msc_nextline( mservclient *p );
int _msc_rend( mservclient *p );
const char *_msc_rcode( mservclient *p );
const char *_msc_rline( mservclient *p );


#endif

