#include <stdlib.h>

#include "tty.h"
#include "events.h"


duc_events events;


static void cb_bcast( dudlc *c, const char *line )
{
	tty_msg( "%s\n", line );
	(void)c;
}

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

static void cb_kicked( dudlc *c )
{
	tty_msg( "you were kicked from the server\n" );
	(void)c;
}

/************************************************************
 * player
 */

static void cb_nexttrack( dudlc *c, duc_track *t, 
		duc_artist *ar, duc_album *al )
{
	char buf[BUFLENTRACK];

	tty_msg( "playing: \n\x1B[1m%s\x1B[0m\n", 
			dfmt_rtrack(buf, BUFLENTRACK, t, ar, al ));
	(void)c;
	(void)ar;
	(void)al;
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

/************************************************************
 * random/filter
 */

static void cb_filter( dudlc *c, const char *f )
{
	tty_msg( "new filter: %s\n", f );
	(void)c;
}

/************************************************************
 * queue
 */

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


/************************************************************
 * init
 */

void events_init( dudlc *c )
{
	memset(&events, 0, sizeof(events));
	duc_setevents( c, &events );

	events.bcast = cb_bcast;

	events.connect = cb_conn;
	events.disconnect = cb_disc;

	events.login = cb_login;
	events.logout = cb_logout;
	events.kicked = cb_kicked;

	events.nexttrack = cb_nexttrack;
	events.stopped = cb_stopped;
	events.paused = cb_paused;
	events.resumed = cb_resumed;
	events.random = cb_random;

	events.filter = cb_filter;

	events.queuefetch = NULL;
	events.queueadd = cb_queueadd;
	events.queuedel = cb_queuedel;
	events.queueclear = cb_queueclear;

	events.sleep = cb_sleep;

	events.tagchange = cb_tagchange;
	events.tagdel = cb_tagdel;
}


