#include <stdio.h>

#include "tty.h"
#include "formatter.h"


/************************************************************
 * client
 */

const char *mkclienthead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%4.4s %4.4s %-15s", "id", "uid", "address" );
	return buf;
}

// TODO: lookup user name
const char *mkclient( char *buf, unsigned int len, msc_client *c )
{
	snprintf( buf, len, "%4d %4d %-15s", c->id, c->uid, c->addr );
	return buf;
}

void dump_clients( msc_it_client *it )
{
	char buf[BUFLENCLIENT];
	msc_client *c;

	tty_msg( "%s\n\n", mkclienthead(buf, BUFLENCLIENT));
	for( c = msc_it_client_cur(it); c; c = msc_it_client_next(it)){
		tty_msg( "%s\n", mkclient(buf, BUFLENCLIENT, c ));
		msc_client_free(c);
	}
}

/************************************************************
 * track
 */

const char *mktrackhead( char *buf, unsigned int len )
{
	snprintf( buf, len, "%5.5s %-20.20s %2.2s %-20.20s %s",
			"id", "album", "nr", "artist", "title" );
	return buf;
}

// TODO: lookup artist and album name
const char *mktrack( char *buf, unsigned int len, msc_track *t )
{
	snprintf( buf, len, "%5d %-20d %2d %-20d %s",
			t->id, t->albumid, t->albumnr, t->artistid, t->title );

	return buf;
}

void dump_tracks( msc_it_track *it )
{
	msc_track *t;
	char buf[BUFLENTRACK];

	tty_msg( "%s\n\n", mktrackhead(buf, BUFLENTRACK));
	for( t = msc_it_track_cur(it); t; t = msc_it_track_next(it)){
		tty_msg( "%s\n", mktrack(buf,BUFLENTRACK,t));
		msc_track_free(t);
	}
}


