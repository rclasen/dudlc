#include <stdio.h>

#include "tty.h"
#include "formatter.h"


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

	tty_msg( "%s\n", mktrackhead(buf, BUFLENTRACK));
	for( t = msc_it_track_cur(it); t; t = msc_it_track_next(it)){
		tty_msg( "%s\n", mktrack(buf,BUFLENTRACK,t));
		msc_track_free(t);
	}
	msc_it_track_done(it);
}


