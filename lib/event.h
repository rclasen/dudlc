#ifndef _MSERVCLIENT_EVENT_H
#define _MSERVCLIENT_EVENT_H

#include <mservclient/client.h>
#include <mservclient/user.h>
#include <mservclient/track.h>
#include <mservclient/queue.h>

typedef void (*msc_ev_argnone)( mservclient *c );
typedef void (*msc_ev_argstring)( mservclient *c, const char *s );
typedef void (*msc_ev_argint)( mservclient *c, int i );
typedef void (*msc_ev_argclient)( mservclient *c, msc_client *t );
typedef void (*msc_ev_argtrack)( mservclient *c, msc_track *t );
typedef void (*msc_ev_argqueue)( mservclient *c, msc_queue *t );

typedef struct {
	/* my connection status */
	msc_ev_argnone connect;
	msc_ev_argnone disconnect;

	/* other users/clients */
	msc_ev_argclient login;
	msc_ev_argclient logout;
	msc_ev_argnone kicked;

	/* player */
	msc_ev_argtrack nexttrack;
	msc_ev_argnone stopped;
	msc_ev_argnone paused;
	msc_ev_argnone resumed;
	msc_ev_argint random;

	/* random/filter */
	msc_ev_argstring filter;

	/* sleep */
	msc_ev_argint sleep;

	/* queue */
	msc_ev_argqueue queuefetch;
	msc_ev_argqueue queueadd;
	msc_ev_argqueue queuedel;
	msc_ev_argnone queueclear;

	// temporary:
	msc_ev_argstring bcast;
} msc_events;

msc_events *msc_getevents( mservclient *p );
msc_events *msc_setevents( mservclient *p, msc_events *e );


#define _MSC_EVENT(c,name,arg...)	\
	if( (c)->ev && ((msc_events*)(c)->ev)->name ){ \
		(*((msc_events*)(c)->ev)->name)(arg); \
	}


#endif


