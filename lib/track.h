#ifndef _MSERVCLIENT_TRACK_H
#define _MSERVCLIENT_TRACK_H

#include <mservclient/client.h>
#include <mservclient/iterate.h>

typedef struct {
	int id;
	int albumid;
	int albumnr;
	char *title;
	int artistid;
	int duration;
	int lastplay;
	int _refs;
} msc_track;

#define msc_it_track	_msc_iter
#define msc_it_track_cur(x)	(msc_track*)_msc_it_cur((_msc_iter*)x)
#define msc_it_track_next(x)	(msc_track*)_msc_it_next((_msc_iter*)x)
#define msc_it_track_done(x)	_msc_it_done((_msc_iter*)x)

msc_track *_msc_track_parse( const char *line, char **end );
void msc_track_free( msc_track *t );

int msc_cmd_tracks( mservclient *c );
msc_track *msc_cmd_track_get( mservclient *c, int id );

msc_it_track *msc_cmd_tracks_search( mservclient *c, const char *substr );
msc_it_track *msc_cmd_tracks_albumid( mservclient *c, int id );
msc_it_track *msc_cmd_tracks_artistid( mservclient *c, int id );

// TODO: alter, callback


#endif
