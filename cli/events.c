#include <stdlib.h>

#include "tty.h"
#include "formatter.h"
#include "events.h"


msc_events events;


static void cb_bcast( mservclient *c, const char *line )
{
	tty_msg( "%s\n", line );
	(void)c;
}

/************************************************************
 * dis-/connect
 */

static void cb_disc( mservclient *c )
{
	tty_msg( "disconnected, trying to reconnect\n");
	if( msc_open(c)){
		tty_msg( "reconnect failed\n" );
	}
}

static void cb_conn( mservclient *c )
{
	tty_msg( "connected\n");
	(void)c;
}

/************************************************************
 * user/clients
 */

static void cb_login( mservclient *c, msc_client *u )
{
	char buf[BUFLENCLIENT];

	tty_msg( "login: %s\n", mkclient(buf,BUFLENTRACK, u));
	(void)c;
}

static void cb_logout( mservclient *c, msc_client *u )
{
	char buf[BUFLENCLIENT];

	tty_msg( "logout: %s\n", mkclient(buf,BUFLENTRACK, u));
	(void)c;
}

static void cb_kicked( mservclient *c )
{
	tty_msg( "you were kicked from the server\n" );
	(void)c;
}

/************************************************************
 * player
 */

static void cb_nexttrack( mservclient *c, msc_track *t, 
		msc_artist *ar, msc_album *al )
{
	char buf[BUFLENTRACK];

	tty_msg( "playing: \n\x1B[1m%s\x1B[0m\n", 
			mkrtrack(buf, BUFLENTRACK, t, ar, al ));
	(void)c;
	(void)ar;
	(void)al;
}

static void cb_stopped( mservclient *c )
{
	tty_msg( "stopped\n" );
	(void)c;
}

static void cb_paused( mservclient *c )
{
	tty_msg( "paused\n" );
	(void)c;
}

static void cb_resumed( mservclient *c )
{
	tty_msg( "resumed\n");
	(void)c;
}

static void cb_random( mservclient *c, int r )
{
	tty_msg( "random mode turned %s\n", r ? "on": "off" );
	(void)c;
}

/************************************************************
 * random/filter
 */

static void cb_filter( mservclient *c, const char *f )
{
	tty_msg( "new filter: %s\n", f );
	(void)c;
}

/************************************************************
 * queue
 */

static void cb_queueadd( mservclient *c, msc_queue *q )
{
	char buf[BUFLENQUEUE];
	tty_msg( "queued: %s\n", mkqueue(buf, BUFLENQUEUE,q ));
	(void)c;
}

static void cb_queuedel( mservclient *c, msc_queue *q )
{
	char buf[BUFLENQUEUE];

	tty_msg( "unqueued: %s\n", mkqueue(buf, BUFLENQUEUE, q ));
	(void)c;
}

static void cb_queueclear( mservclient *c )
{
	tty_msg( "queue was cleared\n" );
	(void)c;
}

/************************************************************
 * sleep
 */

static void cb_sleep( mservclient *c, int del )
{
	tty_msg( "falling asleep in %d sec\n", del );
	(void)c;
}

/************************************************************
 * tag
 */

static void cb_tagchange( mservclient *c, msc_tag *q )
{
	char buf[BUFLENTAG];

	tty_msg( "changed tag: %s\n", mktag(buf, BUFLENTAG, q ));
	(void)c;
}

static void cb_tagdel( mservclient *c, msc_tag *q )
{
	char buf[BUFLENTAG];

	tty_msg( "deleted tag: %s\n", mktag(buf, BUFLENTAG, q ));
	(void)c;
}


/************************************************************
 * init
 */

void events_init( mservclient *c )
{
	memset(&events, 0, sizeof(events));
	msc_setevents( c, &events );

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


