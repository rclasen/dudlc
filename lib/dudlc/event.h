#ifndef _DUDLC_EVENT_H
#define _DUDLC_EVENT_H

#include "session.h"
#include "client.h"
#include "user.h"
#include "track.h"
#include "queue.h"
#include "tag.h"
#include "artist.h"
#include "album.h"

typedef void (*duc_ev_argnone)( dudlc *c );
typedef void (*duc_ev_argstring)( dudlc *c, const char *s );
typedef void (*duc_ev_argint)( dudlc *c, int i );
typedef void (*duc_ev_argclient)( dudlc *c, duc_client *t );
typedef void (*duc_ev_argqueue)( dudlc *c, duc_queue *t );
typedef void (*duc_ev_argtag)( dudlc *c, duc_tag *t );
typedef void (*duc_ev_argntrack)( dudlc *c, 
		duc_track *t, duc_artist *ar, duc_album *al );

typedef struct {
	/* my connection status */
	duc_ev_argnone connect;
	duc_ev_argnone disconnect;

	/* other users/clients */
	duc_ev_argclient login;
	duc_ev_argclient logout;
	duc_ev_argnone kicked;

	/* player */
	duc_ev_argntrack nexttrack;
	duc_ev_argnone stopped;
	duc_ev_argnone paused;
	duc_ev_argnone resumed;
	duc_ev_argint random;

	/* random/filter */
	duc_ev_argstring filter;

	/* sleep */
	duc_ev_argint sleep;

	/* queue */
	duc_ev_argqueue queuefetch;
	duc_ev_argqueue queueadd;
	duc_ev_argqueue queuedel;
	duc_ev_argnone queueclear;

	/* tags */
	duc_ev_argtag tagchange;
	duc_ev_argtag tagdel;

	// temporary:
	duc_ev_argstring bcast;
} duc_events;

duc_events *duc_getevents( dudlc *p );
duc_events *duc_setevents( dudlc *p, duc_events *e );


#define _MSC_EVENT(c,name,arg...)	\
	if( (c)->ev && ((duc_events*)(c)->ev)->name ){ \
		(*((duc_events*)(c)->ev)->name)(arg); \
	}


#endif

void _duc_bcast( dudlc *p, const char *line );

