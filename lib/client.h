#ifndef _MSERVCLIENT_PROTO_H
#define _MSERVCLIENT_PROTO_H

#include <mservclient/sock.h>



typedef struct {
	char *user;
	char *pass;

	void *ev;

	t_msc_sock *con;
	int inreply;
	char code[4];
	const char *line;
} mservclient;

typedef void (*msc_ev_argnone)( mservclient *c );
typedef void (*msc_ev_argstring)( mservclient *c, const char *s );

typedef struct {
	msc_ev_argnone connect;
	msc_ev_argnone disconnect;

	// temporary:
	msc_ev_argstring bcast;
} msc_events;



mservclient *msc_new( const char *hostname, int port, 
		const char *user, const char *pass );
int msc_open( mservclient *p );
void msc_free( mservclient *p );

int msc_fd( mservclient *p );
void msc_poll( mservclient *p );

msc_events *msc_getevents( mservclient *p );
msc_events *msc_setevents( mservclient *p, msc_events *e );

/************************************************************
 * internal protocol parsing functions
 ************************************************************/

int _msc_send( mservclient *p, const char *msg );
int _msc_fsend( mservclient *p, const char *fmt, ... );

int _msc_last( mservclient *p );
int _msc_nextline( mservclient *p );
int _msc_rend( mservclient *p );
const char *_msc_rcode( mservclient *p );
const char *_msc_rline( mservclient *p );

#endif
