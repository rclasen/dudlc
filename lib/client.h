#ifndef _MSERVCLIENT_CLIENT_H
#define _MSERVCLIENT_CLIENT_H

#include <mservclient/sock.h>



typedef struct {
	void *ev;

	char *user;
	char *pass;

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
void msc_free( mservclient *p );

int msc_open( mservclient *p );
void msc_close( mservclient *p );

int msc_fd( mservclient *p );
void msc_poll( mservclient *p );

msc_events *msc_getevents( mservclient *p );
msc_events *msc_setevents( mservclient *p, msc_events *e );

const char *msc_rmsg( mservclient *p );

#endif

