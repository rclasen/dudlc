

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
	msc_open(c);
}

static void cb_conn( mservclient *c )
{
	tty_msg( "connected\n");
	(void)c;
}

/************************************************************
 * player
 */

static void cb_nexttrack( mservclient *c, msc_track *t )
{
	char buf[BUFLENTRACK];

	tty_msg( "playing: \x1B[1m%s\x1B[0m\n", mktrack(buf, BUFLENTRACK, t));
	(void)c;
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

void events_init( mservclient *c )
{
	memset(&events, 0, sizeof(events));
	msc_setevents( c, &events );

	events.bcast = cb_bcast;

	events.connect = cb_conn;
	events.disconnect = cb_disc;

	events.nexttrack = cb_nexttrack;
	events.stopped = cb_stopped;
	events.paused = cb_paused;
	events.resumed = cb_resumed;
}


