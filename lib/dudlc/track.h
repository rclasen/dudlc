#ifndef _MSERVCLIENT_TRACK_H
#define _MSERVCLIENT_TRACK_H

#include "session.h"
#include "iterate.h"

typedef struct {
	int id;
	int albumid;
	int albumnr;
	char *title;
	int artistid;
	int duration;
} msc_track;

#define msc_it_track	_msc_iter
#define msc_it_track_cur(x)	(msc_track*)_msc_it_cur((_msc_iter*)x)
#define msc_it_track_next(x)	(msc_track*)_msc_it_next((_msc_iter*)x)
#define msc_it_track_done(x)	_msc_it_done((_msc_iter*)x)

msc_track *_msc_track_parse( const char *line, char **end );
void msc_track_free( msc_track *t );

int msc_cmd_tracks( mservclient *c );
int msc_cmd_track2id( mservclient *c, int albumid, int nr );
msc_track *msc_cmd_trackget( mservclient *c, int id );

msc_it_track *msc_cmd_tracksearch( mservclient *c, const char *substr );
msc_it_track *msc_cmd_tracksalbum( mservclient *c, int id );
msc_it_track *msc_cmd_tracksartist( mservclient *c, int id );

// TODO: alter 


#endif
