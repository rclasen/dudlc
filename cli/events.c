/*
 * Copyright (c) 2008 Rainer Clasen
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * The GNU General Public License is often shipped with GNU software, and
 * is generally kept in a file called COPYING or LICENSE.  If you do not
 * have a copy of the license, write to the Free Software Foundation,
 * 59 Temple Place, Suite 330, Boston, MA 02111 USA.
 */

#include <stdlib.h>
#include <string.h>

#include "tty.h"
#include "events.h"


duc_events events;


#if 0
static void cb_bcast( dudlc *c, const char *line )
{
	tty_msg( "unhandled bcast: %s\n", line );
	(void)c;
}
#endif

/************************************************************
 * dis-/connect
 */

static void cb_disc( dudlc *c )
{
	tty_msg( "disconnected, trying to reconnect\n");
	if( duc_open(c)){
		tty_msg( "reconnect failed\n" );
	}
}

static void cb_conn( dudlc *c )
{
	tty_msg( "connected\n");
	(void)c;
}

/************************************************************
 * user/clients
 */

#if 0
static void cb_login( dudlc *c, duc_client *u )
{
	char buf[BUFLENCLIENT];

	tty_msg( "login: %s\n", dfmt_client(buf,BUFLENTRACK, u));
	(void)c;
}

static void cb_logout( dudlc *c, duc_client *u )
{
	char buf[BUFLENCLIENT];

	tty_msg( "logout: %s\n", dfmt_client(buf,BUFLENTRACK, u));
	(void)c;
}
#endif

static void cb_kicked( dudlc *c )
{
	tty_msg( "you were kicked from the server\n" );
	(void)c;
}

/************************************************************
 * player
 */

static void cb_nexttrack( dudlc *c, duc_track *t )
{
	char buf[BUFLENTRACK];

	tty_msg( "%s\n", dfmt_track(buf, BUFLENTRACK, t ));
	(void)c;
}

static void cb_stopped( dudlc *c )
{
	tty_msg( "stopped\n" );
	(void)c;
}

static void cb_paused( dudlc *c )
{
	tty_msg( "paused\n" );
	(void)c;
}

static void cb_resumed( dudlc *c )
{
	tty_msg( "resumed\n");
	(void)c;
}

static void cb_random( dudlc *c, int r )
{
	tty_msg( "random mode turned %s\n", r ? "on": "off" );
	(void)c;
}

#if 0
static void cb_elapsed( dudlc *c, int r )
{
	tty_msg( "elapsed: %d\n", r );
	(void)c;
}
#endif

/************************************************************
 * random/filter
 */

#if 0
static void cb_filter( dudlc *c, const char *f )
{
	tty_msg( "new filter: %s\n", f );
	(void)c;
}
#endif

/************************************************************
 * queue
 */

#if 0
static void cb_queueadd( dudlc *c, duc_queue *q )
{
	char buf[BUFLENQUEUE];
	tty_msg( "queued: %s\n", dfmt_queue(buf, BUFLENQUEUE,q ));
	(void)c;
}

static void cb_queuedel( dudlc *c, duc_queue *q )
{
	char buf[BUFLENQUEUE];

	tty_msg( "unqueued: %s\n", dfmt_queue(buf, BUFLENQUEUE, q ));
	(void)c;
}
#endif

static void cb_queueclear( dudlc *c )
{
	tty_msg( "queue was cleared\n" );
	(void)c;
}

/************************************************************
 * sleep
 */

static void cb_sleep( dudlc *c, int del )
{
	tty_msg( "falling asleep in %d sec\n", del );
	(void)c;
}

/************************************************************
 * tag
 */

#if 0
static void cb_tagchange( dudlc *c, duc_tag *q )
{
	char buf[BUFLENTAG];

	tty_msg( "changed tag: %s\n", dfmt_tag(buf, BUFLENTAG, q ));
	(void)c;
}

static void cb_tagdel( dudlc *c, duc_tag *q )
{
	char buf[BUFLENTAG];

	tty_msg( "deleted tag: %s\n", dfmt_tag(buf, BUFLENTAG, q ));
	(void)c;
}
#endif

/************************************************************
 * init
 */

void events_init( dudlc *c )
{
	memset(&events, 0, sizeof(events));
	duc_setevents( c, &events );

	//events.bcast = cb_bcast;

	events.connect = cb_conn;
	events.disconnect = cb_disc;

	//events.login = cb_login;
	//events.logout = cb_logout;
	events.kicked = cb_kicked;

	events.nexttrack = cb_nexttrack;
	events.stopped = cb_stopped;
	events.paused = cb_paused;
	events.resumed = cb_resumed;
	events.random = cb_random;
	//events.elapsed = cb_elapsed;

	//events.filter = cb_filter;

	events.queuefetch = NULL;
	//events.queueadd = cb_queueadd;
	//events.queuedel = cb_queuedel;
	events.queueclear = cb_queueclear;

	events.sleep = cb_sleep;

	//events.tagchange = cb_tagchange;
	//events.tagdel = cb_tagdel;
}


